#include "Request.hpp"
#include "ServerConf.hpp"
#include "Utils.hpp"
#include <fcntl.h>
#include <stdexcept>

Request::Request(mapConfs &mapConfs, ServerConf *defaultConf)
    : _mapConf(mapConfs), _defaultConf(defaultConf), _server(NULL),
      _location(NULL), _statusCode(0), _version(1), _requestSize(0),
      _bodyToRead(-1), _chunkRequest(false), _requestIsDone(true),
      _multipartRequest(false), _tmpFd(-1), _sizeChunk(0),
      _currentMultipart(0) {
  return;
}

Request::~Request(void) {
  if (_tmpFile.empty() == false) {
    unlink(_tmpFile.c_str());
  }
  if (_tmpFd != -1) {
    close(_tmpFd);
  }
  return;
}

Request::Request(Request const &copy) : _mapConf(copy._mapConf) {
  _tmpFd = -1;
  if (this != &copy)
    *this = copy;
  return;
}

Request &Request::operator=(Request const &rhs) {
  if (this != &rhs) {
    _defaultConf = rhs._defaultConf;
    _server = rhs._server;
    _location = rhs._location;
    _statusCode = rhs._statusCode;
    _sMethod = rhs._sMethod;
    _sUri = rhs._sUri;
    _sPath = rhs._sPath;
    _sPathUpload = rhs._sPathUpload;
    _version = rhs._version;
    _headers = rhs._headers;
    _requestSize = rhs._requestSize;
    _vBody = rhs._vBody;
    _vBuffer = rhs._vBuffer;
    _bodyToRead = rhs._bodyToRead;
    _chunkRequest = rhs._chunkRequest;
    _requestIsDone = rhs._requestIsDone;
    if (_tmpFile.empty() == false) {
      unlink(_tmpFile.c_str());
    }
    _tmpFile = rhs._tmpFile;
    if (_tmpFd != -1) {
      close(_tmpFd);
    }
    if (rhs._tmpFd != -1) {
      _tmpFd = open(rhs._tmpFile.c_str(), O_RDWR | O_CLOEXEC | O_CREAT);
      if (_tmpFd == -1)
        throw std::runtime_error("Request::operator=: fail to open _tmpFd");
    }
    _sizeChunk = rhs._sizeChunk;
  }
  return (*this);
}

void Request::resetRequest(void) {
  _server = NULL;
  _location = NULL;
  _statusCode = 0;
  _sMethod = "";
  _sUri = "";
  _sPath = "";
  _sPathUpload = "";
  _sQueryUri = "";
  _version = 1;
  _headers.clear();
  _requestSize = 0;
  resetVector(_vBody);
  resetVector(_vBuffer);
  _bodyToRead = -1;
  _chunkRequest = false;
  _requestIsDone = false;
  _multipartRequest = false;
  _tmpFile = "";
  _tmpFd = -1;
  _sizeChunk = 0;
}

void Request::uriDecoder(std::string &uri) {
  size_t pos = 0;
  while (true) {
    pos = uri.find_first_of('%');
    if (pos == uri.npos)
      break;
    if (pos + 2 > uri.size()) {
      _statusCode = 400;
      return;
    }
    std::string key = uri.substr(pos, 3);
    std::map<std::string, char>::const_iterator it = _uriEncoding.find(key);
    if (it == _uriEncoding.end()) {
      _statusCode = 400;
      return;
    }
    uri.replace(pos, 3, 1, (*it).second);
  }
}

ServerConf *Request::getServerConf(const std::string &host) {
  mapConfs::const_iterator it;
  it = _mapConf.find(host);
  if (it != _mapConf.end()) {
    std::cout << YELLOW << "found via host: " << host
              << "; server name: " << ((*it).second)->getMainServerName()
              << RESET << std::endl;
    return ((*it).second);
  }
  std::cout << YELLOW << "return default server" << RESET << std::endl;
  return (_defaultConf);
}

void Request::removeReturnCarriageNewLine(std::string &line) {
  size_t i = 0;
  if (line[line.size() - 1] == '\n') {
    i++;
    if (line.size() >= 2 && line[line.size() - 2] == '\r')
      i++;
    line.erase(line.size() - i);
  }
}

int64_t Request::hasEmptyLine(int newLine) {
  int64_t pos = -1;
  for (size_t i = newLine; i + 1 < _vBuffer.size(); i++) {
    if (_vBuffer[i] == '\n' && _vBuffer[i + 1] == '\n') {
      pos = i;
      std::cout << GREEN << "FOUND EMPTY LINE" << RESET << std::endl;
      break;
    }
  }
  if (pos == -1)
    std::cout << RED << "No empty line in buffer" << RESET << std::endl;
  return (pos);
}

size_t Request::hasNewLine(void) const {
  for (size_t i = 0; i < _vBuffer.size(); i++) {
    if (_vBuffer[i] == '\n')
      return (i);
  }
  return (0);
}

bool Request::earlyParsing(int newLine) {

  // std::cout << PURP << "EARLY PARSING: _vBuffer.size() = " << _vBuffer.size()
  //           << RESET << std::endl;

  if (newLine > 0) {

    // std::cout << PURP << "earlyPARSING: NEWLINE" << RESET << std::endl;

    std::string early(&_vBuffer[0], &_vBuffer[newLine]);
    // std::cout << BLUE << "earlyParsing: early = " << early << RESET
    // << std::endl;
    size_t tmpStatusCode = parseRequestLine(early);
    _sMethod = "";
    _sUri = "";
    _sQueryUri = "";
    _version = 1;
    if (tmpStatusCode == 400) {
      return (false);
    }
  } else {
    std::string early(&_vBuffer[0], &_vBuffer[_vBuffer.size()]);

    size_t pos = early.find_first_of(' ');
    if (pos == early.npos && early.size() > 8) {
      return (false);
    }
    early = early.substr(0, pos);
    size_t i = 0;
    for (; i < _methodSize && early != _validMethods[i]; i++) {
    }
    if (i == _methodSize) {
      return (false);
    }
    return (true);
  }
  return (true);
}

void Request::checkPathInfo(void) {
  if (_location->hasPathInfo() == false)
    return;
  size_t locator = _location->getUrl().size();
  std::cout << YELLOW << "_sUri = " << _sUri
            << "; _location.uri().size() = " << _location->getUrl().size()
            << RESET << std::endl;
  vec_string extension = _location->availableExtension();
  size_t i = 0;
  size_t pos = _sUri.npos;
  for (; i < extension.size(); i++) {
    pos = _sUri.find(extension[i], locator);
    if (pos != _sUri.npos) {
      std::cout << GREEN << "found pos: extension[i] = " << extension[i] << ";"
                << RESET << std::endl;
      break;
    }
  }
  if (i == extension.size())
    return;
  _sPathInfo = _sUri.substr(pos + extension[i].size(), _sUri.npos);
  _sUri.erase(pos + extension[i].size());
  std::cout << YELLOW << "_sPathinfo = " << _sPathInfo << "; _sUri = " << _sUri
            << RESET << std::endl;
}

int Request::parseUri(const std::string &uri) {
  if (uri[0] != '/')
    return (400);
  if (uri.size() > _uriMaxSize)
    return (414);
  size_t pos = uri.find_first_of('?');
  _sUri = uri.substr(0, pos);
  uriDecoder(_sUri);
  // pathINFO
  if (_sUri.size() > 2048)
    return (414);
  if (pos != uri.npos) {
    _sQueryUri = uri.substr(pos + 1);
  }
  return (0);
}

size_t Request::parseRequestLine(const std::string &requestLine) {
  vec_string split_request = split(requestLine, " ");
  if (split_request.size() != 3) {
    std::cout << RED << "StatusCode(split.size()): " << split_request.size()
              << RESET << std::endl;
    return (400);
  }
  _sMethod = split_request[0];
  size_t i = 0;
  for (; i < _methodSize && _sMethod != _validMethods[i]; i++) {
  }
  if (i == _methodSize) {
    return (400);
  } else if (i > 2) {
    return (405);
  }
  _statusCode = parseUri(split_request[1]);
  if (_statusCode > 0) {
    return (_statusCode);
  }
  if (split_request[2].compare(0, 5, "HTTP/") != 0 ||
      split_request[2].size() < 8) {
    std::cout << RED << "StatusCode(split_compare): " << "400" << RESET
              << std::endl;
    return (400);
  }
  std::string version = split_request[2].substr(5);
  if (std::isdigit(version[0]) == false) {
    std::cout << RED << "StatusCode(isdigit): " << "400" << RESET << std::endl;
    return (400);
  }
  for (i = 1; std::isdigit(version[i]); i++) {
  }
  if (i > 1) {
    return (405);
  }
  if (version[i] != '.') {
    std::cout << RED << "StatusCode(i): " << "400" << RESET << std::endl;
    return (400);
  }
  i++;
  char *after = NULL;
  long int nb = std::strtol(version.c_str() + i, &after, 10);
  if (nb < 0 || nb > 999 || *after != '\0' || errno == ERANGE) {
    std::cout << RED << "StatusCode(nb): " << "400" << RESET << std::endl;
    return (400);
  }
  std::cout << "VERSION = " << nb << "\n";
  if (nb >= 1)
    _version = 1;
  else
    _version = 0;
  return (0);
}

void Request::fillBufferWithoutReturnCarriage(const std::vector<char> &vec) {
  size_t emptyLine = 0;
  size_t i = 0;
  _vBuffer.reserve(_vBuffer.size() + vec.size());
  for (; i < vec.size(); i++) {
    if (vec[i] == '\r') {
      size_t tmp = i;
      i++;
      if (i == vec.size())
        break;
      if (vec[i] != '\n') {
        emptyLine = 0;
        _vBuffer.push_back(*(vec.begin() + tmp));
      } else
        emptyLine++;
    }
    _vBuffer.push_back(*(vec.begin() + i));
    if (vec[i] == '\n')
      emptyLine++;
    else
      emptyLine = 0;
    if (emptyLine == 2)
      break;
  }
  if (i != vec.size())
    _vBuffer.insert(_vBuffer.end(), vec.begin() + i, vec.end());
  while (true) {
    if (_vBuffer.size() == 0)
      break;
    std::vector<char>::iterator it = _vBuffer.begin();
    if (*it == '\n') {
      _vBuffer.erase(it);
    } else {
      break;
    }
  }
  // std::cout << BLUE << "vec: " << vec << RESET << std::endl;
  // std::cout << PURP << "buffer: " << _vBuffer << RESET << std::endl;
  return;
}

size_t Request::insertInMap(std::string &line,
                            std::map<std::string, std::string> &map) {
  if (line.size() > _headerMaxSize)
    return (431);
  std::transform(line.begin(), line.end(), line.begin(), toLower);
  size_t pos = line.find_first_of(':');
  if (pos == line.npos) {
    return (400);
  }
  std::string key = line.substr(0, pos);
  if (key.find_first_of(":\r\n\x1F") != key.npos)
    return (400);
  std::string value = line.substr(pos + 1);
  if (value.size() > _headerMaxSize) {
    return (400);
  }
  trimWhitespace(value, _whiteSpaces);
  std::cout << "KEY = " << key << std::endl;
  std::cout << "VALUE = " << value << std::endl;
  std::map<std::string, std::string>::iterator it;
  if (pos != line.npos) {
    it = map.find(key);
    if (it != map.end()) {
      if (key == "host" || "content-length" || "transfer-encoding" ||
          "content-type")
        return (400);
      (*it).second += ", ";
      (*it).second += value;
    } else if (map.insert(std::make_pair(key, value)).second == false)
      return (500);
  }
  return (0);
}

void Request::vectorToHeadersMap(std::vector<std::string> &request) {
  for (size_t it = 0; it < request.size(); it++) {
    _requestSize += request[it].size();
    if (_requestSize > _headersMaxBuffer) {
      _statusCode = 413;

      std::cout << RED
                << "changin statusCode because if (_requestSize > "
                   "_headersMaxBuffer) to "
                << _statusCode << RESET << std::endl;

      return;
    }
    _statusCode = insertInMap(request[it], _headers);
    if (_headers.count("host") == 0) {
      _statusCode = 400;
      std::cout << RED
                << "changin statusCode because if (_headers.count(\"host\") "
                   "== 0) to "
                << _statusCode << RESET << std::endl;

      if (_statusCode != 0)
        return;
    }
  }
}

bool Request::getLocation(void) {
  _server = getServerConf(_headers.at("host"));
  try {
    _location = &(_server->getPreciseLocation(_sUri));
  } catch (security_error &e) {
    _location = NULL;
    _statusCode = 400;

    std::cout << RED << "caught security_error" << RESET << std::endl;

    return (false);
  } catch (std::exception &e) {
    _location = NULL;
    _statusCode = 404;

    std::cout << RED << "changin statusCode because caught exception to "
              << _statusCode << RESET << std::endl;

    return (false);
  }
  if (_location == NULL) {
    std::cout << RED << "THOMAS EST NULL" << RESET << std::endl;
  } else {
    std::cout << RED << "location = " << _location->getUrl() << RESET
              << std::endl;
  }
  return (true);
}

bool Request::checkMethod(void) {
  if (_sMethod == "GET" && _location->getGetStatus() == false) {
    _statusCode = 405;
  } else if (_sMethod == "POST" && _location->getPostStatus() == false) {
    std::cout << RED << "Client::checkMethod: invalid method on location: "
              << _location->getUrl() << RESET << std::endl;
    _statusCode = 405;
  } else if (_sMethod == "DELETE" && _location->getDeleteStatus() == false) {
    _statusCode = 405;
  }
  if (_statusCode >= 400)
    return (false);
  return (true);
}

bool Request::requestValidbyLocation(void) {
  checkPathInfo();
  if (checkMethod() == false)
    return (false);
  return (true);
}

void Request::setupBodyParsing(void) {
  std::map<std::string, std::string>::iterator multipart;
  multipart = _headers.find("content-type");
  if (_statusCode < 400 && multipart != _headers.end() &&
      (*multipart).second.substr(0, 9) == "multipart") {
    _boundary = getBoundaryString((*multipart).second);
    _multipartRequest = true;
  }
  if (_statusCode >= 400)
    return;
  std::map<std::string, std::string>::iterator itLength;
  itLength = _headers.find("content-length");
  std::map<std::string, std::string>::iterator itChunked;
  itChunked = _headers.find("transfer-encoding");
  if ((itChunked != _headers.end() && itLength != _headers.end()) ||
      (_multipartRequest == true && itChunked != _headers.end())) {
    _statusCode = 400;
    std::cout << RED
              << "both content-length and transfer-encoding headers found"
              << RESET << std::endl;
    return;
  }
  if (itLength != _headers.end()) {
    _bodyToRead = std::strtol(((*itLength).second).c_str(), NULL, 10);
    if (errno == ERANGE || _bodyToRead < 0 ||
        (_bodyToRead > static_cast<int>(_server->getLimitBodySize()) &&
         _server->getLimitBodySize() != 0)) {
      _statusCode = 413;
      std::cout << RED
                << "changin statusCode because (_bodyToRead > "
                   "static_cast<int>(_server->getLimitBodySize()) to "
                << _statusCode << RESET << std::endl;
      std::cout << PURP << "exit limitBdySize: " << _server->getLimitBodySize()
                << RESET << std::endl;
      return;
    }
  } else if (itChunked != _headers.end()) {
    _chunkRequest = true;
    _tmpFile = "webserv_tmpfile_";
    time_t time = getTime();
    std::stringstream t;
    t << time;
    _tmpFile += "id" + t.str();
    _tmpFd = open(_tmpFile.c_str(), O_CREAT | O_RDWR | O_TRUNC | O_CLOEXEC);
    if (_tmpFd == -1) {
      _statusCode = 500;
      return;
    }
  }
}

void Request::parseRequest(std::string &buffer) {
  if (buffer.empty() == true)
    return;
  vec_string request = split(buffer, "\n");
  if (request.size() < 2) {
    _statusCode = 400;
    std::cout << RED << "changin statusCode because if (request.size() < 2) to "
              << _statusCode << RESET << std::endl;
    return;
  }
  _requestSize = request[0].size();
  _statusCode = parseRequestLine(request[0]);
  if (_statusCode >= 400)
    return;
  request.erase(request.begin(), request.begin() + 1);
  vectorToHeadersMap(request);
  if (_statusCode != 0)
    return;
  if (getLocation() == false)
    return;
  if (requestValidbyLocation() == false)
    return;
  if (_sMethod == "POST") {
    setupBodyParsing();
    if (_statusCode == 0 && parseBody() == false) {
      // _requestIsDone = false;
      return;
    }
    if (_vBuffer.size() != 0 && _bodyToRead == 0) {
      _statusCode = 400;
      return;
    }
  }
  if (_location && _location->isCgi(_sUri) == true) {
    setupCgi();
  }
  if (_statusCode == 0)
    _statusCode = 200;
  return;
}

bool Request::addBuffer(const std::vector<char> buffer) {
  if (_bodyToRead > 0 || _requestIsDone == false) {
    return (checkBodyToRead(buffer));
  }
  fillBufferWithoutReturnCarriage(buffer);
  int newLine = hasNewLine();
  if (newLine == 0) {
    return (false);
  }
  if (_vBuffer.size() < 20 || newLine > 0) {
    if (earlyParsing(newLine) == false) {
      _statusCode = 400;
      _server = _defaultConf;
      return (true);
    }
  }
  int64_t pos = hasEmptyLine(newLine);
  if (pos == -1) {
    return (false);
  }
  std::string request(&_vBuffer[0], &_vBuffer[pos]);
  _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + pos + 2);
  parseRequest(request);
  std::cout << RED << "End after parseRequest; StatusCode = " << _statusCode
            << RESET << std::endl;

  if (_vBuffer.empty() == false) {
    if (_bodyToRead > 0) {
      return (checkBodyToRead(_vBuffer));
    } else
      _statusCode = 400;
  }
  if (_statusCode != 0) {
    return (true);
  }
  std::cerr << "Client::addBuffer: end: _statusCode = " << _statusCode
            << std::endl;
  return (false);
}
