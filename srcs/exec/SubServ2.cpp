#include "SubServ.hpp"

SubServ::SubServ(ServerConf &serv) : _main(&serv)
{
    this->_portConfs[serv.getServerNames()[0]] = &serv;
    this->_port = serv.getPort();
}
