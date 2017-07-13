#include <ESP8266WiFi.h>          // https://github.com/esp8266/Arduino
#include <EEPROM.h>

//needed for library
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager

#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson

#include "utils.h"
#include "runtime.h"
#include "configuration_service.h"
#include "wifi_service.h"
#include "rgb_service.h"

#define CONFIG_PIN D1

#define RED_PIN D5
#define GREEN_PIN D6
#define BLUE_PIN D7

void setup() {
  Serial.begin(115200);
  Serial.println("\n Starting");

  Runtime::registerService(new ConfigurationService());
  Runtime::registerService(new WifiService(80, CONFIG_PIN));
  Runtime::registerService(new RGBService(RED_PIN, GREEN_PIN, BLUE_PIN));
  
  Runtime::setup();
}

void loop() {
  Runtime::loop();
}

