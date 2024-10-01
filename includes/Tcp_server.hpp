#pragma once


#include "includes_util.hpp"
#include "../includes/servers.hpp"



#define MAX_CONNECTIONS 2048
#define BACKLOGS 512
class TCPserver
{
    public:
        TCPserver(Servers &server);
        ~TCPserver();
        void exit_error(const std::string err_msg);
        std::vector<int> getSocket() ;
        std::vector<int> getCliSocket() ;
        
        // const & Config getConfig();

    private:
        int _backlogs;

        std::vector<int> _Port;
        std::vector<int> _Socket;
        struct sockaddr_in _sockaddr;
        int start_server(Servers &server);
        void close_server();
        void set_non_blocking(int sock);



    
};