#include <Arduino.h>
#include <ArduinoJson.h>

#include "utils/utils.h"
#include "utils/string_stream.h"
#include "runtime.h"
#include "dispatcher_service.h"
#include "configuration_service.h"
#include "rgb_service.h"

#define NAME "RGBService"

#ifndef PWMRANGE
#define PWMRANGE 255
#endif

namespace ah {
  namespace services {

    struct RGBServiceConfig {
      bool state;
      uint8_t r;
      uint8_t g;
      uint8_t b;
    };

    #if PWMRANGE != 255
    static inline uint16_t pwmValue(uint8_t value) {
      return static_cast<uint32_t>(value) * PWMRANGE / 255;
    }
    #endif

    RGBService::RGBService(const int &prpin, const int &pgpin, const int &pbpin, const char *pid)
     : id(pid ? pid : "rgb"), rpin(prpin), gpin(pgpin), bpin(pbpin), config(nullptr) {
      utils::StringStream ss(settings);
      ss << "r=" << rpin << ", g=" << gpin << ", b=" << bpin;
    }

    void RGBService::init() {
      dispatcher = Runtime::getDispatcherService();
      config = Runtime::getConfigurationService()->createItem<RGBServiceConfig>();

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
      // be sure to enable pwm before use
      analogWrite(rpin, 1);
      analogWrite(gpin, 1);
      analogWrite(bpin, 1);

      config->load();
      apply();
    }

    void RGBService::apply() {
      AH_DEBUG(id << ": apply state=" << config->state << ", red=" << config->r << ", green=" << config->g << ", blue=" << config->b << endl);

      if(config->state) {
    #if PWMRANGE != 255
        analogWrite(rpin, pwmValue(config->r));
        analogWrite(gpin, pwmValue(config->g));
        analogWrite(bpin, pwmValue(config->b));
    #else
        analogWrite(rpin, config->r);
        analogWrite(gpin, config->g);
        analogWrite(bpin, config->b);
    #endif
      } else {
        analogWrite(rpin, 0);
        analogWrite(gpin, 0);
        analogWrite(bpin, 0);
      }

      JsonObject& data = DispatcherService::sharedBuffer().createObject();
      data["state"] = config->state;
      data["r"] = config->r;
      data["g"] = config->g;
      data["b"] = config->b;

      dispatcher->notify(id, data);
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

  } // namespace services
} // namespace ah
