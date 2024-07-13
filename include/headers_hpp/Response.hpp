#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Location.hpp"
#include "Typedef.hpp"
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
  void setHeader(const std::string &headerName, const std::string &headerValue);
  void removeHeader(const std::string &headerName);
  void setBody(const std::string &body);
  void getResponse(std::string &response) const;

protected:
private:
  static const std::map<size_t, std::string> _mapReasonPhrase;
  std::string _responseLine;
  std::map<std::string, std::string> _headers;
  std::string _body;
};

#endif //! RESPONSE_HPP
