#include <ESP8266WiFi.h>          // https://github.com/esp8266/Arduino

#include "utils.h"
#include "list.h"
#include "runtime.h"
#include "service.h"
#include "configuration_service.h"
#include "communication_service.h"

#define VERSION "1.0.0"

static LinkedList<Service> services;
static ConfigurationService *configService = nullptr;
static CommunicationService *commService = nullptr;

void Runtime::registerService(ConfigurationService *service) {
  if(configService) {
    Debug << "configuration service registered twice" << endl;
    panic();
  }
  
  configService = service;
  registerService(static_cast<Service *>(service));
}

void Runtime::registerService(CommunicationService *service) {
  if(commService) {
    Debug << "communication service registered twice" << endl;
    panic();
  }
  
  commService = service;
  registerService(static_cast<Service *>(service));
}

void Runtime::registerService(Service *service) {
  services.add(service);
  service->init();
}

ConfigurationService* Runtime::getConfigurationService() {
  if(!configService) {
    Debug << "configuration service not registered" << endl;
    panic();
  }

  return configService;
}

CommunicationService* Runtime::getCommunicationService() {
  if(!commService) {
    Debug << "communication service not registered" << endl;
    panic();
  }

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

const char *Runtime::getVersion() {
  return VERSION;
}

