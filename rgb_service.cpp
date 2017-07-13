#include <ESP8266WiFi.h>          // https://github.com/esp8266/Arduino
#include <ESP8266WebServer.h>
#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson

#include "utils.h"
#include "runtime.h"
#include "wifi_service.h"
#include "configuration_service.h"
#include "rgb_service.h"

#define VERSION "1.0.0"
#define NAME "RGBService"

struct RGBServiceConfig {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

RGBService::RGBService(const char *pid, const int &prpin, const int &pgpin, const int &pbpin)
 : id(pid), rpin(prpin), gpin(pgpin), bpin(pbpin), config(nullptr) {
}

void RGBService::init() {
  config = Runtime::getConfigurationService()->createItem<RGBServiceConfig>();
  
  auto wifiService = static_cast<WifiService*>(Runtime::getCommunicationService());
  
  // have a persistent uri
  String suri = String("/") + id;
  auto uri = strdup(suri.c_str());

  wifiService->on(uri, HTTP_GET, [this](ESP8266WebServer *server) {
    server->send(200, "application/json", "{ \"r\": " + String(config->r) + ", \"g\": " + String(config->g) + ", \"b\": " + String(config->b) + " }");
  });
  
  wifiService->on(uri, HTTP_POST, [this](ESP8266WebServer *server) {
    StaticJsonBuffer<200> buffer;
    JsonObject& data = buffer.parseObject(server->arg("plain"));

    if(!data.success()) {
      server->send(400);
      return;
    }
    
    config->r = data["r"];
    config->g = data["g"];
    config->b = data["b"];
    
    apply();
    config->save();

    server->send(200);
  });
}

void RGBService::setup() {
  config->load();
  apply();
}

void RGBService::apply() {
  Debug << id << ": apply red=" << config->r << ", green=" << config->g << ", blue=" << config->b << endl;
  
  analogWrite(rpin, config->r);
  analogWrite(gpin, config->g);
  analogWrite(bpin, config->b);
}

const char *RGBService::getVersion() const {
  return VERSION;
}

const char *RGBService::getName() const { 
  return NAME;
}

const char *RGBService::getId() const {
  return id;
}

