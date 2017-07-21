#ifndef __ARDUINO_HOME_RGB_SERVICE_H__
#define __ARDUINO_HOME_RGB_SERVICE_H__

#include "service.h"

template<typename Data>
class ConfigItem;

struct RGBServiceConfig;
struct DispatcherService;

struct RGBService : public Service {
  explicit RGBService(const int &prpin, const int &pgpin, const int &pbpin, const char *pid = nullptr);
  virtual ~RGBService() = default;

  virtual void init();
  virtual void setup();

  virtual const char *getName() const;
  virtual const char *getId() const;
  virtual const char *getSettings() const;

private:
  void apply();

  const char *id;

  int rpin;
  int gpin;
  int bpin;

  DispatcherService *dispatcher;
  ConfigItem<RGBServiceConfig> *config;
  String settings;
};

#endif // __ARDUINO_HOME_RGB_SERVICE_H__
