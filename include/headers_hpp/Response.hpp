#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Location.hpp"
#include "Typedef.hpp"
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

class Response {
public:
  // Constructor
  Response(void);
  Response(Response const &copy);

  // Destructor
  ~Response(void);

  // operator
  Response &operator=(Response const &rhs);

  // method
  void setStatusCode(size_t statusCode);
  void setDate(void);
  void setHeader(const std::string &headerName, int value);
  void setHeader(const std::string &headerName, const std::string &headerValue);
  void removeHeader(const std::string &headerName);
  void addLineToBoddy(const std::string &line);
  void setBody(const std::string &body);
  size_t getBodySize(void) const;
  void BuildResponse(void);
  bool isReady(void) const;
  void reset(void);
  std::string &getAllResponse(void);
  std::string getResponse(void);
  void add400(const Response &error);
  bool isNotDone(void) const;

protected:
private:
  static const size_t _sizeMaxResponse;
  static const std::map<size_t, std::string> _mapReasonPhrase;
  std::string _responseLine;
  std::map<std::string, std::string> _headers;
  std::string _body;
  std::string _response;
  bool _ready;
};

#endif //! RESPONSE_HPP
