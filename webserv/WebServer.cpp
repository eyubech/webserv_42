#include "WebServer.hpp"
#include <cstdlib>
#include <iostream>
#include <map>
#include <sys/fcntl.h>
#include <unistd.h>
#include <utility>
#include <vector>
#include "../includes/servers.hpp"

WebServ::WebServ(){
    this->max_fd = 0;
}

WebServ::~WebServ()
{
    for (int i = 0; i < _servers.size(); i++)
        delete _servers[i];
}

void WebServ::run_servers(std::vector<Servers> Confs)
{
    int pos;
    int find;
    for (int i = 0; i <  Confs.size(); i++)
        _servers.push_back(new TCPserver(Confs[i]));
    SetListeners();
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
        for (int idx = 0; idx < servers_fds.size(); idx++)
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
                _clients.insert(std::make_pair(new_socket, new Client(new_socket)));
                Servers server = getConf(servers_fds[idx], Confs);
                _clients[new_socket]->setConf(server);
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
                start_parsing(it_cli->first);
                if ( _clients[it_cli->first]->_request.getHeader("Transfer-Encoding") != _clients[it_cli->first]->_request.getEndHeaders())
                {
                    if (_buffer.find("0\r\n\r\n") != -1)
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
                    if (_buffer.size() == stoi(_clients[it_cli->first]->_request.getHeader("Content-Length")->second))
                    {
                        _clients[it_cli->first]->_request.findTypeOfPostMethod();
                        _clients[it_cli->first]->_request.setBody(_buffer);
                        _buffer.clear();
                        FD_CLR(it_cli->first, &current_Rsockets);
                        FD_SET(it_cli->first, &current_Wsockets);
                    }
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
    if (_nbytes < sizeof(_buf))
        _buf[_nbytes] = '\0';
    _buffer.append(_buf, _nbytes);
}

void WebServ::start_parsing(int fd_R)
{
    if (_clients.at(fd_R)->getCheck() == false &&_buffer.find("\r\n\r\n") != -1)
    {
        int findPos = _buffer.find("\r\n");
        _clients.at(fd_R)->setCheck();

        _firstline = _buffer.substr(0, findPos);
        _clients.at(fd_R)->_request.parse_request_line(_firstline);
        _buffer = _buffer.substr(findPos + 2);
        if (_clients.at(fd_R)->_request.getMethod() != "POST")
        {
            _clients.at(fd_R)->_request.setHeader(_buffer);
            _clients[fd_R]->_request.isReqWellFormed(_clients[fd_R]->getResponse());
            FD_CLR(fd_R, &current_Rsockets);
            FD_SET(fd_R, &current_Wsockets);
            _buffer.clear();
        }
        else
        {
            findPos = _buffer.find("\r\n\r\n");
            _clients.at(fd_R)->_request.setHeader(_buffer.substr(0, findPos + 2));
            _clients[fd_R]->_request.isReqWellFormed(_clients[fd_R]->getResponse());
            _buffer = _buffer.substr(findPos + 4);
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

Servers WebServ::getConf(int fd, std::vector<Servers> Confs)
{
    for (int i = 0; i < _servers.size(); i++)
    {
        for (int j = 0; j < _servers[i]->getSocket().size(); j++) 
        {
            if (_servers[i]->getSocket()[j] == fd)
            {
                return Confs[i];
            }
        }
    }
    return (Confs[0]);
}
