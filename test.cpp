#include "./include/headers_hpp/Webserv.hpp"

// class test2
// {
// private:
//     /* data */
// public:
//     test2(/* args */);
//     ~test2();
// };

// test2::test2(/* args */)
// {
//     std::cout << "Test" << std::endl;
// }

// test2::~test2()
// {
//     std::cout << "Test2" << std::endl;

// }


// class test
// {
// private:
//     /* data */
// public:
//     test(/* args */);
//     ~test();
// };

// test::test(/* args */)
// {
//     std::string s;
//     s = "nice";
//     test2 t2;
//     throw std::runtime_error("coucou");
// }

// test::~test()
// {
//     std::cout << "cocoou2" << std::endl;
// }

// // vec_string split(const std::string &str, const std::string &charset)
// // {
// //     vec_string split;
// //     size_t pos = 0;
// //     size_t posend = 0;

// //     while(1)
// //     {
// //         if((pos = str.find_first_not_of(charset, pos)) == std::string::npos)
// //             break;
// //         posend = str.find_first_of(charset, pos);
// //         std::string to_add = str.substr(pos, posend - pos);
// //         split.push_back(to_add);
// //         if(posend == std::string::npos)
// //             break;
// //         pos = posend;
// //     }

// //     return(split);
// // }

// // vec_string tokenizer(std::string &str, const std::string &charset)
// // {
// //     vec_string split;
// //     size_t pos = 0;
// //     size_t posend = 0;
// //     size_t pos_token = 0;

// //     while(1)
// //     {
// //         pos_token = str.find_first_of("{};", pos_token);
// //         if(pos_token != std::string::npos)
// //         {
// //             str.insert(pos_token , " ");
// //             str.insert(pos_token + 2, " ");
// //             pos_token+= 2;
// //         }
// //         if((pos = str.find_first_not_of(charset, pos)) == std::string::npos)
// //             break;
// //         posend = str.find_first_of(charset, pos);
// //         std::string to_add = str.substr(pos, posend - pos);
// //         split.push_back(to_add);
// //         if(posend == std::string::npos)
// //             break;
// //         pos = posend;
// //     }

// //     return(split);
// // }

int main(int argc, char **argv)
{
    (void)argv;
    (void)argc;
    std::string s = "/html/html2/coucou";
    size_t pos = s.size();
    int i = 0;
    pos = s.find_last_of("/", pos);
    std::string s1 = s.substr(0, pos + 1);
    std::string s2;
    if(s1 == s)
        std::cout << "TWINNING !" << std::endl;
    else
    {
        std::cout << "EWW A FILE!" << std::endl;
        s2 = s.substr(pos, s.size());
    }
    while (pos != std::string::npos)
    {   
        pos = s.find_last_of("/", pos);
        std::string s1 = s.substr(0, pos + 1);
        std::cout << s1 << " && " << pos << std::endl;
        if(s2.size() > 0)
            std::cout << s2 << std::endl;
        pos--;
    }
    

    // try
    // {
    //     test t;
    // }
    // catch(const std::exception& e)
    // {
    //     std::cerr << e.what() << '\n';
    // }
    
}