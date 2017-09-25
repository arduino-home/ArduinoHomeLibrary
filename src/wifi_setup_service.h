#ifndef __ARDUINO_HOME_WIFI_SETUP_SERVICE_H__
#define __ARDUINO_HOME_WIFI_SETUP_SERVICE_H__

#ifdef ESP8266

#include "service.h"

struct WifiSetupService : public Service {
  explicit WifiSetupService(const int &pconfigPin);
  virtual ~WifiSetupService() = default;

  virtual void init();
  virtual void setup();

  virtual const char *getName() const;
  virtual const char *getId() const;
  virtual const char *getSettings() const;

private:
  int configPin;
  String settings;
};

#endif // ESP8266

#endif // __ARDUINO_HOME_WIFI_SETUP_SERVICE_H__
