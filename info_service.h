#ifndef __ARDUINO_HOME_INFO_SERVICE_H__
#define __ARDUINO_HOME_INFO_SERVICE_H__

#include "service.h"

struct InfoService : public Service {
  explicit InfoService(const char *pid = nullptr);
  virtual ~InfoService() = default;

  virtual void init();

  virtual const char *getVersion() const;
  virtual const char *getName() const;
  virtual const char *getId() const;

private:
  const char *id;
};

#endif // __ARDUINO_HOME_INFO_SERVICE_H__
