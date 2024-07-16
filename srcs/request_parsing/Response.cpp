#include "Response.hpp"

static std::map<size_t, std::string> initializeStatusMap() {
  std::map<size_t, std::string> m;
  m[100] = " Continue";
  m[101] = " Switching Protocols";
  m[102] = " Processing";
  m[103] = " Early Hints";
  m[200] = " OK";
  m[201] = " Created";
  m[202] = " Accepted";
  m[203] = " Non-Authoritative Information";
  m[204] = " No Content";
  m[205] = " Reset Content";
  m[206] = " Partial Content";
  m[207] = " Multi-Status";
  m[208] = " Already Reported";
  m[226] = " IM Used";
  m[300] = " Multiple Choices";
  m[301] = " Moved Permanently";
  m[302] = " Found";
  m[303] = " See Other";
  m[304] = " Not Modified";
  m[305] = " Use Proxy";
  m[306] = " Switch Proxy";
  m[307] = " Temporary Redirect";
  m[308] = " Permanent Redirect";
  m[400] = " Bad Request";
  m[401] = " Unauthorized";
  m[402] = " Payment Required";
  m[403] = " Forbidden";
  m[404] = " Not Found";
  m[405] = " Method Not Allowed";
  m[406] = " Not Acceptable";
  m[407] = " Proxy Authentication Required";
  m[408] = " Request Timeout";
  m[409] = " Conflict";
  m[410] = " Gone";
  m[411] = " Length Required";
  m[412] = " Precondition Failed";
  m[413] = " Payload Too Large";
  m[414] = " URI Too Long";
  m[415] = " Unsupported Media Type";
  m[416] = " Range Not Satisfiable";
  m[417] = " Expectation Failed";
  m[418] = " I'm a teapot";
  m[421] = " Misdirected Request";
  m[422] = " Unprocessable Entity";
  m[423] = " Locked";
  m[424] = " Failed Dependency";
  m[425] = " Too Early";
  m[426] = " Upgrade Required";
  m[428] = " Precondition Required";
  m[429] = " Too Many Requests";
  m[431] = " Request Header Fields Too Large";
  m[451] = " Unavailable For Legal Reasons";
  m[500] = " Internal Server Error";
  m[501] = " Not Implemented";
  m[502] = " Bad Gateway";
  m[503] = " Service Unavailable";
  m[504] = " Gateway Timeout";
  m[505] = " HTTP Version Not Supported";
  m[506] = " Variant Also Negotiates";
  m[507] = " Insufficient Storage";
  m[508] = " Loop Detected";
  m[510] = " Not Extended";
  m[511] = " Network Authentication Required";
  return m;
}

const std::map<size_t, std::string> Response::_mapReasonPhrase =
    initializeStatusMap();

const size_t Response::_sizeMaxResponse = 100;

Response::Response(void)
    : _responseLine("HTTP/1.1 "), _body(""), _response(""), _ready(false) {
  _headers.insert(std::make_pair("Server:", "Webserv/1.0.0"));
  return;
}

Response::~Response(void) { return; }

Response::Response(Response const &copy) {
  if (this != &copy)
    *this = copy;
  return;
}

Response &Response::operator=(Response const &rhs) {
  if (this != &rhs) {
    _responseLine = rhs._responseLine;
    _headers = rhs._headers;
    _body = rhs._body;
    _response = rhs._response;
    _ready = rhs._ready;
  }
  return (*this);
}

void Response::setDate(void) {
  time_t rawtime;

  time(&rawtime);
  tm *gmtTime = gmtime(&rawtime);
  char buffer[80] = {0};
  std::strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S GMT", gmtTime);
  setHeader("Date", buffer);
}

bool Response::isReady() const { return (_ready); }

void Response::setStatusCode(size_t statusCode) {
  if (_mapReasonPhrase.count(statusCode) != 1)
    return;

  std::ostringstream ss;
  ss << statusCode;
  _responseLine = "HTTP/1.1 ";
  _responseLine += ss.str();
  _responseLine += _mapReasonPhrase.at(statusCode);
}

void Response::removeHeader(const std::string &headerName) {
  _headers.erase(headerName);
}

void Response::setHeader(const std::string &headerName, int value) {
  _headers.erase(headerName);
  std::ostringstream ss;
  ss << value;
  _headers.insert(std::make_pair(headerName, ss.str()));
}

void Response::setHeader(const std::string &headerName,
                         const std::string &headerValue) {
  _headers.erase(headerName);
  _headers.insert(std::make_pair(headerName, headerValue));
}

void Response::addLineToBoddy(const std::string &line) {
  _body += line + "\r\n";
}

void Response::setBody(const std::string &body) {
  _body = body;
  std::ostringstream ss;
  ss << _body.size();
  setHeader("Content-Length", ss.str());
}

void Response::BuildResponse(void) {
  _response = _responseLine + "\r\n";
  std::map<std::string, std::string>::const_iterator it = _headers.begin();
  for (; it != _headers.end(); it++) {
    _response += (*it).first + ": " + (*it).second + "\r\n";
  }
  _response += "\r\n";
  _response += _body;
  _ready = true;
  return;
}

void Response::reset(void) {
  _responseLine = "";
  _headers.clear();
  _headers.insert(std::make_pair("Server", "Webserv/1.0.0"));
  _body = "";
}

size_t Response::getBodySize(void) const { return (_body.size()); }

bool Response::isNotDone(void) const {
  if (_response.empty() == true)
    return (false);
  return (true);
}

void Response::add400(const Response &error) {
  _response += "\r\n";
  _response += error._response;
}

std::string Response::getResponse(void) {
  // std::cout << BLUE << "inside response: " << _response << RESET << std::endl;
  std::string ret = _response.substr(0, _sizeMaxResponse);
  _response.erase(0, _sizeMaxResponse);
  return (ret);
}
