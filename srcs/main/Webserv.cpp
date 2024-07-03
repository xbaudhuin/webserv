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
        config = "./config/good_config/test.conf";
    this->parseConfig(config);

	this->_epollFd = epoll_create1(EPOLL_CLOEXEC);
	if (this->_epollFd == -1)
	{
		std::cerr << "wevserv: Webserv::constructor: epoll_create1: " << strerror(errno) << std::endl;
		throw std::logic_error("webserv: Webserv: failure in constructor");
	}
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
        this->conf = rhs.conf;
    }
    return(*this);
}

Webserv::~Webserv()
{
	close(this->_epollFd);
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
    this->conf.parse(tokenizer(str, " \n\t\r\b\v\f", "{};"));
}

int	Webserv::addSocketToEpoll(int socketFd)
{
	struct epoll_event	epollEvent;
	int					status;
	
	std::memset(&epollEvent, 0, sizeof (epollEvent));
	epollEvent.events = EPOLLIN;
	epollEvent.data.fd = socketFd;
	status = epoll_ctl(this->_epollFd, EPOLL_CTL_ADD, socketFd, &epollEvent);
	if (status != 0)
	{
		std::cerr << "webserv: Webserv::addSocketToEpoll: epoll_ctl: " << strerror(errno) << std::endl;
		return (1);
	}
	std::cout << "webserv: successfully add socket fd " << socketFd << " to epoll fd " << this->_epollFd << std::endl;
	return (0);
}

int	Webserv::removeFdFromIdMap(int socketFd)
{
	SubServ	subservTemp;

	try
	{
		subservTemp = this->idMap.at(socketFd);
		this->idMap.erase(socketFd);
		std::cout << "webserv: successfully erased socket fd " << socketFd << " from ID Map" << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << "webserv: Webserv::removeFdFromIdMap: trying to remove unexisting fd" << std::endl;
		return (1);
	}
	return (0);
}