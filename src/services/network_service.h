#ifndef __ARDUINO_HOME_NETWORK_SERVICE_H__
#define __ARDUINO_HOME_NETWORK_SERVICE_H__

#include "Client.h"
#include "Server.h"

#include "service.h"

namespace ah {
  namespace services {

    struct NetworkService : public Service {
      virtual ~NetworkService() = default;

      virtual Server *createServer(const int &port) = 0;
      virtual Client *createClient() = 0;
      virtual Client *serverAvailable(Server *server) = 0;
      virtual void serverClose(Server *server) = 0;

      virtual bool isOnline() = 0;
    };

  } // namespace services
} // namespace ah


#endif // __ARDUINO_HOME_NETWORK_SERVICE_H__
