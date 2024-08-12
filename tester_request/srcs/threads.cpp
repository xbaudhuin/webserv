#include "tester_request.hpp"

void * launch_request(void *v)
{
    t_request *r = (t_request*)v;
    if(!r->os->is_open())
        return(NULL);
    char buffer[20048] = {0};
    if ((r->sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        *(r->os) << "Error: fail to create socket" << std::endl;
        r->os->close();
        delete r->os;
        return NULL;
    }
    memset(&(r->serv_adr), 0, sizeof(r->serv_adr));
    r->serv_adr.sin_family = AF_INET;
    r->serv_adr.sin_port = htons(r->PORT);
    if (inet_pton(AF_INET, r->host, &(r->serv_adr).sin_addr) <= 0)
    {
        *(r->os) << "Error: fail to inet_pton" << std::endl;
        close(r->sock);
        r->os->close();
        delete r->os;
        return (NULL);
    }
    if (connect(r->sock, (struct sockaddr *)&(r->serv_adr), sizeof(r->serv_adr)) < 0)
    {
        *(r->os) << "Connection failed" << std::endl;
        close(r->sock);
        r->os->close();
        delete r->os;
        return (NULL);
    }
    send(r->sock, r->request.c_str(), r->request.size(), 0);
    *(r->os) << "**********SENT REQUEST [" << r->i << "] :********** \n" << r->request << "\n" << std::endl;
    r->valread = read(r->sock, buffer, 20048);
    *(r->os) << buffer << std::endl;
    *(r->os) << "READ = " << r->valread << "\n" << std::endl;
    r->valread = read(r->sock, buffer, 20048);
    *(r->os) << buffer << "\n" << std::endl;
    *(r->os) << "READ = " << r->valread << std::endl;
    close(r->sock);
    r->os->close();
    delete r->os;
    return(NULL);
}
