#ifdef ESP8266

#include <ESP8266WiFi.h> // https://github.com/esp8266/Arduino
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

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
