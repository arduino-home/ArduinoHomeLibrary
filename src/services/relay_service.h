#ifndef __ARDUINO_HOME_RELAY_SERVICE_H__
#define __ARDUINO_HOME_RELAY_SERVICE_H__

#include "service.h"

namespace ah {
  namespace services {

    template<typename Data>
    class ConfigItem;

    struct RelayServiceConfig;
    struct DispatcherService;

    struct RelayService : public Service {
      explicit RelayService(const int &ppin, const int &pon_value, const char *pid = nullptr);
      virtual ~RelayService() = default;

      virtual void init();
      virtual void setup();

      virtual const char *getName() const;
      virtual const char *getId() const;
      virtual const char *getSettings() const;

    private:
      void apply();

      const char *id;

      int pin;
      int on_value;

      DispatcherService *dispatcher;
      ConfigItem<RelayServiceConfig> *config;
      String settings;
    };

  } // namespace services
} // namespace ah

#endif // __ARDUINO_HOME_RELAY_SERVICE_H__
