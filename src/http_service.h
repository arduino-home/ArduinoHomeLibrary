#ifndef __ARDUINO_HOME_HTTP_SERVICE_H__
#define __ARDUINO_HOME_HTTP_SERVICE_H__

#include "communication_service.h"

class ESP8266WebServer;
enum HTTPMethod;

struct HttpService : public CommunicationService {
  typedef std::function<void(ESP8266WebServer *)> handler_t;

  explicit HttpService(const int &pport);
  virtual ~HttpService() = default;

  virtual void setup();
  virtual void loop();

  virtual const char *getName() const;
  virtual const char *getId() const;
  virtual const char *getSettings() const;

  void on(const char* uri, handler_t handler);
  void on(const char* uri, HTTPMethod method, handler_t handler);

private:
  ESP8266WebServer *server;
  int port;
  String settings;
};

#endif // __ARDUINO_HOME_HTTP_SERVICE_H__
