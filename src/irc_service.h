#ifndef __ARDUINO_HOME_IRC_SERVICE_H__
#define __ARDUINO_HOME_IRC_SERVICE_H__

#include "service.h"

class IrcMessageParser;
struct DispatcherService;

struct IrcService : public Service {

  explicit IrcService(const char *pnick, const char *pchannel, const char *pserver, uint16_t pport = 6667);
  virtual ~IrcService() = default;

  virtual void setup();
  virtual void loop();

  virtual const char *getName() const;
  virtual const char *getId() const;
  virtual const char *getSettings() const;

private:
  bool checkConnection();
  bool checkRegistration();
  void read();
  void process(const IrcMessageParser &msg);
  bool connected() const;

  String nick;
  const char *channel;
  const char *server;
  uint16_t port;

  Client *client;
  unsigned long lastTry;
  enum { no, pending, success } registered;
  String buffer;
  DispatcherService *dispatcher;

  String settings;
};

#endif // __ARDUINO_HOME_IRC_SERVICE_H__
