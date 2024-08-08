#include "Client.hpp"

void Client::uriDecoder(std::string &uri) {
  size_t pos = 0;
  while (true) {
    pos = uri.find_first_of('%');
    if (pos == uri.npos)
      break;
    if (pos + 2 > uri.size()) {
      logErrorClient("Client::uriDecoder: invalid uri" + uri);
      _statusCode = 400;
      return;
    }
    std::string key = uri.substr(pos, 3);
    std::map<std::string, char>::const_iterator it = _uriEncoding.find(key);
    if (it == _uriEncoding.end()) {
      logErrorClient("Client::uriDecoder: invalid uri" + uri);
      _statusCode = 400;
      return;
    }
    uri.replace(pos, 3, 1, (*it).second);
  }
}

int Client::parseUri(const std::string &uri) {
  if (uri[0] != '/') {
    logErrorClient("Client::uriDecoder: invalid uri: uri without /" + uri);
    return (400);
  }
  if (uri.size() > _uriMaxSize) {
    logErrorClient("Client::uriDecoder: invalid uri: uri too large: " + uri);
    return (414);
  }
  size_t pos = uri.find_first_of('?');
  _sUri = uri.substr(0, pos);
  uriDecoder(_sUri);
  if (_sUri.size() > 2048) {
    logErrorClient("Client::uriDecoder: invalid uri: uri too large: " + uri);
    return (414);
  }
  if (pos != uri.npos) {
    _sQueryUri = uri.substr(pos + 1);
  }
  return (0);
}

size_t Client::parseRequestLine(const std::string &requestLine) {
  // std::cout << BLUE << "requestLine = " << requestLine << RESET << std::endl;
  vec_string split_request = split(requestLine, " ");
  if (split_request.size() != 3) {
    logErrorClient(
        "Client::parseRequestLine: invalid uri: invalid number of SP");
    return (400);
  }
  _sMethod = split_request[0];
  size_t i = 0;
  for (; i < _methodSize && _sMethod != _validMethods[i]; i++) {
  }
  if (i == _methodSize) {
    logErrorClient("Client::parseRequestLine: invalid uri: unknown method");
    return (400);
  } else if (i > 2) {
    logErrorClient(
        "Client::parseRequestLine: invalid uri: method not supported");
    return (405);
  }
  _statusCode = parseUri(split_request[1]);
  if (_statusCode > 0) {
    return (_statusCode);
  }
  if (split_request[2].compare(0, 5, "HTTP/") != 0 ||
      split_request[2].size() < 8) {
    logErrorClient("Client::parseRequestLine: invalid uri: no HTTP/");
    return (400);
  }
  std::string version = split_request[2].substr(5);
  if (std::isdigit(version[0]) == false) {
    logErrorClient("Client::parseRequestLine: invalid uri: invalid version");
    return (400);
  }
  for (i = 1; std::isdigit(version[i]); i++) {
  }
  if (i > 1) {
    logErrorClient("Client::parseRequestLine: invalid uri: invalid version");
    return (405);
  }
  if (version[i] != '.') {
    logErrorClient("Client::parseRequestLine: invalid uri: invalid version");
    return (400);
  }
  i++;
  char *after = NULL;
  long int nb = std::strtol(version.c_str() + i, &after, 10);
  if (nb < 0 || nb > 999 || *after != '\0' || errno == ERANGE) {
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
  if (line.size() > _headerMaxSize)
    return (431);
  std::transform(line.begin(), line.end(), line.begin(), toLower);
  size_t pos = line.find_first_of(':');
  if (pos == line.npos) {
    logErrorClient("Client::insertInMap: invalid header: no ':'");
    return (400);
  }
  std::string key = line.substr(0, pos);
  if (key.find_first_of(":\r\n\x1F") != key.npos) {
    logErrorClient("Client::insertInMap: invalid header: invalid char");
    return (400);
  }
  std::string value = line.substr(pos + 1);
  if (value.size() > _headerMaxSize) {
    logErrorClient("Client::insertInMap: invalid header: header too large");
    return (400);
  }
  trimWhitespace(value, _whiteSpaces);
  // std::cout << "KEY = " << key;
  // std::cout << "; VALUE = " << value << std::endl;
  std::map<std::string, std::string>::iterator it;
  if (pos != line.npos) {
    it = map.find(key);
    if (it != map.end()) {
      if (key == "host" || "content-length" || "transfer-encoding" ||
          "content-type") {
        logErrorClient("Client::insertInMap: invalid header: unique header "
                       "already filled");
        return (400);
      }
      (*it).second += ",\n";
      (*it).second += value;
    } else if (map.insert(std::make_pair(key, value)).second == false) {
      logErrorClient(
          "Client::insertInMap: invalid header: fail to insert in map");
      return (500);
    }
  }
  return (0);
}

bool Client::checkMethod(void) {

  if (_sMethod == "GET" && _location->getGetStatus() == false) {
    logErrorClient("Client::checkMethod: invalid method: GET not allowed: " +
                   _sUri);
    _statusCode = 405;
  } else if (_sMethod == "POST" && _location->getPostStatus() == false) {
    logErrorClient("Client::checkMethod: invalid method: POST not allowed" +
                   _sUri);
    _statusCode = 405;
  } else if (_sMethod == "DELETE" && _location->getDeleteStatus() == false) {
    logErrorClient("Client::checkMethod: invalid method: DELETE not allowed" +
                   _sUri);
    _statusCode = 405;
  }
  if (_statusCode < 400 && _sMethod != "POST") {
    if (_headers.count("content-length") != 0 ||
        _headers.count("transfer-encoding") != 0) {
      logErrorClient("Client::checkMethod: invalid method:" + _sMethod +
                     " with body header");
      _statusCode = 413;
    }
  }
  if (_statusCode < 400 && _sMethod == "POST") {
    std::map<std::string, std::string>::iterator it =
        _headers.find("content-type");
    if (_headers.count("transfer-encoding") == 0) {
      if (it == _headers.end()) {
        logErrorClient("Client::checkMethod: invalid method:" + _sMethod +
                       " without body header");
        _statusCode = 400;
        return (false);
      } else if ((*it).second.find("multipart") == (*it).second.npos &&
                 _headers.count("content-length") == 0) {
        logErrorClient("Client::checkMethod: invalid method:" + _sMethod +
                       " without multipart or length");
        _statusCode = 411;
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
  std::cout << PURP << "uploadpath : " << _sPathUpload << RESET << std::endl;
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
  // std::cout << YELLOW << "CLIENT::GETSIZECHUNKFROMBUFFER " << RESET
  // << std::endl;

  // std::cout << PURP << "Initial buffer: " << _vBuffer << RESET << std::endl;
  // std::cout << BLUE << "Initial body: " << _vBody << RESET << std::endl;
  for (; i < _vBuffer.size(); i++) {
    if (_vBuffer[i] == '\n' ||
        (_vBuffer[i] == '\r' && i + 1 < _vBuffer.size() &&
         _vBuffer[i + 1] == '\n'))
      break;
    if (isHexadecimal(_vBuffer[i]) == false) {
      logErrorClient("Client::getSizeChunkFromBuffer: invalid size of chunk");
      _statusCode = 400;
      return (0);
    }
  }
  std::string tmp(_vBuffer.begin(), _vBuffer.begin() + i);
  int64_t nb = std::strtoll(tmp.c_str(), NULL, 16);
  if (errno == ERANGE || nb < 0 ||
      (_bodyToRead > static_cast<int64_t>(_server->getLimitBodySize()) &&
       _server->getLimitBodySize() != 0)) {
    logErrorClient("Client::getSizeChunkFromBuffer: invalid size of chunk");
    _statusCode = 413;
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
    // std::cout << GREEN << "_vbuffer: " << _vBuffer << RESET << std::endl;
    logErrorClient(
        "Client::removeTrailingLineFromBuffer: fail to remove trailing line");
    _statusCode = 400;
    return;
  }
  _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + i);
}

bool Client::getTrailingHeader(void) {
  std::cout << GREEN
            << "Client::getTrailingHeader: start: _statusCode = " << _statusCode
            << "; buffer = " << _vBuffer << RESET << std::endl;
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
  std::cout << YELLOW << "Client::parseChunkRequest: " << RESET << std::endl;
  std::cout << YELLOW << "bodytoRead = " << _bodyToRead << RESET << std::endl;
  std::cout << YELLOW << "buffer.size() = " << _vBuffer.size() << RESET
            << std::endl;
  // std::cout << PURP << "Initial buffer: " << _vBuffer << RESET << std::endl;
  // std::cout << BLUE << "Initial body: " << _vBody << RESET << std::endl;
  if (_statusCode != 0) {
    if (_tmpFd != -1)
      close(_tmpFd);
    return (true);
  }
  if (_bodyToRead > 0) {
    size_t min = std::min(static_cast<size_t>(_bodyToRead), _vBuffer.size());
    // std::string tmp(_vBuffer.begin(), _vBuffer.begin() + min);
    // std::cout << PURP << "_bodyToRead = " << _bodyToRead << RESET <<
    // std::endl; std::cout << PURP2 << "add to _vBody and removing from
    // _vbuffer : " << tmp
    // << RESET << std::endl;
    _vBody.insert(_vBody.end(), _vBuffer.begin(), _vBuffer.begin() + min);
    _bodyToRead -= min;
    _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + min);
    std::cout << BLUE << "insert and removing size of " << min << RESET
              << std::endl;
    if (_vBody.size() > 0)
      saveToTmpFile();
    if (_bodyToRead == 0)
      removeTrailingLineFromBuffer();
  }
  if (hasNewLine() == false)
    return (false);
  if (_bodyToRead <= 0) {
    _bodyToRead = getSizeChunkFromBuffer();
    _sizeChunk += _bodyToRead;
    if (_location->getLimitBodySize() != 0 &&
        static_cast<size_t>(_sizeChunk) > _location->getLimitBodySize()) {
      logErrorClient("Client::parseChunkRequest: invalid size of body");
      _statusCode = 413;
    }
  }
  if (_bodyToRead == 0) {
    close(_tmpFd);
    return (getTrailingHeader());
  }
  return (parseChunkRequest());
}

std::string Client::getLineFromBuffer() {
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
  if ((i + carriage) != 0 && i + carriage < _vBuffer.size())
    i++;
  std::string line(_vBuffer.begin(), _vBuffer.begin() + i + carriage);
  _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + i + carriage);
  return (line);
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
    logErrorClient("Client::saveToTmpFile: fail to write all bytes");
    _statusCode = 500;
    return (false);
  }
  return (true);
}

bool Client::saveToTmpFile(void) {
  if (_tmpFd == -1) {
    if (isCgi() == true)
      _tmpFile = _location->getCgiPath(_sUri) + "infileCgi_";
    else
      _tmpFile = "webserv_tmpfile_";
    std::stringstream ss;
    ss << getTime();
    _tmpFile += ss.str();
    _tmpFd = open(_tmpFile.c_str(), O_CLOEXEC | O_RDWR | O_CREAT, 00644);
    if (_tmpFd == -1) {
      logErrorClient("Client::saveToTmpFile: fail to open: " + _tmpFile);
      _statusCode = 500;
      return (false);
    }
  }
  ssize_t writeBytes = write(_tmpFd, &(_vBody)[0], _vBody.size());
  if (writeBytes == -1 || static_cast<size_t>(writeBytes) < _vBody.size()) {
    logErrorClient("Client::saveToTmpFile: fail to write all bytes");
    _statusCode = 500;
    return (false);
  }
  resetVector(_vBody);
  return (true);
}

bool Client::saveMultiToTmpfile(multipartRequest &multi) {
  if (multi.tmpFilename == "") {
    std::stringstream ss;
    ss << _multipart.size();
    multi.tmpFilename = "webserv_tmpmulti" + ss.str();
    ss.str("");
    ss.clear();
    ss << getTime();
    multi.tmpFilename += "id" + ss.str();
    std::cout << "saving to " << multi.tmpFilename << std::endl;
    std::cout << "for file = " << multi.file << std::endl;
    _tmpFd = open(multi.tmpFilename.c_str(),
                  O_RDWR | O_CLOEXEC | O_CREAT | O_TRUNC, 00644);
    if (_tmpFd == -1) {
      logErrorClient("Client::saveMultiToTmpFile: fail to open: " +
                     multi.tmpFilename);
      _statusCode = 500;
      return (false);
    }
  }
  bool ret = saveToTmpFile(multi.body);
  resetVector(multi.body);
  return (ret);
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

bool Client::checkBoundary(void) {
  if (_vBuffer.size() < _boundary.size())
    return (false);
  std::string tmp(_vBuffer.begin(), _vBuffer.begin() + _boundary.size());
  // std::cout << YELLOW << "Client::checkBoundary: tmp = " << tmp << RESET
  // << std::endl;
  if (tmp.compare(_boundary) == 0)
    return (true);
  return (false);
}

bool Client::checkEndBoundary(multipartRequest &multi) {
  if (_vBuffer.size() < _boundary.size() + 2)
    return (false);
  std::string tmp(_vBuffer.begin(), _vBuffer.begin() + _boundary.size() + 2);
  // std::cout << PURP << "Client::checkBoundary: tmp = " << tmp << RESET
  // << std::endl;
  if (tmp.compare(0, _boundary.size(), _boundary) == 0) {
    multi.isDone = true;
    _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + _boundary.size());
    // std::cout << PURP << "Client::checkBoundary: tmp.sub = "
    // << tmp.substr(_boundary.size(), 2) << RESET << std::endl;
    if (tmp.substr(_boundary.size(), 2) == "--") {
      _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + 2);
      removeTrailingLineFromBuffer();
      return (true);
    }
    removeTrailingLineFromBuffer();
  }
  if (_statusCode >= 400) {
    return (true);
  }
  return (false);
}

bool Client::checkHeaderMulti(multipartRequest &multi) {
  if (_statusCode >= 400)
    return (false);
  std::map<std::string, std::string>::iterator it;
  it = multi.header.find("content-type");
  if (it == multi.header.end()) {
    logErrorClient("Client::checkHeaderMulti: no content-type header");
    _statusCode = 400;
    return (false);
  }
  it = multi.header.find("content-disposition");
  if (it == multi.header.end()) {
    logErrorClient("Client::checkHeaderMulti: no content-disposition header");
    _statusCode = 400;
    return (false);
  }
  std::string tmp = (*it).second;
  size_t pos = tmp.find("name=\"");
  if (pos == tmp.npos || pos + 7 >= tmp.size()) {
    logErrorClient(
        "Client::checkHeaderMulti: invalid content-disposition header");
    _statusCode = 422;
    return (false);
  }
  pos += 7;
  pos = tmp.find_first_of('\"');
  if (pos == tmp.npos) {
    logErrorClient(
        "Client::checkHeaderMulti: invalid content-disposition header");
    _statusCode = 422;
    return (false);
  }
  pos = tmp.find("filename=\"", pos);
  if (pos == tmp.npos) {
    logErrorClient(
        "Client::checkHeaderMulti: invalid content-disposition header");
    _statusCode = 422;
    return (false);
  }
  pos += 10;
  size_t end = pos;
  end = tmp.find_first_of('\"', end);
  if (end == tmp.npos) {
    logErrorClient(
        "Client::checkHeaderMulti: invalid content-disposition header");
    _statusCode = 422;
    return (false);
  }
  if (_location->getUploadLocation() == "")
    multi.file = _location->getRoot();
  else
    multi.file = "." + _location->getUploadLocation();
  multi.file += tmp.substr(pos, end - pos);
  multi.headerDone = true;
  return (true);
}

bool Client::checkBodyMultipartCgi(std::string &boundary) {
  _vBody.insert(_vBody.end(), _vBuffer.begin(), _vBuffer.end());
  std::string tmp(_vBuffer.begin(), _vBuffer.end());
  _vBuffer.erase(_vBuffer.begin(), _vBuffer.end());
  size_t pos = tmp.rfind(boundary + "--");
  if (pos != tmp.npos)
    return (true);
  return (false);
}

bool Client::getHeaderMulti(multipartRequest &multi) {
  std::string line = getLineFromBuffer();
  while (line.empty() == false && _statusCode < 400) {
    removeReturnCarriageNewLine(line);
    // std::cout << BLUE << "New line header: " << line.substr(0, 20) << RESET
    // << std::endl;
    if (line == "" || _vBuffer.size() < _boundary.size())
      break;
    insertInMap(line, multi.header);
    if (_vBuffer.size() < _boundary.size())
      return (true);
    line = getLineFromBuffer();
  }
  removeTrailingLineFromBuffer();
  return (checkHeaderMulti(multi));
}

bool Client::parseMultipartRequest() {
  if (_location && _location->isCgi(_sUri) == true)
    return (checkBodyMultipartCgi(_boundary));
  if (_vBuffer.size() < _boundary.size())
    return (false);
  if (_checkMulti == false) {
    if (checkBoundary() == false) {
      logErrorClient("Client::parseMultipartRequest: no boundary string");
      _statusCode = 400;
      return (true);
    }
    _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + _boundary.size());
    removeTrailingLineFromBuffer();
    if (_statusCode >= 400)
      return (true);
    _checkMulti = true;
  }
  if (_multipart.size() == 0 || _multipart.back().isDone == true) {
    multipartRequest ref;
    ref.isDone = false;
    ref.headerDone = false;
    _multipart.push_back(ref);
  }
  multipartRequest &multi = _multipart.back();
  if (multi.headerDone == false && getHeaderMulti(multi) == false)
    return (true);
  if (_vBuffer.size() < _boundary.size())
    return (false);
  if (getMultipartBody(multi) == true)
    return (true);
  return (parseMultipartRequest());
}

bool Client::getMultipartBody(multipartRequest &multi) {
  std::string line;
  std::string next;
  while (true) {
    if (_vBuffer.size() < _boundary.size()) {
      // std::cout
      //     << PURP2
      //     << "Client::getMultipartBody: break cause buffer.size(): _vbuffer =
      //     "
      //     << _vBuffer << RESET << std::endl;
      break;
    }
    line = getLineFromBuffer();
    if (line == "") {
      multi.body.insert(multi.body.end(), _vBuffer.begin(),
                        _vBuffer.begin() + 1);
      _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + 1);
    }
    // std::cout << BLUE << "New line body: " << line << RESET << std::endl;
    if (checkBoundary() == true) {
      multi.isDone = true;
      // std::cout << PURP2
      //           << "Client::getMultipartBody: break cause checkBoundary ==
      //           true"
      //           << RESET << std::endl;
      break;
    }
    // }
    else
      multi.body.insert(multi.body.end(), line.begin(), line.end());
  }
  checkBodyHeader(multi, multi.body);
  saveMultiToTmpfile(multi);
  if (multi.isDone == true)
    close(_tmpFd);
  if (checkEndBoundary(multi) == true) {
    if (_vBuffer.size() > 0) {
      logErrorClient("Client::getMultipartBody: body after end boundary");
      _statusCode = 400;
    } else
      _statusCode = 201;
    return (true);
  }
  return (false);
}

std::string Client::getBoundaryString(std::string &boundaryHeader) {
  size_t pos;
  std::cout << YELLOW
            << "Client::getBoundaryString: starting: boundaryHeader = "
            << boundaryHeader << RESET << std::endl;
  pos = boundaryHeader.find(';');
  if (pos == boundaryHeader.npos) {
    logErrorClient("Client::getBoundaryString: invalid boundary header");
    _statusCode = 400;
    return ("");
  }
  pos++;
  if (boundaryHeader[pos] == ' ')
    pos++;
  if (pos + 11 >= boundaryHeader.size() ||
      boundaryHeader.substr(pos, 11) != "boundary=--") {
    logErrorClient("Client::getBoundaryString: invalid boundary header");
    _statusCode = 400;
    return ("");
  }
  return ("----" + boundaryHeader.substr(pos + 11));
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
    logErrorClient("Client::setupBodyParsing: both content-length and "
                   "transfer-encoding header");
    _statusCode = 400;
    return;
  }
  if (itLength != _headers.end()) {
    _bodyToRead = std::strtol(((*itLength).second).c_str(), NULL, 10);
    if (errno == ERANGE || _bodyToRead < 0 ||
        (_bodyToRead > static_cast<int>(_server->getLimitBodySize()) &&
         _server->getLimitBodySize() != 0)) {
      logErrorClient(
          "Client::setupBodyParsing: invalid size of content-length");
      _statusCode = 413;
      return;
    }
  } else if (itChunked != _headers.end()) {
    _chunkRequest = true;
  }
}

bool Client::isCgi(void) {
  if (_location && _location->isCgi(_sUri) == true)
    return (true);
  return (false);
}

bool Client::parseBody(void) {
  std::cout << GREEN
            << "Client::parseBody: _vBuffer.size() = " << _vBuffer.size()
            << "; _bodyToRead = " << _bodyToRead << RESET << std::endl;
  if (_vBuffer.size() == 0)
    return (_bodyToRead == 0);
  if (_multipartRequest == true)
    return (parseMultipartRequest());
  else if (_chunkRequest == true)
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
      logErrorClient(
          "Client::vectorToHeadersMap: invalid size of request(all header)");
      _statusCode = 413;
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
  // size_t emptyLine = 0;
  // size_t i = 0;
  // _vBuffer.reserve(_vBuffer.size() + vec.size());
  // for (; i < vec.size(); i++) {
  //   if (vec[i] == '\r') {
  //     size_t tmp = i;
  //     i++;
  //     if (i == vec.size())
  //       break;
  //     if (vec[i] != '\n') {
  //       emptyLine = 0;
  //       _vBuffer.push_back(*(vec.begin() + tmp));
  //     } else
  //       emptyLine++;
  //   }
  //   _vBuffer.push_back(*(vec.begin() + i));
  //   if (vec[i] == '\n')
  //     emptyLine++;
  //   else
  //     emptyLine = 0;
  //   if (emptyLine == 2)
  //     break;
  // }
  // if (i != vec.size())
  //   _vBuffer.insert(_vBuffer.end(), vec.begin() + i, vec.end());
  // while (true) {
  //   if (_vBuffer.size() == 0)
  //     break;
  //   std::vector<char>::iterator it = _vBuffer.begin();
  //   if (*it == '\n') {
  //     _vBuffer.erase(it);
  //   } else {
  //     break;
  //   }
  // }
  // std::cout << BLUE << "vec: " << vec << RESET << std::endl;
  // std::cout << PURP << "buffer: " << _vBuffer << RESET << std::endl;
  // return;

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

  // std::cout << BLUE << "vec: " << vec << RESET << std::endl;
  // std::cout << PURP << "buffer: " << _vBuffer << RESET << std::endl;

  return;
}

size_t Client::hasNewLine(void) const {
  for (size_t i = 0; i < _vBuffer.size(); i++) {
    if (_vBuffer[i] == '\n')
      return (i);
  }
  return (0);
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
  } catch (security_error &e) {
    _location = NULL;
    _statusCode = 400;
    logErrorClient("Client::getLocation: security alert: tried to go before "
                   "server root: " +
                   _sUri);
    return (false);
  } catch (std::exception &e) {
    _location = NULL;
    _statusCode = 404;
    logErrorClient("Client::getLocation: No location found" + _sUri);
    return (false);
  }
  return (true);
}

void Client::parseRequest(std::string &buffer) {
  if (buffer.empty() == true)
    return;
  vec_string request = split(buffer, "\n");
  if (request.size() < 2) {
    logErrorClient("Client::parseRequest: invalid number of header");
    _statusCode = 400;
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
    logErrorClient(
        "Client::parseRequest: body size different from body size given");
    _statusCode = 400;
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
  if (_statusCode >= 400)
    return (true);
  if (_vBuffer.empty() == false) {
    if (_bodyToRead > 0) {
      _vBuffer.insert(_vBuffer.end(), buffer.begin(), buffer.end());
      return (parseBody());
    } else
      _statusCode = 400;
  }
  if (_statusCode != 0 && _cgiPid == 0) {
    return (true);
  }
  std::cout << "Client::addBuffer: end: _statusCode = " << _statusCode
            << std::endl;
  return (false);
}
bool Client::addBuffer(std::vector<char> &buffer) {
  bool ret = parseBuffer(buffer);
  std::cout << YELLOW << "ret = " << ret << ";statusCode = " << _statusCode
            << RESET << std::endl;
  _time = getTime();
  return (ret);
}
