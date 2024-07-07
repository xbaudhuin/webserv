#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Config.hpp"
#include "ServerConf.hpp"
#include "SubServ.hpp"
#include <cctype>
#include <ctime>
#include <exception>
#include <map>
#include <sstream>
#include <string>

class ServerConf;

class Client {
public:
  // Constructor
  Client(const int fd, const mapConfs &map, ServerConf *defaultServer);
  Client(Client const &copy);

  // Destructor
  ~Client(void);

  // operator
  Client &operator=(Client const &rhs);

  // method
  void print();

protected:
private:
  const int _socket;
  const mapConfs &_mapConf;
  ServerConf *_defaultConf;
  ServerConf *_server;
  Location *_location;
  time_t _time;
  size_t _statusCode;
  std::string _method;
  std::string _uri;
  std::vector<std::string> _queryUri;
  size_t _version;
  std::string _host;
  std::map<std::string, std::string> _headers;
  std::string _body;
  size_t _requestSize;
  int _bodySize;
  std::string _buffer;
  std::string _responseBody;

  static const char *_validMethods[];
  static const size_t _methodSize;
  static const char *_whiteSpaces;
  static const size_t _uriMaxSize;
  static const size_t _headerMaxSize;
  static const std::map<std::string, char> uriEncoding;

  // Constructor
  void parseBuffer(void);
  std::string getDate(void);
  void parseRequest(std::string &request);
  bool insertInMap(std::string &line);
  size_t parseRequestLine(const std::string &requestLine);
  int parseUri(const std::string &uri);
  void uriDecoder(std::string &uri);
  void readRequest(void);
  ServerConf *getServerConf(void);
  void getResponseBody(void);
  void sendResponse(int statusCode);

  time_t getTime(void);
  bool isTimedOut(void);
};

#endif //! CLIENT_HPP
