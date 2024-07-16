#include "SubServ.hpp"

SubServ::SubServ()
{
	return ;
}

SubServ::SubServ(ServerConf &serv) : _main(&serv)
{
	for (size_t i = 0; i < serv.getServerNames().size(); i++)
	{
    	this->_portConfs[serv.getServerNames()[i]] = &serv;
	}
    this->_port = serv.getPort();
	this->_address = serv.getHost();
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
		this->_port = otherSubServ._port;
		this->_serverSocket = otherSubServ._serverSocket;
		this->_clientSockets = otherSubServ._clientSockets;
		this->_clientRequests = otherSubServ._clientRequests;
		this->_portConfs = otherSubServ._portConfs;
		this->_main = otherSubServ._main;
		this->_address = otherSubServ._address;

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
		protectedClose(clientSocket);
		std::cerr << "webserv: SubServ::acceptNewConnection: " << e.what() << std::endl;
		return (-1);
	}
	try
	{
		this->_clientRequests.insert(std::make_pair(clientSocket, Client(clientSocket, this->_portConfs, this->_main)));
	}
	catch(const std::exception& e)
	{
		protectedClose(clientSocket);
		std::cerr << "webserv: SubServ::acceptNewConnection: " << e.what() << std::endl;
		this->removeSocketFromClientVector(clientSocket);
		return (-1);
	}
	std::cout << "wevserv: new client connection accepted on port " << this->_port << " to socket fd " << clientSocket << std::endl;
	return (clientSocket);
}

int	SubServ::removeSocketFromRequestMap(int socket)
{
	int	status;

	status = this->_clientRequests.erase(socket);
	if (status == 0)
	{
		std::cerr << "webserv: SubServ::removeSocketFromRequestMap: could not remove socket fd " << socket << " from request map" << std::endl;
		return (1);
	}
	std::cout << "webserv: successfully remove client socket " << socket << " in subserver map listening on port " << this->_port << std::endl;
	return (0);
}

int	SubServ::removeSocketFromClientVector(int socket)
{
	std::vector<int>::iterator	iter;

	iter = std::find(this->_clientSockets.begin(), this->_clientSockets.end(), socket);
	if (iter != this->_clientSockets.end())
	{
		this->_clientSockets.erase(iter);
		std::cout << "webserv: successfully remove client socket " << socket << " in subserver vector listening on port " << this->_port << std::endl;
		return (0);
	}
	else
	{
		std::cerr << "webserv: SubServ::removeClientSocket: trying to remove non existing client socket from vector of subserv port " <<  this->_port << std::endl;
		return (1);
	}
}

int	SubServ::removeClientSocket(int clientSocket)
{
	int status;

	status = 0;
	status += this->removeSocketFromClientVector(clientSocket);
	status += this->removeSocketFromRequestMap(clientSocket);
	return (status);
}

bool	SubServ::isClientSocket(int fd)
{
	if (std::find(this->_clientSockets.begin(), this->_clientSockets.end(), fd) != this->_clientSockets.end())
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

bool	SubServ::isServerSocket(int fd)
{
	if (fd == this->_serverSocket)
	{
		return (true);
	}
	else
	{
		return (false);
	}
}

int	SubServ::initPortSocket(void)
{
	this->_serverSocket = createServerSocket(this->_port, this->_address);
	return (this->_serverSocket);
}

int	SubServ::getPort(void)
{
	return (this->_port);
}

void	SubServ::addClientsToBounce(std::vector<int> &clientsToBounce)
{
	mapClients::iterator	iter;

	iter = this->_clientRequests.begin();
	while (iter != this->_clientRequests.end())
	{
		if ((*iter).second.isTimedOut() == true)
		{
			clientsToBounce.push_back((*iter).first);
		}
		iter++;
	}
}

Client	*SubServ::getClient(int clientSocket)
{
	Client	*clientToReturn;

	try
	{
		clientToReturn = &this->_clientRequests.at(clientSocket);
		return (clientToReturn);
	}
	catch(const std::exception& e)
	{
		return (NULL);
	}
}

void	SubServ::printPortConfs(void)
{
	std::cout << "Main = "<< this->_main << std::endl;
	mapConfs::iterator	iter2 = this->_portConfs.begin();
	while (iter2 != this->_portConfs.end())
	{
		std::cout << "---SERVER NAME : " <<(*iter2).first << "---" << std::endl << std::endl;
		std::cout << *(*iter2).second << std::endl;
		iter2++;
	}
}

void	SubServ::addToConf(const std::string &name, ServerConf *newConf)
{
	this->_portConfs[name] = newConf;
}