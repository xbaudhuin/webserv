#include "Client.hpp"
#include "Colors.hpp"
#include "Error.hpp"
#include "Utils.hpp"
#include <vector>

void Client::uriDecoder(std::string &uri) {
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

int Client::parseUri(const std::string &uri) {
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

size_t Client::parseRequestLine(const std::string &requestLine) {
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

size_t Client::insertInMap(std::string &line) {
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
    it = _headers.find(key);
    if (it != _headers.end()) {
      if (key == "host" || "content-length" || "transfer-encoding" ||
          "content-type")
        return (400);
      (*it).second += ",\n";
      (*it).second += value;
    } else if (_headers.insert(std::make_pair(key, value)).second == false &&
               key == "host")
      return (400);
  }
  return (0);
}

bool Client::checkMethod(void) {
  if (_sMethod == "GET" && _location->getGetStatus() == false) {
    _statusCode = 405;
    return (false);
  } else if (_sMethod == "POST" && _location->getPostStatus() == false) {
    std::cout << RED << "Client::checkMethod: invalid method" << RESET
              << std::endl;
    _statusCode = 405;
    return (false);
  } else if (_sMethod == "DELETE" && _location->getDeleteStatus() == false) {
    _statusCode = 405;
    return (false);
  }
  return (true);
}

bool Client::checkIfValid(void) {
  if (checkMethod() == false)
    return (false);
  return (true);
}

bool Client::isHexadecimal(char c) {
  if (c >= '0' && c <= '9')
    return (true);
  if (c >= 'a' && c <= 'f')
    return (true);
  if (c >= 'A' && c <= 'F')
    return (true);
  return (false);
}

int Client::getSizeChunkFromBuffer(void) {
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
      return (-1);
    }
  }
  std::string tmp(_vBuffer.begin(), _vBuffer.begin() + i);
  std::cout << YELLOW << "get nb string of size(" << i << ") = " << tmp << RESET
            << std::endl;
  int nb = std::strtol(tmp.c_str(), NULL, 16);
  if (errno == ERANGE || nb < 0 ||
      (_bodyToRead > static_cast<int>(_server->getLimitBodySize()) &&
       _server->getLimitBodySize() != 0)) {
    std::cout << RED << "FAIL IN GET NB" << RESET << std::endl;
    return (-1);
  }
  std::cout << "Client::getSizeChunkFromBuffer: " << "\n nb = " << nb
            << "; i = " << i << "\n";
  std::string out(_vBuffer.begin(), _vBuffer.begin() + i + 1);
  std::cout << "erase from buffer: " << out << std::endl;
  _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + i + 1);
  return (nb);
}

void Client::removeTrailingLineFromBuffer(void) {
  size_t i = 0;
  if (_vBuffer.size() >= 2 && _vBuffer[i] == '\r' && _vBuffer[i + 1] == '\n')
    i += 2;
  else if (_vBuffer[i] == '\n')
    i++;
  else {
    std::cout << GREEN << "_vbuffer: " << _vBuffer << RESET << std::endl;
    std::cout << RED
              << "Client::parseChunkRequest: failed to remove trailing line"
              << RESET << std::endl;
    _statusCode = 400;
    return;
  }
  _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + i);
}

bool Client::getTrailingHeader(void) {
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

bool Client::parseChunkRequest(void) {
  std::cout << YELLOW << "Client::parseChunkRequest: " << RESET << std::endl;
  std::cout << YELLOW << "bodytoRead = " << _bodyToRead << RESET << std::endl;
  std::cout << PURP << "Initial buffer: " << _vBuffer << RESET << std::endl;
  std::cout << BLUE << "Initial body: " << _vBody << RESET << std::endl;
  if (_statusCode != 0)
    return (true);
  if (static_cast<long long int>(_vBuffer.size()) < _bodyToRead) {
    _vBody.insert(_vBody.end(), _vBuffer.begin(), _vBuffer.end());
    _bodyToRead -= _vBuffer.size();
    return (false);
  }
  if (_location->getLimitBodySize() != 0 &&
      _vBody.size() > _location->getLimitBodySize()) {
    _statusCode = 413;
    return (true);
  }
  if (_bodyToRead > 0) {
    std::string tmp(_vBuffer.begin(), _vBuffer.begin() + _bodyToRead);
    std::cout << PURP << "_bodyToRead = " << _bodyToRead << RESET << std::endl;
    std::cout << PURP2 << "add to _vBody and removing from _vbuffer : " << tmp
              << RESET << std::endl;
    _vBody.insert(_vBody.end(), _vBuffer.begin(),
                  _vBuffer.begin() + _bodyToRead);
    _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + _bodyToRead);
    removeTrailingLineFromBuffer();
  }
  if (_vBuffer.size() < 2 || _statusCode >= 400) {
    _statusCode = 400;
    return (true);
  }
  _bodyToRead = getSizeChunkFromBuffer();
  if (_bodyToRead == -1) {
    _statusCode = 413;
    return (true);
  }
  if (_bodyToRead == 0) {
    return (getTrailingHeader());
  }
  return (parseChunkRequest());
}

void Client::parseBody(void) {
  std::map<std::string, std::string>::iterator itLength;
  itLength = _headers.find("content-length");
  std::map<std::string, std::string>::iterator itChunked;
  itChunked = _headers.find("transfer-encoding");
  if (itChunked != _headers.end() && itLength != _headers.end()) {
    _statusCode = 400;
    std::cout << RED
              << "both content-length and transfer-encoding headers found"
              << RESET << std::endl;
  }
  if (itLength != _headers.end()) {
    if (_sMethod != "POST") {
      _statusCode = 413;

      std::cout << RED
                << "changin statusCode because if (_sMethod != \"POST\")to "
                << _statusCode << RESET << std::endl;

      return;
    }
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
    if (_sMethod != "POST") {
      _statusCode = 413;

      std::cout << RED
                << "changin statusCode because if (_sMethod != \"POST\")to "
                << _statusCode << RESET << std::endl;

      return;
    }
    _chunkRequest = true;
    parseChunkRequest();
    return;
  }
}

void Client::vectorToHeadersMap(std::vector<std::string> &request) {
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
    _statusCode = insertInMap(request[it]);
    if (_statusCode != 0)
      return;
  }
}

void Client::checkPathInfo(void) {
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

void Client::parseRequest(std::string &buffer) {
  if (buffer.empty() == true)
    return;
  vec_string request = split(buffer, "\n");

  std::cout << YELLOW << "vec_string request.size() = " << request.size()
            << RESET << std::endl;

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
  if (_headers.count("host") == 0) {
    _statusCode = 400;

    std::cout
        << RED
        << "changin statusCode because if (_headers.count(\"host\") == 0) to "
        << _statusCode << RESET << std::endl;

    return;
  }

  std::cout << RED << "Before at" << RESET << std::endl;

  _sHost = _headers.at("host");

  std::cout << YELLOW << "HERE: " << "host : " << _headers.at("host") << RESET
            << std::endl;

  _server = getServerConf();

  std::cout << YELLOW << "serverName = " << _server->getMainServerName()
            << " on port: " << _server->getPort() << RESET << std::endl;

  try {
    _location = &(_server->getPreciseLocation(_sUri));
    checkPathInfo();
    std::cout << YELLOW << "location = " << _location->getUrl() << RESET
              << std::endl;

  } catch (security_error &e) {
    _location = NULL;
    _statusCode = 400;

    std::cout << RED << "caught security_error" << RESET << std::endl;

    return;
  } catch (std::exception &e) {
    _location = NULL;
    _statusCode = 404;

    std::cout << RED << "changin statusCode because caught exception to "
              << _statusCode << RESET << std::endl;

    return;
  }
  if (_location == NULL) {

    std::cout << RED << "THOMAS EST NULL" << RESET << std::endl;

  } else {

    std::cout << RED << "location = " << _location->getUrl() << RESET
              << std::endl;
  }
  if (checkIfValid() == false)
    return;
  parseBody();

  if (_bodyToRead > 0) {

    std::cout << PURP << "_vBodysize = " << _bodyToRead
              << "; body.size() = " << _vBody.size() << RESET << std::endl;

    _vBody.insert(_vBody.end(), _vBuffer.begin(),
                  _vBuffer.begin() + _bodyToRead);
    if (static_cast<int>(_vBuffer.size()) >= _bodyToRead)
      _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + _bodyToRead);
    _bodyToRead -= _vBody.size();
    if (_bodyToRead <= 0)
      _bodyToRead = -1;
  } else {

    std::cout << PURP << "No body" << RESET << std::endl;
  }
  if (_vBuffer.size() != 0) {
    _statusCode = 400;
    std::cout << GREEN << "body = " << _vBody << RESET << std::endl;
    std::cout << RED << "buffer != 0, buffer.size() = " << _vBuffer.size()
              << "; buffer : " << _vBuffer << RESET << std::endl;
    std::cout << RED
              << "changin statusCode because if (_vBuffer.size() != 0) to "
              << _statusCode << RESET << std::endl;
  }
  if (_location && _location->isCgi(_sUri) == true) {
    setupCgi();
  }
  if (_statusCode == 0)
    _statusCode = 200;
  return;
}

bool Client::earlyParsing(int newLine) {

  std::cout << PURP << "EARLY PARSING: _vBuffer.size() = " << _vBuffer.size()
            << RESET << std::endl;

  if (newLine) {

    std::cout << PURP << "earlyPARSING: NEWLINE" << RESET << std::endl;

    std::string early(&_vBuffer[0], &_vBuffer[newLine]);
    std::cout << BLUE << "earlyParsing: early = " << early << RESET
              << std::endl;
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

void Client::removeReturnCarriage(std::vector<char> &vec) {

  _vBuffer.reserve(_vBuffer.size() + vec.size());
  for (size_t it = 0; it < vec.size(); it++) {
    if (vec[it] == '\r') {
      size_t tmp = it;
      it++;
      if (it == vec.size())
        break;
      if (vec[it] != '\n') {
        _vBuffer.push_back(*(vec.begin() + tmp));
      }
    }
    _vBuffer.push_back(*(vec.begin() + it));
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

  std::cout << BLUE << "vec: " << vec << RESET << std::endl;
  std::cout << PURP << "buffer: " << _vBuffer << RESET << std::endl;

  return;
}

size_t Client::hasNewLine(void) const {
  for (size_t i = 0; i < _vBuffer.size(); i++) {
    if (_vBuffer[i] == '\n')
      return (i);
  }
  return (0);
}

bool Client::checkBodyToRead(std::vector<char> buffer) {

  std::cout << RED << "_bodyToRead > 0" << RESET << std::endl;

  _vBody.insert(_vBody.end(), buffer.begin(), buffer.end());
  if (_chunkRequest == true) {
    return (parseChunkRequest());
  }
  if (buffer.size() > static_cast<long unsigned int>(_bodyToRead)) {
    _statusCode = 413;
  }
  _bodyToRead -= buffer.size();
  _time = getTime();
  if (_bodyToRead <= 0 || _statusCode != 0)
    return (true);
  return (false);
}

bool Client::addBuffer(std::vector<char> buffer) {

  std::cout << BLUE << "Client::addBuffer: " << buffer << RESET << std::endl;
  if (_bodyToRead > 0) {
    return (checkBodyToRead(buffer));
  }
  // std::cout << YELLOW << "buffer:\n" << _vBuffer << RESET << std::endl;
  // size_t pos = _vBuffer.find("\n\n", _requestSize);
  // if (pos == _vBuffer.npos) {
  //   std::cout << RED << "No empty line in buffer" << RESET << std::endl;
  //   _time = getTime();
  //   return (false);
  // }

  removeReturnCarriage(buffer);
  int newLine = hasNewLine();

  std::cout << PURP << "earlyPARSING: NEWLINE = " << newLine << RESET
            << std::endl;

  if (_vBuffer.size() < 20 || newLine > 0) {
    if (earlyParsing(newLine) == false) {
      _statusCode = 400;
      _server = _defaultConf;
      return (true);
    }
  }
  if (newLine == 0) {
    return (false);
  }
  int pos = -1;
  for (size_t i = newLine; i + 1 < _vBuffer.size(); i++) {
    if (_vBuffer[i] == '\n' && _vBuffer[i + 1] == '\n') {
      pos = i;

      std::cout << GREEN << "FOUND EMPTY LINE" << RESET << std::endl;

      break;
    }
  }
  if (pos == -1) {

    std::cout << RED << "No empty line in buffer" << RESET << std::endl;

    _time = getTime();
    return (false);
  }

  std::string request(&_vBuffer[0], &_vBuffer[pos]);

  std::cout << RED << "_vbuffer[pos] = " << _vBuffer[pos] << "\n";
  std::cout << RED << "_vbuffer[pos + 1] = " << _vBuffer[pos + 1] << "\n";
  // std::cout << RED << "_vbuffer[pos + 2] = " << _vBuffer[pos + 2] << "\n";
  //
  _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + pos + 2);

  std::cout << YELLOW << "buffer after request: " << _vBuffer << RESET
            << std::endl;
  std::cout << PURP << "requestLine: " << request << RESET << std::endl;

  // _vBuffer = _vBuffer.substr(pos, _vBuffer.size() - pos);
  // std::cout << RED << "separating request from buffer\n" << RESET;
  // std::cout << GREEN << "request:\n" << request << RESET;
  // std::cout << YELLOW << "buffer:\n" << _vBuffer << RESET << std::endl;
  std::cout << RED << "End before parseRequest; StatusCode = " << _statusCode
            << RESET << std::endl;

  parseRequest(request);

  std::cout << RED << "End after parseRequest; StatusCode = " << _statusCode
            << RESET << std::endl;

  if (_vBuffer.empty() == false) {
    if (_bodyToRead > 0) {
      return (checkBodyToRead(_vBuffer));
    } else
      _statusCode = 400;
  }
  _time = getTime();
  if (_statusCode != 0 && _cgiPid == 0) {
    return (true);
  }
  std::cerr << "Client::addBuffer: end: _statusCode = " << _statusCode
            << std::endl;
  return (false);
}
