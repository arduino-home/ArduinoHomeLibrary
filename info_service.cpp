#include <ESP8266WiFi.h>          // https://github.com/esp8266/Arduino
#include <ESP8266WebServer.h>
#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson

#include "utils.h"
#include "list.h"
#include "runtime.h"
#include "wifi_service.h"
#include "info_service.h"

#define NAME "InfoService"

void InfoService::init() {

  auto wifiService = static_cast<WifiService*>(Runtime::getCommunicationService());

  wifiService->on("/info", HTTP_GET, [this](ESP8266WebServer *server) {
    const auto &services = Runtime::getServices();
    StaticJsonBuffer<1024> jsonBuffer;
    JsonArray& list = jsonBuffer.createArray();

    JsonObject& rtitem = list.createNestedObject();
    rtitem["id"] = "Runtime";
    rtitem["name"] = "Runtime";
    rtitem["settings"] = String("version=") + Runtime::getVersion();

    for(const auto *service : services) {
      JsonObject& item = list.createNestedObject();
      item["id"] = service->getId();
      item["name"] = service->getName();
      const auto *settings = service->getSettings();
      if(settings) {
        item["settings"] = settings;
      }
    }

    String response;
    list.printTo(response); // TODO: avoid string ?
    server->send(200, "application/json", response);
  });
}

const char *InfoService::getName() const { 
  return NAME;
}

const char *InfoService::getId() const {
  return NAME;
}

