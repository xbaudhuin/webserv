#include "Error.hpp"
#include "Config.hpp"

using std::cout;

void printConfig(const map_confs &cf)
{
    map_confs::const_iterator it = cf.begin();
    while(it != cf.end())
    {
        for (size_t i = 0; i < it->first.size(); i++)
        {
            cout << BLUE << "Server Name n["<< i << "]: " << it->first[i] << RESET << std::endl;
        }
        
        ServerConf serv = it->second;
        map_err_pages err = serv.getErrPages();
        map_err_pages::iterator err_it = err.begin();
        cout << PURP2 << "Error pages: " << RESET << std::endl;
        while (err_it != err.end())
        {
          cout << PURP2 << "Code Error: " << err_it->first << " && url: " << err_it->second << RESET << std::endl;
          err_it++;
        }
        cout << GREEN << "Max body size: " << serv.getLimitBodySize() << RESET << std::endl;
        uint32_t ip = ntohl(serv.getHost());
        cout << ORANGE << "IP: " 
                  << ((ip >> 24) & 0xFF) << "."
                  << ((ip >> 16) & 0xFF) << "."
                  << ((ip >> 8) & 0xFF) << "."
                  << (ip & 0xFF) << std::endl;
        cout << "Port: " << serv.getPort() << RESET <<std::endl;
        vec_location loc = serv.getLocations();
        size_t size_loc = loc.size();
        cout << YELLOW << "Now printing locations:" << std::endl;
        for (size_t i = 0; i < size_loc; i++)
        {
            cout << "Loc[" << i << "]:\n\t";
            if(loc[i].getUrl().size() >  0)
                cout << "Url: " << loc[i].getUrl() << "\n\t";
            if(loc[i].getRoot().size() >  0)
                cout << "Root: " << loc[i].getRoot() << "\n\t";
            if(loc[i].getRedirection().size() > 0)
                cout << "Redirection URL and CODE: " << loc[i].getRedirection() << " && " << loc[i].getRedirCode() << "\n\t";
            cout << "Limit body size: " << loc[i].getLimitBodySize() << "\n\t";
            cout << "Directory Listing Status: " << loc[i].getAutoIndex() << std::endl;
        }
        cout << RESET << "\t\tRank: " << serv.getRank() << "\n"<< std::endl;
        
        it++;
    }
}