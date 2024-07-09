#include "SubServ.hpp"

SubServ::SubServ()
{
	return ;
}

SubServ::SubServ(ServerConf &serv) : _main(&serv)
{
    this->_portConfs[serv.getServerNames()[0]] = &serv;
    this->_port = serv.getPort();
}


SubServ::~SubServ(void)
{
	return ;
}

// SubServ::SubServ(const SubServ &otherSubServ)
// {
// 	if (this != &otherSubServ)
// 	{
// 		*this = otherSubServ;
// 	}
// 	return;
// }

SubServ &SubServ::operator=(const SubServ &otherSubServ)
{
	if (this != &otherSubServ)
	{
	}		
	return(*this);
}

const ServerConf	*SubServ::getConf(const std::string &serverName)
{
	try
	{
		return ((this->_portConfs.at(serverName)));
	}
	catch(const std::exception& e)
	{
		return (this->_main);
	}
	
}

int	SubServ::acceptNewConnection(void)
{
	int	clientSocket;
	
	clientSocket = accept(this->_serverSocket, NULL, NULL);
	if (clientSocket == -1)
	{
		std::cerr << "webserv: SubServ::acceptNewConncetion: accept: " << strerror(errno) << std::endl;
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
		return (-1);
	}
	return (clientSocket);
}

int	SubServ::removeClientSocket(int clientSocket)
{
	std::vector<int>::iterator	iter;

	iter = std::find(this->_clientSockets.begin(), this->_clientSockets.end(), clientSocket);
	if (iter != this->_clientSockets.end())
	{
		this->_clientSockets.erase(iter);
		std::cout << "webserv: successfully remove client socket " << clientSocket << " in subserver listening on port " << this->_port << std::endl;
		return (0);
	}
	else
	{
		std::cerr << "webserv: SubServ::removeClientSocket: trying to remove non existing client socket from vector of subserv port " <<  this->_port << std::endl;
		return (1);
	}
}

bool	SubServ::isClientSocket(int socketFd)
{
	if (std::find(this->_clientSockets.begin(), this->_clientSockets.end(), socketFd) != this->_clientSockets.end())
	{
		return (false);
	}
	else
	{
		return (true);
	}
}

bool	SubServ::isServerSocket(int socketFd)
{
	if (socketFd == this->_serverSocket)
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

int	SubServ::initServerSocket(void)
{
	this->_serverSocket = createServerSocket(this->_port);
	return (this->_serverSocket);
}

int	SubServ::getPort(void)
{
	return (this->_port);
}