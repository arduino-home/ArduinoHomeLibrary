#ifndef __ARDUINO_HOME_RUNTIME_H__
#define __ARDUINO_HOME_RUNTIME_H__

class Service;
class ConfigurationService;
class CommunicationService;

template<typename Node>
class LinkedList;

struct Runtime {

  static void setName(const char *pname);

  static void registerService(ConfigurationService *service);
  static void registerService(CommunicationService *service);
  static void registerService(Service *service);
  static ConfigurationService* getConfigurationService();
  static CommunicationService* getCommunicationService();

  static void setup();
  static void loop();

  static const char *getName();
  static const char *getVersion();

  static const LinkedList<Service> & getServices();
};

#endif // __ARDUINO_HOME_RUNTIME_H__
