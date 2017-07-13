#ifndef __ARDUINO_HOME_CONFIGURATION_SERVICE_H__
#define __ARDUINO_HOME_CONFIGURATION_SERVICE_H__

#include <EEPROM.h>

#include "utils.h"
#include "service.h"

template<typename Data>
class ConfigItem : public Data {

  int offset;

public:

  explicit ConfigItem(const int &off)
   : offset(off) {
  }
    
  void load() {
    Debug << "config load at " << offset << endl;
    EEPROM.get(offset, *static_cast<Data*>(this));
  }
  
  void save() const {
    Debug << "config save at " << offset << endl;
    EEPROM.put(offset, *static_cast<const Data*>(this));
    EEPROM.commit();
  }

  int size() const {
    return sizeof(Data);
  }
};

struct ConfigurationService : public Service {
  explicit ConfigurationService();
  virtual ~ConfigurationService() = default;
  
  virtual void setup();

  virtual const char *getVersion() const;
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

#endif // __ARDUINO_HOME_CONFIGURATION_SERVICE_H__
