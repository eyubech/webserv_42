
#include "../includes/WebServer.hpp"
#include "../includes/PhpCgiHandler.hpp"

WebServ::WebServ(){
    this->max_fd = 0;
}

WebServ::~WebServ()
{
    for (unsigned int i = 0; i < _servers.size(); i++)
        delete _servers[i];
}

void WebServ::run_servers(std::map<int, std::vector<Servers> > grouped, std::vector<Servers> confs)
{
    (void)grouped;
    // int pos;
    // int find;
    for (unsigned int i = 0; i <  confs.size(); i++)
    {
        // for (unsigned int j = 0; j < confs[i].size(); j++){
            _servers.push_back(new TCPserver(confs[i]));
        // }
    }
    SetListeners();
// exit(0);
    while (true)
    {
        ready_Rsockets = current_Rsockets;
        ready_Wsockets = current_Wsockets;
        int selectResult = select(max_fd + 1, &ready_Rsockets, &ready_Wsockets, NULL, NULL);
        if (selectResult < 0) {
            std::cerr << "Error in select(): " << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }
        //Loop through the Server FDs//////////
        for (unsigned int idx = 0; idx < servers_fds.size(); idx++)
        {
            if (FD_ISSET(servers_fds[idx], &ready_Rsockets))
            {
                int new_socket = 0;
                socklen_t _sockaddr_len = sizeof(_sockaddr);
                if ((new_socket = accept(servers_fds[idx], (sockaddr *)&_sockaddr,  &_sockaddr_len)) < 0){
                    perror("Server failed to accept incoming connection");
                    continue;
                }
                if (set_non_blocking(new_socket) < 0) {
                    std::cerr << "Failed to set non-blocking for socket " << new_socket << ": " << strerror(errno) << std::endl;
                    close(new_socket);
                    continue;
                }
                std::cout << "New connection accepted on clientFD: " << new_socket << std::endl;
                Client *newClient = new Client(new_socket);
                _clients.insert(std::make_pair(new_socket, newClient));
                std::cout << "new clients :" << new_socket << std::endl;
                // Servers server = getConf(servers_fds[idx], grouped);
                // _clients[new_socket]->setConf(server);
                FD_SET(new_socket, &current_Rsockets);
                if (new_socket > this->max_fd)
                    this->max_fd = new_socket;
            }
        }
        ////////////////////

        //Loop through the Clients FDs//////////
        for (itClient it_cli = _clients.begin();  it_cli != _clients.end();)
        {
            if (FD_ISSET(it_cli->first, &ready_Rsockets))
            {
                read_request(it_cli->first);
                try
                {
                    start_parsing(it_cli->first, grouped);
                    if ( _clients[it_cli->first]->_request.getHeader("Transfer-Encoding") != _clients[it_cli->first]->_request.getEndHeaders())
                    {
                        if (_buffer.find("0\r\n\r\n") != std::string::npos)
                        {
                            _clients[it_cli->first]->_request.findTypeOfPostMethod();
                            _clients[it_cli->first]->_request.setBody(_buffer);
                            _buffer.clear();
                            FD_CLR(it_cli->first, &current_Rsockets);
                            FD_SET(it_cli->first, &current_Wsockets);
                        }
                    }
                    else if (_clients[it_cli->first]->_request.getHeader("Content-Length")  != _clients[it_cli->first]->_request.getEndHeaders())
                    {
                        if (_buffer.size() == static_cast<size_t>(stoi(_clients[it_cli->first]->_request.getHeader("Content-Length")->second)))
                        {
                            _clients[it_cli->first]->_request.findTypeOfPostMethod();
                            _clients[it_cli->first]->_request.setBody(_buffer);
                            _buffer.clear();
                            FD_CLR(it_cli->first, &current_Rsockets);
                            FD_SET(it_cli->first, &current_Wsockets);
                        }
                    }
                }
                catch(...) 
                {
                    std::string htmlfile = "<!DOCTYPE html>\n";
                                htmlfile += "<html lang=\"en\">\n";
                                htmlfile += "<head>\n";
                                htmlfile += "    <meta charset=\"UTF-8\">\n";
                                htmlfile += "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
                                htmlfile += "    <title>Document</title>\n";
                                htmlfile += "</head>\n";
                                htmlfile += "<body>\n";
                                htmlfile += "    <div>\n";
                                htmlfile += "        <p style=\"color: red; font-size: 35px;\"> ERROR </p>\n";
                                htmlfile += "    </div>\n";
                                htmlfile += "</body>\n";
                                htmlfile += "</html>\n";
                    _clients[it_cli->first]->_response.setHeader("Content-Type", "text/html");
                    _clients[it_cli->first]->_response.setHeader("Content-Length", htmlfile.length());
                    _clients[it_cli->first]->_response.setBody(htmlfile);
                    _clients[it_cli->first]->_response.generateHeaderResponse();
                    FD_CLR(it_cli->first, &current_Rsockets);
                    FD_SET(it_cli->first, &current_Wsockets);
                    _clients.at(it_cli->first)->_response.Send(it_cli->first);
                    _clients.at(it_cli->first)->setTypeData(CLOSESOCKET);
                }
            }
            if (FD_ISSET(it_cli->first, &ready_Wsockets))
            {
                if (_clients.at(it_cli->first)->getOnetime() == false) {
                    handler = createHandler(_clients.at(it_cli->first)->_request);
                }
                handler->handleRequest(_clients.at(it_cli->first));
                if (_clients.at(it_cli->first)->getTypeData() == WRITEDATA)
                {
                    _clients.at(it_cli->first)->_response.Send(it_cli->first);
                    if ( _clients.at(it_cli->first)->_response.getResponse().size() == 0) {
                            _clients.at(it_cli->first)->setTypeData(CLOSESOCKET);
                    }
                }
                if (_clients.at(it_cli->first)->getTypeData() == CLOSESOCKET)
                {
                    FD_CLR(it_cli->first, &current_Wsockets);
                    close(it_cli->first);
                    itClient temp = it_cli;
                    ++it_cli;
                    _clients.erase(temp);
                    continue;
                }
            }
            ++it_cli;
        }
        ///////////////////
    }
}


void WebServ::read_request(int fd_R)
{
    if ((_nbytes = recv(fd_R, _buf, 25000, 0)) <= 0)
    {
        if (_nbytes == 0)
        {
            std::cout << std::strerror(errno) << std::endl;
        }
        else
            perror("read");
        close(fd_R);
        FD_CLR(fd_R, &current_Rsockets);
    }
    if (_nbytes < static_cast<int>(sizeof(_buf)))
        _buf[_nbytes] = '\0';
    _buffer.append(_buf, _nbytes);
}

void WebServ::start_parsing(int fd_R, std::map<int, std::vector<Servers> > grouped)
{
    if (_clients.at(fd_R)->getCheck() == false &&_buffer.find("\r\n\r\n") != std::string::npos)
    {
        int findPos = _buffer.find("\r\n");
        _clients.at(fd_R)->setCheck();

        _firstline = _buffer.substr(0, findPos);
        // std::cout << _firstline << std::endl;
        _clients.at(fd_R)->_request.parse_request_line(_firstline);
        std::cout << "first line is ------> " << _firstline << std::endl;
        _buffer = _buffer.substr(findPos + 2);
        if (_clients.at(fd_R)->_request.getMethod() != "POST")
        {
            // std::cout << "start << \n" << _buffer  << "\n<< end"<< std::endl;
            _clients.at(fd_R)->_request.setHeader(_buffer);
            _clients[fd_R]->_request.isReqWellFormed(_clients[fd_R]->getResponse());
            // std::cout << _clients[fd_R]->_request.getHeader("Host")->second << std::endl;
            // Servers server = getConf(server s_fds[idx], grouped);
                // _clients[new_socket]->setConf(server);
            Servers server = getConf(fd_R, grouped,  _clients[fd_R]->_request.getHeader("Host")->second);
            _clients[fd_R]->setConf(server);
            // exit(0);
            FD_CLR(fd_R, &current_Rsockets);
            FD_SET(fd_R, &current_Wsockets);
            _buffer.clear();
        }
        else
        {
            findPos = _buffer.find("\r\n\r\n");
            _clients.at(fd_R)->_request.setHeader(_buffer.substr(0, findPos + 2));
            _clients[fd_R]->_request.isReqWellFormed(_clients[fd_R]->getResponse());
            Servers server = getConf(fd_R, grouped,  _clients[fd_R]->_request.getHeader("Host")->second);
            _clients[fd_R]->setConf(server);
            _buffer = _buffer.substr(findPos + 4);
        }
    }

}

void WebServ::SetListeners()
{
    FD_ZERO(&current_Rsockets);
    FD_ZERO(&current_Wsockets);
    for (unsigned int i = 0; i < _servers.size(); i++)
    {
        for (unsigned int j = 0; j < _servers[i]->getSocket().size(); j++)
        {
            servers_fds.push_back(_servers[i]->getSocket()[j]);
            FD_SET(_servers[i]->getSocket()[j], &current_Rsockets);
            if (_servers[i]->getSocket()[j] > this->max_fd)
                max_fd = _servers[i]->getSocket()[j];
        }
    }
}

int WebServ::set_non_blocking(int sock) 
{
    if (fcntl(sock, F_SETFL, O_NONBLOCK) == -1) 
    {
        perror("fcntl F_SETFL");
        return(-1);
    }
    return (0);
}


RequestHandler* WebServ::createHandler(Request &request) 
{
    if (isPHPCGIRequest(request.getURL()))
    {
        return new PhpCgiHandler();
    }
    else
    {
        return new StaticFileHandler();
    }
}

bool WebServ::isPHPCGIRequest(const std::string url) 
{
    if (url.find(".php") != std::string::npos) // this check is not 100% done!!!!
    {
        return true;
    }
    return false;
}

Servers WebServ::getConf(int fd, std::map<int, std::vector<Servers> > confs, std::string host)
{
    (void)fd;
    int twoPoint = host.find(":");
    std::string ip = host.substr(0, twoPoint);
    int port = stoi(host.substr(twoPoint + 1));
    // std::cout << port << " " << ip << std::endl;
    // std::cout << fd << std::endl;
    
    std::vector<Servers> servers = confs[port];
    for (size_t i = 0; i < servers.size(); i++) {
        if (servers[i].get_host() == ip) return servers[i];
        // std::cout << servers[i].get_host() << std::endl;
    }
    return servers[0];
}
