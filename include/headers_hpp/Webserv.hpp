#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "Config.hpp"
#include "Error.hpp"
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include "SubServ.hpp"
#include "ServerConf.hpp"
#include "Typedef.hpp"
#include "Utils.hpp"
#include "sockets.hpp"

const int BACKLOG_LISTEN = 100;
const int MAX_EVENTS = 500;

class Webserv
{
    private:
		int					    _epollFd;
		mapSubServs	            _subServs;
        vec_string              env;
        char                    **env_char;
        vec_confs               confs;
        Webserv();
        void                    parseConfig(const std::string &conf);
        void                    parse(vec_string split); 
		std::map<int, SubServ*>	idMap;
        void 					createMaps(void);
		void					setServerSockets(void);
		void					closeFds(void);
		int						handlePortEvent(int serverSocket);
		void					handleEvents(const struct epoll_event *events, int nbEvents);

    public:
      		 	 Webserv(const char *s);
      		 	 Webserv(const Webserv &rhs);
      		 	 Webserv& operator=(const Webserv &rhs);
       		 	~Webserv();
        void	addEnv(char **env);
        char	** getEnv(void);
		int		removeFdFromIdMap(int fd);
		int		closeClientConnection(int clientSocket);
		int		getEpollFd(void);
		int		start(void);
		int		isClientSocket(int fd);
		int		isServerSocket(int fd);
		
};

#endif
