#include <Arduino.h>
#include <ArduinoJson.h>

#include "utils.h"
#include "list.h"
#include "runtime.h"
#include "dispatcher_service.h"
#include "info_service.h"

#define NAME "InfoService"

namespace ah {
  namespace services {

    void InfoService::init() {
      auto dispatcher = Runtime::getDispatcherService();

      dispatcher->registerGetter("info", [this](ArduinoJson::JsonVariant &value) {
        const auto &services = Runtime::getServices();
        JsonArray& list = DispatcherService::sharedBuffer().createArray();

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

        value = list;
        return true;
      });
    }

    void InfoService::setup() {
      const auto &services = Runtime::getServices();

      AH_DEBUG("Runtime: name=" << Runtime::getName() << ", version=" << Runtime::getVersion() << endl);

      for(const auto *service : services) {
        AH_DEBUG(service->getName() << ": id=" << service->getId());
        const auto *settings = service->getSettings();
        if(settings) {
          AH_DEBUG(", " << settings);
        }
        AH_DEBUG(endl);
      }
    }

    const char *InfoService::getName() const {
      return NAME;
    }

    const char *InfoService::getId() const {
      return NAME;
    }

  } // namespace services
} // namespace ah
