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

int	SubServ::acceptNewConnection(void)
{
	int	clientSocket;
	
	clientSocket = accept(this->_serverSocket, NULL, NULL);
	if (clientSocket == -1)
	{
		std::cerr << "webserv: accept: " << strerror(errno) << std::endl;
		return (-1);
	}
	try
	{
		this->_clientSockets.push_back(clientSocket);
	}
	catch(const std::exception& e)
	{
		/* Send error to client ; */
		close(clientSocket);
		std::cerr << "webserv: SubServ::acceptNewConnection: " << e.what() << std::endl;
	}
	
}

int	SubServ::removeClientSocket(int clientSocket)
{
	if (std::find(this->_clientSockets.begin(), this->_clientSockets.end(), clientSocket) != this->_clientSockets.end())
	{
·
	}
}