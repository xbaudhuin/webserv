#include "Client.hpp"

const char *Client::_validMethods[] = {"GET",     "POST",  "DELETE",
                                       "HEAD",    "PUT",   "CONNECT",
                                       "OPTIONS", "TRACE", "PATCH"};

const size_t Client::_methodSize = 9;

const char *Client::_whiteSpaces = " \t";

const size_t Client::_uriMaxSize = 8192;

const size_t Client::_headerMaxSize = 8192;

const size_t Client::_headersMaxBuffer = 32768;

const size_t Client::_sizeMaxResponse = 8000;

std::map<std::string, char> initMap() {
  std::map<std::string, char> m;

  m["%20"] = ' ';
  m["%21"] = '!';
  m["%22"] = '"';
  m["%23"] = '#';
  m["%24"] = '$';
  m["%25"] = '%';
  m["%26"] = '&';
  m["%27"] = '\'';
  m["%28"] = '(';
  m["%29"] = ')';
  m["%2A"] = '*';
  m["%2B"] = '+';
  m["%2C"] = ',';
  m["%2D"] = '-';
  m["%2E"] = '.';
  m["%2F"] = '/';
  m["%30"] = '0';
  m["%31"] = '1';
  m["%32"] = '2';
  m["%33"] = '3';
  m["%34"] = '4';
  m["%35"] = '5';
  m["%36"] = '6';
  m["%37"] = '7';
  m["%38"] = '8';
  m["%39"] = '9';
  m["%3A"] = ':';
  m["%3B"] = ';';
  m["%3C"] = '<';
  m["%3D"] = '=';
  m["%3E"] = '>';
  m["%3F"] = '?';
  m["%40"] = '@';
  m["%41"] = 'A';
  m["%42"] = 'B';
  m["%43"] = 'C';
  m["%44"] = 'D';
  m["%45"] = 'E';
  m["%46"] = 'F';
  m["%47"] = 'G';
  m["%48"] = 'H';
  m["%49"] = 'I';
  m["%4A"] = 'J';
  m["%4B"] = 'K';
  m["%4C"] = 'L';
  m["%4D"] = 'M';
  m["%4E"] = 'N';
  m["%4F"] = 'O';
  m["%50"] = 'P';
  m["%51"] = 'Q';
  m["%52"] = 'R';
  m["%53"] = 'S';
  m["%54"] = 'T';
  m["%55"] = 'U';
  m["%56"] = 'V';
  m["%57"] = 'W';
  m["%58"] = 'X';
  m["%59"] = 'Y';
  m["%5A"] = 'Z';
  m["%5B"] = '[';
  m["%5C"] = '\\';
  m["%5D"] = ']';
  m["%5E"] = '^';
  m["%5F"] = '_';
  m["%60"] = '`';
  m["%61"] = 'a';
  m["%62"] = 'b';
  m["%63"] = 'c';
  m["%64"] = 'd';
  m["%65"] = 'e';
  m["%66"] = 'f';
  m["%67"] = 'g';
  m["%68"] = 'h';
  m["%69"] = 'i';
  m["%6A"] = 'j';
  m["%6B"] = 'k';
  m["%6C"] = 'l';
  m["%6D"] = 'm';
  m["%6E"] = 'n';
  m["%6F"] = 'o';
  m["%70"] = 'p';
  m["%71"] = 'q';
  m["%72"] = 'r';
  m["%73"] = 's';
  m["%74"] = 't';
  m["%75"] = 'u';
  m["%76"] = 'v';
  m["%77"] = 'w';
  m["%78"] = 'x';
  m["%79"] = 'y';
  m["%7A"] = 'z';
  m["%7B"] = '{';
  m["%7C"] = '|';
  m["%7D"] = '}';
  m["%7E"] = '~';
  m["%7F"] = '\x7F';
  return m;
}

const std::map<std::string, char> Client::_uriEncoding = initMap();
