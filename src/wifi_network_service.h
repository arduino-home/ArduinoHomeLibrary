#ifndef __ARDUINO_HOME_WIFI_NETWORK_SERVICE_H__
#define __ARDUINO_HOME_WIFI_NETWORK_SERVICE_H__

#ifdef ESP8266

#include "network_service.h"

struct WifiNetworkService : public NetworkService {
  explicit WifiNetworkService() = default;
  virtual ~WifiNetworkService() = default;

  virtual void init();
  virtual void loop();

  virtual Server *createServer(const int &port);
  virtual Client *createClient();
  virtual Client *serverAvailable(Server *server);
  virtual void serverClose(Server *server);

  virtual bool isOnline();

  virtual const char *getName() const;
  virtual const char *getId() const;

private:
  int configPin;
  String settings;
};

#endif // ESP8266

#endif // __ARDUINO_HOME_WIFI_NETWORK_SERVICE_H__
