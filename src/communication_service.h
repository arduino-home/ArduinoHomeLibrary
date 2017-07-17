#ifndef __ARDUINO_HOME_COMMUNICATION_SERVICE_H__
#define __ARDUINO_HOME_COMMUNICATION_SERVICE_H__

#include "service.h"

struct CommunicationService : public Service {
  virtual ~CommunicationService() = default;
};

#endif // __ARDUINO_HOME_COMMUNICATION_SERVICE_H__
