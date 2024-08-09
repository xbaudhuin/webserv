#ifndef CGIEXCEPTION_HPP
# define CGIEXCEPTION_HPP

#include <iostream>
#include <exception>

class cgiException : private std::exception
{
  private:
      std::string s;

  public:
      cgiException(const std::string &s);
      const char* what() const throw();
      ~cgiException() throw();
};

#endif
