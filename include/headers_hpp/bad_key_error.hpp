#ifndef BAD_KEY_ERROR_HPP
# define BAD_KEY_ERROR_HPP

#include <iostream>
#include <exception>

class bad_key_error : public std::exception
{
  private:
      std::string s;

  public:
      bad_key_error(const std::string &s);
      const char* what() const throw();
      ~bad_key_error() throw();
};

#endif
