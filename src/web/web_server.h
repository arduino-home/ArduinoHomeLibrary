#ifndef __ARDUINO_HOME_WEB_SERVER_H__
#define __ARDUINO_HOME_WEB_SERVER_H__

// from https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WebServer/src/ESP8266WebServer.h

#include "utils/function.h"

#define HTTP_DOWNLOAD_UNIT_SIZE 1460

#ifndef HTTP_UPLOAD_BUFLEN
#define HTTP_UPLOAD_BUFLEN 2048
#endif

#define HTTP_MAX_DATA_WAIT 1000 //ms to wait for the client to send the request
#define HTTP_MAX_POST_WAIT 1000 //ms to wait for POST data to arrive
#define HTTP_MAX_SEND_WAIT 5000 //ms to wait for data chunk to be ACKed
#define HTTP_MAX_CLOSE_WAIT 2000 //ms to wait for the client to close the connection

#define CONTENT_LENGTH_UNKNOWN ((size_t) -1)
#define CONTENT_LENGTH_NOT_SET ((size_t) -2)

namespace ah {
  namespace web {

    enum HTTPMethod { HTTP_ANY, HTTP_GET, HTTP_POST, HTTP_PUT, HTTP_PATCH, HTTP_DELETE, HTTP_OPTIONS };
    enum HTTPUploadStatus { UPLOAD_FILE_START, UPLOAD_FILE_WRITE, UPLOAD_FILE_END,
                            UPLOAD_FILE_ABORTED };
    enum HTTPClientStatus { HC_NONE, HC_WAIT_READ, HC_WAIT_CLOSE };
    enum HTTPAuthMethod { BASIC_AUTH, DIGEST_AUTH };


    struct HTTPUpload {
      HTTPUploadStatus status;
      String  filename;
      String  name;
      String  type;
      size_t  totalSize;    // file size
      size_t  currentSize;  // size of data currently in buf
      uint8_t buf[HTTP_UPLOAD_BUFLEN];
    };

    class WebServer;

    class RequestHandler {
    public:
        virtual ~RequestHandler() { }
        virtual bool canHandle(HTTPMethod method, String uri) { (void) method; (void) uri; return false; }
        virtual bool canUpload(String uri) { (void) uri; return false; }
        virtual bool handle(WebServer& server, HTTPMethod requestMethod, String requestUri) { (void) server; (void) requestMethod; (void) requestUri; return false; }
        virtual void upload(WebServer& server, String requestUri, HTTPUpload& upload) { (void) server; (void) requestUri; (void) upload; }

        RequestHandler* next() { return _next; }
        void next(RequestHandler* r) { _next = r; }

    private:
        RequestHandler* _next = nullptr;
    };

    class WebServer
    {
    public:
      WebServer(services::NetworkService *netService, int port = 80);
      ~WebServer();

      void begin();
      void handleClient();

      void close();
      void stop();

      typedef utils::function<void(void)> THandlerFunction;
      void on(const String &uri, THandlerFunction handler);
      void on(const String &uri, HTTPMethod method, THandlerFunction fn);
      void on(const String &uri, HTTPMethod method, THandlerFunction fn, THandlerFunction ufn);
      void addHandler(RequestHandler* handler);
      void onNotFound(THandlerFunction fn);  //called when handler is not assigned
      void onFileUpload(THandlerFunction fn); //handle file uploads

      String uri() { return _currentUri; }
      HTTPMethod method() { return _currentMethod; }
      Client *client() { return _currentClient; }
      HTTPUpload& upload() { return _currentUpload; }

      String arg(String name);        // get request argument value by name
      String arg(int i);              // get request argument value by number
      String argName(int i);          // get request argument name by number
      int args();                     // get arguments count
      bool hasArg(String name);       // check if argument exists
      void collectHeaders(const char* headerKeys[], const size_t headerKeysCount); // set the request headers to collect
      String header(String name);      // get request header value by name
      String header(int i);              // get request header value by number
      String headerName(int i);          // get request header name by number
      int headers();                     // get header count
      bool hasHeader(String name);       // check if header exists

      String hostHeader();            // get request host header if available or empty String if not

      // send response to the client
      // code - HTTP response code, can be 200 or 404
      // content_type - HTTP content type, like "text/plain" or "image/png"
      // content - actual content body
      void send(int code, const char* content_type = NULL, const String& content = String(""));
      void send(int code, char* content_type, const String& content);
      void send(int code, const String& content_type, const String& content);

      void setContentLength(size_t contentLength);
      void sendHeader(const String& name, const String& value, bool first = false);
      void sendContent(const String& content);

      static String urlDecode(const String& text);

    template<typename T> size_t streamFile(T &file, const String& contentType){
      setContentLength(file.size());
      if (String(file.name()).endsWith(".gz") &&
          contentType != "application/x-gzip" &&
          contentType != "application/octet-stream"){
        sendHeader("Content-Encoding", "gzip");
      }
      send(200, contentType, "");
      return _currentClient->write(file);
    }

    protected:
      void _addRequestHandler(RequestHandler* handler);
      void _handleRequest();
      bool _parseRequest(Client* client);
      void _parseArguments(String data);
      static String _responseCodeToString(int code);
      bool _parseForm(Client* client, String boundary, uint32_t len);
      bool _parseFormUploadAborted();
      void _uploadWriteByte(uint8_t b);
      uint8_t _uploadReadByte(Client* client);
      void _prepareHeader(String& response, int code, const char* content_type, size_t contentLength);
      bool _collectHeader(const char* headerName, const char* headerValue);

      // for extracting Auth parameters
      String _exractParam(String& authReq,const String& param,const char delimit = '"');

      void _closeClient();

      struct RequestArgument {
        String key;
        String value;
      };

      services::NetworkService *_netService;
      Server  *_server;

      Client  *_currentClient;
      HTTPMethod  _currentMethod;
      String      _currentUri;
      uint8_t     _currentVersion;
      HTTPClientStatus _currentStatus;
      unsigned long _statusChange;

      RequestHandler*  _currentHandler;
      RequestHandler*  _firstHandler;
      RequestHandler*  _lastHandler;
      THandlerFunction _notFoundHandler;
      THandlerFunction _fileUploadHandler;

      int              _currentArgCount;
      RequestArgument* _currentArgs;
      HTTPUpload       _currentUpload;

      int              _headerKeysCount;
      RequestArgument* _currentHeaders;
      size_t           _contentLength;
      String           _responseHeaders;

      String           _hostHeader;
      bool             _chunked;

      String           _snonce;  // Store noance and opaque for future comparison
      String           _sopaque;
      String           _srealm;  // Store the Auth realm between Calls
    };

  } // namespace web
} // namespace ah

#endif // __ARDUINO_HOME_WEB_SERVER_H__
