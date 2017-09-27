#include <Arduino.h>

#include "utils/utils.h"
#include "utils/string_stream.h"
#include "irc_service.h"
#include "dispatcher_service.h"
#include "network_service.h"
#include "runtime.h"

#define NAME "IrcService"

namespace ah {
  namespace services {

    static const char *irc_endl = "\r\n";
    static const char irc_last_arg = ':';

    namespace internal {
      class IrcMessageParser {
        size_t len;
        char *buffer;
        char *pos;

      public:

        const char *from;
        const char *command;
        const char *args[11]; // 11 for last at nullptr always

      private:

        const char *readUntil(char sep) {
          char *begin = pos;
          while(*pos && *pos != sep) {
            ++pos;
          }
          *(pos++) = 0;
          return begin;
        }

        bool end() {
          return pos >= buffer + len;
        }

      public:

        explicit IrcMessageParser(const char *pline, const size_t &plen)
        : len(plen), buffer(new char[len + 1]), pos(buffer),
          from(nullptr), command(nullptr), args({ nullptr }) {

          memcpy(buffer, pline, len);
          buffer[len] = 0;

          if(*pos == ':') {
            ++pos;
            from = readUntil(' ');
          }

          command = readUntil(' ');

          for(int i=0; i<10; ++i) {
            if(end()) { break; }

            if(*pos == irc_last_arg) {
              ++pos;
              args[i] = readUntil(0);
              break;
            }

            args[i] = readUntil(' ');
          }
        }

        ~IrcMessageParser() {
          delete[] buffer;
        }
      };
    } // namespace internal

    IrcService::IrcService(const char *pnick, const char *pchannel, const char *pserver, uint16_t pport)
     : nick(pnick ? String(pnick) : (String(Runtime::getName()) + "-" + String(Runtime::getUid()))),
       channel(pchannel),
       server(pserver),
       port(pport),
       client(nullptr),
       registered(no),
       lastTry(0),
       dispatcher(nullptr) {
      utils::StringStream ss(settings);
      ss << "nick=" << pnick << ", channel=" << pchannel << ", server=" << pserver << ", port=" << pport;
    }

    void IrcService::setup() {
      dispatcher = Runtime::getDispatcherService();
      net = Runtime::getNetworkService();

      client = net->createClient();

      dispatcher->registerNotifier([this](const String &id, const ArduinoJson::JsonVariant &value) {
        if(!connected()) {
          return;
        }

        auto &ostream = *(this->client);
        ostream << "PRIVMSG " << this->channel << " :notify " << id << " ";
        value.printTo(ostream);
        ostream << irc_endl;
      });
    }

    void IrcService::loop() {
      if(!checkConnection()) {
        return;
      }

      if(!checkRegistration()) {
        return;
      }

      read();
    }

    const char *IrcService::getName() const {
      return NAME;
    }

    const char *IrcService::getId() const {
      return NAME;
    }

    const char *IrcService::getSettings() const {
      return settings.c_str();
    }

    bool IrcService::checkConnection() {

      if(!net->isOnline()) {
        client->stop();
        return false;
      }

      if(client->connected()) {
        return true;
      }

      buffer = "";
      registered = no;

      if(lastTry + 5000 > millis()) {
        return false;
      }

      AH_DEBUG("IrcService: connecting" << endl);
      lastTry = millis();
      client->stop();
      return client->connect(server, port);
    }

    bool IrcService::checkRegistration() {
      if(registered != no) {
        return true;
      }

      AH_DEBUG("IrcService: registering" << endl);
      (*client) << "NICK " << nick << irc_endl << "USER " << nick << " * * :ArduinoHomeLibrary.IrcService" << irc_endl;

      registered = pending;

      return true;
    }

    void IrcService::read() {
      while (client->available()) {
        buffer += static_cast<char>(client->read());

        if(buffer.endsWith(irc_endl)) {
          internal::IrcMessageParser parser(buffer.c_str(), buffer.length() - 2);
          buffer = "";
          process(parser);
        }
      }
    }

    void IrcService::process(const internal::IrcMessageParser &msg) {
      AH_DEBUG("Message: ");
      if(msg.from) {
        AH_DEBUG(msg.from << " ");
      }
      AH_DEBUG(msg.command);
      for(const char * const *arg = msg.args; *arg; ++arg) {
        AH_DEBUG(" " << *arg);
      }
      AH_DEBUG(endl);

      if(!strcmp(msg.command, "001")) {
        registered = success;
        AH_DEBUG("IrcService: registered" << endl);
        (*client) << "JOIN " << channel << irc_endl;
        return;
      }

      if(!strcmp(msg.command, "PING")) {
        (*client) << "PONG " << irc_last_arg << msg.args[0] << irc_endl;
        return;
      }

      if(!strcmp(msg.command, "PRIVMSG")) {
        char *self = strtok(const_cast<char *>(msg.args[1]), " "); // TODO: constness ?
        char *id   = strtok(nullptr, " ");
        char *data = strtok(nullptr, "");

        if(!self || nick !=self) {
          return;
        }

        if(!id) {
          return;
        }

        if(data) {
          // setter
          const JsonVariant& value = DispatcherService::sharedBuffer().parse(data);
          switch(dispatcher->set(id, value)) {

            case DispatcherService::HandlerResult::success:
              (*client) << "PRIVMSG " << this->channel << " :set " << id << " success" << irc_endl;
              break;

            case DispatcherService::HandlerResult::not_found:
              (*client) << "PRIVMSG " << this->channel << " :set " << id << " not_found" << irc_endl;
              break;

            case DispatcherService::HandlerResult::handler_error:
              (*client) << "PRIVMSG " << this->channel << " :set " << id << " handler_error" << irc_endl;
              break;

            default:
              (*client) << "PRIVMSG " << this->channel << " :set " << id << " unknow_result" << irc_endl;
              break;
          }

          return;
        }

        // getter
        JsonVariant value;
        switch(dispatcher->get(id, value)) {

          case DispatcherService::HandlerResult::success: {
            (*client) << "PRIVMSG " << this->channel << " :get " << id << " ";
            value.printTo(*client);
            (*client) << irc_endl;
            break;
          }

          case DispatcherService::HandlerResult::not_found:
            (*client) << "PRIVMSG " << this->channel << " :get " << id << " not_found" << irc_endl;
            break;

          case DispatcherService::HandlerResult::handler_error:
            (*client) << "PRIVMSG " << this->channel << " :get " << id << " handler_error" << irc_endl;
            break;

          default:
            (*client) << "PRIVMSG " << this->channel << " :get " << id << " unknow_result" << irc_endl;
            break;
        }

        return;
      }
    }

    bool IrcService::connected() const {
      return registered == success;
    }

  } // namespace services
} // namespace ah
