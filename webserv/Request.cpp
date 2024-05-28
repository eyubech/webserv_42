#include "Request.hpp"
#include <cstdlib>
#include <string>
// #include "../includes/utils.hpp"

Request::Request()
{
}

Request::~Request(){};

void Request::parse_request_line(std::string &_rawReq)
{
    _lineRequest = _rawReq;
    std::string oneline;
    std::istringstream iss(_rawReq);
    std::getline(iss, oneline);
    std::string dele = " ";
    std::vector<std::string> tokens = Utils::split(oneline, dele);
    _method = tokens[0];
    _URL = tokens[1];
    std::cout << tokens.size() << std::endl;
    _http_version = tokens[2];
}


void Request::parse_headers_body(std::string _rawReq)
{
    std::istringstream iss(_rawReq);
    std::string line;
    while (std::getline(iss, line)) /////?
    {
        if (line == "\r")
            break;
        std::string::size_type pos = line.find(':');
        if (pos != std::string::npos)
        {
           _headers.insert(std::pair<std::string, std::string>(line.substr(0, pos - 1),
                line.substr(pos + 1, line.length())));
        }
    }
    while (std::getline(iss, line))
    {
       if (line != "\r")
       {
           _body.append(line);
           _body.append("\n");
       }
    }
}

void Request::setHeader(std::string buffer)
{
    _header = buffer;
    setHeaders();
    
}

std::string Request::getHeader() const
{
    return _header;
}

std::string Request::getURL() const
{
    return _URL;
}

void Request::setHeaders()
{
    size_t pos = 0;
    size_t separator_pos = 0;
    std::string line;
    while ((pos = _header.find("\r\n")) != std::string::npos) {

        line = _header.substr(0, pos);
        separator_pos = line.find(": ");
        if (separator_pos != std::string::npos)
            _headers[line.substr(0, separator_pos)] = line.substr(separator_pos + 2);
        _header.erase(0, pos + 2);
    }

    /*************** PRINT ***************/
        printHeaders();
        
}

itHeaders Request::getHeader(const char* key) const
{
    return _headers.find(key);
}

itHeaders Request::getStartHeaders() const
{
    return _headers.begin();
}

itHeaders Request::getEndHeaders() const
{
    return _headers.end();
}

void Request::findTypeOfPostMethod()
{
    itHeaders it = _headers.find("Content-Type");
    if (it->second.find("boundary") != -1)
    {
        std::string type = it->second.substr(it->second.find("=") + 1);
        _headers["typeMethodPost"] = type;
        _headers["type"] = "form-data";
    }
    else if (it->second.find("application/x-www-form") != -1)
        _headers["type"] = "form";
    else 
        _headers["type"] = "none";
}

void Request::setBody(std::string &buffer)
{
    _body = buffer;
}

std::string Request::getBody() const
{
    return _body;
}

void Request::isReqWellFormed(Response &response)
{
    if (_method == "POST")
    {
        itHeaders it1 = _headers.find("Transfer-Encoding");
        itHeaders it2 = _headers.find("Content-Length");
        if (it1 != _headers.end())
        {
            if (it1->second != "chunked")
            {
                response.setStatus(501);
                response.setStatusMsg("Not Implemented");
            }
        }
        if ( _method == "POST" && it1 == _headers.end() && it2 == _headers.end() )
        {
            response.setStatus(400);
            response.setStatusMsg("Bad Request");
            std::cout << response.getStatus() << std::endl;
        }
    }
    if (_URL.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=") != std::string::npos)
    {
        response.setStatus(400);
        response.setStatusMsg("Bad Request");
    }
    if (_URL.size() > 2048)
    {
        response.setStatus(414);
        response.setStatusMsg("Request-URI Too Large");
    }
    #define CLIENT_MAX_BODY 1000000 /*get client max body size in config file*/ 
    if(getBody().size() > CLIENT_MAX_BODY)
    {
        response.setStatus(413);
        response.setStatusMsg("Request Entity Too Large");
    }
}

/************************ Transfer-Encoding ***********************/

void Request::parceBodyChunked()
{
    int pos = 0;
    int hexaToDecimal = 0;
    int findCarriageReturn = 0;
    for (;true;)
    {
        findCarriageReturn = _body.find("\r\n", pos);
        hexaToDecimal = stoi(_body.substr(pos, findCarriageReturn - pos), 0, 16);
        _body.erase(pos, (findCarriageReturn - pos) + 2);
        pos += hexaToDecimal + 2;
        if (hexaToDecimal == 0)
            break;
    }
    if (_headers.find("type")->second == "")
    std::cout << _body << std::endl;
}

/************************ Transfer-Encoding ***********************/

// /************************ Transfer-Encoding ***********************/

//     void Request::parceBodyChunked()
//     {
//         std::string newBody;
//         std::string hexaStr = _body.substr(0, _body.find("\r\n"));
//         int hexa;
//         hexa = stoi(_body.substr(0, _body.find("\r\n")),nullptr,16);
//         std::cout << _body.substr(5, hexa) << std::endl;
//         newBody += _body.substr(5, hexa);
//         _body = _body.substr(5);
//         // for (; hexa == 0;)
//         // {
//         //     hexa = 
//         // }
            
//         // // }
//         // std::cout << "start" << std::endl;
//         // std::vector<std::string> result= Utils::split(_body, "\r\n\r\n");
//         // for (int i = 0; i < result.size(); i++)
//         //     std::cout << result[i] << " 1x1 "<< std::endl;
//         // std::cout << "end" << std::endl;
        
//     }

// /************************ Transfer-Encoding ***********************/