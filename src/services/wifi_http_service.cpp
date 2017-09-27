#ifdef ESP8266

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

#include "utils/utils.h"
#include "utils/string_stream.h"
#include "wifi_http_service.h"
#include "dispatcher_service.h"
#include "runtime.h"

#define NAME "WifiHttpService"

namespace ah {
  namespace services {

    class ServiceRequestHandler : public RequestHandler {

      DispatcherService *dispatcher;

      void handleGet(ESP8266WebServer& server, const String &id) {
        JsonVariant value;
        switch(dispatcher->get(id, value)) {

          case DispatcherService::HandlerResult::success: {
            String response;
            value.printTo(response); // TODO: avoid string ?
            server.send(200, "application/json", response);
            break;
          }

          case DispatcherService::HandlerResult::not_found:
            server.send(404);
            break;

          case DispatcherService::HandlerResult::handler_error:
            server.send(400);
            break;

          default:
            server.send(500);
            break;
        }
      }

      void handleSet(ESP8266WebServer& server, const String &id) {
        const JsonVariant& value = DispatcherService::sharedBuffer().parse(server.arg("plain"));
        switch(dispatcher->set(id, value)) {

          case DispatcherService::HandlerResult::success:
            server.send(200);
            break;

          case DispatcherService::HandlerResult::not_found:
            server.send(404);
            break;

          case DispatcherService::HandlerResult::handler_error:
            server.send(400);
            break;

          default:
            server.send(500);
            break;
        }
      }

    public:
        ServiceRequestHandler(DispatcherService *pdispatcher)
        : dispatcher(pdispatcher) {
        }

        virtual bool canHandle(HTTPMethod method, String uri) {
          if(!uri.length()) {
            return false;
          }
          if(uri[0] != '/') {
            return false;
          }
          uri = uri.substring(1);
          switch(method) {
            case HTTP_GET:
              return dispatcher->hasGetter(uri);
            case HTTP_POST:
              return dispatcher->hasSetter(uri);
          }
          return false;
        }

        virtual bool handle(ESP8266WebServer& server, HTTPMethod method, String uri) {
          uri = uri.substring(1);

          switch(method) {
            case HTTP_GET:
              handleGet(server, uri);
              return true;
            case HTTP_POST:
              handleSet(server, uri);
              return true;
          }
          return false;
        }
    };

    WifiHttpService::WifiHttpService(const int &pport)
     : server(new ESP8266WebServer(pport)) {
      utils::StringStream ss(settings);
      ss << "port=" << pport;
    }

    void WifiHttpService::setup() {
      auto dispatcher = Runtime::getDispatcherService();
      server->addHandler(new ServiceRequestHandler(dispatcher));
      server->begin();
    }

    void WifiHttpService::loop() {
      server->handleClient();
    }

    const char *WifiHttpService::getName() const {
      return NAME;
    }

    const char *WifiHttpService::getId() const {
      return NAME;
    }

    const char *WifiHttpService::getSettings() const {
      return settings.c_str();
    }

  } // namespace services
} // namespace ah

#endif // ESP8266

