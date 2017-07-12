#ifndef __ARDUINO_HOME_SERVICE_H__
#define __ARDUINO_HOME_SERVICE_H__

struct Service {
  Service *next;
  
  virtual ~Service() = default;
  
  virtual void init() = 0;
  virtual void setup() = 0;
  virtual void loop() = 0;
};

#endif // __ARDUINO_HOME_SERVICE_H__
