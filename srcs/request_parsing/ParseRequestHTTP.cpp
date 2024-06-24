#include <iostream>
#include <vector>
#include <algorithm>
#include "Error.hpp"
#include <vector>
#include <sstream>
// HANDLE LF AND CRLF
// REQUIRED: first line: method local_path Version HTTP


void ParseRequest(std::string &request){

  std::vector<std::string> split_request;
  std::istringstream split(request);
  for (std::string each; std::getline(split, each); split_request.push_back(each));

}
