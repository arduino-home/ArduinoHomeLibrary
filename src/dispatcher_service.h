#ifndef __ARDUINO_HOME_DISPATCHER_SERVICE_H__
#define __ARDUINO_HOME_DISPATCHER_SERVICE_H__

#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson
#include "service.h"
#include "list.h"

struct DispatcherGetterNode;
struct DispatcherSetterNode;
struct DispatcherNotifierNode;

struct DispatcherService : public Service {
  typedef std::function<bool(ArduinoJson::JsonVariant &)> getter_t;
  typedef std::function<bool(const ArduinoJson::JsonVariant &)> setter_t;
  typedef std::function<void(const String &, const ArduinoJson::JsonVariant &)> notifier_t;
  enum HandlerResult { success, not_found, handler_error };

  virtual ~DispatcherService() = default;

  virtual const char *getName() const;
  virtual const char *getId() const;

  void registerGetter(const String &id, getter_t getter);
  void registerSetter(const String &id, setter_t setter);
  void registerNotifier(notifier_t notifier);

  HandlerResult get(const String &id, ArduinoJson::JsonVariant &value) const;
  HandlerResult set(const String &id, const ArduinoJson::JsonVariant &value) const;
  void notify(const String &id, const ArduinoJson::JsonVariant &value) const;

  bool hasGetter(const String &id) const;
  bool hasSetter(const String &id) const;

  static StaticJsonBuffer<1024> &sharedBuffer();

private:
  LinkedList<DispatcherGetterNode> getters;
  LinkedList<DispatcherSetterNode> setters;
  LinkedList<DispatcherNotifierNode> notifiers;
};

#endif // __ARDUINO_HOME_DISPATCHER_SERVICE_H__
