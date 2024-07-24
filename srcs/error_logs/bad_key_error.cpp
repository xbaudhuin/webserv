#include "ServerConf.hpp"

bad_key_error::bad_key_error(const std::string &s) : exception(), s(s)
{
    this->what();
}

bad_key_error::~bad_key_error() throw()
{

}

const char* bad_key_error::what() const throw(){
    return(this->s.c_str());
}
