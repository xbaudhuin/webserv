#include "Webserv.hpp"

extern int	gSignal;

Webserv::Webserv()
{
}

Webserv::Webserv(const char* file)
{
    std::string config;

    if(file)
        config = file;
    else
	{
        config = "./config/good_config/test.conf";
	}
    this->parseConfig(config);
#if PRINT == 2
    vec_string v;
    v.push_back("/coucou/test/test2");
    
    for (size_t i = 0; i < v.size(); i++)
    {
        try
        {
            this->_confs[0].second.getPreciseLocation(v[i]);
        }
        catch(const std::exception& e)
        {
            std::cout << PURP2 << e.what() << RESET << '\n';
        }
    }
#endif
	this->_epollFd = epoll_create1(EPOLL_CLOEXEC);
	if (this->_epollFd == -1)
	{
		std::cerr << "wevserv: Webserv::constructor: epoll_create1: " << strerror(errno) << std::endl;
		throw std::logic_error("webserv: Webserv: failure in constructor");
	}
	std::cout << "webserv: epoll successfully created on fd " << this->_epollFd << std::endl;
	this->setServerSockets();
}


Webserv::Webserv(const Webserv& rhs)
{
    this->operator=(rhs);
}

Webserv& Webserv::operator=(const Webserv &rhs)
{
    if(this != &rhs)
    {
        this->_env = rhs._env;
        this->_confs = rhs._confs;
    }
    return(*this);
}

Webserv::~Webserv()
{
	this->closeFds();
}

void Webserv::addEnv(char **env)
{
    this->_env_char = env;
    for(size_t i = 0; env[i]; i++)
    {
        std::string str = env[i];
        this->_env.push_back(str);
    }
}

char** Webserv::getEnv() const
{
    return(this->_env_char);
}

bool checkNumberBrackets(const vec_string &split)
{
    size_t size = split.size();
    size_t count = 0;

    for (size_t i = 0; i < size; i++)
    {
        if(split[i] == "{")
        {
            count++;
        }
        else if(split[i] == "}")
        {
            if(count == 0 )
                return 1;
            count--;
        }
    }
    return 0;
}

void Webserv::createMaps(void)
{
    size_t size = this->_confs.size();

    for (size_t i = 0; i < size; i++)
    {
		std::pair<uint32_t, int> IpPortPair(this->_confs[i].second.getHost(), this->_confs[i].second.getPort());
        //mapPorts::iterator it = this->_Ports.find(this->_confs[i].second.getPort());
		mapPorts::iterator it = this->_Ports.find(IpPortPair);

        if(it == this->_Ports.end())
        {
            //this->_Ports.insert(std::make_pair(this->_confs[i].second.getPort(), Port(this->_confs[i].second)));
			this->_Ports.insert(std::make_pair(IpPortPair, Port(this->_confs[i].second)));
        }
        else
        {
			for (size_t j = 0; j < this->_confs[i].second.getServerNames().size(); j++)
			{
            	std::string name = this->_confs[i].second.getServerNames()[j];
				// std::pair<uint32_t, int>(this->_confs[i].second.getHost(), this->_confs[i].second.getPort());
				this->_Ports[IpPortPair].addToConf(name, &(this->_confs[i].second));
				//this->_Ports[this->_confs[i].second.getPort()].addToConf(name, &(this->_confs[i].second));
			}
        }
    }
}

void Webserv::parse(vec_string split)
{

    int check = 0;
    size_t size = split.size();

    if(checkNumberBrackets(split))
        return(errorParsing("Issue with the file, uneven number of {}"));
    for(size_t i = 0; i < size; i++)
    {
        if(split[i] == "server")
        {
            i++;
            try
            {
                ServerConf newConf = parser(split, i, size);
                vec_string name = newConf.getServerNames();
                this->_confs.push_back(std::make_pair(name, newConf));
                check++;
            }
            catch(const std::exception& e)
            {
                writeInsideLog(e, errorParsing);
            }
        }
    }
    if(!check)
    {
        throw std::invalid_argument("Webserv: Error:\nNo configuration found");
	}
	this->checkConfigs();
    this->createMaps();
}


void Webserv::parseConfig(const std::string &conf)
{
    std::ifstream	config;
    size_t 			check = conf.find(".conf", 0);

    if(check == std::string::npos)
        throw std::invalid_argument("Error\nFile extension isn't a .conf");
    config.open(conf.c_str());
    if(!config.is_open())
        throw std::logic_error("Error\nCouldn't open config file");
    std::stringstream strm;
    strm << config.rdbuf();
    std::string str = strm.str();
    config.close();
    this->parse(tokenizer(str, " \n\t\r\b\v\f", "{};"));
}

int	Webserv::removeFdFromIdMap(int fd)
{
	int	result;

	result = this->_idMap.erase(fd);
	if (result == 1)
	{
		std::cout << "webserv: successfully erased socket fd " << fd << " from ID Map" << std::endl;
		return (SUCCESS);
	}
	else
	{
		std::cerr << "webserv: Webserv::removeFdFromIdMap: trying to remove unexisting fd " << fd << std::endl;
		return (FAILURE);
	}
}

void	Webserv::closeFds(void)
{
	std::map<int, Port*>::iterator	iter;

	iter = this->_idMap.begin();
	while (iter != this->_idMap.end())
	{
		protectedClose((*iter).first);
		iter++;
	}
	protectedClose(this->_epollFd);
}

void	Webserv::setServerSockets(void)
{
	mapPorts::iterator	iter;
	int					serverSocket;

	iter = this->_Ports.begin();
	while (iter != this->_Ports.end())
	{
		serverSocket = (*iter).second.initPortSocket();
		if (serverSocket == BAD_FD)
		{
			this->closeFds();
			throw std::logic_error("Can not create server socket");
		}
		if (addSocketToEpoll(this->_epollFd, serverSocket, EPOLLIN) != SUCCESS)
		{
			this->closeFds();
			throw std::logic_error("Can not add socket to epoll");
		}
		this->_idMap[serverSocket] = &((*iter).second);
		iter++;
	}
}

int	Webserv::closeClientConnection(int clientSocket)
{
	mapID::iterator	iter;
	int				status = 0;

	iter = this->_idMap.find(clientSocket);
	if (iter == this->_idMap.end())
	{
		std::cerr << "webserv: Webserv::closeClientConnection: client socket " << clientSocket << " is not in the ID Map" << std::endl;
		status = 1;
	}
	else if ((*iter).second->removeClientSocket(clientSocket) != 0)
	{
		std::cerr << "webserv: Webserv::closeClientConnection: failed remove client socket from Port" << std::endl;
		status = 1;
	}
	this->removeFdFromIdMap(clientSocket);
	if (epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, clientSocket, NULL) != 0)
	{
		std::cerr << "webserv: Webserv::closeClientConnection: epoll_ctl: " << strerror(errno) << std::endl;
		status = 1;
	}
	status += protectedClose(clientSocket);
	if (status == 0)
	{
		std::cout << "webserv: successfully closed connection with client on socket fd " << clientSocket << std::endl;
	}
	return (status);
}

int	Webserv::getEpollFd(void) const
{
	return (this->_epollFd);
}

int	Webserv::isClientSocket(int fd) const
{
	try
	{
		return (this->_idMap.at(fd)->isClientSocket(fd));
	}
	catch(const std::exception& e)
	{
		return (false);
	}
}

int	Webserv::isServerSocket(int fd) const
{
	try
	{
		return (this->_idMap.at(fd)->isServerSocket(fd));
	}
	catch(const std::exception& e)
	{
		return (false);
	}
	
}

int	Webserv::isOldClient(int fd) const
{
	try
	{
		return (this->_idMap.at(fd)->isOldClient(fd));
	}
	catch(const std::exception& e)
	{
		return (false);
	}
}

int	Webserv::bounceOldClients(void)
{
	mapID::iterator	current;
	mapID::iterator	next;
	int				socket;

	current = this->_idMap.begin();
	while (true)
	{
		if (current == this->_idMap.end())
		{
			break ;
		}
		next = ++current;
		current--;
		socket = (*current).first;
		if (this->isClientSocket(socket) == true && this->isOldClient(socket) == true)
		{
			this->closeClientConnection(socket);
		}
		current = next;
	}
	return (SUCCESS);
}

int	Webserv::handlePortEvent(int serverSocket)
{
	int	newClient;

	newClient = this->_idMap[serverSocket]->acceptNewConnection();
	if (newClient == BAD_FD)
	{
		return (FAILURE);
	}
	if (addSocketToEpoll(this->_epollFd, newClient, EPOLLIN | EPOLLRDHUP) != SUCCESS)
	{
		this->_idMap[serverSocket]->removeClientSocket(newClient);
		protectedClose(newClient);
		return (FAILURE);
	}
	try
	{
		this->_idMap[newClient] = this->_idMap[serverSocket];
	}
	catch(const std::exception& e)
	{
		std::cerr << "wevserv: Webserv::handlePortEvent: " << e.what() << std::endl;
		if (epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, newClient, NULL) != SUCCESS)
			std::cerr << "webserv: Webserv::handlePortEvent: epoll_ctl: " << strerror(errno) << std::endl;
		this->_idMap[serverSocket]->removeClientSocket(newClient);
		protectedClose(newClient);
		return (FAILURE);
	}
	return (SUCCESS);
}

int	Webserv::receive(int clientSocket)
{
	char				buffer[BUFSIZ];
	int					bytesRead;
	Client				*clientRequest;
	std::vector<char>	vecBuffer;
	try
	{
		bytesRead = recv(clientSocket, buffer, BUFSIZ - 1, 0);
		if (bytesRead < 0)
		{
			std::cerr << "webserv: Webserv::receive: recv: " << strerror(errno) << std::endl;
			this->closeClientConnection(clientSocket);
			return (FAILURE);
		}
		else if (bytesRead == 0)
		{
			std::cout << "webserv: client on fd " << clientSocket << " closed connection with the server (via recv of size 0)" << std::endl;
			return (this->closeClientConnection(clientSocket));
		}
		else
		{
			vecBuffer.insert(vecBuffer.begin(), &buffer[0], &buffer[bytesRead]);
			clientRequest = this->_idMap[clientSocket]->getClient(clientSocket);
			if (clientRequest == NULL)
			{
				return(this->closeClientConnection(clientSocket));
			}
			if (clientRequest->addBuffer(vecBuffer) == true)
			{
				if (changeEpollEvents(this-> _epollFd, clientSocket, (EPOLLIN | EPOLLOUT | EPOLLRDHUP)) != SUCCESS)
				{
					return (this->closeClientConnection(clientSocket));
				}
				std::cout << "webserv: changing epoll event to EPOLLIN | EPOLLRDHUP | EPOLLOUT for fd " << clientSocket << std::endl;
			}
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << "webserv: Webserv::receive: catch error: " << e.what() << std::endl;
		return (this->closeClientConnection(clientSocket));
	}
	return (SUCCESS);
}

int	Webserv::handleEndResponse(int clientSocket, const Client* clientRequest)
{
	if (clientRequest->keepConnectionOpen() == false)
	{
		return (this->closeClientConnection(clientSocket));
	}
	else
	{
		if (changeEpollEvents(this-> _epollFd, clientSocket, (EPOLLIN | EPOLLRDHUP)) != SUCCESS)
		{
			this->closeClientConnection(clientSocket);
			return (FAILURE);
		}
		std::cout << "webserv: changing epoll event to EPOLLIN | EPOLLRDHUP for fd " << clientSocket << std::endl;
		return (SUCCESS);
	}	
}

int	Webserv::respond(int clientSocket, uint32_t events)
{
	int					bytesSend;
	std::vector<char>	response;
	bool				remainRequest;
	Client				*clientRequest;

	try
	{
		clientRequest = this->_idMap[clientSocket]->getClient(clientSocket);
		if (clientRequest == NULL)
		{
			return(this->closeClientConnection(clientSocket));
		}
		if (checkEvent(events, EPOLLIN) == true)
		{
			clientRequest->add400Response();
		}
		remainRequest = clientRequest->sendResponse(response);
		bytesSend = send(clientSocket, &response[0], response.size(), 0);
		std::cout << "Bytes send to fd " << clientSocket << " = " << bytesSend << std::endl;
		if (bytesSend < 0)
		{
			std::cerr << "webserv: Webserv::respond: send: " << strerror(errno) << std::endl;
			return (this->closeClientConnection(clientSocket));
		}
		if (remainRequest == false)
		{
			return (this->handleEndResponse(clientSocket, clientRequest));
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << "webserv: Webserv::respond: catch error: " << e.what() << std::endl;
		this->closeClientConnection(clientSocket);
		return (FAILURE);
	}
	return (SUCCESS);
}

int	Webserv::handleClientEvent(int clientSocket, uint32_t event)
{
	if (checkEvent(event, EPOLLRDHUP))
	{
		std::cout << "webserv: event EPOLLRDHUP triggered for client on fd " << clientSocket << std::endl;
		return (this->closeClientConnection(clientSocket));
	}
	else if (checkEvent(event, EPOLLIN))
	{
		std::cout << "webserv: event EPOLLIN triggered for client on fd " << clientSocket << std::endl;
		return (this->receive(clientSocket));
	}
	else if (checkEvent(event, EPOLLOUT))
	{
		std::cout << "webserv: event EPOLLOUT triggered for client on fd " << clientSocket << std::endl;
		return (this->respond(clientSocket, event));
	}
	else
	{
		std::cerr << "webserv: Webserv::handleClientEvent: unknown event for client on fd " << clientSocket << std::endl;
		return (FAILURE);
	}
}

void	Webserv::handleEvents(const struct epoll_event *events, int nbEvents)
{
	int	fd;

	for (int i = 0; i < nbEvents; i++)
	{
		fd = events[i].data.fd;
		if (this->isServerSocket(fd) == true)
		{
			this->handlePortEvent(fd);
		}
		else if (this->isClientSocket(fd) == true)
		{
			this->handleClientEvent(fd, events[i].events);
		}
		else
		{
			std::cerr << "webserv: Webserv::handleEvents: file descriptor is neither a server socket nor a client socket" << std::endl;
		}
		this->doCheckRoutine();
	}
}

void	Webserv::printAllConfig(void)
{
	mapPorts::iterator	iter = this->_Ports.begin();
	while (iter != this->_Ports.end())
	{
		std::cout << "adress IP map key = " << (*iter).first.first << " | port map key = " << (*iter).first.second << std::endl;
		std::cout << "Port = " << (*iter).second.getPort() << std::endl;
		(*iter).second.printPortConfs();
		++iter;
	}
}

void	Webserv::checkSigint(void) const
{
	if (gSignal == SIGINT)
	{
		throw Webserv::StopServer();
	}
}

void	Webserv::doCheckRoutine(void)
{
	this->checkSigint();
	this->bounceOldClients();
}

// int	test(void)
// {
// 	std::vector<int> vec;

// 	for (size_t i = 0; i < 5; i++)
// 	{
// 		try
// 		{
			
// 				vec.push_back(i);
// 			if (i == 2)
// 				throw std::logic_error("test");
// 		}
// 		catch(const std::exception& e)
// 		{
// 			std::cerr << e.what() << '\n';
// 		}
// 	}
// 	for (size_t i = 0; i < vec.size(); i++)
// 	{
// 		std::cout << "[" << i << "] = " << vec[i] << std::endl;
// 	}
// 	return (SUCCESS);
// }

int	Webserv::start(void)
{
	int					nbEvent;
	struct epoll_event	events[MAX_EVENTS];

#ifdef PRINT
	this->printAllConfig();
#endif
	std::cout << "webserv: starting server..." << std::endl;
	while (true)
	{
		nbEvent = epoll_wait(this->_epollFd, events, MAX_EVENTS, 500);
		if (nbEvent == -1)
		{
			std::cerr << "webserv: Webserv::start: epoll_wait: " << strerror(errno) << std::endl;
			continue;
		}
		else if (nbEvent == 0)
		{
			std::cout << "webserv: waiting..." << std::endl;
		}
		else
		{
			this->handleEvents(events, nbEvent);
		}
		this->doCheckRoutine();
	}
	return (SUCCESS);
}

const char	*Webserv::StopServer::what(void) const throw()
{
	return ("Server is stoping");
}
