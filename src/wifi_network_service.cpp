#ifdef ESP8266

#include "system.h"

#include "wifi_network_service.h"
#include "runtime.h"

#define NAME "WifiNetworkService"

void WifiNetworkService::init() {
  Runtime::setUid(ESP.getChipId());
}

Server *WifiNetworkService::createServer(const int &port) {
  return new WiFiServer(port);
}

Client *WifiNetworkService::createClient() {
  return new WiFiClient();
}

const char *WifiNetworkService::getName() const {
  return NAME;
}

const char *WifiNetworkService::getId() const {
  return NAME;
}

#endif // ESP8266
