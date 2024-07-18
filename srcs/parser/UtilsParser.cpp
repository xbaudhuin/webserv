#include "Utils.hpp"

void goToNextIndex(const vec_string &split, size_t &i) {
  size_t pos = 0;
  while (1) {
    pos = split[i].find("}", 0);
    std::cerr << split[i] << std::endl;
    if (pos != std::string::npos)
      break;
    i++;
  }
}

void addErrorPagesNumber(std::vector<int> &vec, const vec_string &split,
                         size_t &i) {
  size_t size = split.size();
  char *ptr = NULL;
  size_t pos = 0;
  i++;
  while (i < size) {

    if (i + 1 < size && split[i + 1] == ";") {
      if (split[i].find('/', 0) == std::string::npos)
        throw std::logic_error("Error:\nError_page directive missing the url");
      break;
    } else if (split[i].find_first_of("{}", 0) != std::string::npos) {
      throw std::logic_error("Error:\nError_page directive found invalid "
                             "because of misuse of '{}'");
    }
    if (split[i][0] == '=')
      ;
    else if ((pos = split[i].find_first_not_of("0123456789", 0)) !=
             std::string::npos) {
      throw std::logic_error(
          "Error:\nError_page directive found invalid because we found non "
          "numerical characters inside the error codes");
    }
    long int code = std::strtol(split[i].c_str(), &ptr, 10);
    if (code < 100 || code > 599) {
      throw std::logic_error(
          "Error:\nError_page directive found invalid because the error_codes "
          "do not conform to standard http request codes limits");
    }
    vec.push_back(static_cast<int>(code));
    i++;
  }
}

void trimWhitespace(std::string &str, const char *whiteSpaces) {
  str.erase(0, str.find_first_not_of(whiteSpaces));
  str.erase(str.find_last_not_of(whiteSpaces) + 1);
}

unsigned char toLower(char c) {
  return (std::tolower(static_cast<unsigned char>(c)));
}

void insertStringInVector(std::vector<char> &vec, const std::string &str) {
  vec.resize(vec.size() + str.size());
  for (size_t i = 0; i < str.size(); i++) {
    vec.push_back(str[i]);
  }
}

std::ostream &operator<<(std::ostream &stream, const std::vector<char> &vec) {
  for (std::vector<char>::const_iterator it = vec.begin(); it != vec.end();
       it++) {
    std::cout << *it;
  }
  return (stream);
}

void resetVector(std::vector<char> &vec) {
  std::vector<char> tmp;
  vec.clear();
  vec.swap(tmp);
}
