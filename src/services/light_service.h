#ifndef __ARDUINO_HOME_LIGHT_SERVICE_H__
#define __ARDUINO_HOME_LIGHT_SERVICE_H__

#include "service.h"

namespace ah {
  namespace services {

    template<typename Data>
    class ConfigItem;

    struct LightServiceConfig;
    struct DispatcherService;

    struct LightService : public Service {
      explicit LightService(const int &ppin, const char *pid = nullptr);
      virtual ~LightService() = default;

      virtual void init();
      virtual void setup();

      virtual const char *getName() const;
      virtual const char *getId() const;
      virtual const char *getSettings() const;

    private:
      void apply();

      const char *id;

      int pin;

      DispatcherService *dispatcher;
      ConfigItem<LightServiceConfig> *config;
      String settings;
    };

  } // namespace services
} // namespace ah

#endif // __ARDUINO_HOME_LIGHT_SERVICE_H__
