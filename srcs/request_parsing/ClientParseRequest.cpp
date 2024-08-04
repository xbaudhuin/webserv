#include "Client.hpp"
#include <fcntl.h>
// #include "Colors.hpp"
// #include "Error.hpp"
// #include "Utils.hpp"
// #include <algorithm>
// #include <cstdint>
// #include <fcntl.h>
// #include <sstream>
// #include <vector>

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
  std::cout << BLUE << "requestLine = " << requestLine << RESET << std::endl;
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

size_t Client::insertInMap(std::string &line,
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
  // std::cout << "KEY = " << key << std::endl;
  // std::cout << "VALUE = " << value << std::endl;

  std::map<std::string, std::string>::iterator it;
  if (pos != line.npos) {
    it = map.find(key);
    if (it != map.end()) {
      if (key == "host" || "content-length" || "transfer-encoding" ||
          "content-type")
        return (400);
      (*it).second += ",\n";
      (*it).second += value;
    } else if (map.insert(std::make_pair(key, value)).second == false &&
               key == "host")
      return (400);
  }
  return (0);
}

bool Client::checkMethod(void) {

  if (_sMethod == "GET" && _location->getGetStatus() == false) {
    std::cout << RED << "GET not allowed" << RESET << std::endl;
    _statusCode = 405;
  } else if (_sMethod == "POST" && _location->getPostStatus() == false) {
    std::cout << RED << "Client::checkMethod: invalid method on location: "
              << _location->getUrl() << RESET << std::endl;
    _statusCode = 405;
  } else if (_sMethod == "DELETE" && _location->getDeleteStatus() == false) {
    std::cout << RED << "DELETE not allowed" << RESET << std::endl;
    _statusCode = 405;
  }
  if (_statusCode < 400 && _sMethod != "POST") {
    if (_headers.count("content-length") != 0 ||
        _headers.count("transfer-encoding") != 0) {
      std::cout << RED
                << "Client::checkMethod: Invalid header with method not POST"
                << RESET << std::endl;
      _statusCode = 413;
    }
  }
  if (_sMethod == "POST") {
    std::map<std::string, std::string>::iterator it =
        _headers.find("content-type");
    if (it == _headers.end() && _headers.count("transfer-encoding") == 0) {
      std::cout << RED << "no header content-type" << RESET << std::endl;
      _statusCode = 400;
      return (false);
    }
    if ((*it).second.find("multipart") == (*it).second.npos &&
        _headers.count("content-length") == 0) {
      std::cout << RED << "Client::checkMethod: no multipart" << std::endl;
      _statusCode = 413;
    }
  }
  if (_statusCode >= 400)
    return (false);
  return (true);
}

void Client::getPathUpload(void) {
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

void Client::removeTrailingLineFromBuffer(void) {
  size_t i = 0;
  if (_vBuffer.size() >= 2 && _vBuffer[i] == '\r' && _vBuffer[i + 1] == '\n')
    i += 2;
  else if (_vBuffer[i] == '\n')
    i++;
  else {
    // std::cout << GREEN << "_vbuffer: " << _vBuffer << RESET << std::endl;
    std::cout << RED
              << "Client:removeTrailingLineFromBuffer: failed to remove "
                 "trailing line"
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
    if (static_cast<size_t>(_bodyToRead) > min)
      _bodyToRead -= _vBuffer.size();
    else {
      ssize_t bytesWrite = write(_tmpFd, &_vBody[0], _vBody.size());
      if (bytesWrite == -1 ||
          static_cast<size_t>(bytesWrite) != _vBody.size()) {
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
        static_cast<size_t>(_sizeChunk) > _location->getLimitBodySize()) {
      std::cout << RED << "Client::parseChunkRequest: invalid body size "
                << RESET << std::endl;
      _statusCode = 413;
      return (true);
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
  if (i + carriage < _vBuffer.size())
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

bool Client::saveToTmpFile(int fd, std::vector<char> body) {
  ssize_t writeBytes = write(fd, &(body)[0], body.size());
  if (writeBytes == -1 || static_cast<size_t>(writeBytes) < body.size()) {
    std::cout << RED << "Client::saveToTmpFile: fail to write to tmpfile"
              << RESET << std::endl;
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
  }
  _tmpFd = open(_tmpFile.c_str(), O_CLOEXEC | O_RDWR | O_CREAT, 00644);
  if (_tmpFd == -1) {
    _statusCode = 500;
    return (false);
  }
  ssize_t writeBytes = write(_tmpFd, &(_vBody)[0], _vBody.size());
  if (writeBytes == -1 || static_cast<size_t>(writeBytes) < _vBody.size()) {
    _statusCode = 500;
    return (false);
  }
  resetVector(_vBody);
  return (true);
}

bool Client::saveMultiToTmpfile(void) {
  multipartRequest &multi = _multipart.back();
  std::stringstream ss;
  ss << _multipart.size();
  multi.tmpFilename = "webserv_tmpmulti" + ss.str();
  ss.clear();
  ss << getTime();
  multi.tmpFilename += "id" + ss.str();
  std::cout << "saving to " << multi.tmpFilename << std::endl;
  std::cout << "for file = " << multi.file << std::endl;
  int fd = open(multi.tmpFilename.c_str(),
                O_RDWR | O_CLOEXEC | O_CREAT | O_TRUNC, 00644);
  if (fd == -1) {
    std::cout << RED << "Client::saveMultiToTmpfile: fail to open: "
              << multi.tmpFilename << RESET << std::endl;
    _statusCode = 500;
    return (false);
  }
  bool ret = saveToTmpFile(fd, multi.body);
  close(fd);
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
  std::string tmp(_vBuffer.begin(), _vBuffer.end());
  if (tmp.find(_boundary) != 0) {
    std::cout << RED << "Client::checkBoundary: boundary not found" << RESET
              << std::endl;
    _statusCode = 400;
    return (true);
  }
  if (tmp.find(_boundary, _boundary.size()) == tmp.npos) {
    return (false);
  }
  return (true);
}

bool Client::checkHeaderMulti(multipartRequest &multi) {
  std::map<std::string, std::string>::iterator it;
  it = multi.header.find("content-type");
  if (it == multi.header.end()) {
    _statusCode = 400;
    return (false);
  }
  it = multi.header.find("content-disposition");
  if (it == multi.header.end()) {
    _statusCode = 400;
    return (false);
  }
  std::string tmp = (*it).second;
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
  size_t end = pos;
  end = tmp.find_first_of('\"', end);
  if (end == tmp.npos) {
    _statusCode = 422;
    return (false);
  }
  if (_location->getUploadLocation() == "")
    multi.file = _location->getRoot();
  else
    multi.file = "." + _location->getUploadLocation();
  multi.file += tmp.substr(pos, end - pos);
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

bool Client::parseMultipartRequest(std::string &boundary, bool check) {
  if (_location && _location->isCgi(_sUri) == true)
    return (checkBodyMultipartCgi(boundary));
  std::cout << YELLOW
            << "Client::parseMultipartRequest starting: boundary = " << boundary
            << RESET << std::endl;
  if (_vBuffer.size() < boundary.size() && _vBuffer.size() > 0) {
    return (false);
  }
  if (check == false) {
    if (checkBoundary() == false)
      return (false);
    if (_statusCode >= 400)
      return (true);
    _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + boundary.size());
    removeTrailingLineFromBuffer();
  }
  multipartRequest multi;
  std::string line = getLineFromBuffer();
  while (line.empty() == false && _statusCode < 400) {
    removeReturnCarriageNewLine(line);
    std::cout << BLUE << "New line header: " << line << RESET << std::endl;
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
  line = getLineFromBuffer();
  std::cout << BLUE << "New line check: " << line << RESET;
  while (true) {
    if (line.compare(0, boundary.size(), boundary) == 0) {
      if (line.compare(boundary.size(), 2, "--") == 0) {
        std::cout << GREEN << "Client::multipartRequest: compare = end" << RESET
                  << std::endl;
        _statusCode = 200;
        // removeTrailingLineFromBuffer();
      }
      break;
    }
    if (_vBuffer.size() < boundary.size()) {
      std::cout << RED << "CLient::multipartRequest: _vBuffer too small"
                << RESET << std::endl;
      return (false);
    }
    std::cout << BLUE << "New line check: " << line << RESET;
    getMultipartBody(multi, line);
  }
  _multipart.push_back(multi);
  if (saveMultiToTmpfile() == false)
    return (true);
  if (_statusCode == 200)
    return (true);
  return (parseMultipartRequest(boundary, true));
}

bool Client::getMultipartBody(multipartRequest &multi, std::string &line) {
  while (true) {
    std::cout << BLUE << "New line body : " << line << RESET << std::endl;
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
    line = getLineFromBuffer();
  }
  std::vector<char> tmp;
  checkBodyHeader(multi, tmp);
  return (false);
}

std::string Client::getBoundaryString(std::string &boundaryHeader) {
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
    _statusCode = 400;
    return ("");
  }
  std::cout << YELLOW << "Client::getBoundaryString: BOUNDARY = " << "----"
            << boundaryHeader.substr(pos + 11) << RESET << std::endl;
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
      std::cout << PURP << "errno = " << errno
                << "; _bodyToRead = " << _bodyToRead
                << "; _serverLimit = " << _server->getLimitBodySize()
                << "; cast = " << static_cast<int>(_server->getLimitBodySize())
                << RESET << std::endl;
      return;
    }
  } else if (itChunked != _headers.end()) {
    _chunkRequest = true;
    if (isCgi() == true)
      _tmpFile = _location->getCgiPath(_sUri);
    else
      _tmpFile = "webserv_tmpfile_";
    std::stringstream ss;
    ss << _socket;
    _tmpFile = "." + ss.str() + "fd";
    time_t time = getTime();
    std::stringstream t;
    t << time;
    _tmpFile += "id" + t.str();
    _tmpFd =
        open(_tmpFile.c_str(), O_CREAT | O_RDWR | O_TRUNC | O_CLOEXEC, 00644);
    if (_tmpFd == -1) {
      _statusCode = 500;
      return;
    }
  }
}

bool Client::isCgi(void) {
  if (_location && _location->isCgi(_sUri) == true)
    return (true);
  return (false);
}

bool Client::parseBody(void) {
  if (_vBuffer.size() == 0)
    return (_bodyToRead == 0);
  if (_multipartRequest == true)
    return (parseMultipartRequest(_boundary, false));
  else if (_chunkRequest == true)
    return (parseChunkRequest());
  else {
    if (_bodyToRead > 0) {
      int64_t min =
          std::min(_bodyToRead, static_cast<int64_t>(_vBuffer.size()));
      _vBody.insert(_vBody.end(), _vBuffer.begin(), _vBuffer.begin() + min);
      _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + min);
      _bodyToRead -= min;
      return (_bodyToRead == 0);
    }
  }
  return (_bodyToRead == 0);
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
    _statusCode = insertInMap(request[it], _headers);
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
      std::cout << GREEN << "FOUND EMPTY LINE at " << i << RESET << std::endl;
      break;
    }
  }
  if (pos == -1)
    std::cout << RED << "No empty line in buffer" << RESET << std::endl;
  return (pos);
}

bool Client::getLocation(void) {
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

void Client::parseRequest(std::string &buffer) {
  if (buffer.empty() == true)
    return;
  std::cout << "Client::parseRequest: starting" << buffer << std::endl;
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
    return;
  }
  if (_location && _location->isCgi(_sUri) == true) {
    setupCgi();
  }
  if (_statusCode == 0)
    _statusCode = 200;
  return;
}

bool Client::parseBuffer(const std::vector<char> &buffer) {
  // std::cout << YELLOW << "Client::parseBuffer: " << buffer << RESET
  // << std::endl;
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
  // std::cout << YELLOW << "request = " << request << RESET << std::endl;
  // std::cout << BLUE << "_vBuffer = " << _vBuffer << RESET << std::endl;
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
  if (_statusCode != 0 && _cgiPid == 0) {
    return (true);
  }
  std::cerr << "Client::addBuffer: end: _statusCode = " << _statusCode
            << std::endl;
  return (false);
}
bool Client::addBuffer(const std::vector<char> &buffer) {
  // std::cout << YELLOW << "buffer = " << buffer << RESET << std::endl;
  bool ret = parseBuffer(buffer);
  std::cout << YELLOW << "ret = " << ret << ";statusCode = " << _statusCode
            << RESET << std::endl;
  _time = getTime();
  return (ret);
}

// bool Client::saveToTmpFile(void) {
//   multipartRequest &multi = _multipart.back();
//   std::stringstream ss;
//   ss << _multipart.size();
//   multi.tmpFilename = "tmpmulti" + ss.str();
//   ss.clear();
//   ss << _socket;
//   multi.tmpFilename += ss.str() + "socket";
//   ss.clear();
//   ss << getTime();
//   multi.tmpFilename += "id" + ss.str();
//   int fd = open(multi.tmpFilename.c_str(), O_RDWR | O_CLOEXEC);
//   if (fd == -1) {
//     _statusCode = 500;
//     return (false);
//   }
//   ssize_t writeBytes = write(fd, &(multi.body)[0], multi.body.size());
//   if (writeBytes == -1 || writeBytes < multi.body.size()) {
//     _statusCode = 500;
//     return (false);
//   }
//   close(fd);
//   resetVector(multi.body);
//   return (true);
// }
// void Client::getMultipartBody(multipartRequest &multi) {
//   std::string line;
//   while (true) {
//     line = getLineFromBuffer();
//     break;
//   }
//   else {
//     getMultipartBody(multi);
//     multi._vBody.insert(multi._vBody.end(), line.begin(), line.end());
//   }
// }

// bool Client::checkMethod(void) {
//   if (_sMethod == "GET" && _location->getGetStatus() == false) {
//     _statusCode = 405;
//     return (false);
//   } else if (_sMethod == "POST" && _location->getPostStatus() == false) {
//     std::cout << RED << "Client::checkMethod: invalid method on location: "
//               << _location->getUrl() << RESET << std::endl;
//     _statusCode = 405;
//     return (false);
//   } else if (_sMethod == "DELETE" && _location->getDeleteStatus() == false)
//   {
//     _statusCode = 405;
//     return (false);
//   }
//   return (true);
// }

// void Client::parseRequest(std::string &buffer) {
//   if (buffer.empty() == true)
//     return;
//   vec_string request = split(buffer, "\n");
//
//   std::cout << YELLOW << "vec_string request.size() = " << request.size()
//             << RESET << std::endl;
//
//   if (request.size() < 2) {
//     _statusCode = 400;
//
//     std::cout << RED << "changin statusCode because if (request.size() < 2)
//     to "
//               << _statusCode << RESET << std::endl;
//
//     return;
//   }
//   _requestSize = request[0].size();
//   _statusCode = parseRequestLine(request[0]);
//   if (_statusCode >= 400)
//     return;
//   request.erase(request.begin(), request.begin() + 1);
//   vectorToHeadersMap(request);
//   if (_statusCode != 0)
//     return;
//   if (_headers.count("host") == 0) {
//     _statusCode = 400;
//
//     std::cout << RED
//               << "changin statusCode because if (_headers.count(\"host\") "
//                  "== 0) to "
//               << _statusCode << RESET << std::endl;
//
//     return;
//   }
//
//   std::cout << RED << "Before at" << RESET << std::endl;
//
//   _sHost = _headers.at("host");
//
//   std::cout << YELLOW << "HERE: " << "host : " << _headers.at("host") <<
//   RESET
//             << std::endl;
//
//   _server = getServerConf();
//
//   std::cout << YELLOW << "serverName = " << _server->getMainServerName()
//             << " on port: " << _server->getPort() << RESET << std::endl;
//
//   try {
//     _location = &(_server->getPreciseLocation(_sUri));
//     checkPathInfo();
//     std::cout << YELLOW << "location = " << _location->getUrl() << RESET
//               << std::endl;
//
//   } catch (security_error &e) {
//     _location = NULL;
//     _statusCode = 400;
//
//     std::cout << RED << "caught security_error" << RESET << std::endl;
//
//     return;
//   } catch (std::exception &e) {
//     _location = NULL;
//     _statusCode = 404;
//
//     std::cout << RED << "changin statusCode because caught exception to "
//               << _statusCode << RESET << std::endl;
//
//     return;
//   }
//   if (_location == NULL) {
//
//     std::cout << RED << "THOMAS EST NULL" << RESET << std::endl;
//
//   } else {
//
//     std::cout << RED << "location = " << _location->getUrl() << RESET
//               << std::endl;
//   }
//   if (checkIfValid() == false)
//     return;
//   setupBodyParsing();
//   if (_statusCode != 0)
//     return;
//   if (parseBody() == false) {
//     _requestIsDone = false;
//     return;
//   }
//   if (_vBuffer.size() != 0 && _bodyToRead == 0) {
//     _statusCode = 400;
//     std::cout << GREEN << "body = " << _vBody << RESET << std::endl;
//     std::cout << RED << "buffer != 0, buffer.size() = " << _vBuffer.size()
//               << "; buffer : " << _vBuffer << RESET << std::endl;
//     std::cout << RED
//               << "changin statusCode because if (_vBuffer.size() != 0) to "
//               << _statusCode << RESET << std::endl;
//   }
//   if (_location && _location->isCgi(_sUri) == true) {
//     setupCgi();
//   }
//   if (_statusCode == 0)
//     _statusCode = 200;
//   return;
// }

// void Client::removeReturnCarriage(std::vector<char> &vec) {
//
//   _vBuffer.reserve(_vBuffer.size() + vec.size());
//   for (size_t it = 0; it < vec.size(); it++) {
//     if (vec[it] == '\r') {
//       size_t tmp = it;
//       it++;
//       if (it == vec.size())
//         break;
//       if (vec[it] != '\n') {
//         _vBuffer.push_back(*(vec.begin() + tmp));
//       }
//     }
//     _vBuffer.push_back(*(vec.begin() + it));
//   }
//   while (true) {
//     if (_vBuffer.size() == 0)
//       break;
//     std::vector<char>::iterator it = _vBuffer.begin();
//     if (*it == '\n') {
//       _vBuffer.erase(it);
//     } else {
//       break;
//     }
//   }
//
//   // std::cout << BLUE << "vec: " << vec << RESET << std::endl;
//   // std::cout << PURP << "buffer: " << _vBuffer << RESET << std::endl;
//
//   return;
// }
//

// bool Client::checkBodyToRead(std::vector<char> buffer) {
//
//   std::cout << RED << "_bodyToRead > 0" << RESET << std::endl;
//
//   _vBuffer.insert(_vBuffer.end(), buffer.begin(), buffer.end());
//   bool ret = parseBody();
//   return (ret);
// }

// bool Client::addBuffer(std::vector<char> &buffer) {
//
//   std::cout << BLUE << "Client::addBuffer: " << buffer << RESET <<
//   std::endl; if (_bodyToRead > 0 || _requestIsDone == false) {
//     return (checkBodyToRead(buffer));
//   }
//   // std::cout << YELLOW << "buffer:\n" << _vBuffer << RESET << std::endl;
//   // size_t pos = _vBuffer.find("\n\n", _requestSize);
//   // if (pos == _vBuffer.npos) {
//   //   std::cout << RED << "No empty line in buffer" << RESET << std::endl;
//   //   _time = getTime();
//   //   return (false);
//   // }
//
//   removeReturnCarriage(buffer);
//   int newLine = hasNewLine();
//
//   std::cout << PURP << "earlyPARSING: NEWLINE = " << newLine << RESET
//             << std::endl;
//
//   if (_vBuffer.size() < 20 || newLine > 0) {
//     if (earlyParsing(newLine) == false) {
//       _statusCode = 400;
//       _server = _defaultConf;
//       _time = getTime();
//       return (true);
//     }
//   }
//   if (newLine == 0) {
//     _time = getTime();
//     return (false);
//   }
//   int pos = -1;
//   for (size_t i = newLine; i + 1 < _vBuffer.size(); i++) {
//     if (_vBuffer[i] == '\n' && _vBuffer[i + 1] == '\n') {
//       pos = i;
//
//       std::cout << GREEN << "FOUND EMPTY LINE" << RESET << std::endl;
//
//       break;
//     }
//   }
//   if (pos == -1) {
//
//     std::cout << RED << "No empty line in buffer" << RESET << std::endl;
//
//     _time = getTime();
//     return (false);
//   }
//
//   std::string request(&_vBuffer[0], &_vBuffer[pos]);
//
//   std::cout << RED << "_vbuffer[pos] = " << _vBuffer[pos] << "\n";
//   std::cout << RED << "_vbuffer[pos + 1] = " << _vBuffer[pos + 1] << "\n";
//   // std::cout << RED << "_vbuffer[pos + 2] = " << _vBuffer[pos + 2] <<
//   // "\n";
//   //
//   _vBuffer.erase(_vBuffer.begin(), _vBuffer.begin() + pos + 2);
//
//   std::cout << YELLOW << "buffer after request: " << _vBuffer << RESET
//             << std::endl;
//   std::cout << PURP << "requestLine: " << request << RESET << std::endl;
//
//   // _vBuffer = _vBuffer.substr(pos, _vBuffer.size() - pos);
//   // std::cout << RED << "separating request from buffer\n" << RESET;
//   // std::cout << GREEN << "request:\n" << request << RESET;
//   // std::cout << YELLOW << "buffer:\n" << _vBuffer << RESET << std::endl;
//   std::cout << RED << "End before parseRequest; StatusCode = " <<
//   _statusCode
//             << RESET << std::endl;
//
//   parseRequest(request);
//
//   std::cout << RED << "End after parseRequest; StatusCode = " <<
//   _statusCode
//             << RESET << std::endl;
//
//   if (_vBuffer.empty() == false) {
//     if (_bodyToRead > 0) {
//       return (checkBodyToRead(_vBuffer));
//     } else
//       _statusCode = 400;
//   }
//   _time = getTime();
//   if (_statusCode != 0 && _cgiPid == 0) {
//     return (true);
//   }
//   std::cerr << "Client::addBuffer: end: _statusCode = " << _statusCode
//             << std::endl;
//   return (false);
// }
