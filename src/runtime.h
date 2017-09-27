#ifndef __ARDUINO_HOME_RUNTIME_H__
#define __ARDUINO_HOME_RUNTIME_H__

namespace ah {

  namespace utils {
    template<typename Node>
    class LinkedList;
  } // namespace utils

  namespace services {
    class Service;
    class ConfigurationService;
    class DispatcherService;
    class NetworkService;
  } // namespace services

  struct Runtime {

    static void setName(const char *pname);
    static void setUid(const uint32_t &puid);

    static void registerService(services::ConfigurationService *service);
    static void registerService(services::DispatcherService *service);
    static void registerService(services::NetworkService *service);
    static void registerService(services::Service *service);

    static services::ConfigurationService* getConfigurationService();
    static services::DispatcherService* getDispatcherService();
    static services::NetworkService* getNetworkService();

    static void setup();
    static void loop();

    static const uint32_t &getUid();
    static const char *getName();
    static const char *getVersion();

    static const utils::LinkedList<services::Service> & getServices();
  };

} // namespace ah

#endif // __ARDUINO_HOME_RUNTIME_H__
