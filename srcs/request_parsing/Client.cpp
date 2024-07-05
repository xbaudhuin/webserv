#include "Client.hpp"
#include "Error.hpp"
#include "ServerConf.hpp"
#include "SubServ.hpp"
#include "Utils.hpp"

const char *Client::_validMethods[] = {"GET",     "POST",  "DELETE",
                                       "HEAD",    "PUT",   "CONNECT",
                                       "OPTIONS", "TRACE", "PATCH"};

const size_t Client::_methodSize = 9;

const char *Client::_whiteSpaces = " \t";

const size_t Client::_uriMaxSize = 8000;

const size_t Client::_headerMaxSize = 8000;

Client::Client(const int fd, const mapConfs &mapConfs,
               const ServerConf *defaultConf)
    : _socket(fd), _mapConf(mapConfs), _defaultConf(defaultConf),
      _statusCode(200), _method(""), _uri(""), _version(0), _host(""),
      _body(""), _buffer(""), _bodySize(-1), _requestSize(0) {
  return;
}

Client::~Client(void) { return; }

Client::Client(Client const &copy)
    : _socket(copy._socket), _mapConf(copy._mapConf),
      _defaultConf(copy._defaultConf) {
  if (this != &copy)
    *this = copy;
  return;
}

Client &Client::operator=(Client const &rhs) {
  if (this != &rhs) {
    _method = rhs._method;
    _uri = rhs._uri;
    _version = rhs._version;
    _host = rhs._host;
    _headers = rhs._headers;
    _body = rhs._body;
  }
  return (*this);
}

void trimWhitespace(std::string &str, const char *whiteSpaces) {
  str.erase(0, str.find_first_not_of(whiteSpaces));
  str.erase(str.find_last_not_of(whiteSpaces) + 1);
}

int Client::parseUri(const std::string &uri) {
  if (uri[0] != '/')
    return (400);
  if (uri.size() > _uriMaxSize)
    return (414);
  size_t pos = uri.find_first_of('?');
  _uri = uri.substr(0, pos);
  if (_uri.size() > 2048)

    if (pos != uri.npos) {
      std::string query = uri.substr(pos + 1);
      if (query.empty() == false)
        _queryUri = split(query, "+&");
    }
  return (200);
}

size_t Client::parseRequestLine(const std::string &requestLine) {
  vec_string split_request = split(requestLine, " ");
  if (split_request.size() != 3) {
    std::cout << RED << "StatusCode(split.size()): " << "400" << RESET
              << std::endl;
    return (400);
  }
  _method = split_request[0];
  size_t i = 0;
  for (; i < _methodSize && _method != _validMethods[i]; i++) {
  }
  if (i == _methodSize) {
    return (405);
  } else if (i > 2) {
    return (401);
  }
  _statusCode = parseUri(split_request[1]);
  if (_statusCode >= 300) {
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
  if (nb < 0 || nb > 999 || *after != '\0') {
    std::cout << RED << "StatusCode(nb): " << "400" << RESET << std::endl;
    return (400);
  }
  std::cout << "VERSION = " << nb << "\n";
  if (nb >= 1)
    _version = 1;
  else
    _version = 0;
  return (200);
}

unsigned char ToLower(char c) {
  return (std::tolower(static_cast<unsigned char>(c)));
}

bool Client::insertInMap(std::string &line) {
  std::transform(line.begin(), line.end(), line.begin(), ToLower);
  size_t pos = line.find_first_of(':');
  std::string key = line.substr(0, pos);
  std::string value = line.substr(pos + 1);
  if (value.size() > _headerMaxSize) {
    return (false);
  }
  trimWhitespace(value, _whiteSpaces);
  std::cout << "KEY = " << key << std::endl;
  std::cout << "VALUE = " << value << std::endl;

  if (pos != line.npos) {
    if (_headers.insert(std::make_pair(key, value)).second == false &&
        key == "host")
      return (false);
  }
  return (true);
}

void Client::readRequest(void) {
  char buf[BUFSIZ] = {0};
  ssize_t read = recv(_socket, buf, BUFSIZ, 0);
  if (read == -1) {
    std::cout << RED << "initial recv return -1 on " << _socket << RESET
              << std::endl;
    return;
  }
  if (read == 0) {
    std::cout << RED << "initial recv return 0 on " << _socket << RESET
              << std::endl;
    return;
  }
  _buffer += buf;
  size_t start = _requestSize;
  while (true) {
    start = _buffer.find_first_of('\r', start);
    if (start == _buffer.npos)
      break;
    if (_buffer[start + 1] == '\n')
      _buffer.erase(start, 1);
  }
  if (_bodySize > 0) {
    if (_bodySize > read) {
      _requestSize += read;
      _bodySize -= read;
      return;
    }
    _requestSize += _bodySize;
    _bodySize = -1;
    std::string request = _buffer.substr(0, _requestSize);
    _buffer = _buffer.substr(_requestSize, _buffer.size() - _requestSize);
    parseRequest(request);
  }
  parseBuffer();
}

void Client::parseBuffer(void) {
  size_t pos = _buffer.find("\n\n");
  if (pos == _buffer.npos) {
    return;
  }
  std::string request = _buffer.substr(0, pos);
  pos++;
  _buffer = _buffer.substr(pos, _buffer.size() - pos);
  parseRequest(request);
  parseBuffer();
}

std::string Client::getDate(void) {
  time_t rawtime;

  time(&rawtime);
  tm *gmtTime = gmtime(&currentTime);
  char buffer[80] = {0};
  strftime(buffer, sizeof(buffer), "Date: %a, %d, %b, %Y, %H:%M:%S GMT\r\n");
  return (buffer);
}

void Client::sendResponse(int statusCode) {
  std::string response;
  response += "HTTP/1.1 ";
  {
    std::ostringstream ss;
    ss << statusCode;
    response += ss.str();
  }
  response += "reasonPhrase\r\n";
  response += "Webserv: 1.0.0\r\n";
  response += getDate();
}

void Client::parseRequest(std::string &buffer) {
  if (buffer.empty() == true)
    return;
  vec_string request = split(buffer, "\n");
  if (request.size() < 2) {
    _statusCode = 400;
    return;
  }
  std::string requestLine;
  if (_headers.count("host") == 0) {
    _statusCode = 400;
    return;
  }
  _host = _headers.at("host");
  _statusCode = parseRequestLine(requestLine);
  if (_statusCode >= 400)
    return;
  try {
    // int size = std::strtol(_headers.at("content-lenght").c_str(), NULL,
    // 10); read(size) and add body
  } catch (std::exception &e) {
  }
  if (line.empty() == false)
    throw(std::logic_error(""));
}

void Client::print() {
  std::cout << "status code = " << _statusCode << "\n";
  std::cout << "method      = " << _method << "\n";
  std::cout << "uri         = " << _uri << "\n";
  std::cout << "_version    = " << _version << "\n";
  std::cout << "_host       = " << _host << "\n";
  // std::cout << "_headers       = " << _host << "\n";
  std::cout << "_body       = " << _body << "\n";
}
