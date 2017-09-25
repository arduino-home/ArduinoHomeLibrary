#ifndef __ARDUINO_HOME_RUNTIME_H__
#define __ARDUINO_HOME_RUNTIME_H__

class Service;
class ConfigurationService;
class DispatcherService;
class NetworkService;

template<typename Node>
class LinkedList;

struct Runtime {

  static void setName(const char *pname);
  static void setUid(const uint32_t &puid);

  static void registerService(ConfigurationService *service);
  static void registerService(DispatcherService *service);
  static void registerService(NetworkService *service);
  static void registerService(Service *service);

  static ConfigurationService* getConfigurationService();
  static DispatcherService* getDispatcherService();
  static NetworkService* getNetworkService();

  static void setup();
  static void loop();

  static const uint32_t &getUid();
  static const char *getName();
  static const char *getVersion();

  static const LinkedList<Service> & getServices();
};

#endif // __ARDUINO_HOME_RUNTIME_H__
