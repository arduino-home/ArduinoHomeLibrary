#ifndef __ARDUINO_HOME_NETWORK_SERVICE_H__
#define __ARDUINO_HOME_NETWORK_SERVICE_H__

#include "service.h"

class Server;
class Client;

struct NetworkService : public Service {
  virtual ~NetworkService() = default;

  virtual Server *createServer(const int &port) = 0;
  virtual Client *createClient() = 0;
};

#endif // __ARDUINO_HOME_NETWORK_SERVICE_H__
