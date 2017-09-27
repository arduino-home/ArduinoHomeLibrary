#ifndef __ARDUINO_HOME_HTTP_SERVICE_H__
#define __ARDUINO_HOME_HTTP_SERVICE_H__

#include "service.h"

class Client;
class Server;

namespace ah {
  namespace web {
    class WebServer;
  } // namespace web

  namespace services {

    struct HttpService : public Service {

      explicit HttpService(const int &pport = 80);
      virtual ~HttpService() = default;

      virtual void setup();
      virtual void loop();

      virtual const char *getName() const;
      virtual const char *getId() const;
      virtual const char *getSettings() const;

    private:
      String settings;
      web::WebServer *server;
    };

  } // namespace services
} // namespace ah

#endif // __ARDUINO_HOME_HTTP_SERVICE_H__
