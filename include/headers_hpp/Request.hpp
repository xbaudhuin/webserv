#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Response.hpp"
#include "ServerConf.hpp"
#include "Utils.hpp"
// #include <cstdint>
#include <ctime>
#include <ctype.h>
#include <dirent.h>
#include <exception>
#include <fstream>
#include <map>
#include <signal.h>
#include <sstream>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct multipartRequest {
  std::map<std::string, std::string> header;
  std::vector<char> body;
  std::string tmpFilename;
  std::string file;
} multipartRequest;

class Request {
public:
  // attribute

  size_t _statusCode;
  // Constructor
  Request(mapConfs &mapConfs, ServerConf *defaultConf);
  Request(Request const &copy);

  // Destructor
  ~Request(void);

  // operator
  Request &operator=(Request const &rhs);

  // getter

  // setter

  // method
  void resetRequest(void);
  bool addBuffer(const std::vector<char> buffer);

protected:
private:
  // static const attribute
  static const size_t _uriMaxSize;
  static const size_t _methodSize;
  static const char *_whiteSpaces;
  static const char *_validMethods[];
  static const size_t _headerMaxSize;
  static const size_t _headersMaxBuffer;
  static const std::map<std::string, char> _uriEncoding;

  // attribute
  mapConfs &_mapConf;
  ServerConf *_defaultConf;
  ServerConf *_server;
  Location *_location;

  std::string _sMethod;
  std::string _sUri;
  std::string _sPath;
  std::string _sPathInfo;
  std::string _sPathUpload;
  std::string _sQueryUri;
  size_t _version;

  std::map<std::string, std::string> _headers;
  size_t _requestSize; // ?
  std::vector<char> _vBody;
  std::vector<char> _vBuffer;
  int64_t _bodyToRead;
  bool _chunkRequest;
  bool _requestIsDone;
  std::string _boundary;
  bool _multipartRequest;
  std::string _tmpFile;
  // int _fdUpload;
  int _tmpFd;
  int64_t _sizeChunk;
  std::vector<multipartRequest> _multipart;
  size_t _currentMultipart;

  // method
  void parseRequest(std::string &buffer);
  size_t parseRequestLine(const std::string &requestLine);
  int parseUri(const std::string &uri);
  void uriDecoder(std::string &uri);

  // utils
  void checkPathInfo(void);
  bool checkMethod(void);
  bool requestValidbyLocation(void);
  bool getLocation(void);
  ServerConf *getServerConf(const std::string &host);
  void removeReturnCarriageNewLine(std::string &line);
  size_t hasNewLine(void) const;
  int64_t hasEmptyLine(int newLine);
  void fillBufferWithoutReturnCarriage(const std::vector<char> &vec);
  bool earlyParsing(int newLine);
  size_t insertInMap(std::string &line,
                     std::map<std::string, std::string> &map);
  void vectorToHeadersMap(std::vector<std::string> &request);
};
#endif //! REQUEST_HPP
