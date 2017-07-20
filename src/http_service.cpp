#include <ESP8266WiFi.h>          // https://github.com/esp8266/Arduino

//needed for library
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager

#include "utils.h"
#include "string_stream.h"
#include "http_service.h"
#include "dispatcher_service.h"
#include "runtime.h"

#define NAME "HttpService"

class ServiceRequestHandler : public RequestHandler {

  DispatcherService *dispatcher;

  void handleGet(ESP8266WebServer& server, const String &id) {
    Serial << "handleGet " << id << endl;
    JsonVariant value;
    switch(dispatcher->get(id, value)) {

      case DispatcherService::HandlerResult::success: {
        String response;
        value.printTo(response); // TODO: avoid string ?
        server.send(200, "application/json", response);
        break;
      }

      case DispatcherService::HandlerResult::not_found:
        server.send(404);
        break;

      case DispatcherService::HandlerResult::handler_error:
        server.send(400);
        break;

      default:
        server.send(500);
        break;
    }
  }

  void handleSet(ESP8266WebServer& server, const String &id) {
    const JsonVariant& value = DispatcherService::sharedBuffer().parse(server.arg("plain"));
    switch(dispatcher->set(id, value)) {

      case DispatcherService::HandlerResult::success:
        server.send(200);
        break;

      case DispatcherService::HandlerResult::not_found:
        server.send(404);
        break;

      case DispatcherService::HandlerResult::handler_error:
        server.send(400);
        break;

      default:
        server.send(500);
        break;
    }
  }

public:
    ServiceRequestHandler(DispatcherService *pdispatcher)
    : dispatcher(pdispatcher) {
    }

    virtual bool canHandle(HTTPMethod method, String uri) {
      if(!uri.length()) {
        return false;
      }
      if(uri[0] != '/') {
        return false;
      }
      uri = uri.substring(1);
      switch(method) {
        case HTTP_GET:
          return dispatcher->hasGetter(uri);
        case HTTP_POST:
          return dispatcher->hasSetter(uri);
      }
      return false;
    }

    virtual bool handle(ESP8266WebServer& server, HTTPMethod method, String uri) {
      Serial << "handle " << method << " " << uri << endl;
      uri = uri.substring(1);

      switch(method) {
        case HTTP_GET:
          handleGet(server, uri);
          return true;
        case HTTP_POST:
          handleSet(server, uri);
          return true;
      }
      return false;
    }
};

HttpService::HttpService(const int &pport)
 : server(new ESP8266WebServer(pport)) {
  StringStream ss(settings);
  ss << "port=" << pport;
}

void HttpService::setup() {
  auto dispatcher = Runtime::getDispatcherService();
  server->addHandler(new ServiceRequestHandler(dispatcher));
  server->begin();
}

void HttpService::loop() {
  server->handleClient();
}

const char *HttpService::getName() const {
  return NAME;
}

const char *HttpService::getId() const {
  return NAME;
}

const char *HttpService::getSettings() const {
  return settings.c_str();
}
