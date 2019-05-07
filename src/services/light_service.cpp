#include <Arduino.h>
#include <ArduinoJson.h>

#include "utils/utils.h"
#include "utils/string_stream.h"
#include "runtime.h"
#include "dispatcher_service.h"
#include "configuration_service.h"
#include "light_service.h"

#define NAME "LightService"

#ifndef PWMRANGE
#define PWMRANGE 255
#endif

namespace ah {
  namespace services {

    struct LightServiceConfig {
      bool state;
      uint8_t value;
    };

    #if PWMRANGE != 255
    static inline uint16_t pwmValue(uint8_t value) {
      return static_cast<uint32_t>(value) * PWMRANGE / 255;
    }
    #endif

    LightService::LightService(const int &ppin, const char *pid)
     : id(pid ? pid : "light"), pin(ppin), config(nullptr) {
      utils::StringStream ss(settings);
      ss << "pin=" << pin;
    }

    void LightService::init() {
      dispatcher = Runtime::getDispatcherService();
      config = Runtime::getConfigurationService()->createItem<LightServiceConfig>();

      dispatcher->registerGetter(id, [this](ArduinoJson::JsonVariant &value) {
        JsonObject& data = DispatcherService::sharedBuffer().createObject();
        data["state"] = config->state;
        data["value"] = config->value;

        value = data;
        return true;
      });

      dispatcher->registerSetter(id, [this](const ArduinoJson::JsonVariant &value) {
        if(!value.is<JsonObject>()) {
          return false;
        }

        JsonObject& data = value.as<JsonObject>();

        if(data.containsKey("state")) { config->state = data["state"]; }
        if(data.containsKey("value")) { config->value = data["value"]; }

        apply();
        config->save();

        return true;
      });
    }

    void LightService::setup() {
      // be sure to enable pwm before use
      analogWrite(pin, 1);

      config->load();
      apply();
    }

    void LightService::apply() {
      AH_DEBUG(id << ": apply state=" << config->state << ", value=" << config->value << endl);

      if(config->state) {
    #if PWMRANGE != 255
        analogWrite(pin, pwmValue(config->value));
    #else
        analogWrite(pin, config->value);
    #endif
      } else {
        analogWrite(pin, 0);
      }

      JsonObject& data = DispatcherService::sharedBuffer().createObject();
      data["state"] = config->state;
      data["value"] = config->value;

      dispatcher->notify(id, data);
    }

    const char *LightService::getName() const {
      return NAME;
    }

    const char *LightService::getId() const {
      return id;
    }

    const char *LightService::getSettings() const {
      return settings.c_str();
    }

  } // namespace services
} // namespace ah
