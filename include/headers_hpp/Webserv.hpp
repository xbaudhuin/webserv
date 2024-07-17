#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "Error.hpp"
#include "Port.hpp"
#include "ServerConf.hpp"
#include "sockets.hpp"
#include <signal.h>
#include "Client.hpp"

const int MAX_EVENTS = 500;

enum	signal_handler
{
	IGNORE = 0,
	DEFAULT = 1,
	STOP = 2,
};

class Webserv
{
    private:
		int			_epollFd;
		mapPorts	_Ports;
        vec_string	_env;
        char		**_env_char;
        vec_confs	_confs;
        			Webserv();
        void		parseConfig(const std::string &conf);
        void		parse(vec_string split); 
		void		checkConfigs(void);
		mapID		_idMap;
        void		createMaps(void);
		void		setServerSockets(void);
		void		closeFds(void);
		int			receive(int clientSocket);
		int			respond(int clientSocket, uint32_t events);
		int			handlePortEvent(int serverSocket);
		int			handleClientEvent(int clientSocket, uint32_t event);
		void		handleEvents(const struct epoll_event *events, int nbEvents);
		int			removeFdFromIdMap(int fd);
		int			isClientSocket(int fd) const;
		int			isServerSocket(int fd) const;
		int			bounceOldClients(void);
		int			closeClientConnection(int clientSocket);
		void		printAllConfig(void);	
		void		checkSigint(void) const;
		void		doCheckRoutine(void);
		int			handleEndResponse(int clientSocket, const Client* clientRequest);

    public:
      		 	 Webserv(const char *s);
      		 	 Webserv(const Webserv &rhs);
      		 	 Webserv& operator=(const Webserv &rhs);
       		 	~Webserv();
        void	addEnv(char **env);
        char	** getEnv(void) const;
		int		getEpollFd(void) const;
		int		start(void);

		class	StopServer : public std::exception
		{
			virtual const char *what() const throw();
		};
};

#endif
