#ifndef __ARDUINO_HOME_WIFI_SERVICE_H__
#define __ARDUINO_HOME_WIFI_SERVICE_H__

#include "communication_service.h"

class ESP8266WebServer;
enum HTTPMethod;

struct WifiService : public CommunicationService {
  typedef std::function<void(ESP8266WebServer *)> handler_t;
  
  explicit WifiService(const int &pport, const int &pconfigPin);
  virtual ~WifiService() = default;

  virtual void init();
  virtual void setup();
  virtual void loop();

  virtual const char *getVersion() const;
  virtual const char *getName() const;
  virtual const char *getId() const;

  void on(const char* uri, handler_t handler);
  void on(const char* uri, HTTPMethod method, handler_t handler);
  
private:
  ESP8266WebServer *server;
  int port;
  int configPin;
};

#endif // __ARDUINO_HOME_WIFI_SERVICE_H__
