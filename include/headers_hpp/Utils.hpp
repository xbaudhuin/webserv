#ifndef UTILS_HPP
# define UTILS_HPP

#include "Typedef.hpp"

ServerConf parser(const vec_string &split, size_t &i, const size_t &size);
void ParserLocation(const vec_string &split, size_t &i,const size_t &size, ServerConf &cf);
void addErrorPagesNumber(std::vector<int> &vec, const vec_string &split, size_t &i);
void printConfig(const vec_confs &conf);
vec_string split(const std::string &str, const std::string &charset);
vec_string tokenizer(std::string &str, const std::string &charset, const std::string &tokens);
std::string findErrorPage(int error_code, const map_err_pages& map);

#endif
