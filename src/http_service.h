#ifndef __ARDUINO_HOME_HTTP_SERVICE_H__
#define __ARDUINO_HOME_HTTP_SERVICE_H__

#include "service.h"

class Client;
class Server;

class WebServer;

struct HttpService : public Service {

  explicit HttpService(const int &pport = 80);
  virtual ~HttpService() = default;

  virtual void setup();
  virtual void loop();

  virtual const char *getName() const;
  virtual const char *getId() const;
  virtual const char *getSettings() const;

private:
  String settings;
  WebServer *server;
};

#endif // __ARDUINO_HOME_HTTP_SERVICE_H__
