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
    v.push_back("/");
    
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

	// for (size_t i = 0; i < split.size(); i++)
	// {
	// 	std::cout << split[i] << " && " << i << " | ";
	// }
	
    for (size_t i = 0; i < size; i++)
    {
        if(split[i] == "{")
        {
            count++;
        }
        else if(split[i] == "}")
        {
            count--;
        }
		// std::cout << split[i] << " && " << i << " && " << size << std::endl;
    }
	if(count != 0)
		return 1;
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
        throw std::invalid_argument("Webserv: Error:\nIssue with the file, uneven number of {}");
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
#if PRINT == 3
	try
	{
		std::cout << findErrorPage(600, this->_confs[0].second) << std::endl;
		// if(this->_confs[0].second.getLocations()[0].isCgi("/html/python/cgi/cookies.php"))
		// {
		// 	std::cout << RED << this->_confs[0].second.getLocations()[0].getExecutePath("/html/python/cgi/cookies.php") << std::endl;
		// 	std::cout << this->_confs[0].second.getLocations()[0].getCgiFile("/html/python/cgi/cookies.php") << std::endl;
		// 	std::cout << this->_confs[0].second.getLocations()[0].getCgiPath("/html/python/cgi/cookies.php") << RESET << std::endl;
		// }
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
#endif
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

int	Webserv::removeFromMapPID(int fd)
{
	int	result;

	result = this->_PID.erase(fd);
	if (result == 1)
	{
		std::cout << "webserv: successfully erased socket fd " << fd << " from PID Map" << std::endl;
		return (SUCCESS);
	}
	else
	{
		std::cerr << "webserv: Webserv::removeFdFromMapPID: trying to remove unexisting fd " << fd << std::endl;
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
	else if ((*iter).second->removeClientSocket(clientSocket) != SUCCESS)
	{
		std::cerr << "webserv: Webserv::closeClientConnection: failed remove client socket from Port" << std::endl;
		status = 1;
	}
	if (this->_PID.find(clientSocket) != this->_PID.end())
	{
		kill(this->_PID[clientSocket], SIGKILL);
		waitpid(this->_PID[clientSocket], NULL, 0);
		this->removeFromMapPID(clientSocket);
	}
	this->removeFdFromIdMap(clientSocket);
	if (epoll_ctl(this->_epollFd, EPOLL_CTL_DEL, clientSocket, NULL) != SUCCESS)
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


int Webserv::getSocketFromPID(pid_t pid) const
{
	mapPID::const_iterator	iter;

	iter = this->_PID.begin();
	while (iter != this->_PID.end())
	{
		if ((*iter).second == pid)
		{
			return ((*iter).first);
		}
		iter++;
	}
	std::cerr << "webserv: Webserv::getSocketFromPID " << pid << " pid is not present in the map (maybe kill by destructor)" << std::endl;
	return (BAD_FD);
}

int	Webserv::isClientSocket(int fd) const
{
	mapID::const_iterator	iter;

	iter = this->_idMap.find(fd);
	if (iter == this->_idMap.end())
	{
		std::cerr << "webserv: Webserv::isClientSocket: fd " << fd << " is not in ID map" << std::endl; 
		return (false);
	}
	else
	{
		return ((*iter).second->isClientSocket(fd));
	}
}

int	Webserv::isServerSocket(int fd) const
{
	mapID::const_iterator	iter;

	iter = this->_idMap.find(fd);
	if (iter == this->_idMap.end())
	{
		std::cerr << "webserv: Webserv::isClientSocket: fd " << fd << " is not in ID map" << std::endl; 
		return (false);
	}
	else
	{
		return ((*iter).second->isServerSocket(fd));
	}
}

bool	Webserv::isOldClient(int fd) const
{
	mapID::const_iterator	iter;

	iter = this->_idMap.find(fd);
	if (iter == this->_idMap.end())
	{
		std::cerr << "webserv: Webserv::isOldClient: fd " << fd << " is not in ID map" << std::endl; 
		return (false);
	}
	else
	{
		return ((*iter).second->isOldClient(fd));
	}
}

bool	Webserv::isOldChild(int fd) const
{
	mapID::const_iterator	iter;

	iter = this->_idMap.find(fd);
	if (iter == this->_idMap.end())
	{
		std::cerr << "webserv: Webserv::isOldChild: fd " << fd << " is not in ID map" << std::endl; 
		return (false);
	}
	else
	{
		return ((*iter).second->isOldChild(fd));
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

int	Webserv::killOldChilds(void)
{
	mapPID::iterator	current;
	mapPID::iterator	next;
	int					fd;
	pid_t				pid;
	int					status;
	
	current = this->_PID.begin();
	while (true)
	{
		if (current == this->_PID.end())
		{
			break ;
		}
		fd = (*current).first;
		pid = (*current).second;
		next = ++current;
		current--;
		if (this->isClientSocket(fd) == true && this->isOldChild(fd) == true)
		{
			if (kill(pid, SIGKILL) != SUCCESS)
			{
				std::cerr << "webserv: Webserv::killOldChilds: kill: " << strerror(errno) << std::endl;
			}
			else
			{
				if (waitpid(pid, &status, 0) != pid)
					std::cerr << "webserv: Webserv::killOldChilds: waitpid: " << strerror(errno) << std::endl;
				else
					this->handleChildExit(pid, status);
				std::cout << "webserv: successfully killed process with PID " << pid << " attached to socket client fd " << fd << std::endl;
			}
		}
		current = next;
	}
	return (SUCCESS);
}

int	Webserv::handleChildExit(pid_t pid, int codeExit)
{
	int		fd;
	Client	*request;

	fd = this->getSocketFromPID(pid);
	if (fd == BAD_FD)
	{
		return (FAILURE);
	}
	this->removeFromMapPID(fd);
	if (changeEpollEvents(this->_epollFd, fd, (EPOLLIN | EPOLLOUT | EPOLLRDHUP)) != SUCCESS)
	{
		this->closeClientConnection(fd);
		return (FAILURE);
	}
	std::cout << "webserv: changing epoll event (in handleChildExit) to EPOLLIN | EPOLLOUT | EPOLLRDHUP for fd " << fd << std::endl;
	if (this->_idMap.find(fd) == this->_idMap.end())
	{
		std::cerr << "webserv: Webserv::handleChildExit: fd " << fd << "does not exist in map ID" << std::endl;
		this->closeClientConnection(fd);
		return (FAILURE);
	}
	request = this->_idMap[fd]->getClient(fd);
	if (request == NULL)
	{
		this->closeClientConnection(fd);
		return (FAILURE);
	}
	request->setStatusCode(getExitStatus(codeExit));
	std::cout << "webserv: send exit status " << codeExit << " to Client class for fd " << fd << std::endl;
	return (SUCCESS);
}

int	Webserv::checkChildsEnd(void)
{
	pid_t	pid;
	int		status;

	if (this->_PID.size() == 0)
	{
		return (SUCCESS);
	}
	while (true)
	{
		pid = waitpid((pid_t)-1, &status, WNOHANG);
		if (pid == 0)
		{
			break ;
		}
		else if (pid == (pid_t)-1)
		{
			std::cerr << "wevserv: Webserv::checkChildsEnd: waitpid: " << std::endl;
			return (FAILURE);
		}
		else
		{
			this->handleChildExit(pid, status);
		}
		std::cout << "PID = " << pid << std::endl;
	}
	return (SUCCESS);
}

int	Webserv::checkTooManyRequests(int clientSocket)
{
	Client	*request;

	request = this->_idMap[clientSocket]->getClient(clientSocket);
	if (request == NULL)
	{
		this->closeClientConnection(clientSocket);
		return (FAILURE);
	}
	if (this->_idMap.size() > MAX_FD)
	{
		try
		{
			std::cout << request->isTimedOut() << std::endl;
			request->addErrorResponse(503);
		}
		catch(const std::exception& e)
		{
			std::cerr << "webserv: Webserv::checkTooManyRequests: catch error: " << e.what() << std::endl;
			this->closeClientConnection(clientSocket);
		}
		if (changeEpollEvents(this->_epollFd, clientSocket, (EPOLLIN | EPOLLOUT | EPOLLRDHUP)) != SUCCESS)
		{
			this->closeClientConnection(clientSocket);
			return (FAILURE);
		}
		std::cout << "webserv: can not process request for client on fd " << clientSocket << " due to too many requests" << std::endl;
		return (SUCCESS);
	}
	else
	{
		return (SUCCESS);
	}
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
	return (this->checkTooManyRequests(newClient));
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
			clientRequest->addCgiToMap(this->_PID);
		}
	}
	catch(const cgiException &e)
	{
		throw e;
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
			std::cout << "webserv: EPOLLIN event detected inside EPOLLOUT event, we will send error 400 to client on fd " << clientSocket << std::endl;
			clientRequest->addErrorResponse(400);
		}
		remainRequest = clientRequest->sendResponse(response);
		std::cout << "webserv: size of response = " << response.size() << std::endl;
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
			std::cerr << "webserv: Webserv::handleEvents: fd " << fd << " is neither a server socket nor a client socket. It may be bounced for time out before traiting event" << std::endl;
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
	this->killOldChilds();
	this->checkChildsEnd();
}

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
