#ifndef TYPEDEF_HPP
# define TYPEDEF_HPP

#include <map>
#include <vector>
#include <iostream>
#include <stdint.h>
#include <algorithm>
#include <map>
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>

class	Port;
class	ServerConf;
class	Location;
class	Client;

typedef std::map<std::pair<uint32_t, int>, Port> mapPorts; /* key = pair IP address / port */
typedef std::map<std::string, ServerConf*> mapConfs;
typedef std::vector<std::string> vec_string;
typedef std::map<int, std::string> map_err_pages;
typedef	std::map<int, Port*> mapID;
typedef std::vector<Location> vec_location;
typedef std::map<int, Client> mapClients;
typedef std::vector<std::pair<vec_string, ServerConf> > vec_confs;
typedef	std::map<int, pid_t> mapPID;

#endif
