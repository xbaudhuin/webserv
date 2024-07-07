#include "./include/headers_hpp/Webserv.hpp"

// vec_string split(const std::string &str, const std::string &charset)
// {
//     vec_string split;
//     size_t pos = 0;
//     size_t posend = 0;

//     while(1)
//     {
//         if((pos = str.find_first_not_of(charset, pos)) == std::string::npos)
//             break;
//         posend = str.find_first_of(charset, pos);
//         std::string to_add = str.substr(pos, posend - pos);
//         split.push_back(to_add);
//         if(posend == std::string::npos)
//             break;
//         pos = posend;
//     }

//     return(split);
// }

// vec_string tokenizer(std::string &str, const std::string &charset)
// {
//     vec_string split;
//     size_t pos = 0;
//     size_t posend = 0;
//     size_t pos_token = 0;

//     while(1)
//     {
//         pos_token = str.find_first_of("{};", pos_token);
//         if(pos_token != std::string::npos)
//         {
//             str.insert(pos_token , " ");
//             str.insert(pos_token + 2, " ");
//             pos_token+= 2;
//         }
//         if((pos = str.find_first_not_of(charset, pos)) == std::string::npos)
//             break;
//         posend = str.find_first_of(charset, pos);
//         std::string to_add = str.substr(pos, posend - pos);
//         split.push_back(to_add);
//         if(posend == std::string::npos)
//             break;
//         pos = posend;
//     }

//     return(split);
// }

int main(int argc, char **argv)
{
    (void) argc;
    std::ifstream config;
    config.open(argv[1]);
    if(!config.is_open())
        throw std::logic_error("Error\nCouldn't open config file");
    std::stringstream strm;
    strm << config.rdbuf();
    std::string str = strm.str();
    config.close();
    for (size_t i = 100; i < 512; i++)
    {
        if(i == 404 || i == 403 || i == 500)
            continue;
        else if (i == 104)
        {
            i = 200;
        }
        else if (i == 104)
        {
            i = 226;
        }
        else if (i == 209)
        {
            i = 226;
        }
        
        else if (i == 227)
        {
            i = 300;
        }
        else if (i == 309)
        {
            i = 400;
        }
        else if (i == 419)
        {
            i = 421;
        }
        else if (i == 427)
        {
            i++;
        }
        else if (i == 430)
        {
            i = 431;
        }
        else if (i == 432)
        {
            i = 451;
        }
        else if (i == 452)
        {
            i = 501;
        }
        else if (i == 509)
        {
            i++;
        }
        std::stringstream tmp;
        tmp << i;
        std::string tmp2 = tmp.str();
        std::string filename = "html/" + tmp2 + ".html";
        // std::cout  << filename << std::endl;
        std::string buf = str;
        size_t pos = 0;
        while ((pos = buf.find(tmp2, pos)) != std::string::npos)
        {
            // std::cout << "coucou" << std::endl;
            buf.erase(pos, tmp2.size());
            buf.insert(pos, tmp2);
            pos += tmp2.size();
        }
        
        std::ofstream outfile;
        outfile.open(filename.c_str());
        outfile << buf << std::endl;
    }
}