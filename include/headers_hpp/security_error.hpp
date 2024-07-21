#ifndef SECURITY_ERROR_HPP
# define SECURITY_ERROR_HPP

#include <iostream>
#include <exception>

class security_error : public std::exception
{
  private:
      std::string s;

  public:
      security_error(const std::string &s);
      const char* what() const throw();
      ~security_error() throw();
};

#endif
