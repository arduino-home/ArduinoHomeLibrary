#ifndef __ARDUINO_HOME_SERVICE_H__
#define __ARDUINO_HOME_SERVICE_H__

struct Service {
  Service *next;
  
  virtual ~Service() = default;
  
  virtual void init() { }
  virtual void setup() { }
  virtual void loop() { }
};

#endif // __ARDUINO_HOME_SERVICE_H__
