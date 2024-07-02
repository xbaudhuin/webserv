#include "Config.hpp"
#include "Error.hpp"
#include "HTTPRequest.hpp"
#include "Location.hpp"
#include "ServerConf.hpp"
#include "Webserv.hpp"
#include <exception>
#include <iostream>

int main(){
  try{
  std::string http_request = "POST /index.html HTTP/1.0145\n\r";
  http_request += "host: bonjour\n\r";
  http_request += "host: bonjour2\n\r";
  http_request += "\n\r";
  std::istringstream rq(http_request);
  HTTPRequest request(rq);
  request.print();
  }
         catch (std::exception &e){
    std::cout << "Caught error: " << e.what() << std::endl;
  }
}
