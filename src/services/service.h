#ifndef __ARDUINO_HOME_SERVICE_H__
#define __ARDUINO_HOME_SERVICE_H__

namespace ah {
  namespace services {

    struct Service {
      Service *next;

      virtual ~Service() = default;

      virtual void init() { }
      virtual void setup() { }
      virtual void loop() { }

      virtual const char *getName() const = 0;
      virtual const char *getId() const = 0;
      virtual const char *getSettings() const { return nullptr; }
    };
  } // namespace services
} // namespace ah


#endif // __ARDUINO_HOME_SERVICE_H__
