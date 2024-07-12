#include "Client.hpp"
#include "Error.hpp"
#include "ServerConf.hpp"
#include "SubServ.hpp"
#include "Utils.hpp"
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <ostream>

const char *Client::_validMethods[] = {"GET",     "POST",  "DELETE",
                                       "HEAD",    "PUT",   "CONNECT",
                                       "OPTIONS", "TRACE", "PATCH"};

const size_t Client::_methodSize = 9;

const char *Client::_whiteSpaces = " \t";

const size_t Client::_uriMaxSize = 8192;

const size_t Client::_headerMaxSize = 8192;

const size_t Client::_headersMaxBuffer = 32768;

std::map<std::string, char> initMap() {
  std::map<std::string, char> m;

  m["%20"] = ' ';
  m["%21"] = '!';
  m["%22"] = '"';
  m["%23"] = '#';
  m["%24"] = '$';
  m["%25"] = '%';
  m["%26"] = '&';
  m["%27"] = '\'';
  m["%28"] = '(';
  m["%29"] = ')';
  m["%2A"] = '*';
  m["%2B"] = '+';
  m["%2C"] = ',';
  m["%2D"] = '-';
  m["%2E"] = '.';
  m["%2F"] = '/';
  m["%30"] = '0';
  m["%31"] = '1';
  m["%32"] = '2';
  m["%33"] = '3';
  m["%34"] = '4';
  m["%35"] = '5';
  m["%36"] = '6';
  m["%37"] = '7';
  m["%38"] = '8';
  m["%39"] = '9';
  m["%3A"] = ':';
  m["%3B"] = ';';
  m["%3C"] = '<';
  m["%3D"] = '=';
  m["%3E"] = '>';
  m["%3F"] = '?';
  m["%40"] = '@';
  m["%41"] = 'A';
  m["%42"] = 'B';
  m["%43"] = 'C';
  m["%44"] = 'D';
  m["%45"] = 'E';
  m["%46"] = 'F';
  m["%47"] = 'G';
  m["%48"] = 'H';
  m["%49"] = 'I';
  m["%4A"] = 'J';
  m["%4B"] = 'K';
  m["%4C"] = 'L';
  m["%4D"] = 'M';
  m["%4E"] = 'N';
  m["%4F"] = 'O';
  m["%50"] = 'P';
  m["%51"] = 'Q';
  m["%52"] = 'R';
  m["%53"] = 'S';
  m["%54"] = 'T';
  m["%55"] = 'U';
  m["%56"] = 'V';
  m["%57"] = 'W';
  m["%58"] = 'X';
  m["%59"] = 'Y';
  m["%5A"] = 'Z';
  m["%5B"] = '[';
  m["%5C"] = '\\';
  m["%5D"] = ']';
  m["%5E"] = '^';
  m["%5F"] = '_';
  m["%60"] = '`';
  m["%61"] = 'a';
  m["%62"] = 'b';
  m["%63"] = 'c';
  m["%64"] = 'd';
  m["%65"] = 'e';
  m["%66"] = 'f';
  m["%67"] = 'g';
  m["%68"] = 'h';
  m["%69"] = 'i';
  m["%6A"] = 'j';
  m["%6B"] = 'k';
  m["%6C"] = 'l';
  m["%6D"] = 'm';
  m["%6E"] = 'n';
  m["%6F"] = 'o';
  m["%70"] = 'p';
  m["%71"] = 'q';
  m["%72"] = 'r';
  m["%73"] = 's';
  m["%74"] = 't';
  m["%75"] = 'u';
  m["%76"] = 'v';
  m["%77"] = 'w';
  m["%78"] = 'x';
  m["%79"] = 'y';
  m["%7A"] = 'z';
  m["%7B"] = '{';
  m["%7C"] = '|';
  m["%7D"] = '}';
  m["%7E"] = '~';
  m["%7F"] = '\x7F';
  return m;
}

const std::map<std::string, char> Client::_uriEncoding = initMap();

Client::Client(const int fd, const mapConfs &mapConfs, ServerConf *defaultConf)
    : _socket(fd), _mapConf(mapConfs), _defaultConf(defaultConf), _server(NULL),
      _location(NULL), _statusCode(0), _method(""), _uri(""), _version(0),
      _host(""), _body(""), _requestSize(0), _bodySize(-1), _buffer(""),
      _keepConnectionAlive(false), _chunkRequest(false) {
  _time = getTime();
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

time_t Client::getTime(void) { return (std::time(0)); }

bool Client::isTimedOut(void) {
  time_t current;
  time(&current);
  double timeOut = std::difftime(current, _time);
  if (timeOut >= 60.0)
    return (true);
  return (false);
}

void trimWhitespace(std::string &str, const char *whiteSpaces) {
  str.erase(0, str.find_first_not_of(whiteSpaces));
  str.erase(str.find_last_not_of(whiteSpaces) + 1);
}

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
  _uri = uri.substr(0, pos);
  uriDecoder(_uri);
  if (_uri.size() > 2048)
    return (414);
  if (pos != uri.npos) {
    _queryUri = uri.substr(pos + 1);
  }

  return (0);
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
  if (nb < 0 || nb > 999 || *after != '\0') {
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

unsigned char toLower(char c) {
  return (std::tolower(static_cast<unsigned char>(c)));
}

size_t Client::insertInMap(std::string &line) {
  if (line.size() > _headerMaxSize)
    return (431);
  std::transform(line.begin(), line.end(), line.begin(), toLower);
  size_t pos = line.find_first_of(':');
  std::string key = line.substr(0, pos);
  std::string value = line.substr(pos + 1);
  if (value.size() > _headerMaxSize) {
    return (400);
  }
  trimWhitespace(value, _whiteSpaces);
  std::cout << "KEY = " << key << std::endl;
  std::cout << "VALUE = " << value << std::endl;

  if (pos != line.npos) {
    if (_headers.insert(std::make_pair(key, value)).second == false &&
        key == "host")
      return (400);
  }
  return (0);
}

bool Client::addBuffer(std::string &buffer) {
  if (_bodySize > 0) {
    std::cout << RED << "_bodySize > 0" << RESET << std::endl;
    std::string tmp_body = buffer.substr(0, _bodySize);
    _body += tmp_body;
    _bodySize -= tmp_body.size();
    if (tmp_body.size() < buffer.size())
      _statusCode = 400;
    if (_bodySize <= 0 || _statusCode != 0)
      return (1);
    return (0);
  }
  size_t start = 0;
  std::cout << GREEN << "start = " << start << RESET << std::endl;
  while (true) {
    start = buffer.find_first_of('\r', start);
    if (start == buffer.npos) {
      std::cout << RED << "No \\r found" << RESET << std::endl;
      break;
    }
    std::cout << GREEN << "found at start = " << start << RESET << std::endl;
    if (buffer[start + 1] == '\n') {
      std::cout << GREEN << "erasing 1 \\r" << RESET << std::endl;
      buffer.erase(start, 1);
    }
  }
  _buffer += buffer;
  std::cout << YELLOW << "buffer:\n" << _buffer << RESET << std::endl;
  size_t pos = _buffer.find("\n\n", _requestSize);
  if (pos == _buffer.npos) {
    std::cout << RED << "No empty line in buffer" << RESET << std::endl;
    return (0);
  }
  std::string request = _buffer.substr(0, pos);
  pos++;
  _buffer = _buffer.substr(pos, _buffer.size() - pos);
  std::cout << RED << "separating request from buffer\n" << RESET;
  std::cout << GREEN << "request:\n" << request << RESET;
  std::cout << YELLOW << "buffer:\n" << _buffer << RESET << std::endl;
  parseRequest(request);
  if (_statusCode != 0)
    return (true);
  return (false);
}

std::string Client::getDate(void) {
  time_t rawtime;

  time(&rawtime);
  tm *gmtTime = gmtime(&rawtime);
  char buffer[80] = {0};
  std::strftime(buffer, 80, "Date: %a, %d %b %Y %H:%M:%S GMT\r\n", gmtTime);
  return (buffer);
}

void Client::findPages(const std::string &url) {
  std::ifstream file(url.c_str(), std::ios::in);
  if (file.is_open() == false) {
    std::cout << RED << "failed to open file: " << url << RESET << std::endl;
    if (access(url.c_str(), F_OK) == -1)
      _statusCode = 404;
    else
      _statusCode = 403;
    return;
  }
  std::string s;
  while (getline(file, s)) {
    _responseBody.append(s);
    _responseBody.append("\r\n");
  }
  if (file.fail() == true)
    _statusCode = 400;
}

void Client::createResponseBody(void) {
  if (_statusCode > 0 && _statusCode < 400) {
    std::cout << PURP2 << "uri : " << _location->getUrl() << " ?" << RESET
              << std::endl;
    findPages(_location->getUrl());
  }
  if (_statusCode >= 400) {
    _responseBody = findErrorPage(_statusCode, _server->getErrPages());
  }
}

void Client::resetClient(void) {
  _server = NULL;
  _location = NULL;
  _time = getTime();
  _statusCode = 0;
  _method = "";
  _uri = "";
  _queryUri = "";
  _version = 0;
  _host = "";
  _headers.clear();
  _body = "";
  _requestSize = 0;
  _bodySize = 0;
  _buffer = "";
  _responseBody = "";
  _keepConnectionAlive = false;
  _chunkRequest = false;
}

void Client::sendResponse(std::string &response) {
  response += "HTTP/1.1 ";
  {
    std::ostringstream ss;
    ss << _statusCode;
    response += ss.str();
  }

  createResponseBody();
  response += "reasonPhrase\r\n";
  response += "Webserv: 1.0.0\r\n";
  response += getDate();
  response += "Connection: ";
  if (_statusCode >= 400 || _version == 0) {
    response += "close\r\n";
  } else {
    response += "keep-alive\r\n";
  }
  if (_statusCode < 300 || _statusCode >= 400) {
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: ";
    std::cout << PURP << "responseBody.size() = " << _responseBody.size()
              << RESET << std::endl;
    response += _responseBody.size();
    std::cout << PURP << "responseBody.size() = " << _responseBody.size()
              << RESET << std::endl;
    response += " \r\n";
  }
  response += "\r\n";
  response += _responseBody;
  std::cout << BLUE << "response:\n" << response << RESET << std::endl;
  resetClient();
  return;
}

ServerConf *Client::getServerConf(void) {
  mapConfs::const_iterator it;
  it = _mapConf.find(_host);
  if (it != _mapConf.end()) {
    std::cout << YELLOW << "found via host: " << _host
              << "; server name: " << ((*it).second)->getMainServerName()
              << RESET << std::endl;
    return ((*it).second);
  }
  std::cout << YELLOW << "return default server" << RESET << std::endl;
  return (_defaultConf);

  // return (cf);
}

// void Client::getResponseBody(void) { std::ifstream file(); }

void Client::parseRequest(std::string &buffer) {
  if (buffer.empty() == true)
    return;
  vec_string request = split(buffer, "\n");
  if (request.size() < 2) {
    _statusCode = 400;
    return;
  }
  _requestSize = request[0].size();
  _statusCode = parseRequestLine(request[0]);
  if (_statusCode >= 400)
    return;
  for (size_t it = 1; it < request.size(); it++) {
    _requestSize += request[it].size();
    if (_requestSize > _headersMaxBuffer) {
      _statusCode = 413;
      return;
    }
    _statusCode = insertInMap(request[it]);
    if (_statusCode != 0)
      return;
  }
  _buffer.erase(0, _requestSize);
  if (_headers.count("host") == 0) {
    _statusCode = 400;
    return;
  }
  std::cout << RED << "Before at" << RESET << std::endl;
  _host = _headers.at("host");
  std::cout << YELLOW << "HERE: " << "host : " << _headers.at("host") << RESET
            << std::endl;
  _server = getServerConf();
  std::cout << YELLOW << "serverName = " << _server->getMainServerName()
            << " on port: " << _server->getPort() << RESET << std::endl;
  try {
    _location = &(_server->getPreciseLocation(_uri));
    std::cout << YELLOW << "location = " << _location->getUrl() << RESET
              << std::endl;
  } catch (std::exception &e) {
    _location = NULL;
    _statusCode = 404;
    return;
  }
  if (checkIfValid() == false)
    return;
  std::map<std::string, std::string>::iterator it =
      _headers.find("content-length");
  if (it != _headers.end()) {
    if (_method != "POST") {
      _statusCode = 413;
      return;
    }
    _bodySize = std::strtol(((*it).second).c_str(), NULL, 10);
    if (errno == ERANGE || _bodySize < 0 ||
        (_bodySize > static_cast<int>(_server->getLimitBodySize()) &&
         _server->getLimitBodySize() != 0)) {
      std::cout << PURP << "exit limitBdySize: " << _server->getLimitBodySize()
                << RESET << std::endl;
      _statusCode = 413;
      return;
    }
  }
  if (_bodySize > 0) {
    std::cout << PURP << "_bodysize = " << _bodySize
              << "; body.size() = " << _body.size() << RESET << std::endl;
    _body = _buffer.substr(0, _bodySize);
    if (static_cast<int>(_buffer.size()) > _bodySize)
      _buffer = _buffer.substr(_bodySize);
    _bodySize -= _body.size();
    if (_bodySize <= 0)
      _bodySize = -1;
  } else
    std::cout << PURP << "No body" << RESET << std::endl;
  if (_buffer.size() != 0)
    _statusCode = 400;
  else
    _statusCode = 200;
  return;
}

bool Client::checkMethod(void) {
  if (_method == "GET" && _location->getGetStatus() == false) {
    _statusCode = 405;
    return (false);
  } else if (_method == "POST" && _location->getPostStatus() == false) {
    _statusCode = 405;
    return (false);
  } else if (_method == "POST" && _location->getDeleteStatus() == false) {
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

const std::string &Client::getBuffer(void) const { return (_buffer); }

int Client::getBodySize(void) const { return (_bodySize); }

void Client::print() {

  if (_server == NULL)
    std::cout << RED << "no server\n" << RESET;
  else
    std::cout << GREEN "server    = " << _server->getServerNames()[0] << RESET
              << "\n";
  if (_location == NULL)
    std::cout << RED << "no location\n" << RESET;
  else
    std::cout << GREEN "location    = " << _location->getUrl() << RESET << "\n";
  std::cout << "time        = " << _time << "\n";
  std::cout << "time out ?  = " << isTimedOut() << "\n";

  std::cout << "_socket     = " << _socket << "\n";
  std::cout << "status code = " << _statusCode << "\n";
  std::cout << "method      = " << _method << "\n";
  std::cout << "uri         = " << _uri << "\n";
  std::cout << "queryUri    = " << _queryUri << "\n";
  std::cout << "_requestSize= " << _requestSize << "\n";
  std::cout << "_version    = " << _version << "\n";
  std::cout << "_host       = " << _host << "\n";
  std::cout << "_headers: key = value" << "\n";
  for (std::map<std::string, std::string>::iterator i = _headers.begin();
       i != _headers.end(); i++) {
    std::cout << (*i).first << " = " << (*i).second << "\n";
  }
  std::cout << "_body       = " << _body << "\n";
  std::cout << "bodySize    = " << _bodySize << "\n";
  std::cout << "remain buffer = " << _buffer << std::endl;
}
