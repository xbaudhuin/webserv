#include "Client.hpp"
#include <sstream>

void Client::uriDecoder(std::string &uri) {
  size_t pos = 0;
  while (true) {
    pos = uri.find_first_of('%');
    if (pos == uri.npos)
      break;
    if (pos + 2 > uri.size()) {
      _statusCode = 400;
      logErrorClient("Client::uriDecoder: invalid uri");
      return;
    }
    std::string key = uri.substr(pos, 3);
    std::map<std::string, char>::const_iterator it = _uriEncoding.find(key);
    if (it == _uriEncoding.end()) {
      _statusCode = 400;
      logErrorClient("Client::uriDecoder: invalid uri");
      return;
    }
    uri.replace(pos, 3, 1, (*it).second);
  }
}

int Client::parseUri(const std::string &uri) {
  if (uri[0] != '/') {
    return (400);
  }

  if (uri.size() > _uriMaxSize) {
    return (414);
  }

  size_t pos = uri.find_first_of('?');
  _sUri = uri.substr(0, pos);
  uriDecoder(_sUri);
  if (_sUri.size() > 2048) {
    return (414);
  }

  if (pos != uri.npos) {
    _sQueryUri = uri.substr(pos + 1);
  }
  return (0);
}

size_t Client::parseRequestLine(const std::string &requestLine) {
  vec_string split_request = split(requestLine, " ");
  if (split_request.size() != 3) {
    _statusCode = 400;
    logErrorClient(
        "Client::parseRequestLine: invalid uri: invalid number of SP");
    return (400);
  }

  _sMethod = split_request[0];
  size_t i = 0;
  for (; i < _methodSize && _sMethod != _validMethods[i]; i++) {
  }
  if (i == _methodSize) {
    _statusCode = 400;
    logErrorClient("Client::parseRequestLine: invalid uri: unknown method");
    return (400);
  } else if (i > 2) {
    _statusCode = 405;
    logErrorClient(
        "Client::parseRequestLine: invalid uri: method not supported");
    return (405);
  }

  _statusCode = parseUri(split_request[1]);
  if (_statusCode > 0) {
    logErrorClient("Client::uriDecoder: invalid uri");
    return (_statusCode);
  }
  if (split_request[2].compare(0, 5, "HTTP/") != 0 ||
      split_request[2].size() < 8) {
    _statusCode = 400;
    logErrorClient("Client::parseRequestLine: invalid uri: no HTTP/");
    return (400);
  }
  std::string version = split_request[2].substr(5);
  if (std::isdigit(version[0]) == false) {
    _statusCode = 400;
    logErrorClient("Client::parseRequestLine: invalid uri: invalid version");
    return (400);
  }
  for (i = 1; std::isdigit(version[i]); i++) {
  }
  if (i > 1) {
    _statusCode = 405;
    logErrorClient("Client::parseRequestLine: invalid uri: invalid version");
    return (405);
  }
  if (version[i] != '.') {
    _statusCode = 400;
    logErrorClient("Client::parseRequestLine: invalid uri: invalid version");
    return (400);
  }

  i++;
  char *after = NULL;
  long int nb = std::strtol(version.c_str() + i, &after, 10);
  if (nb < 0 || nb > 999 || *after != '\0' || errno == ERANGE) {
    _statusCode = 400;
    logErrorClient("Client::parseRequestLine: invalid uri: invalid version");
    return (400);
  }
  if (nb >= 1)
    _version = 1;
  else
    _version = 0;
  return (0);
}

size_t Client::insertInMap(std::string &line,
                           std::map<std::string, std::string> &map) {
  if (line.size() > _headerMaxSize) {
    _statusCode = 431;
    logErrorClient("Client::insertInMap: invalid header");
    return (431);
  }
  std::transform(line.begin(), line.end(), line.begin(), toLower);
  size_t pos = line.find_first_of(':');
  if (pos == line.npos) {
    _statusCode = 400;
    logErrorClient("Client::insertInMap: invalid header: no ':'");
    return (400);
  }
  std::string key = line.substr(0, pos);
  if (key.find_first_of(":\r\n\x1F") != key.npos) {
    _statusCode = 400;
    logErrorClient("Client::insertInMap: invalid header: invalid char");
    return (400);
  }
  std::string value = line.substr(pos + 1);
  if (value.size() > _headerMaxSize) {
    _statusCode = 400;
    logErrorClient("Client::insertInMap: invalid header: header too large");
    return (400);
  }
  trimWhitespace(value, _whiteSpaces);
  std::map<std::string, std::string>::iterator it;
  if (pos != line.npos) {
    it = map.find(key);
    if (it != map.end()) {
      if (key == "host" || "content-length" || "transfer-encoding" ||
          "content-type") {
        _statusCode = 400;
        logErrorClient("Client::insertInMap: invalid header: unique header "
                       "already filled");
        return (400);
      }
      (*it).second += ",\n";
      (*it).second += value;
    } else if (map.insert(std::make_pair(key, value)).second == false) {
      _statusCode = 500;
      logErrorClient(
          "Client::insertInMap: invalid header: fail to insert in map");
      return (500);
    }
  }
  return (0);
}

bool Client::checkMethod(void) {

  if (_sMethod == "GET" && _location->getGetStatus() == false) {
    _statusCode = 405;
    logErrorClient("Client::checkMethod: invalid method: GET not allowed");
  } else if (_sMethod == "POST" && _location->getPostStatus() == false) {
    _statusCode = 405;
    logErrorClient("Client::checkMethod: invalid method: POST not allowed");
  } else if (_sMethod == "DELETE" && _location->getDeleteStatus() == false) {
    _statusCode = 405;
    logErrorClient("Client::checkMethod: invalid method: DELETE not allowed");
  }
  if (_statusCode < 400 && _sMethod != "POST") {
    if (_headers.count("content-length") != 0 ||
        _headers.count("transfer-encoding") != 0) {
      _statusCode = 413;
      logErrorClient("Client::checkMethod: invalid method:" + _sMethod +
                     " with body header");
    }
  }
  if (_statusCode < 400 && _sMethod == "POST") {
    std::map<std::string, std::string>::iterator it =
        _headers.find("content-type");
    if (_headers.count("transfer-encoding") == 0) {
      if (it == _headers.end()) {
        _statusCode = 400;
        logErrorClient("Client::checkMethod: invalid method:" + _sMethod +
                       " without body header");
        return (false);
      } else if ((*it).second.find("multipart") == (*it).second.npos &&
                 _headers.count("content-length") == 0) {
        _statusCode = 411;
        logErrorClient("Client::checkMethod: invalid method:" + _sMethod +
                       " without multipart or length");
      }
    }
  }
  if (_statusCode >= 400)
    return (false);
  return (true);
}

void Client::getPathUpload(void) {
  _sPathUpload = "." + _location->getUploadLocation();
  if (_sPathUpload == ".") {
    _sPathUpload = _sUri;
    if (_sPathUpload == _location->getUrl()) {
      std::stringstream ss;
      ss << getTime();
      _sPathUpload += "/";
      _sPathUpload += ss.str();
    }
  } else {
    std::string path = _sUri.substr(_location->myUri().size());
    _sPathUpload += path;
  }
  if (_sPathUpload[_sPathUpload.size() - 1] == '/') {
    std::stringstream ss;
    ss << getTime();
    _sPathUpload += "/";
    _sPathUpload += ss.str();
  }
}

bool Client::requestValidByLocation(void) {
  if (_sMethod == "POST") {
    getPathUpload();
  }
  checkPathInfo();
  if (checkMethod() == false)
    return (false);
  return (true);
}

int64_t Client::getSizeChunkFromBuffer(void) {
  size_t i = 0;
  for (; i < _vBuffer.size(); i++) {
    if (_vBuffer[i] == '\n' ||
        (_vBuffer[i] == '\r' && i + 1 < _vBuffer.size() &&
         _vBuffer[i + 1] == '\n'))
      break;
    if (isHexadecimal(_vBuffer[i]) == false) {
      _statusCode = 400;
      logErrorClient("Client::getSizeChunkFromBuffer: invalid size of chunk");
      return (0);
    }
  }
  std::string tmp(_vBuffer.begin(), _vBuffer.begin() + i);
  int64_t nb = std::strtoll(tmp.c_str(), NULL, 16);
  if (errno == ERANGE || nb < 0 ||
      (_bodyToRead > static_cast<int64_t>(_server->getLimitBodySize()) &&
       _server->getLimitBodySize() != 0)) {
    _statusCode = 413;
    logErrorClient("Client::getSizeChunkFromBuffer: invalid size of chunk");
    return (0);
  }
  _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + i);
  removeTrailingLineFromBuffer();
  return (nb);
}

void Client::removeTrailingLineFromBuffer(void) {
  size_t i = 0;
  if (_vBuffer.size() >= 2 && _vBuffer[i] == '\r' && _vBuffer[i + 1] == '\n')
    i += 2;
  else if (_vBuffer[i] == '\n')
    i++;
  else {
    _statusCode = 400;
    logErrorClient(
        "Client::removeTrailingLineFromBuffer: fail to remove trailing line");
    return;
  }
  _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + i);
}

bool Client::getTrailingHeader(void) {
  removeTrailingLineFromBuffer();
  std::string tmp(_vBuffer.begin(), _vBuffer.end());
  _vBuffer.clear();
  vec_string trailing;
  trailing = split(tmp, "\n");
  vectorToHeadersMap(trailing);
  if (_statusCode == 0)
    _statusCode = 201;
  return (true);
}

bool Client::parseChunkRequest(void) {
  if (_statusCode != 0) {
    if (_tmpFd != -1)
      close(_tmpFd);
    return (true);
  }

  if (_bodyToRead > 0) {
    size_t min = std::min(static_cast<size_t>(_bodyToRead), _vBuffer.size());
    _vBody.insert(_vBody.end(), _vBuffer.begin(), _vBuffer.begin() + min);
    _bodyToRead -= min;
    _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + min);
    if (_vBody.size() > 0)
      saveToTmpFile();
    if (_bodyToRead == 0) {
      if (hasNewLine() == -1)
        return (false);
      removeTrailingLineFromBuffer();
    }
  }

  if (_vBuffer.size() == 0 || hasNewLine() == -1)
    return (false);
  if (_bodyToRead <= 0) {
    _bodyToRead = getSizeChunkFromBuffer();
  }
  if (_bodyToRead == 0) {
    close(_tmpFd);
    return (getTrailingHeader());
  }

  return (parseChunkRequest());
}

void Client::removeReturnCarriageNewLine(std::string &line) {
  size_t i = 0;
  if (line[line.size() - 1] == '\n') {
    i++;
    if (line.size() >= 2 && line[line.size() - 2] == '\r')
      i++;
    line.erase(line.size() - i);
  }
}

bool Client::saveToTmpFile(std::vector<char> &body) {
  ssize_t writeBytes = write(_tmpFd, &(body)[0], body.size());
  if (writeBytes == -1 || static_cast<size_t>(writeBytes) < body.size()) {
    _statusCode = 500;
    logErrorClient("Client::saveToTmpFile: fail to write all bytes");
    return (false);
  }
  return (true);
}

bool Client::saveToTmpFile(void) {
  if (_tmpFd == -1) {
    if (isCgi() == true) {
      _tmpFile = _location->getCgiPath(_sUri) + "infileCgi_";
    } else
      _tmpFile = "webserv_tmpfile_";
    std::stringstream ss;
    ss << getTime();
    _tmpFile += ss.str();
    _tmpFd = open(_tmpFile.c_str(), O_CLOEXEC | O_RDWR | O_CREAT, 00644);
    if (_tmpFd == -1) {
      _statusCode = 500;
      logErrorClient("Client::saveToTmpFile: fail to open: " + _tmpFile);
      return (false);
    }
  }

  ssize_t writeBytes = write(_tmpFd, &(_vBody)[0], _vBody.size());
  if (writeBytes == -1 || static_cast<size_t>(writeBytes) < _vBody.size()) {
    _statusCode = 500;
    logErrorClient("Client::saveToTmpFile: fail to write all bytes");
    return (false);
  }

  resetVector(_vBody);
  return (true);
}

void Client::checkBodyHeader(multipartRequest &multi, std::vector<char> &body) {
  std::map<std::string, std::string>::const_iterator itHeader;
  std::map<std::string, std::string>::const_iterator itMulti;
  itHeader = _headers.find("content-type");
  itMulti = multi.header.find("content-disposition");
  if (itHeader != _headers.end()) {
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

void Client::setupBodyParsing(void) {
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
    logErrorClient("Client::setupBodyParsing: both content-length and "
                   "transfer-encoding header");
    return;
  }

  if (itLength != _headers.end()) {
    _bodyToRead = std::strtol(((*itLength).second).c_str(), NULL, 10);
    if (errno == ERANGE || _bodyToRead <= 0 ||
        (_bodyToRead > static_cast<int>(_server->getLimitBodySize()) &&
         _server->getLimitBodySize() != 0)) {
      _statusCode = 413;
      if (_bodyToRead == 0)
        _statusCode = 400;
      logErrorClient(
          "Client::setupBodyParsing: invalid size of content-length");
      return;
    }
  } else if (itChunked != _headers.end()) {
    _chunkRequest = true;
  }
}

bool Client::isCgi(void) {
  if (_sMethod != "DELETE" && _location && _location->isCgi(_sUri) == true)
    return (true);
  return (false);
}

bool Client::parseBody(void) {
  if (_vBuffer.size() == 0)
    return (_bodyToRead == 0);
  _sizeChunk += _vBuffer.size();
  if (_location && _location->getLimitBodySize() != 0 &&
      _sizeChunk > _location->getLimitBodySize()) {
    _statusCode = 413;
    logErrorClient("Client::parseChunkRequest: invalid size of body");
    return (true);
  }
  if (_multipartRequest == true) {
    bool test = parseMultipartRequest();
    return (test);
  } else if (_chunkRequest == true)
    return (parseChunkRequest());
  else {
    if (_bodyToRead > 0) {
      int64_t min =
          std::min(_bodyToRead, static_cast<int64_t>(_vBuffer.size()));
      _vBody.insert(_vBody.end(), _vBuffer.begin(), _vBuffer.begin() + min);
      _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + min);
      _bodyToRead -= min;
      saveToTmpFile();
      resetVector(_vBody);
      if (_bodyToRead > 0) {
        return (false);
      }
      _statusCode = 201;
      close(_tmpFd);
      return (true);
    }
  }
  return (_bodyToRead == 0);
}

void Client::vectorToHeadersMap(std::vector<std::string> &request) {
  for (size_t it = 0; it < request.size(); it++) {
    _requestSize += request[it].size();
    if (_requestSize > _headersMaxBuffer) {
      _statusCode = 413;
      logErrorClient("Client::vectorToHeadersMap: invalid size of request");
      return;
    }
    _statusCode = insertInMap(request[it], _headers);
    if (_statusCode != 0)
      return;
  }
}

void Client::checkPathInfo(void) {
  if (_location->hasPathInfo() == false)
    return;
  size_t locator = _location->getUrl().size();
  vec_string extension = _location->availableExtension();
  size_t i = 0;
  size_t pos = _sUri.npos;
  for (; i < extension.size(); i++) {
    pos = _sUri.find(extension[i], locator);
    if (pos != _sUri.npos) {
      break;
    }
  }
  if (i == extension.size())
    return;
  _sPathInfo = _sUri.substr(pos + extension[i].size(), _sUri.npos);
  _sUri.erase(pos + extension[i].size());
}

bool Client::earlyParsing(int newLine) {
  if (newLine) {
    std::string early(&_vBuffer[0], &_vBuffer[newLine]);
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

void Client::fillBufferWithoutReturnCarriage(const std::vector<char> &vec) {
  bool newLine = false;
  size_t it = 0;
  _vBuffer.reserve(_vBuffer.size() + vec.size());
  for (; it < vec.size(); it++) {
    if (vec[it] == '\r') {
      size_t tmp = it;
      it++;
      if (it == vec.size())
        break;
      if (vec[it] != '\n') {
        _vBuffer.push_back(*(vec.begin() + tmp));
      }
    }
    if (vec[it] == '\n') {
      if (newLine == false) {
        newLine = true;
      } else
        break;
    } else
      newLine = false;
    _vBuffer.push_back(*(vec.begin() + it));
  }

  if (it < vec.size()) {
    _vBuffer.insert(_vBuffer.end(), vec.begin() + it, vec.end());
  }
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
  return;
}

int Client::hasNewLine(void) const {
  for (size_t i = 0; i < _vBuffer.size(); i++) {
    if (_vBuffer[i] == '\n')
      return (i);
  }
  return (-1);
}

int64_t Client::hasEmptyLine(int newLine) {
  int64_t pos = -1;
  for (size_t i = newLine; i + 1 < _vBuffer.size(); i++) {
    if (_vBuffer[i] == '\n' && _vBuffer[i + 1] == '\n') {
      pos = i;
      break;
    }
  }
  return (pos);
}

bool Client::getLocation(void) {
  _server = getServerConf(_headers.at("host"));
  try {
    _location = &(_server->getPreciseLocation(_sUri));
    std::cout << YELLOW << *_location << RESET << std::endl;
  } catch (security_error &e) {
    _location = NULL;
    _statusCode = 400;
    logErrorClient("Client::getLocation: security alert: tried to go before "
                   "server root");
    return (false);
  } catch (std::exception &e) {
    _location = NULL;
    _statusCode = 404;
    logErrorClient("Client::getLocation: No location found");
    return (false);
  }
  return (true);
}

void Client::parseRequest(std::string &buffer) {
  if (buffer.empty() == true)
    return;

  std::cout << GREEN << buffer << RESET << std::endl;
  vec_string request = split(buffer, "\n");
  if (request.size() < 2) {
    _statusCode = 400;
    logErrorClient("Client::parseRequest: invalid number of header");
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
  if (requestValidByLocation() == false)
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
    logErrorClient(
        "Client::parseRequest: body size different from body size given");
    return;
  }
  if (_location && _location->isCgi(_sUri) == true) {
    setupCgi();
  }
  if (_statusCode == 0)
    _statusCode = 200;
  return;
}

bool Client::parseBuffer(std::vector<char> &buffer) {
  if (_bodyToRead > 0 || _requestIsDone == false) {
    _vBuffer.insert(_vBuffer.end(), buffer.begin(), buffer.end());
    return (parseBody());
  }
  fillBufferWithoutReturnCarriage(buffer);
  buffer.erase(buffer.begin(), buffer.begin() + _vBuffer.size());
  int newLine = hasNewLine();
  if (newLine == -1) {
    return (false);
  }
  if (_vBuffer.size() < 20 || newLine > 0) {
    if (earlyParsing(newLine) == false) {
      if (_statusCode == 0) {
        _statusCode = 400;
        logErrorClient("Client::earlyParsing: fail in earlyParsing");
      }
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
  if (_statusCode >= 400)
    return (true);
  if (_vBuffer.empty() == false && _bodyToRead == 0) {
    logErrorClient("Client::parseBuffer: buffer not empty");
    _statusCode = 400;
  }
  if (_statusCode != 0 && _cgiPid == 0) {
    return (true);
  }
  return (false);
}
bool Client::addBuffer(std::vector<char> &buffer) {
  // std::cout << "buffer: " << buffer << std::endl;
  bool ret = parseBuffer(buffer);
  std::cout << YELLOW;
  if (ret == false) {
    std::cout << "Request is not done: ";
    if (_multipartRequest == true) {
      std::cout << "multipart request: current size: " << _sizeChunk;
    } else if (_chunkRequest == true) {
      std::cout << "chunk request; current size = " << _sizeChunk
                << "; left to read in current chunk: " << _bodyToRead;
    } else {
      std::cout << "current size: " << _sizeChunk
                << "; left to read: " << _bodyToRead;
    }
  } else {
    std::cout << "Request is done: statusCode = " << _statusCode;
  }
  std::cout << RESET << std::endl;
  _time = getTime();
  return (ret);
}
