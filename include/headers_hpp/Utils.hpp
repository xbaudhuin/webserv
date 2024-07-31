#ifndef UTILS_HPP
#define UTILS_HPP

#include "Colors.hpp"
#include "Typedef.hpp"
#include "bad_key_error.hpp"
#include "cgiException.hpp"
#include "security_error.hpp"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

ServerConf parser(const vec_string &split, size_t &i, const size_t &size);
void ParserLocation(const vec_string &split, size_t &i, const size_t &size,
                    ServerConf &cf);
void addErrorPagesNumber(std::vector<int> &vec, const vec_string &split,
                         size_t &i);
void printConfig(const vec_confs &conf);
vec_string split(const std::string &str, const std::string &charset);
vec_string tokenizer(std::string &str, const std::string &charset,
                     const std::string &tokens);
std::vector<char> findErrorPage(int error_code, ServerConf &map);

void trimWhitespace(std::string &str, const char *whiteSpaces);
unsigned char toLower(char c);
std::ostream &operator<<(std::ostream &stream, const std::vector<char> &vec);
// void resetVector(std::vector<char> &vec);
template <typename T> void resetVector(std::vector<T> &vec) {
  std::vector<T> tmp;
  vec.clear();
  vec.swap(tmp);
}
#endif
