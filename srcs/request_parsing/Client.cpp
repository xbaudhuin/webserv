#include "Client.hpp"
#include "Error.hpp"
#include "ServerConf.hpp"
#include "SubServ.hpp"
#include <stdexcept>


Client::Client(int fd, mapConfs &mapConfs, ServerConf *defaultConf)
    : _socket(fd), _mapConf(mapConfs), _defaultConf(defaultConf), _server(NULL),
      _response(), _location(NULL), _statusCode(0), _method(""), _uri(""),
      _version(0), _host(""), _requestSize(0), _bodyToRead(-1), _buffer(""),
      _keepConnectionAlive(false), _chunkRequest(false) {
  _time = getTime();
  if (defaultConf == NULL)
    throw (std::logic_error("Default server is NULL"));
  return;
}

Client::~Client(void) { return; }

Client::Client(Client const &copy)
    : _socket(copy._socket), _mapConf(copy._mapConf) {
  if (this != &copy)
    *this = copy;
  return;
}

Client &Client::operator=(Client const &rhs) {
  if (this != &rhs) {
    _defaultConf = rhs._defaultConf;
    _server = rhs._server;
    _location = rhs._location;
    _time = rhs._time;
    _statusCode = rhs._statusCode;
    _method = rhs._method;
    _uri = rhs._uri;
    _queryUri = rhs._queryUri;
    _version = rhs._version;
    _host = rhs._host;
    _headers = rhs._headers;
    _requestSize = rhs._requestSize;
    _bodyToRead = rhs._bodyToRead;
    _buffer = rhs._buffer;
    _keepConnectionAlive = rhs._keepConnectionAlive;
    _chunkRequest = rhs._chunkRequest;
    _response = rhs._response;
  }
  return (*this);
}

time_t Client::getTime(void) { return (std::time(0)); }

bool Client::isTimedOut(void) {
  time_t current;
  time(&current);
  double timeOut = std::difftime(current, _time);
  if (timeOut >= 10.0)
    return (true);
  return (false);
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
  _requestSize = 0;
  _bodyToRead = -1;
  _buffer = "";
  _keepConnectionAlive = false;
  _chunkRequest = false;
  _response.reset();
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
}

const std::string &Client::getBuffer(void) const { return (_buffer); }

int Client::getBodyToRead(void) const { return (_bodyToRead); }

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
  std::cout << "_bodyToRead    = " << _bodyToRead << "\n";
  std::cout << "remain buffer = " << _buffer << std::endl;
}
