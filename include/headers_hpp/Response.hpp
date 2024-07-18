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
  void addReadToBoddy(const std::string &read);
  void setBody(const std::vector<char> &body, size_t size);
  void setBody(const std::string &body, size_t size);
  size_t getBodySize(void) const;
  void BuildResponse(void);
  bool isReady(void) const;
  void reset(void);
  std::string &getAllResponse(void);
  std::vector<char> getResponse(void);
  void add400(const Response &error);
  bool isNotDone(void) const;

  std::vector<char> getBody(void);

protected:
private:
  static const std::map<size_t, std::string> _mapReasonPhrase;
  std::string _responseLine;
  std::map<std::string, std::string> _headers;
  std::vector<char> _body;
  std::vector<char> _response;
  bool _ready;
};

#endif //! RESPONSE_HPP
