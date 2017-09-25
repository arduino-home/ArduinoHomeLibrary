#include <Arduino.h>

#include "utils.h"
#include "list.h"
#include "runtime.h"
#include "service.h"
#include "configuration_service.h"
#include "dispatcher_service.h"
#include "network_service.h"

#define VERSION "1.0.6"

static const char *name = "Unamed";
static uint32_t uid = 0;

static LinkedList<Service> services;
static ConfigurationService *configService = nullptr;
static DispatcherService *dispService = nullptr;
static NetworkService *netService = nullptr;

void Runtime::setName(const char *pname) {
  name = pname;
}

void Runtime::setUid(const uint32_t &puid) {
  uid = puid;
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

void Runtime::registerService(NetworkService *service) {
  AH_ASSERT(!netService, "network service registered twice");

  netService = service;
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

NetworkService* Runtime::getNetworkService() {
  AH_ASSERT(netService, "network service not registered");
  return netService;
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

const uint32_t &Runtime::getUid() {
  AH_ASSERT(uid, "uid not initialized");
  return uid;
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

