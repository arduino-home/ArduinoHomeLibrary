#ifndef __ARDUINO_HOME_WIFI_HTTP_SERVICE_H__
#define __ARDUINO_HOME_WIFI_HTTP_SERVICE_H__

#include "service.h"

class ESP8266WebServer;

struct WifiHttpService : public Service {

  explicit WifiHttpService(const int &pport = 80);
  virtual ~WifiHttpService() = default;

  virtual void setup();
  virtual void loop();

  virtual const char *getName() const;
  virtual const char *getId() const;
  virtual const char *getSettings() const;

private:
  ESP8266WebServer *server;
  String settings;
};

#endif // __ARDUINO_HOME_WIFI_HTTP_SERVICE_H__
