#include "Webserv.hpp"

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
    v.push_back("/coucou/test/index.html");
    v.push_back("/");
    v.push_back("/coucou/file");
    v.push_back("/error_pages/test/");
    v.push_back("/error_pages/coucou/html/");
    v.push_back("/error_pages/test/");
    v.push_back("/coucou");
    v.push_back("/notfound/");
    v.push_back("/no");
    
    for (size_t i = 0; i < v.size(); i++)
    {
        try
        {
            this->confs[0].second.getPreciseLocation(v[i]);
        }
        catch(const std::exception& e)
        {
            std::cout << PURP2 << e.what() << RESET << '\n';
        }
        
    }
#endif
#ifdef PRINT
    std::cout << findErrorPage(403, this->confs[0].second.getErrPages()) << std::endl;
#endif
	this->_epollFd = epoll_create1(EPOLL_CLOEXEC);
	if (this->_epollFd == -1)
	{
		std::cerr << "wevserv: Webserv::constructor: epoll_create1: " << strerror(errno) << std::endl;
		throw std::logic_error("webserv: Webserv: failure in constructor");
	}
	std::cout << "webserv: epoll successfully created on fd" << this->_epollFd << std::endl;
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
        this->env = rhs.env;
        this->confs = rhs.confs;
    }
    return(*this);
}

Webserv::~Webserv()
{
	this->closeFds();
}

void Webserv::addEnv(char **env)
{
    this->env_char = env;
    for(size_t i = 0; env[i]; i++)
    {
        std::string str = env[i];
        this->env.push_back(str);
    }
}

char** Webserv::getEnv()
{
    return(this->env_char);
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
    size_t size = this->confs.size();
    for (size_t i = 0; i < size; i++)
    {
        mapSubServs::iterator it = this->_subServs.find(this->confs[i].second.getPort());
        if(it == this->_subServs.end())
        {
            this->_subServs.insert(std::make_pair(this->confs[i].second.getPort(), SubServ(this->confs[i].second)));
        }
        else
        {
            std::string name = this->confs[i].second.getServerNames()[0];
            this->_subServs[this->confs[i].second.getPort()]._portConfs[name] =  &(this->confs[i].second);
        }
    }
    //std::cout << this->_subServs.size() << std::endl;
#ifdef PRINT
    mapSubServs::iterator it = this->_subServs.begin();
    while(it != this->_subServs.end())
    {
        mapConfs::iterator ite = it->second._portConfs.begin();
        while (ite != it->second._portConfs.end())
        {
            std::cout << (*ite->second) << std::endl;
            ite++;
        }
        
        it++;
    }
#endif
}

void Webserv::parse(vec_string split)
{
    int check = 0;
    size_t size = split.size();
    // for(size_t jt = 0; jt < size; jt++)
    // {
    //     std::cout << split[jt] << std::endl;
    // }
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
                this->confs.push_back(std::make_pair(name, newConf));
                check++;
            }
            catch(const std::exception& e)
            {
                writeInsideLog(e, errorParsing);
                // throw;
            }
        }
    }
    // std::cout << "Test: " << this->confs.size() << std::endl;
    if(check)
    {
        // std::cout << "Coucou" << std::endl;
        // printConfig(this->confs);  
    }
    else
        throw std::invalid_argument("Webserv: Error:\nNo configuration found");
    this->createMaps();
}


void Webserv::parseConfig(const std::string &conf)
{
    std::ifstream config;
    size_t check = conf.find(".conf", 0);
    if(check == std::string::npos)
        throw std::invalid_argument("Error\nFile extension isn't a .conf");
    config.open(conf.c_str());
    if(!config.is_open())
        throw std::logic_error("Error\nCouldn't open config file");
    std::stringstream strm;
    strm << config.rdbuf();
    std::string str = strm.str();
    config.close();
   //this->conf.parse(tokenizer(str, " \n\t\r\b\v\f", "{};"));
    this->parse(tokenizer(str, " \n\t\r\b\v\f", "{};"));
}

int	Webserv::removeFdFromIdMap(int fd)
{
	int	result;

	result = this->idMap.erase(fd);
	if (result == 1)
	{
		std::cout << "webserv: successfully erased socket fd " << fd << " from ID Map" << std::endl;
		return (0);
	}
	else
	{
		std::cerr << "webserv: Webserv::removeFdFromIdMap: trying to remove unexisting fd " << fd << std::endl;
		return (1);
	}
}

void	Webserv::closeFds(void)
{
	std::map<int, SubServ*>::iterator	iter;

	iter = this->idMap.begin();
	while (iter != this->idMap.end())
	{
		protectedClose((*iter).first);
		iter++;
	}
	protectedClose(this->_epollFd);
}

void	Webserv::setServerSockets(void)
{
	mapSubServs::iterator	iter;
	int						serverSocket;

	iter = this->_subServs.begin();
	while (iter != this->_subServs.end())
	{
		serverSocket = (*iter).second.initPortSocket();
		if (serverSocket == -1)
		{
			this->closeFds();
			throw std::logic_error("Can not create server socket");
		}
		if (addSocketToEpoll(this->_epollFd, serverSocket, EPOLLIN) != 0)
		{
			this->closeFds();
			throw std::logic_error("Can not add socket to epoll");
		}
		this->idMap[serverSocket] = &((*iter).second);
		iter++;
	}
}

int		Webserv::closeClientConnection(int clientSocket)
{
	std::map<int, SubServ*>::iterator	iter;
	int									status = 0;

	iter = this->idMap.find(clientSocket);
	if (iter == this->idMap.end())
	{
		std::cerr << "webserv: Webserv::closeClientConnection: client socket " << clientSocket << " is not in the ID Map" << std::endl;
		status = 1;
	}
	else if ((*iter).second->removeClientSocket(clientSocket) != 0)
	{
		std::cerr << "webserv: Webserv::closeClientConnection: failed remove client socket from subserv" << std::endl;
		status = 1;
	}
	/* Need to erase request related to the client in Xaviers's class*/
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

int	Webserv::getEpollFd(void)
{
	return (this->_epollFd);
}

int	Webserv::isClientSocket(int fd)
{
	try
	{
		return (this->idMap.at(fd)->isClientSocket(fd));
	}
	catch(const std::exception& e)
	{
		return (false);
	}
}

int	Webserv::isServerSocket(int fd)
{
	try
	{
		return (this->idMap.at(fd)->isServerSocket(fd));
	}
	catch(const std::exception& e)
	{
		return (false);
	}
	
}

int	Webserv::handlePortEvent(int serverSocket)
{
	int	newClient;

	newClient = this->idMap[serverSocket]->acceptNewConnection();
	if (newClient == -1)
	{
		return (1);
	}
	if (addSocketToEpoll(this->_epollFd, newClient, EPOLLIN) != 0)
	{
		this->idMap[serverSocket]->removeClientSocket(newClient);
		protectedClose(newClient);
		return (1);
	}
	try
	{
		this->idMap[newClient] = this->idMap[serverSocket];
	}
	catch(const std::exception& e)
	{
		std::cerr << "wevserv: Webserv::handlePortEvent: " << e.what() << std::endl;
		/* Send error to client */
		this->idMap[serverSocket]->removeClientSocket(newClient);
		protectedClose(newClient);
		return (1);
	}
	return (0);
}

int	Webserv::receive(int clientSocket)
{
	char	buffer[BUFSIZ];
	int		bytesRead;
	int		status = 0;

	bytesRead = recv(clientSocket, buffer, BUFSIZ, 0);
	if (bytesRead < 0)
	{
		std::cerr << "webserv: Webserv::receive: recv: " << strerror(errno) << std::endl;
		this->closeClientConnection(clientSocket);
		return (1);
	}
	else if (bytesRead == 0)
	{
		std::cout << "webserv: client on fd " << clientSocket << " closed connection with the server" << std::endl;
		status = this->closeClientConnection(clientSocket);
	}
	else
	{
		buffer[bytesRead] = '\0';
		/* Add buffer to the request */
		std::cout << "webserv: client on fd " << clientSocket << "says: " << buffer << std::endl;
	}
	return (status);
}

int	Webserv::handleClientEvent(int clientSocket, uint32_t event)
{
	if (event == EPOLLIN)
	{
		return (this->receive(clientSocket));
	}
	else
	{
		return (0);
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
	}
}

int	Webserv::start(void)
{
	int	status;
	struct epoll_event	events[MAX_EVENTS];
	
	std::cout << "webserv: starting server..." << std::endl;
	while (true)
	{
		status = epoll_wait(this->_epollFd, events, MAX_EVENTS, 2000);
		if (status == -1)
		{
			std::cerr << "webserv: Webserv::start: epoll_wait: " << strerror(errno) << std::endl;
			continue;
		}
		else if (status == 0)
		{
			std::cout << "webserv: waiting..." << std::endl;
		}
		else
		{
			this->handleEvents(events, status);
		}
		/* Need to check client time out, fork time out, waitPID ect
		after each epoll_wait() (not only betweeneach event) */
	}
	return (0);
}
