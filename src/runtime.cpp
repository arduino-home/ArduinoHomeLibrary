#include <Arduino.h>

#include "utils/utils.h"
#include "utils/list.h"
#include "runtime.h"
#include "services/service.h"
#include "services/configuration_service.h"
#include "services/dispatcher_service.h"
#include "services/network_service.h"

#define VERSION "1.0.9"

namespace ah {

  static const char *name = "Unamed";
  static uint32_t uid = 0;

  static utils::LinkedList<services::Service> serviceList;
  static services::ConfigurationService *configService = nullptr;
  static services::DispatcherService *dispService = nullptr;
  static services::NetworkService *netService = nullptr;

  void Runtime::setName(const char *pname) {
    name = pname;
  }

  void Runtime::setUid(const uint32_t &puid) {
    uid = puid;
  }

  void Runtime::registerService(services::ConfigurationService *service) {
    AH_ASSERT(!configService, "configuration service registered twice");

    configService = service;
    registerService(static_cast<services::Service *>(service));
  }

  void Runtime::registerService(services::DispatcherService *service) {
    AH_ASSERT(!dispService, "dispatcher service registered twice");

    dispService = service;
    registerService(static_cast<services::Service *>(service));
  }

  void Runtime::registerService(services::NetworkService *service) {
    AH_ASSERT(!netService, "network service registered twice");

    netService = service;
    registerService(static_cast<services::Service *>(service));
  }

  void Runtime::registerService(services::Service *service) {
    serviceList.add(service);
    service->init();
  }

  services::ConfigurationService* Runtime::getConfigurationService() {
    AH_ASSERT(configService, "configuration service not registered");
    return configService;
  }

  services::DispatcherService* Runtime::getDispatcherService() {
    AH_ASSERT(dispService, "dispatcher service not registered");
    return dispService;
  }

  services::NetworkService* Runtime::getNetworkService() {
    AH_ASSERT(netService, "network service not registered");
    return netService;
  }

  void Runtime::setup() {
    for(auto service : serviceList) {
      service->setup();
    }
  }

  void Runtime::loop() {
    for(auto service : serviceList) {
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

  const utils::LinkedList<services::Service> & Runtime::getServices() {
    return serviceList;
  }

} // namespace ah

