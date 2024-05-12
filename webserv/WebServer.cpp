#include "Webserver.hpp"
#include <vector>


WebServ::WebServ()
{
    // fill infos from config class;
    std::vector<int> port;
    port.push_back(4242);
    _ports.push_back(port);
}

WebServ::~WebServ()
{
    for (int i = 0; i < _servers.size(); i++)
        delete _servers[i];
}

void WebServ::run_servers()
{
    for (int i = 0; i < _ports.size(); i++)
    {
        _servers.push_back(new TCPserver(_ports[i]));
    }
    SetListeners();
    while (true)
    {
        ready_Rsockets = current_Rsockets;
        ready_Wsockets = current_Wsockets;
        if (select(FD_SETSIZE, &ready_Rsockets, &ready_Wsockets, NULL, NULL) < 0)
                std::cerr << "Error in select(): " << strerror(errno) << std::endl;
        for (int idx = 0; idx < FD_SETSIZE; idx++)  
        {
            if (FD_ISSET(idx, &ready_Rsockets))
            {
                std::vector<int>::iterator it = std::find(servers_fds.begin(), servers_fds.end(), idx);
                if (it != servers_fds.end() /*&& servers[i]->getClients().size() < MAX_CONNECTIONS*/)
                {
                    int new_socket = 0;
                    socklen_t _sockaddr_len = sizeof(_sockaddr);
                    if ((new_socket = accept(*it, (sockaddr *)&_sockaddr,  &_sockaddr_len)) < 0)
                        perror("Server failed to accept incoming connection");
                    clients_fds.push_back(new_socket);
                    FD_SET(new_socket, &current_Rsockets);
                    FD_SET(new_socket, &current_Wsockets);
                    printf("New connection accepted.\n");
                }
                else
                {
                    // Handle parsing of the request for the ready for read socket
                    FD_CLR(idx, &current_Rsockets);
                }
            }
            else if (FD_ISSET(idx, &ready_Wsockets))
            {
                // send response to the ready for write socket if the request is fullfilled !
                FD_CLR(idx, &current_Wsockets);
            }
        }
    }
}



void WebServ::SetListeners()
{
    FD_ZERO(&current_Rsockets);
    FD_ZERO(&current_Wsockets);
    for (int i = 0; i < _servers.size(); i++)
    {
        for (int j = 0; j < _servers[i]->getSocket().size(); j++)
        {
            servers_fds.push_back(_servers[i]->getSocket()[j]);
            FD_SET(_servers[i]->getSocket()[j], &current_Rsockets);
            FD_SET(_servers[i]->getSocket()[j], &current_Wsockets);
        }
    }
}
