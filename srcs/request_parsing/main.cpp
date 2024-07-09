#include "Client.hpp"
#include "Config.hpp"
#include "Error.hpp"
#include "Location.hpp"
#include "ServerConf.hpp"
#include "SubServ.hpp"
#include "Webserv.hpp"
#include <exception>
#include <iostream>
#include <utility>

int main() {
  try {
    std::string http_request = "GET /coucou/test/index.html HTTP/1.0145\r\n";
    http_request += "host: bonjour\r\n";
    // http_request += "host: bonjour2\n\r";
    http_request += "\r\n";
    ServerConf *cf = new ServerConf;
    cf->addPortOrHost("127.0.0.1:443");
    cf->addServerName("Webserv");
    Location loc;
    loc.addUrl("/coucou/test/", "");
    loc.setAutoIndex("on");
    loc.setMethod("POST", "on");
    loc.setMethod("DELETE", "on");
    loc.setMethod("GET", "on");
    loc.setIndexFile("index.html");
    loc.fixUrl("/html");
    loc.fixIndexFile();
    loc.fixRoot();
    cf->addLocation(loc);
    Location test;
    test.addUrl("/coucou/test/test2/", "");
    test.setAutoIndex("on");
    test.setMethod("POST", "on");
    test.setMethod("DELETE", "on");
    test.setMethod("GET", "on");
    test.setIndexFile("index.html");
    test.fixUrl("/html");
    test.fixIndexFile();
    test.fixRoot();
    cf->addLocation(test);
    Location loc2;
    loc2.addUrl("/coucou/test2/", "");
    loc2.setAutoIndex("on");
    loc2.setMethod("POST", "on");
    loc2.setMethod("DELETE", "on");
    loc2.setMethod("GET", "on");
    loc2.setIndexFile("index.html");
    loc2.fixUrl("/html");
    loc2.fixIndexFile();
    loc2.fixRoot();
    cf->addLocation(loc2);
    cf->setRootToErrorPages();
    std::cout << *cf << std::endl;
    mapConfs map;
    // map[cf->getServerNames()] = cf;
    std::cout << RED << "serverName = " << cf->getMainServerName() << RESET
              << std::endl;
    map.insert(std::make_pair(cf->getMainServerName(), cf));
    Client request(5, map, cf);
    size_t pos = http_request.find_first_of('\r', 0);
    if (pos == http_request.npos)
      std::cout << RED << "no \\r" << RESET << std::endl;
    else
      std::cout << GREEN << "found \\r" << RESET << std::endl;
    request.addBuffer(http_request);
    request.print();
    delete cf;
  } catch (std::exception &e) {
    std::cout << "Caught error: " << e.what() << std::endl;
  }
}
