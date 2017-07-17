#include <ESP8266WiFi.h>          // https://github.com/esp8266/Arduino

//needed for library
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager

#include "utils.h"
#include "string_stream.h"
#include "wifi_service.h"
#include "runtime.h"

#define NAME "WifiService"

static inline void configWifi() {
  WiFiManager wifiManager;
  wifiManager.setDebugOutput(true);

  String name = String(Runtime::getName()) + "-" + String(ESP.getChipId());
  if (!wifiManager.startConfigPortal(name.c_str())) {
    Debug << "failed to connect and hit timeout" << endl;
    panic();
  }

  Debug << "connected" << endl;
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

  Debug << WiFi.SSID() << " " << wifiStatusToString();
  if(oldStatus == WL_CONNECTED) { Debug << " " << WiFi.localIP(); }
  Debug << endl;
}

WifiService::WifiService(const int &pport, const int &pconfigPin)
 : server(nullptr), port(pport), configPin(pconfigPin) {
  StringStream ss(settings);
  ss << "port=" << port << ", configPin=" << configPin;
}

void WifiService::init() {
  pinMode(configPin, INPUT_PULLUP);

  if(isConfigRequested(configPin)) {
    configWifi();
    ESP.reset();
  }

  // build the server after wifi management
  server = new ESP8266WebServer(port);
}

void WifiService::setup() {
  WiFi.begin();
  server->begin();
}

void WifiService::loop() {
  debugWifiStatus();

  server->handleClient();
}

void WifiService::on(const char* uri, handler_t handler) {
  server->on(uri, [this, handler]() { handler(server); });
}

void WifiService::on(const char* uri, HTTPMethod method, handler_t handler) {
  server->on(uri, method, [this, handler]() { handler(server); });
}

const char *WifiService::getName() const { 
  return NAME;
}

const char *WifiService::getId() const {
  return NAME;
}

const char *WifiService::getSettings() const {
  return settings.c_str();
}

