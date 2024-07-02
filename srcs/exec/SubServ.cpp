#include "SubServ.hpp"



SubServ::SubServ(int port) : _port(port)
{
	return ;
}

SubServ::~SubServ(void)
{
	return ;
}

SubServ::SubServ(const SubServ &otherSubServ)
{
	if (this != &otherSubServ)
	{
		*this = otherSubServ;
	}
	return;
}

SubServ &SubServ::operator=(const SubServ &otherSubServ)
{
	if (this != &otherSubServ)
	{
	}		
	return(*this);
}