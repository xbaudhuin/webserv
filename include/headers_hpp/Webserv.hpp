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
#include <signal.h>
#include "Client.hpp"

const int BACKLOG_LISTEN = 100;
const int MAX_EVENTS = 500;

enum	signal_handler : int
{
	IGNORE = 0,
	DEFAULT = 1,
	STOP = 2,
};

enum	status : int
{
	SUCCESS = 0,
	FAILURE = 1,
	BAD_FD = -1,
};

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
		mapID					idMap;
        void 					createMaps(void);
		void					setServerSockets(void);
		void					closeFds(void);
		int						receive(int clientSocket);
		int						handlePortEvent(int serverSocket);
		int						handleClientEvent(int clientSocket, uint32_t event);
		void					handleEvents(const struct epoll_event *events, int nbEvents);
		int						removeFdFromIdMap(int fd);
		int						isClientSocket(int fd);
		int						isServerSocket(int fd);
		int						bounceOldClients(void);

    public:
      		 	 Webserv(const char *s);
      		 	 Webserv(const Webserv &rhs);
      		 	 Webserv& operator=(const Webserv &rhs);
       		 	~Webserv();
        void	addEnv(char **env);
        char	** getEnv(void);
		int		closeClientConnection(int clientSocket);
		int		getEpollFd(void);
		int		start(void);
		void	printAllConfig(void);

		class	StopServer : public std::exception
		{
			virtual const char *what() const throw();
		};
};

#endif
