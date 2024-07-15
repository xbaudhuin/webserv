#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Response.hpp"
#include "ServerConf.hpp"
#include <ctime>
#include <dirent.h>
#include <exception>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>

class ServerConf;

class Client {
public:
  // Constructor
  Client(int fd, mapConfs &map, ServerConf *defaultServer);

  // Destructor
  ~Client(void);

  // operator

  Client(Client const &copy);
  Client &operator=(Client const &rhs);
  // method
  void print();
  bool addBuffer(std::string &buffer);
  const std::string &getBuffer(void) const;
  int getBodyToRead(void) const;
  bool sendResponse(std::string &response);
  bool isTimedOut(void);

protected:
private:
  int _socket;
  mapConfs &_mapConf;
  ServerConf *_defaultConf;
  ServerConf *_server;
  Response _response;
  Location *_location;
  time_t _time;
  size_t _statusCode;
  std::string _method;
  std::string _uri;
  std::string _queryUri;
  size_t _version;
  std::string _host;
  std::map<std::string, std::string> _headers;
  size_t _requestSize;
  std::string _body;
  int _bodyToRead;
  std::string _buffer;
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
  void addConnectionHeader(void);
  void defaultHTMLResponse(void);
  bool handleError(std::string &send);
  bool handleRedirection(std::string &send);
  void createResponseBody(void);
  bool checkMethod(void);
  bool checkIfValid(void);

  void resetClient(void);
  time_t getTime(void);
};

#endif //! CLIENT_HPP
