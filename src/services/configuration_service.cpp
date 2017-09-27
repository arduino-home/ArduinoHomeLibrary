#include <Arduino.h>
#include <EEPROM.h>

#include "utils/utils.h"
#include "configuration_service.h"

#define NAME "ConfigurationService"

namespace ah {
  namespace services {

    ConfigurationService::ConfigurationService()
     : size(0) {
    }

    void ConfigurationService::setup() {
      EEPROM.begin(size);
    }

    const char *ConfigurationService::getName() const {
      return NAME;
    }

    const char *ConfigurationService::getId() const {
      return NAME;
    }

  } // namespace services
} // namespace ah
