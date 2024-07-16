#include "ServerConf.hpp"

security_error::security_error(const std::string &s) : exception(), s(s)
{
    this->what();
}

security_error::~security_error() throw()
{

}

const char* security_error::what() const throw(){
    return(this->s.c_str());
}
