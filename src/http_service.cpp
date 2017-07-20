#include <ESP8266WiFi.h>          // https://github.com/esp8266/Arduino

//needed for library
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager

#include "utils.h"
#include "string_stream.h"
#include "wifi_service.h"
#include "runtime.h"

#define NAME "HttpService"

HttpService::HttpService(const int &pport, const int &pconfigPin)
 : server(new ESP8266WebServer(port)), port(pport) {
  StringStream ss(settings);
  ss << "port=" << port;
}

void HttpService::setup() {
  server->begin();
}

void HttpService::loop() {
  server->handleClient();
}

void HttpService::on(const char* uri, handler_t handler) {
  server->on(uri, [this, handler]() { handler(server); });
}

void HttpService::on(const char* uri, HTTPMethod method, handler_t handler) {
  server->on(uri, method, [this, handler]() { handler(server); });
}

const char *HttpService::getName() const {
  return NAME;
}

const char *HttpService::getId() const {
  return NAME;
}

const char *HttpService::getSettings() const {
  return settings.c_str();
}

