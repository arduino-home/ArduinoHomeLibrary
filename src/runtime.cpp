#include <ESP8266WiFi.h>          // https://github.com/esp8266/Arduino

#include "utils.h"
#include "list.h"
#include "runtime.h"
#include "service.h"
#include "configuration_service.h"
#include "dispatcher_service.h"

#define VERSION "1.0.5"

static const char *name = "Unamed";
static LinkedList<Service> services;
static ConfigurationService *configService = nullptr;
static DispatcherService *dispService = nullptr;

void Runtime::setName(const char *pname) {
  name = pname;
}

void Runtime::registerService(ConfigurationService *service) {
  AH_ASSERT(!configService, "configuration service registered twice");

  configService = service;
  registerService(static_cast<Service *>(service));
}

void Runtime::registerService(DispatcherService *service) {
  AH_ASSERT(!dispService, "dispatcher service registered twice");

  dispService = service;
  registerService(static_cast<Service *>(service));
}

void Runtime::registerService(Service *service) {
  services.add(service);
  service->init();
}

ConfigurationService* Runtime::getConfigurationService() {
  AH_ASSERT(configService, "configuration service not registered");
  return configService;
}

DispatcherService* Runtime::getDispatcherService() {
  AH_ASSERT(dispService, "dispatcher service not registered");
  return dispService;
}

void Runtime::setup() {
  for(auto service : services) {
    service->setup();
  }
}

void Runtime::loop() {
  for(auto service : services) {
    service->loop();
  }
}

const char *Runtime::getName() {
  return name;
}

const char *Runtime::getVersion() {
  return VERSION;
}

const LinkedList<Service> & Runtime::getServices() {
  return services;
}

