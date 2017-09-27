#ifndef __ARDUINO_HOME_DISPATCHER_SERVICE_H__
#define __ARDUINO_HOME_DISPATCHER_SERVICE_H__

#include <functional>
#include <ArduinoJson.h>
#include "service.h"
#include "list.h"

namespace ah {
  namespace services {

    namespace internal {
      struct DispatcherGetterNode;
      struct DispatcherSetterNode;
      struct DispatcherNotifierNode;
    } // namespace internal

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
      utils::LinkedList<internal::DispatcherGetterNode> getters;
      utils::LinkedList<internal::DispatcherSetterNode> setters;
      utils::LinkedList<internal::DispatcherNotifierNode> notifiers;
    };

  } // namespace services
} // namespace ah

#endif // __ARDUINO_HOME_DISPATCHER_SERVICE_H__
