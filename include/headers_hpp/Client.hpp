#ifndef CLIENT_HPP
#define CLIENT_HPP

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

class ServerConf;

typedef struct multipartRequest {
  std::map<std::string, std::string> header;
  std::vector<char> body;
  std::string tmpFilename;
  std::string file;
} multipartRequest;

class Client {
public:
  // Constructor
  Client(int fd, mapConfs &map, ServerConf *defaultServer);
  Client(Client const &copy);

  // Destructor
  ~Client(void);

  // operator
  Client &operator=(Client const &rhs);

  // method to debug
  void print();
  int getBodyToRead(void) const;

  // method
  bool addBuffer(std::vector<char> buffer);
  const std::vector<char> &getBuffer(void) const;
  bool sendResponse(std::vector<char> &response);
  void setStatusCode(size_t code);
  bool isTimedOut(void) const;
  void addErrorResponse(size_t errorCode);
  bool keepConnectionOpen(void) const;
  void addCgiToMap(std::map<int, pid_t> &mapCgi);

protected:
private:
  int _socket;
  mapConfs &_mapConf;
  ServerConf *_defaultConf;
  ServerConf *_server;
  Location *_location;
  time_t _time;

  // requestLine attribute
  size_t _statusCode;
  std::string _sMethod;
  std::string _sUri;
  std::string _sPath;
  std::string _sQueryUri;
  size_t _version;

  // request attribute
  std::string _sHost;
  std::map<std::string, std::string> _headers;
  size_t _requestSize;
  std::vector<char> _vBody;
  std::vector<char> _vBuffer;
  int64_t _bodyToRead;
  bool _chunkRequest;
  bool _requestIsDone;
  std::string _boundary;
  bool _multipartRequest;
  std::string _chunkFile;
  int _fdUpload;
  int _chunkFd;
  int64_t _sizeChunk;
  std::vector<multipartRequest> _multipart;
  size_t _currentMultipart;

  // Response attribute
  Response _response;
  bool _keepConnectionAlive;
  bool _diffFileSystem;
  bool _epollIn;
  int _filefd;
  size_t _leftToRead;

  // cgi attributes
  std::string _infileCgi;
  std::string _outfileCgi;
  pid_t _cgiPid;
  std::string _sPathInfo;

  // static const attribut
  static const double _timeOutClient;
  static const double _timeOutCgi;
  static const char *_validMethods[];
  static const size_t _methodSize;
  static const char *_whiteSpaces;
  static const size_t _uriMaxSize;
  static const size_t _sizeMaxResponse;
  static const size_t _headerMaxSize;
  static const size_t _headersMaxBuffer;
  static const std::map<std::string, char> _uriEncoding;

  // Parsing Method
  bool checkMethod(void);
  bool checkIfValid(void);
  void removeTrailingLineFromBuffer(void);
  std::string getLineFromBuffer();
  void removeReturnCarriageNewLine(std::string &line);
  void removeReturnCarriage(std::vector<char> &vec);
  size_t hasNewLine(void) const;
  bool earlyParsing(int newLine);
  void parseRequest(std::string &request);
  size_t parseRequestLine(const std::string &requestLine);
  void checkPathInfo(void);
  std::string getBoundaryString(std::string &boundaryHeader);
  bool saveToTmpFile(void);
  void checkBodyHeader(multipartRequest &multi, std::vector<char> &body);
  bool checkBoundary(void);
  bool checkHeaderMulti(multipartRequest &multi);
  bool getMultipartBody(multipartRequest &multi);
  bool parseMultipartRequest(std::string &boundary);
  bool parseChunkRequest(void);
  void setupBodyParsing(void);
  bool parseBody(void);
  void uriDecoder(std::string &uri);
  int parseUri(const std::string &uri);
  void vectorToHeadersMap(std::vector<std::string> &request);
  size_t insertInMap(std::string &line,
                     std::map<std::string, std::string> &map);
  int64_t getSizeChunkFromBuffer(void);
  bool getTrailingHeader(void);
  ServerConf *getServerConf(void);

  // Response Method
  bool getResponse(std::string &response);
  void findPages(const std::string &url);
  bool findIndex(std::string &url);
  void buildListingDirectory(std::string &url);
  void buildResponse(void);
  void addContentTypeHeader(void);
  void addConnectionHeader(void);
  void defaultHTMLResponse(void);
  void handleError(void);
  void uploadTmpFileDifferentFileSystem(std::string &tmp, std::string &outfile);
  void handleMultipartSameFileSystem(void);
  void handleMultipartDifferentFileSystem(void);
  void handlePOST(void);
  void handleRedirection(void);
  void createResponseBody(void);
  void readFile(std::vector<char> &vec);
  void readFile(void);
  bool checkBodyToRead(std::vector<char> buffer);

  // cgi Method
  void handleDelete(void);
  void handleCgi(std::vector<char> &response);
  bool isTimedOutCgi(void) const;
  void cgiPOSTMethod(void);
  void cgiOutfile(void);
  void addHeaderToEnv(std::vector<char *> &vEnv, const std::string &envVariable,
                      const std::string &headerKey);
  void addVariableToEnv(std::vector<char *> &vEnv,
                        const std::string &envVariable);
  void buildEnv(std::vector<char *> &vEnv);
  void buildArguments(std::vector<char *> &argument);
  void freeVector(std::vector<char *> &vEnv, std::vector<char *> &argument);
  void setupChild(std::string &cgiPathScript);
  void setupCgi();
  // utils Method
  bool isHexadecimal(char c);
  std::string getDateOfFile(time_t rawtime) const;
  std::string getDate(void);
  time_t getTime(void);
  void resetClient(void);
};

#endif //! CLIENT_HPP
