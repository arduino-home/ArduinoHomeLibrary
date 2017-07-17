#include <ESP8266WiFi.h>          // https://github.com/esp8266/Arduino

#include "utils.h"
#include "list.h"
#include "runtime.h"
#include "service.h"
#include "configuration_service.h"
#include "communication_service.h"

#define VERSION "1.0.0"

static const char *name = "Unamed";
static LinkedList<Service> services;
static ConfigurationService *configService = nullptr;
static CommunicationService *commService = nullptr;

void Runtime::setName(const char *pname) {
  name = pname;
}

void Runtime::registerService(ConfigurationService *service) {
  AH_ASSERT(!configService, "configuration service registered twice");
  
  configService = service;
  registerService(static_cast<Service *>(service));
}

void Runtime::registerService(CommunicationService *service) {
  AH_ASSERT(!commService, "communication service registered twice");
  
  commService = service;
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

CommunicationService* Runtime::getCommunicationService() {
  AH_ASSERT(commService, "communication service not registered");
  return commService;
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

