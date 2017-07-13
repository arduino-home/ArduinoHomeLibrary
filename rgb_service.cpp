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
  uint8_t state;
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

RGBService::RGBService(const int &prpin, const int &pgpin, const int &pbpin, const char *pid)
 : id(pid ? pid : "rgb"), rpin(prpin), gpin(pgpin), bpin(pbpin), config(nullptr) {
}

void RGBService::init() {
  config = Runtime::getConfigurationService()->createItem<RGBServiceConfig>();
  
  auto wifiService = static_cast<WifiService*>(Runtime::getCommunicationService());
  
  // have a persistent uri
  String suri = String("/") + id;
  auto uri = strdup(suri.c_str());

  wifiService->on(uri, HTTP_GET, [this](ESP8266WebServer *server) {
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& data = jsonBuffer.createObject();
    data["state"] = config->state;
    data["r"] = config->r;
    data["g"] = config->g;
    data["b"] = config->b;
    String response;
    data.printTo(response); // TODO: avoid string ?
    server->send(200, "application/json", response);
  });
  
  wifiService->on(uri, HTTP_POST, [this](ESP8266WebServer *server) {
    StaticJsonBuffer<200> buffer;
    JsonObject& data = buffer.parseObject(server->arg("plain"));

    if(!data.success()) {
      server->send(400);
      return;
    }

    if(data.containsKey("state")) { config->state = data["state"]; }
    if(data.containsKey("r")) { config->r = data["r"]; }
    if(data.containsKey("g")) { config->g = data["g"]; }
    if(data.containsKey("b")) { config->b = data["b"]; }
    
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
  Debug << id << ": apply state=" << config->state << ", red=" << config->r << ", green=" << config->g << ", blue=" << config->b << endl;

  if(config->state) {
    analogWrite(rpin, config->r);
    analogWrite(gpin, config->g);
    analogWrite(bpin, config->b);
  } else {
    analogWrite(rpin, 0);
    analogWrite(gpin, 0);
    analogWrite(bpin, 0);
  }
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

