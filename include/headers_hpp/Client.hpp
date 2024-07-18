#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Response.hpp"
#include "ServerConf.hpp"
#include "Utils.hpp"
#include <ctime>
#include <ctype.h>
#include <dirent.h>
#include <exception>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
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
  bool addBuffer(std::string buffer);
  const std::string &getBuffer(void) const;
  int getBodyToRead(void) const;
  bool sendResponse(std::string &response);
  bool isTimedOut(void);
  void add400Response(void);
  bool keepConnectionOpen(void) const;

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
  bool _epollIn;
  std::string _path;
  std::ifstream _file;
  size_t _leftToRead;
  size_t _nbRead;

  static const char *_validMethods[];
  static const size_t _methodSize;
  static const char *_whiteSpaces;
  static const size_t _uriMaxSize;
  static const size_t _sizeMaxResponse;
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
  ServerConf *getServerConf(void);
  void findPages(const std::string &url);
  bool findIndex(std::string &url);
  bool getResponse(std::string &response);
  void buildListingDirectory(std::string &url);
  void buildResponse(void);
  void addConnectionHeader(void);
  void defaultHTMLResponse(void);
  void handleError(void);
  void handleRedirection(void);
  void createResponseBody(void);
  bool earlyParsing(void);
  bool checkMethod(void);
  bool checkIfValid(void);
  void readFile(std::string &response);

  void resetClient(void);
  time_t getTime(void);
  std::string getDateOfFile(time_t rawtime) const;
};

#endif //! CLIENT_HPP
