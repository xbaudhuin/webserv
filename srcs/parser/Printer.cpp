#include "Error.hpp"
#include "Config.hpp"

using std::cout;

void printConfig(const vec_confs &cf)
{
    vec_confs::const_iterator it = cf.begin();
    while(it != cf.end())
    {
        cout << it->second << std::endl;
        it++;
    }
}