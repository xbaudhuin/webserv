#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "Config.hpp"
#include "ServerConf.hpp"
#include "SubServ.hpp"
#include <cctype>
#include <exception>
#include <map>
#include <sstream>
#include <string>

class ServerConf;

class HTTPRequest {
public:
  // Constructor
  HTTPRequest(int fd);
  HTTPRequest(HTTPRequest const &copy);

  // Destructor
  ~HTTPRequest(void);

  // operator
  HTTPRequest &operator=(HTTPRequest const &rhs);

  // method
  void print();

protected:
private:
  int _socket;
  size_t _statusCode;
  std::string _method;
  std::string _uri;
  std::vector<std::string> _queryUri;
  size_t _version;
  std::string _host;
  std::map<std::string, std::string> _headers; // map or vecotr?
  std::string _body;
  ServerConf *_server;
  static const char *_validMethods[];
  static const size_t _methodSize ;
  static const char *_whiteSpaces;
  static const size_t _uriMaxSize;
  static const size_t _headerMaxSize;

  // Constructor
  HTTPRequest(void);
  void removeReturnCarriage(std::string &line);
  void parseRequest(std::istringstream &request, const mapConfs &mapServerConf, ServerConf &defaultServer);
  bool insertInMap(std::string &line);
  size_t parseRequestLine(const std::string &requestLine);
  int parseUri(const std::string &uri);
  void getServerconf(const mapConfs mapServerConf,ServerConf &defaultServer);
};

#endif //! HTTPREQUEST_HPP
