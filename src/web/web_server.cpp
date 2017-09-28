// from https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WebServer/src/ESP8266WebServer.cpp

#include <Arduino.h>
//#include <libb64/cencode.h>

#include "services/network_service.h"
#include "web_server.h"

namespace ah {
  namespace web {

    class FunctionRequestHandler : public RequestHandler {
    public:
        FunctionRequestHandler(WebServer::THandlerFunction fn, WebServer::THandlerFunction ufn, const String &uri, HTTPMethod method)
        : _fn(fn)
        , _ufn(ufn)
        , _uri(uri)
        , _method(method)
        {
        }

        bool canHandle(HTTPMethod requestMethod, String requestUri) override  {
            if (_method != HTTP_ANY && _method != requestMethod)
                return false;

            if (requestUri != _uri)
                return false;

            return true;
        }

        bool canUpload(String requestUri) override  {
            if (!_ufn || !canHandle(HTTP_POST, requestUri))
                return false;

            return true;
        }

        bool handle(WebServer& server, HTTPMethod requestMethod, String requestUri) override {
            (void) server;
            if (!canHandle(requestMethod, requestUri))
                return false;

            _fn();
            return true;
        }

        void upload(WebServer& server, String requestUri, HTTPUpload& upload) override {
            (void) server;
            (void) upload;
            if (canUpload(requestUri))
                _ufn();
        }

    protected:
        WebServer::THandlerFunction _fn;
        WebServer::THandlerFunction _ufn;
        String _uri;
        HTTPMethod _method;
    };

    static const char * AUTHORIZATION_HEADER = "Authorization";

    WebServer::WebServer(services::NetworkService *netService, int port)
    : _netService(netService)
    , _server(_netService->createServer(port))
    , _currentMethod(HTTP_ANY)
    , _currentVersion(0)
    , _currentStatus(HC_NONE)
    , _statusChange(0)
    , _currentHandler(0)
    , _firstHandler(0)
    , _lastHandler(0)
    , _currentArgCount(0)
    , _currentArgs(0)
    , _headerKeysCount(0)
    , _currentHeaders(0)
    , _contentLength(0)
    , _chunked(false)
    {
    }

    WebServer::~WebServer() {
      if (_currentHeaders)
        delete[]_currentHeaders;
      _headerKeysCount = 0;
      RequestHandler* handler = _firstHandler;
      while (handler) {
        RequestHandler* next = handler->next();
        delete handler;
        handler = next;
      }
      close();

      if(_currentClient) {
        delete _currentClient;
      }
      delete _server;
    }

    void WebServer::begin() {
      _closeClient();
      _server->begin();
      if(!_headerKeysCount)
        collectHeaders(0, 0);
    }

    String WebServer::_exractParam(String& authReq,const String& param,const char delimit){
      int _begin = authReq.indexOf(param);
      if (_begin==-1) return "";
      return authReq.substring(_begin+param.length(),authReq.indexOf(delimit,_begin+param.length()));
    }

    void WebServer::_closeClient() {
      if(_currentClient) {
        delete _currentClient;
        _currentClient = nullptr;
      }
      _currentStatus = HC_NONE;
    }

    void WebServer::on(const String &uri, WebServer::THandlerFunction handler) {
      on(uri, HTTP_ANY, handler);
    }

    void WebServer::on(const String &uri, HTTPMethod method, WebServer::THandlerFunction fn) {
      on(uri, method, fn, _fileUploadHandler);
    }

    void WebServer::on(const String &uri, HTTPMethod method, WebServer::THandlerFunction fn, WebServer::THandlerFunction ufn) {
      _addRequestHandler(new FunctionRequestHandler(fn, ufn, uri, method));
    }

    void WebServer::addHandler(RequestHandler* handler) {
        _addRequestHandler(handler);
    }

    void WebServer::_addRequestHandler(RequestHandler* handler) {
        if (!_lastHandler) {
          _firstHandler = handler;
          _lastHandler = handler;
        }
        else {
          _lastHandler->next(handler);
          _lastHandler = handler;
        }
    }

    void WebServer::handleClient() {
      if (_currentStatus == HC_NONE) {
        Client *client = _netService->serverAvailable(_server);
        if (!client) {
          return;
        }

        _currentClient = client;
        _currentStatus = HC_WAIT_READ;
        _statusChange = millis();
      }

      if (!_currentClient || !_currentClient->connected()) {
        _closeClient();
        return;
      }

      // Wait for data from client to become available
      if (_currentStatus == HC_WAIT_READ) {
        if (!_currentClient->available()) {
          if (millis() - _statusChange > HTTP_MAX_DATA_WAIT) {
            _closeClient();
          }
          yield();
          return;
        }

        if (!_parseRequest(_currentClient)) {
          _closeClient();
          return;
        }
        _currentClient->setTimeout(HTTP_MAX_SEND_WAIT);
        _contentLength = CONTENT_LENGTH_NOT_SET;
        _handleRequest();

        if (!_currentClient->connected()) {
          _closeClient();
          return;
        } else {
          _currentStatus = HC_WAIT_CLOSE;
          _statusChange = millis();
          return;
        }
      }

      if (_currentStatus == HC_WAIT_CLOSE) {
        if (millis() - _statusChange > HTTP_MAX_CLOSE_WAIT) {
          _closeClient();
        } else {
          yield();
          return;
        }
      }
    }

    void WebServer::close() {
      _netService->serverClose(_server);
    }

    void WebServer::stop() {
      close();
    }

    void WebServer::sendHeader(const String& name, const String& value, bool first) {
      String headerLine = name;
      headerLine += ": ";
      headerLine += value;
      headerLine += "\r\n";

      if (first) {
        _responseHeaders = headerLine + _responseHeaders;
      }
      else {
        _responseHeaders += headerLine;
      }
    }

    void WebServer::setContentLength(size_t contentLength) {
        _contentLength = contentLength;
    }

    void WebServer::_prepareHeader(String& response, int code, const char* content_type, size_t contentLength) {
        response = "HTTP/1."+String(_currentVersion)+" ";
        response += String(code);
        response += " ";
        response += _responseCodeToString(code);
        response += "\r\n";

        if (!content_type)
            content_type = "text/html";

        sendHeader("Content-Type", content_type, true);
        if (_contentLength == CONTENT_LENGTH_NOT_SET) {
            sendHeader("Content-Length", String(contentLength));
        } else if (_contentLength != CONTENT_LENGTH_UNKNOWN) {
            sendHeader("Content-Length", String(_contentLength));
        } else if(_contentLength == CONTENT_LENGTH_UNKNOWN && _currentVersion){ //HTTP/1.1 or above client
          //let's do chunked
          _chunked = true;
          sendHeader("Accept-Ranges","none");
          sendHeader("Transfer-Encoding","chunked");
        }
        sendHeader("Connection", "close");

        response += _responseHeaders;
        response += "\r\n";
        _responseHeaders = String();
    }

    void WebServer::send(int code, const char* content_type, const String& content) {
        String header;
        // Can we asume the following?
        //if(code == 200 && content.length() == 0 && _contentLength == CONTENT_LENGTH_NOT_SET)
        //  _contentLength = CONTENT_LENGTH_UNKNOWN;
        _prepareHeader(header, code, content_type, content.length());
        _currentClient->print(header);
        if(content.length())
          sendContent(content);
    }

    void WebServer::send(int code, char* content_type, const String& content) {
      send(code, (const char*)content_type, content);
    }

    void WebServer::send(int code, const String& content_type, const String& content) {
      send(code, (const char*)content_type.c_str(), content);
    }

    void WebServer::sendContent(const String& content) {
      const char * footer = "\r\n";
      size_t len = content.length();
      if(_chunked) {
        char * chunkSize = (char *)malloc(11);
        if(chunkSize){
          sprintf(chunkSize, "%x%s", len, footer);
          _currentClient->print(chunkSize);
          free(chunkSize);
        }
      }
      _currentClient->print(content);
      if(_chunked){
        _currentClient->print(footer);
      }
    }

    String WebServer::arg(String name) {
      for (int i = 0; i < _currentArgCount; ++i) {
        if ( _currentArgs[i].key == name )
          return _currentArgs[i].value;
      }
      return String();
    }

    String WebServer::arg(int i) {
      if (i < _currentArgCount)
        return _currentArgs[i].value;
      return String();
    }

    String WebServer::argName(int i) {
      if (i < _currentArgCount)
        return _currentArgs[i].key;
      return String();
    }

    int WebServer::args() {
      return _currentArgCount;
    }

    bool WebServer::hasArg(String  name) {
      for (int i = 0; i < _currentArgCount; ++i) {
        if (_currentArgs[i].key == name)
          return true;
      }
      return false;
    }


    String WebServer::header(String name) {
      for (int i = 0; i < _headerKeysCount; ++i) {
        if (_currentHeaders[i].key.equalsIgnoreCase(name))
          return _currentHeaders[i].value;
      }
      return String();
    }

    void WebServer::collectHeaders(const char* headerKeys[], const size_t headerKeysCount) {
      _headerKeysCount = headerKeysCount + 1;
      if (_currentHeaders)
         delete[]_currentHeaders;
      _currentHeaders = new RequestArgument[_headerKeysCount];
      _currentHeaders[0].key = AUTHORIZATION_HEADER;
      for (int i = 1; i < _headerKeysCount; i++){
        _currentHeaders[i].key = headerKeys[i-1];
      }
    }

    String WebServer::header(int i) {
      if (i < _headerKeysCount)
        return _currentHeaders[i].value;
      return String();
    }

    String WebServer::headerName(int i) {
      if (i < _headerKeysCount)
        return _currentHeaders[i].key;
      return String();
    }

    int WebServer::headers() {
      return _headerKeysCount;
    }

    bool WebServer::hasHeader(String name) {
      for (int i = 0; i < _headerKeysCount; ++i) {
        if ((_currentHeaders[i].key.equalsIgnoreCase(name)) &&  (_currentHeaders[i].value.length() > 0))
          return true;
      }
      return false;
    }

    String WebServer::hostHeader() {
      return _hostHeader;
    }

    void WebServer::onFileUpload(THandlerFunction fn) {
      _fileUploadHandler = fn;
    }

    void WebServer::onNotFound(THandlerFunction fn) {
      _notFoundHandler = fn;
    }

    void WebServer::_handleRequest() {
      bool handled = false;
      if (_currentHandler){
        handled = _currentHandler->handle(*this, _currentMethod, _currentUri);
      }

      if (!handled) {
        if(_notFoundHandler) {
          _notFoundHandler();
        }
        else {
          send(404, "text/plain", String("Not found: ") + _currentUri);
        }
      }

      _currentUri = String();
    }

    String WebServer::_responseCodeToString(int code) {
      switch (code) {
        case 100: return F("Continue");
        case 101: return F("Switching Protocols");
        case 200: return F("OK");
        case 201: return F("Created");
        case 202: return F("Accepted");
        case 203: return F("Non-Authoritative Information");
        case 204: return F("No Content");
        case 205: return F("Reset Content");
        case 206: return F("Partial Content");
        case 300: return F("Multiple Choices");
        case 301: return F("Moved Permanently");
        case 302: return F("Found");
        case 303: return F("See Other");
        case 304: return F("Not Modified");
        case 305: return F("Use Proxy");
        case 307: return F("Temporary Redirect");
        case 400: return F("Bad Request");
        case 401: return F("Unauthorized");
        case 402: return F("Payment Required");
        case 403: return F("Forbidden");
        case 404: return F("Not Found");
        case 405: return F("Method Not Allowed");
        case 406: return F("Not Acceptable");
        case 407: return F("Proxy Authentication Required");
        case 408: return F("Request Time-out");
        case 409: return F("Conflict");
        case 410: return F("Gone");
        case 411: return F("Length Required");
        case 412: return F("Precondition Failed");
        case 413: return F("Request Entity Too Large");
        case 414: return F("Request-URI Too Large");
        case 415: return F("Unsupported Media Type");
        case 416: return F("Requested range not satisfiable");
        case 417: return F("Expectation Failed");
        case 500: return F("Internal Server Error");
        case 501: return F("Not Implemented");
        case 502: return F("Bad Gateway");
        case 503: return F("Service Unavailable");
        case 504: return F("Gateway Time-out");
        case 505: return F("HTTP Version not supported");
        default:  return "";
      }
    }

  } // namespace web
} // namespace ah
