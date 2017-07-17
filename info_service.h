#ifndef __ARDUINO_HOME_INFO_SERVICE_H__
#define __ARDUINO_HOME_INFO_SERVICE_H__

#include "service.h"

struct InfoService : public Service {
  explicit InfoService() = default;
  virtual ~InfoService() = default;

  virtual void init();
  virtual void setup();

  virtual const char *getName() const;
  virtual const char *getId() const;
};

#endif // __ARDUINO_HOME_INFO_SERVICE_H__
