#ifndef __ARDUINO_HOME_COMMUNICATION_SERVICE_H__
#define __ARDUINO_HOME_COMMUNICATION_SERVICE_H__

struct CommunicationService : public Service {
  virtual ~CommunicationService() = default;
  
  virtual void init() = 0;
  virtual void setup() = 0;
  virtual void loop() = 0;
};

#endif // __ARDUINO_HOME_COMMUNICATION_SERVICE_H__
