#ifdef ESP8266

#include "system.h"

//needed for library
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager

#include "utils.h"
#include "string_stream.h"
#include "wifi_setup_service.h"
#include "runtime.h"

#define NAME "WifiSetupService"

static inline void configWifi() {
  WiFiManager wifiManager;
  wifiManager.setDebugOutput(true);

  String name = String(Runtime::getName()) + "-" + String(ESP.getChipId());
  if (!wifiManager.startConfigPortal(name.c_str())) {
    AH_DEBUG("failed to connect and hit timeout" << endl);
    panic();
  }

  AH_DEBUG("connected" << endl);
}

static inline bool isConfigRequested(const int &pin) {
  return digitalRead(pin) == LOW;
}

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

WifiSetupService::WifiSetupService(const int &pconfigPin)
 : configPin(pconfigPin) {
  StringStream ss(settings);
  ss << "configPin=" << configPin;
}

void WifiSetupService::init() {
  pinMode(configPin, INPUT_PULLUP);

  if(isConfigRequested(configPin)) {
    configWifi();
    ESP.reset();
  }
}

void WifiSetupService::setup() {
  WiFi.mode(WIFI_STA);
  WiFi.begin();
}

void WifiSetupService::loop() {
  debugWifiStatus();
}

const char *WifiSetupService::getName() const {
  return NAME;
}

const char *WifiSetupService::getId() const {
  return NAME;
}

const char *WifiSetupService::getSettings() const {
  return settings.c_str();
}

#endif // ESP8266
