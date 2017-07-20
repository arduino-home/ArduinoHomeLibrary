#include <ESP8266WiFi.h>          // https://github.com/esp8266/Arduino
#include <ESP8266WebServer.h>
#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson

#include "utils.h"
#include "string_stream.h"
#include "runtime.h"
#include "dispatcher_service.h"
#include "configuration_service.h"
#include "rgb_service.h"

#define NAME "RGBService"

struct RGBServiceConfig {
  uint8_t state;
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

RGBService::RGBService(const int &prpin, const int &pgpin, const int &pbpin, const char *pid)
 : id(pid ? pid : "rgb"), rpin(prpin), gpin(pgpin), bpin(pbpin), config(nullptr) {
  StringStream ss(settings);
  ss << "r=" << rpin << ", g=" << gpin << ", b=" << bpin;
}

void RGBService::init() {
  config = Runtime::getConfigurationService()->createItem<RGBServiceConfig>();
  auto dispatcher = Runtime::getDispatcherService();

  dispatcher->registerGetter(id, [this](ArduinoJson::JsonVariant &value) {
    JsonObject& data = DispatcherService::sharedBuffer().createObject();
    data["state"] = config->state;
    data["r"] = config->r;
    data["g"] = config->g;
    data["b"] = config->b;

    value = data;
    return true;
  });

  dispatcher->registerSetter(id, [this](const ArduinoJson::JsonVariant &value) {
    if(!value.is<JsonObject>()) {
      return false;
    }

    JsonObject& data = value.as<JsonObject>();

    if(data.containsKey("state")) { config->state = data["state"]; }
    if(data.containsKey("r")) { config->r = data["r"]; }
    if(data.containsKey("g")) { config->g = data["g"]; }
    if(data.containsKey("b")) { config->b = data["b"]; }

    apply();
    config->save();

    return true;
  });
}

void RGBService::setup() {
  config->load();
  apply();
}

void RGBService::apply() {
  AH_DEBUG(id << ": apply state=" << config->state << "red=" << config->r << ", green=" << config->g << ", blue=" << config->b << endl);

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

const char *RGBService::getName() const {
  return NAME;
}

const char *RGBService::getId() const {
  return id;
}

const char *RGBService::getSettings() const {
  return settings.c_str();
}

