#include "ServerConf.hpp"

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

std::ostream& operator<<(std::ostream& out, const Location& loc){
    if(loc.getUrl().size() >  0)
        out << "Url: " << loc.getUrl() << "\n\t";
    out << "Server Root: " << loc.getRootServer() << "\n\t";
    out << "Exact Match: " << (loc.isExactMatch() ? "YES" : "NO") << "\n\t";
    out << "Base Uri: " << loc.myUri() << "\n\t";
    if(loc.hasRoot())
        out << "Root: " << loc.getRoot() << "\n\t";
    if(loc.hasAlias())
        out << "Alias: " << loc.getAlias() << "\n\t";
    if(loc.getIndexFile().size() > 0)
        for (size_t j = 0; j < loc.getIndexFile().size(); j++)
        {
            out << "Index File[" << j << "]: " << loc.getIndexFile()[j] << "\n\t";
        }
    if(loc.getCgi().size() > 0)
    {
        for (size_t j = 0; j < loc.getCgi().size(); j++)
        {
            out << "Cgi File: " << loc.getCgi()[j].second << "\n\t";
        }
    }
    if(loc.getUploadLocation().size() > 0)
        out << "Cgi Upload Location: " << loc.getUploadLocation() << "\n\t";
    if(loc.getRedirection().size() > 0)
        out << "Redirection URL and CODE: " << loc.getRedirection() << " && " << loc.getRedirCode() << "\n\t";
    out << "Path info: " << (loc.hasPathInfo() ? "ON" : "OFF") << "\n\t";
    out << "Limit body size: " << loc.getLimitBodySize() << "\n\t";
    out << "Method GET status: " << (loc.getGetStatus() ? "on" : "off") << "\n\t";
    out << "Method POST status: " << (loc.getPostStatus() ? "on" : "off") << "\n\t";
    out << "Method DELETE status: " << (loc.getDeleteStatus() ? "on" : "off") << "\n\t";
    out << "Directory Listing Status: " << loc.getAutoIndex() << std::endl;
    return(out);
}


std::ostream& operator<<(std::ostream& out, const ServerConf& cf)
{
    ServerConf serv = cf;
    for (size_t i = 0; i < serv.getServerNames().size(); i++)
    {
        out << BLUE << "Server Name n["<< i << "]: " << serv.getServerNames()[i] << RESET << std::endl;
    }
    out << YELLOW << "Root: " << serv.getRoot() << std::endl;
#if PRINT == 4
    map_err_pages err = serv.getErrPages();
    map_err_pages::iterator err_it = err.begin();
    out << PURP2 << "Error pages: " << RESET << std::endl;
    while (err_it != err.end())
    {
      out << PURP2 << "Code Error: " << err_it->first << " && url: " << err_it->second << RESET << std::endl;
      err_it++;
    }
#endif
    out << GREEN << "Max body size: " << serv.getLimitBodySize() << RESET << std::endl;
    uint32_t ip = ntohl(serv.getHost());
    out << ORANGE << "IP: " 
              << ((ip >> 24) & 0xFF) << "."
              << ((ip >> 16) & 0xFF) << "."
              << ((ip >> 8) & 0xFF) << "."
              << (ip & 0xFF) << std::endl;
    out << "Port: " << serv.getPort() << RESET <<std::endl;
    vec_location loc = serv.getLocations();
    size_t size_loc = loc.size();
    out << YELLOW << "Now printing locations:" << std::endl;
    for (size_t i = 0; i < size_loc; i++)
    {
        out << "Loc[" << i << "]:\n\t";
        out << loc[i] << std::endl;
    }
    out << RESET << "\t\tRank: " << serv.getRank() << "\n"<< std::endl;       
    return(out);
}

