#ifndef __ARDUINO_HOME_SYSTEM_H__
#define __ARDUINO_HOME_SYSTEM_H__

#ifdef ESP8266

#include <ESP8266WiFi.h> // https://github.com/esp8266/Arduino

#else // ESP8266

#error "unsupported board"

#endif

#endif // __ARDUINO_HOME_SYSTEM_H__
