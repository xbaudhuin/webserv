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
# include "sockets.hpp"


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

    public:
      		 	 Webserv(const char *s);
      		 	 Webserv(const Webserv &rhs);
      		 	 Webserv& operator=(const Webserv &rhs);
       		 	~Webserv();
        void	addEnv(char **env);
        char	** getEnv(void);
		int		addSocketToEpoll(int socketFd);
		int		removeFdFromIdMap(int socketFd);
		int		closeClientConnection(int clientSocket);
		int		getEpollFd(void);
		
};

#endif
