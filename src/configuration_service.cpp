#include <ESP8266WiFi.h>          // https://github.com/esp8266/Arduino
#include <EEPROM.h>

#include "utils.h"
#include "configuration_service.h"

#define NAME "ConfigurationService"

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

