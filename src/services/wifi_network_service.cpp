#ifdef ESP8266

#include <ESP8266WiFi.h> // https://github.com/esp8266/Arduino

#include "utils/utils.h"
#include "wifi_network_service.h"
#include "runtime.h"

#define NAME "WifiNetworkService"

namespace ah {
  namespace services {

    static inline const char * wifiStatusToString() {
      switch(WiFi.status()) {
        case WL_CONNECTED: return "WL_CONNECTED";
        case WL_NO_SHIELD: return "WL_NO_SHIELD";
        case WL_IDLE_STATUS: return "WL_IDLE_STATUS";
        case WL_NO_SSID_AVAIL: return "WL_NO_SSID_AVAIL";
        case WL_SCAN_COMPLETED: return "WL_SCAN_COMPLETED";
        case WL_CONNECT_FAILED: return "WL_CONNECT_FAILED";
        case WL_CONNECTION_LOST: return "WL_CONNECTION_LOST";
        case WL_DISCONNECTED: return "WL_DISCONNECTED";
      }
      return "<unknown>";
    }

    static inline void debugWifiStatus() {
      static int oldStatus = -1;

      if(oldStatus == WiFi.status()) { return; }
      oldStatus = WiFi.status();

      AH_DEBUG(WiFi.SSID() << " " << wifiStatusToString());
      if(oldStatus == WL_CONNECTED) { AH_DEBUG(" " << WiFi.localIP()); }
      AH_DEBUG(endl);
    }

    void WifiNetworkService::init() {
      Runtime::setUid(ESP.getChipId());
    }

    void WifiNetworkService::loop() {
      debugWifiStatus();
    }

    Server *WifiNetworkService::createServer(const int &port) {
      return new WiFiServer(port);
    }

    Client *WifiNetworkService::createClient() {
      return new WiFiClient();
    }

    Client *WifiNetworkService::serverAvailable(Server *server) {
      auto wserver = static_cast<WiFiServer *>(server);
      auto client = wserver->available();
      if(!client) {
        return nullptr;
      }
      return new WiFiClient(client);
    }

    void WifiNetworkService::serverClose(Server *server) {
      auto wserver = static_cast<WiFiServer *>(server);
      wserver->close();
    }

    bool WifiNetworkService::isOnline() {
      return WiFi.status() == WL_CONNECTED;
    }

    const char *WifiNetworkService::getName() const {
      return NAME;
    }

    const char *WifiNetworkService::getId() const {
      return NAME;
    }

  } // namespace services
} // namespace ah

#endif // ESP8266
