#include <Arduino.h>
#include <ArduinoJson.h>

#include "utils/utils.h"
#include "utils/string_stream.h"
#include "runtime.h"
#include "dispatcher_service.h"
#include "configuration_service.h"
#include "relay_service.h"

#define NAME "RelayService"

namespace ah {
  namespace services {

    struct RelayServiceConfig {
      bool state;
      int on_value;
    };

    RelayService::RelayService(const int &ppin, const int &pon_value, const char *pid)
     : id(pid ? pid : "relay"), pin(ppin), on_value(pon_value), config(nullptr) {
      utils::StringStream ss(settings);
      ss << "pin=" << pin << "on_value=" << on_value;
    }

    void RelayService::init() {
      dispatcher = Runtime::getDispatcherService();
      config = Runtime::getConfigurationService()->createItem<RelayServiceConfig>();

      dispatcher->registerGetter(id, [this](ArduinoJson::JsonVariant &value) {
        JsonObject& data = DispatcherService::sharedBuffer().createObject();
        data["state"] = config->state;

        value = data;
        return true;
      });

      dispatcher->registerSetter(id, [this](const ArduinoJson::JsonVariant &value) {
        if(!value.is<JsonObject>()) {
          return false;
        }

        JsonObject& data = value.as<JsonObject>();

        if(data.containsKey("state")) { config->state = data["state"]; }

        apply();
        config->save();

        return true;
      });
    }

    void RelayService::setup() {
      // be sure to enable pwm before use
      analogWrite(pin, 1);

      config->load();
      apply();
    }

    void RelayService::apply() {
      AH_DEBUG(id << ": apply state=" << config->state << endl);

      if(config->state) {
        digitalWrite(pin, on_value);
      } else {
        digitalWrite(pin, on_value == HIGH ? LOW : HIGH);
      }

      JsonObject& data = DispatcherService::sharedBuffer().createObject();
      data["state"] = config->state;

      dispatcher->notify(id, data);
    }

    const char *RelayService::getName() const {
      return NAME;
    }

    const char *RelayService::getId() const {
      return id;
    }

    const char *RelayService::getSettings() const {
      return settings.c_str();
    }

  } // namespace services
} // namespace ah
