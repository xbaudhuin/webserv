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

std::string Request::getLineFromBuffer() {
  bool carriage = false;
  size_t i = 0;
  for (; i < _vBuffer.size(); i++) {
    if (_vBuffer[i] == '\r' && i + 1 < _vBuffer.size())
      carriage = true;
    else
      carriage = false;
    if (_vBuffer[i + carriage] == '\n')
      break;
  }
  if (i + carriage < _vBuffer.size())
    i++;
  std::string line(_vBuffer.begin(), _vBuffer.begin() + i + carriage);
  _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + i + carriage);
  return (line);
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
  if (_statusCode < 400 && _sMethod != "POST") {
    if (_headers.count("content-length") != 0 ||
        _headers.count("transfer-encoding") != 0)
      _statusCode = 413;
    std::map<std::string, std::string>::iterator it =
        _headers.find("content-type");
    if ((*it).second.find("multipart") != (*it).second.npos)
      _statusCode = 413;
  }
  if (_statusCode >= 400)
    return (false);
  return (true);
}

void Request::getPathUpload(void) {
  _sPathUpload = _location->getUploadLocation();
  if (_sPathUpload.empty() == true) {
    _sPathUpload = _sUri;
    if (_sPathUpload == _location->getUrl()) {
      std::stringstream ss;
      ss << getTime();
      _sPathUpload += "/";
      _sPathUpload += ss.str();
    }
  }
}

bool Request::requestValidbyLocation(void) {
  if (_sMethod == "POST") {
    getPathUpload();
  }
  checkPathInfo();
  if (checkMethod() == false)
    return (false);
  return (true);
}

std::string Request::getBoundaryString(std::string &boundaryHeader) {
  size_t pos;
  std::cout << YELLOW
            << "Client::getBoundaryString: starting: boundaryHeader = "
            << boundaryHeader << RESET << std::endl;
  pos = boundaryHeader.find(';');
  if (pos == boundaryHeader.npos) {
    std::cout << RED << "Client::getBoundaryString: pos = npos " << RESET
              << std::endl;
    _statusCode = 400;
    return ("");
  }
  pos++;
  if (boundaryHeader[pos] == ' ')
    pos++;
  if (pos + 11 >= boundaryHeader.size() ||
      boundaryHeader.substr(pos, 11) != "boundary=--") {
    std::cout << RED << "Client::getBoundaryString: pos(" << pos
              << ") + 11 > boundaryHeader.size(" << boundaryHeader.size()
              << ") " << RESET << std::endl;
    std::cout << RED << "boundaryHeader.substr(pos, pos +11) != boundary=-- :"
              << boundaryHeader.substr(pos, 11) << RESET << std::endl;
    ;
    _statusCode = 400;
    return ("");
  }
  std::cout << YELLOW << "Client::getBoundaryString: BOUNDARY = " << "----"
            << boundaryHeader.substr(pos + 11) << RESET << std::endl;
  return ("----" + boundaryHeader.substr(pos + 11));
}

bool Request::checkBodyMultipartCgi(std::string &boundary) {
  _vBody.insert(_vBody.end(), _vBuffer.begin(), _vBuffer.end());
  std::string tmp(_vBuffer.begin(), _vBuffer.end());
  _vBuffer.erase(_vBuffer.begin(), _vBuffer.end());
  size_t pos = tmp.rfind(boundary + "--");
  if (pos != tmp.npos)
    return (true);
  return (false);
}

bool Request::checkBoundary(void) {
  if (_vBuffer.size() < _boundary.size())
    return (false);
  std::string tmp(_vBuffer.begin(), _vBuffer.end());
  if (tmp.find(_boundary) != 0) {
    _statusCode = 400;
    return (true);
  }
  if (tmp.find(_boundary, _boundary.size()) == tmp.npos) {
    return (false);
  }
  return (true);
}

void Request::removeTrailingLineFromBuffer(void) {
  size_t i = 0;
  if (_vBuffer.size() >= 2 && _vBuffer[i] == '\r' && _vBuffer[i + 1] == '\n')
    i += 2;
  else if (_vBuffer[i] == '\n')
    i++;
  else {
    // std::cout << GREEN << "_vbuffer: " << _vBuffer << RESET << std::endl;
    std::cout
        << RED
        << "Client:removeTrailingLineFromBuffer: failed to remove trailing line"
        << RESET << std::endl;
    _statusCode = 400;
    return;
  }
  _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + i);
}

bool Request::checkHeaderMulti(multipartRequest &multi) {
  if (multi.header.count("content-type") == 0) {
    _statusCode = 400;
    return (false);
  }
  if (multi.header.count("content-disposition") == 0) {
    _statusCode = 400;
    return (false);
  }
  std::string tmp = multi.header.at("content-disposition");
  size_t pos = tmp.find("name=\"");
  if (pos == tmp.npos || pos + 7 >= tmp.size()) {
    _statusCode = 422;
    return (false);
  }
  pos += 7;
  pos = tmp.find_first_of('\"');
  if (pos == tmp.npos) {
    _statusCode = 422;
    return (false);
  }
  pos = tmp.find("filename=\"", pos);
  if (pos == tmp.npos) {
    _statusCode = 422;
    return (false);
  }
  pos += 10;
  pos = tmp.find_first_of('\"', pos);
  if (pos == tmp.npos) {
    _statusCode = 422;
    return (false);
  }
  return (true);
}

void Request::checkBodyHeader(std::map<std::string, std::string> header,
                              std::vector<char> &body) {
  std::map<std::string, std::string>::const_iterator itHeader;
  std::map<std::string, std::string>::const_iterator itMulti;
  itHeader = header.find("content-type");
  itMulti = header.find("content-disposition");
  if (itHeader != header.end()) {
    if (((*itHeader).second.find("application/x-www-form-urlencoded") !=
         (*itHeader).second.npos) ||
        ((*itMulti).second.find("application/x-www-form-urlencoded") !=
         (*itMulti).second.npos)) {
      std::string url(body.begin(), body.end());
      uriDecoder(url);
      std::vector<char> tmp(url.begin(), url.end());
      body = tmp;
    }
  }
}

bool Request::getMultipartBody(multipartRequest &multi) {
  std::string line;
  while (true) {
    line = getLineFromBuffer();
    std::cout << BLUE << "New line: " << line << RESET << std::endl;
    if (line.compare(0, _boundary.size(), _boundary) == 0 &&
        line.size() >= _boundary.size() + 1) {
      if (line.compare(_boundary.size(), 2, "--") == 0) {
        std::cout << GREEN << "Client::multipartRequest: compare = end" << RESET
                  << std::endl;
        _statusCode = 200;
        return (true);
        removeTrailingLineFromBuffer();
      }
      break;
    } else {
      multi.body.insert(multi.body.end(), line.begin(), line.end());
    }
  }
  std::vector<char> tmp;
  checkBodyHeader(multi.header, tmp);
  return (false);
}

bool Request::saveToTmpFile(int fd, std::string &filename,
                            std::vector<char> body) {
  ssize_t writeBytes = write(fd, &(body)[0], body.size());
  if (writeBytes == -1 || writeBytes < body.size()) {
    _statusCode = 500;
    return (false);
  }
  return (true);
}

bool Request::saveToTmpFile(void) {
  if (_tmpFd == -1) {
    if (isCgi() == true)
      _tmpFile = _location->getCgiPath(_sUri) + "infileCgi_";
    else
      _tmpFile = "webserv_tmpfile_";
    std::stringstream ss;
    ss << getTime();
    _tmpFile += ss.str();
  }
  _tmpFd = open(_tmpFile.c_str(), O_CLOEXEC | O_RDWR | O_CREAT);
  if (_tmpFd == -1) {
    _statusCode = 500;
    return (false);
  }
  ssize_t writeBytes = write(_tmpFd, &(_vBody)[0], _vBody.size());
  if (writeBytes == -1 || writeBytes < _vBody.size()) {
    _statusCode = 500;
    return (false);
  }
  resetVector(_vBody);
  return (true);
}

bool Request::saveMultiToTmpfile(void) {
  multipartRequest &multi = _multipart.back();
  std::stringstream ss;
  ss << _multipart.size();
  multi.tmpFilename = "webserv_tmpmulti" + ss.str();
  ss.clear();
  ss << getTime();
  multi.tmpFilename += "id" + ss.str();
  int fd = open(multi.tmpFilename.c_str(), O_RDWR | O_CLOEXEC);
  if (fd == -1) {
    _statusCode = 500;
    return (false);
  }
  bool ret = saveToTmpFile(fd, multi.tmpFilename, multi.body);
  close(fd);
  resetVector(multi.body);
  return (ret);
}

bool Request::parseMultipartRequest(std::string &boundary) {
  if (_location && _location->isCgi(_sUri) == true)
    return (checkBodyMultipartCgi(boundary));
  std::cout << YELLOW
            << "Client::parseMultipartRequest starting: boundary = " << boundary
            << RESET << std::endl;
  if (_vBuffer.size() < boundary.size() && _vBuffer.size() > 0) {
    return (false);
  }
  if (checkBoundary() == false)
    return (false);
  if (_statusCode >= 400)
    return (true);
  _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + boundary.size());
  removeTrailingLineFromBuffer();
  multipartRequest multi;
  std::string line = getLineFromBuffer();
  while (line.empty() == false && _statusCode < 400) {
    removeReturnCarriageNewLine(line);
    std::cout << BLUE << "New line: " << line << RESET << std::endl;
    if (line == "")
      break;
    insertInMap(line, multi.header);
    line = getLineFromBuffer();
  }
  if (checkHeaderMulti(multi) == false)
    return (true);
  if (_statusCode >= 400) {
    std::cout << RED
              << "Client::parseMultipartRequest: afterHeader insertion: "
                 "statusCode = 400"
              << RESET << std::endl;
    return (true);
  }
  while (true) {
    if (_vBuffer.size() < boundary.size()) {
      std::cout << RED << "CLient::multipartRequest: _vBuffer too small"
                << RESET << std::endl;
      return (false);
    }
    line = getLineFromBuffer();
    std::cout << BLUE << "New line: " << line << RESET << std::endl;
    if (line.compare(0, boundary.size(), boundary) == 0 &&
        line.size() >= boundary.size() + 1) {
      break;
    } else
      getMultipartBody(multi);
  }
  _multipart.push_back(multi);
  if (saveMultiToTmpfile() == false)
    return (true);
  if (line.compare(boundary.size(), 2, "--") == 0) {
    std::cout << GREEN << "Client::multipartRequest: compare = end" << RESET
              << std::endl;
    _statusCode = 200;
    removeTrailingLineFromBuffer();
    return (true);
  }
  parseMultipartRequest(boundary);
  return (false);
}

int64_t Request::getSizeChunkFromBuffer(void) {
  size_t i = 0;
  std::cout << YELLOW << "CLIENT::GETSIZECHUNKFROMBUFFER " << RESET
            << std::endl;
  std::cout << PURP << "Initial buffer: " << _vBuffer << RESET << std::endl;
  std::cout << BLUE << "Initial body: " << _vBody << RESET << std::endl;
  for (; i < _vBuffer.size(); i++) {
    if (_vBuffer[i] == '\n')
      break;
    if (isHexadecimal(_vBuffer[i]) == false) {
      std::cout << RED << "is not hexadecimal i(" << i << "): " << _vBuffer[i]
                << RESET << std::endl;
      _statusCode = 400;
      return (-1);
    }
  }
  std::string tmp(_vBuffer.begin(), _vBuffer.begin() + i);
  std::cout << YELLOW << "get nb string of size(" << i << ") = " << tmp << RESET
            << std::endl;
  int64_t nb = std::strtoll(tmp.c_str(), NULL, 16);
  if (errno == ERANGE || nb < 0 ||
      (_bodyToRead > static_cast<int64_t>(_server->getLimitBodySize()) &&
       _server->getLimitBodySize() != 0)) {
    std::cout << RED << "FAIL IN GET NB" << RESET << std::endl;
    _statusCode = 413;
    return (-1);
  }
  std::cout << "Client::getSizeChunkFromBuffer: " << "\n nb = " << nb
            << "; i = " << i << "\n";
  std::string out(_vBuffer.begin(), _vBuffer.begin() + i + 1);
  std::cout << "erase from buffer: " << out << std::endl;
  _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + i + 1);
  return (nb);
}

bool Request::getTrailingHeader(void) {
  std::map<std::string, std::string>::iterator it;
  std::vector<std::string> trailer;
  it = _headers.find("trailer");
  if (it != _headers.end()) {
    std::string header = (*it).second;
    size_t pos = 0;
    size_t start = 0;
    while (true) {
      pos = header.find(",\n");
      if (pos == header.npos)
        break;
      trailer.insert(trailer.end(), header.substr(start, pos));
      start = pos;
    }
  }
  std::vector<std::string> vec;
  bool carriage = false;
  for (size_t i = 0; i < _vBuffer.size(); i++) {
    if (_vBuffer[i] == '\n') {
      if (i > 0 && _vBuffer[i - 1] == '\r') {
        carriage = true;
      }
      std::string tmp(_vBuffer.begin(), _vBuffer.begin() + i - carriage);
      _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + i);
      removeTrailingLineFromBuffer();
      if (_statusCode == 400)
        return (true);
      vec.push_back(tmp);
      carriage = false;
      if (_vBuffer.empty() == true)
        break;
    }
  }
  vectorToHeadersMap(vec);
  if (_statusCode != 0)
    return (true);
  for (size_t i = 0; i < trailer.size(); i++) {
    it = _headers.find(trailer[i]);
    if (it == _headers.end()) {
      _statusCode = 400;
      return (true);
    }
  }
  _chunkRequest = false;
  return (true);
}

bool Request::parseChunkRequest(void) {
  std::cout << YELLOW << "Client::parseChunkRequest: " << RESET << std::endl;
  std::cout << YELLOW << "bodytoRead = " << _bodyToRead << RESET << std::endl;
  std::cout << PURP << "Initial buffer: " << _vBuffer << RESET << std::endl;
  std::cout << BLUE << "Initial body: " << _vBody << RESET << std::endl;
  if (_statusCode != 0)
    return (true);
  if (static_cast<int64_t>(_vBuffer.size()) < _bodyToRead && _bodyToRead > 0) {
    _vBody.insert(_vBody.end(), _vBuffer.begin(), _vBuffer.end());
    _bodyToRead -= _vBuffer.size();
    return (false);
  }
  if (_bodyToRead > 0) {
    size_t min = std::min(static_cast<size_t>(_bodyToRead), _vBuffer.size());
    std::string tmp(_vBuffer.begin(), _vBuffer.begin() + min);
    std::cout << PURP << "_bodyToRead = " << _bodyToRead << RESET << std::endl;
    std::cout << PURP2 << "add to _vBody and removing from _vbuffer : " << tmp
              << RESET << std::endl;
    _vBody.insert(_vBody.end(), _vBuffer.begin(), _vBuffer.begin() + min);
    if (_bodyToRead > min)
      _bodyToRead -= _vBuffer.size();
    else {
      ssize_t bytesWrite = write(_tmpFd, &_vBody[0], _vBody.size());
      if (bytesWrite != _vBody.size()) {
        _statusCode = 500;
        return (true);
      }
      resetVector(_vBody);
      _bodyToRead = 0;
    }
    _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + min);
    removeTrailingLineFromBuffer();
  }
  if (_vBuffer.size() < 2)
    return (false);
  if (_bodyToRead == 0) {
    _bodyToRead = getSizeChunkFromBuffer();
    _sizeChunk += _bodyToRead;
    if (_location->getLimitBodySize() != 0 &&
        _sizeChunk > _location->getLimitBodySize()) {
      _bodyToRead = -1;
    }
  }
  if (_bodyToRead == -1) {
    _statusCode = 413;
    return (true);
  }
  if (_bodyToRead == 0) {
    close(_tmpFd);
    return (getTrailingHeader());
  }
  return (parseChunkRequest());
}

bool Request::parseBody(void) {
  if (_vBuffer.size() == 0)
    return (_bodyToRead == 0);
  if (_multipartRequest == true)
    return (parseMultipartRequest(_boundary));
  else if (_chunkRequest == true)
    return (parseChunkRequest());
  else {
    if (_bodyToRead > 0) {
      int64_t min =
          std::min(_bodyToRead, static_cast<int64_t>(_vBuffer.size()));
      _vBody.insert(_vBody.end(), _vBuffer.begin(), _vBuffer.begin() + min);
      _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + min);
      _bodyToRead -= min;
      if (_bodyToRead != 0)
        // if (saveToTmpFile() == false)
        // return (false);
        return (_bodyToRead == 0);
    }
  }
  return (_bodyToRead == 0);
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
    if (isCgi() == true)
      _tmpFile = _location->getCgiPath(_sUri);
    else
      _tmpFile = "webserv_tmpfile_";
    time_t time = getTime();
    std::stringstream t;
    t << time;
    _tmpFile += "id" + t.str();
    std::cout << "tmpFile = " << _tmpFile << std::endl;
    _tmpFd = open(_tmpFile.c_str(), O_CREAT | O_RDWR | O_TRUNC | O_CLOEXEC);
    if (_tmpFd == -1) {
      _statusCode = 500;
      return;
    }
  }
}

bool Request::isCgi(void) {
  if (_location && _location->isCgi(_sUri) == true)
    return (true);
  return (false);
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
      _requestIsDone = false;
      return;
    }
  }
  if (_vBuffer.size() != 0 && _bodyToRead == 0) {
    _statusCode = 400;
    return;
  }
  // if (_location && _location->isCgi(_sUri) == true) {
  //   setupCgi();
  // }
  if (_statusCode == 0)
    _statusCode = 200;
  return;
}

bool Request::addBuffer(const std::vector<char> buffer) {
  if (_bodyToRead > 0 || _requestIsDone == false) {
    _vBuffer.insert(_vBuffer.end(), buffer.begin(), buffer.end());
    return (parseBody());
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
      _vBuffer.insert(_vBuffer.end(), buffer.begin(), buffer.end());
      return (parseBody());
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
