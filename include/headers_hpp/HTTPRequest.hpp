#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "Config.hpp"
#include "ServerConf.hpp"
#include <cctype>
#include <exception>
#include <map>
#include <sstream>
#include <string>

class HTTPRequest {
public:
  // Constructor
  HTTPRequest(std::istringstream &request);
  HTTPRequest(HTTPRequest const &copy);

  // Destructor
  ~HTTPRequest(void);

  // operator
  HTTPRequest &operator=(HTTPRequest const &rhs);

protected:
private:
  size_t statusCode;
  std::string _method;
  std::string _uri;
  std::string _version;
  std::string _host;
  std::map<std::string, std::string> _headers; // map or vecotr?
  std::string _body;
  static const char *validMethods[];
  static const size_t methodSize;

  // Constructor
  HTTPRequest(void);
  void removeReturnCarriage(std::string &line);
  void parseRequest(std::istringstream &request);
  void parseRequestLine(const std::string &requestLine);
};

#endif //! HTTPREQUEST_HPP
