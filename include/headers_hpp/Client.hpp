#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Config.hpp"
#include "ServerConf.hpp"
#include "SubServ.hpp"
#include <cctype>
#include <ctime>
#include <dirent.h>
#include <exception>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <sys/types.h>

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
  bool addBuffer(std::string &buffer);
  const std::string &getBuffer(void) const;
  int getBodySize(void) const;
  void sendResponse(std::string &response);
  bool isTimedOut(void);

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
  std::string _queryUri;
  size_t _version;
  std::string _host;
  std::map<std::string, std::string> _headers;
  std::string _body;
  size_t _requestSize;
  int _bodySize;
  std::string _buffer;
  std::string _responseBody;
  bool _keepConnectionAlive;
  bool _chunkRequest;

  static const char *_validMethods[];
  static const size_t _methodSize;
  static const char *_whiteSpaces;
  static const size_t _uriMaxSize;
  static const size_t _headerMaxSize;
  static const size_t _headersMaxBuffer;
  static const std::map<std::string, char> _uriEncoding;

  // Constructor
  std::string getDate(void);
  void parseRequest(std::string &request);
  size_t insertInMap(std::string &line);
  size_t parseRequestLine(const std::string &requestLine);
  int parseUri(const std::string &uri);
  void uriDecoder(std::string &uri);
  void readRequest(void);
  ServerConf *getServerConf(void);
  void findPages(const std::string &url);
  void findIndex(std::string &url);
  void createResponseBody(void);
  bool checkMethod(void);
  bool checkIfValid(void);

  void resetClient(void);
  time_t getTime(void);
};

#endif //! CLIENT_HPP
