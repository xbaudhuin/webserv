#include "ServerConf.hpp"

cgiException::cgiException(const std::string &s) : exception(), s(s)
{
    this->what();
}

cgiException::~cgiException() throw()
{

}

const char* cgiException::what() const throw(){
    return(this->s.c_str());
}
