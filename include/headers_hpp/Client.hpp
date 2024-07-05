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
  Client(const int fd, const mapConfs &map, const ServerConf *defaultServer);
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
  const ServerConf *_defaultConf;
  size_t time;
  size_t _statusCode;
  std::string _method;
  std::string _uri;
  std::vector<std::string> _queryUri;
  size_t _version;
  std::string _host;
  std::map<std::string, std::string> _headers; // map or vecotr?
  std::string _body;
  size_t _requestSize;
  int _bodySize;
  ServerConf *_server;
  std::string _buffer;

  static const char *_validMethods[];
  static const size_t _methodSize;
  static const char *_whiteSpaces;
  static const size_t _uriMaxSize;
  static const size_t _headerMaxSize;

  // Constructor
  void parseBuffer(void);
  std::string getDate(void);
  void parseRequest(std::string &request);
  bool insertInMap(std::string &line);
  size_t parseRequestLine(const std::string &requestLine);
  int parseUri(const std::string &uri);
  void readRequest(void);
  void sendResponse(int statusCode);
};

#endif //! CLIENT_HPP
