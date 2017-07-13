#include <ESP8266WiFi.h>          // https://github.com/esp8266/Arduino
#include <ESP8266WebServer.h>
#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson

#include "utils.h"
#include "list.h"
#include "runtime.h"
#include "wifi_service.h"
#include "info_service.h"

#define VERSION "1.0.0"
#define NAME "InfoService"

InfoService::InfoService(const char *pid)
 : id(pid ? pid : "info") {
}

void InfoService::init() {

  auto wifiService = static_cast<WifiService*>(Runtime::getCommunicationService());
  
  // have a persistent uri
  String suri = String("/") + id;
  auto uri = strdup(suri.c_str());

  wifiService->on(uri, HTTP_GET, [this](ESP8266WebServer *server) {
    const auto &services = Runtime::getServices();
    StaticJsonBuffer<512> jsonBuffer;
    JsonArray& list = jsonBuffer.createArray();

    for(const auto *service : services) {
      JsonObject& item = list.createNestedObject();
      item["id"] = service->getId();
      item["name"] = service->getName();
      item["version"] = service->getVersion();
    }

    String response;
    list.printTo(response); // TODO: avoid string ?
    server->send(200, "application/json", response);
  });
}

const char *InfoService::getVersion() const {
  return VERSION;
}

const char *InfoService::getName() const { 
  return NAME;
}

const char *InfoService::getId() const {
  return id;
}

