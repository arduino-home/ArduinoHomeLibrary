#ifndef __ARDUINO_HOME_CONFIGURATION_SERVICE_H__
#define __ARDUINO_HOME_CONFIGURATION_SERVICE_H__

struct ConfigurationService : public Service {
  virtual ~ConfigurationService() = default;
  
  virtual void init() = 0;
  virtual void setup() = 0;
  virtual void loop() = 0;
};

#endif // __ARDUINO_HOME_CONFIGURATION_SERVICE_H__
