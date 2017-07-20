#include <ESP8266WiFi.h>          // https://github.com/esp8266/Arduino
#include <ESP8266WebServer.h>
#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson

#include "utils.h"
#include "list.h"
#include "dispatcher_service.h"

#define NAME "DispatcherService"

struct DispatcherGetterNode {
  DispatcherGetterNode *next;
  String id;
  DispatcherService::getter_t getter;
};

struct DispatcherSetterNode {
  DispatcherSetterNode *next;
  String id;
  DispatcherService::setter_t setter;
};

struct DispatcherNotifierNode {
  DispatcherNotifierNode *next;
  DispatcherService::notifier_t notifier;
};

static StaticJsonBuffer<1024> *_sharedBuffer = nullptr;

StaticJsonBuffer<1024> &DispatcherService::sharedBuffer() {
  if(_sharedBuffer) {
    delete _sharedBuffer;
  }
  _sharedBuffer = new StaticJsonBuffer<1024>();
  return *_sharedBuffer;
}

const char *DispatcherService::getName() const {
  return NAME;
}

const char *DispatcherService::getId() const {
  return NAME;
}

void DispatcherService::registerGetter(const String &id, getter_t getter) {
  auto node = new DispatcherGetterNode();
  node->id = id;
  node->getter = getter;
  getters.add(node);
}

void DispatcherService::registerSetter(const String &id, setter_t setter) {
  auto node = new DispatcherSetterNode();
  node->id = id;
  node->setter = setter;
  setters.add(node);
}

void DispatcherService::registerNotifier(notifier_t notifier) {
  auto node = new DispatcherNotifierNode();
  node->notifier = notifier;
  notifiers.add(node);
}

DispatcherService::HandlerResult DispatcherService::get(const String &id, ArduinoJson::JsonVariant &buffer) const {
  for(const auto &node : getters) {
    if(node->id == id) {
      return node->getter(buffer) ? HandlerResult::success : HandlerResult::handler_error;
    }
  }
  return HandlerResult::not_found;
}

DispatcherService::HandlerResult DispatcherService::set(const String &id, const ArduinoJson::JsonVariant &value) const {
  for(const auto &node : setters) {
    if(node->id == id) {
      return node->setter(value) ? HandlerResult::success : HandlerResult::handler_error;
    }
  }
  return HandlerResult::not_found;
}

void DispatcherService::notify(const String &id, const ArduinoJson::JsonVariant &buffer) const {
  for(const auto &node : notifiers) {
    node->notifier(id, buffer);
  }
}

bool DispatcherService::hasGetter(const String &id) const {
  for(const auto &node : getters) {
    Serial << "getter id=" << node->id << endl;
    if(node->id == id) {
      return true;
    }
  }
  return false;
}

bool DispatcherService::hasSetter(const String &id) const {
  for(const auto &node : setters) {
    if(node->id == id) {
      return true;
    }
  }
  return false;
}
