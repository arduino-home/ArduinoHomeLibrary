#ifndef __ARDUINO_HOME_CONFIGURATION_SERVICE_H__
#define __ARDUINO_HOME_CONFIGURATION_SERVICE_H__

#include <EEPROM.h>

#include "utils/utils.h"
#include "service.h"

namespace ah {
  namespace services {

    template<typename Data>
    class ConfigItem : public Data {

      int offset;

    public:

      explicit ConfigItem(const int &off)
       : offset(off) {
      }

      void load() {
        AH_DEBUG("config load at " << offset << endl);
        EEPROM.get(offset, *static_cast<Data*>(this));
      }

      void save() const {
        AH_DEBUG("config save at " << offset << endl);
        EEPROM.put(offset, *static_cast<const Data*>(this));
#ifdef ESP8266
        EEPROM.commit();
#endif
      }

      int size() const {
        return sizeof(Data);
      }
    };

    struct ConfigurationService : public Service {
      explicit ConfigurationService();
      virtual ~ConfigurationService() = default;

      virtual void setup();

      virtual const char *getName() const;
      virtual const char *getId() const;

      template<typename Data>
      ConfigItem<Data>* createItem() {
        ConfigItem<Data>* item = new ConfigItem<Data>(size);
        size += item->size();
        return item;
      }

    private:
      int size;
    };

  } // namespace services
} // namespace ah


#endif // __ARDUINO_HOME_CONFIGURATION_SERVICE_H__
