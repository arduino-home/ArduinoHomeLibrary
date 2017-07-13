#ifndef __ARDUINO_HOME_SERVICE_H__
#define __ARDUINO_HOME_SERVICE_H__

struct Service {
  Service *next;
  
  virtual ~Service() = default;
  
  virtual void init() { }
  virtual void setup() { }
  virtual void loop() { }

  virtual const char *getVersion() const = 0;
  virtual const char *getName() const = 0;
  virtual const char *getId() const = 0;
};

#endif // __ARDUINO_HOME_SERVICE_H__
