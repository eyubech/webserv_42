#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <cstdlib>
#include "../includes/utils.hpp"
class HTTPRequest
{
    public :
        HTTPRequest();
        HTTPRequest(const std::string &raw_req);
        ~HTTPRequest();
        std::string getMethod() const
        {
            return _method;
        };
        void setMethod();
        std::string getURL() const;
        void setURL();
        std::string getBody() const;
        void setBody();
        void setHeader(std::string &buffer);
        std::string getHeader() const;
        // std::string getStatus() const;
        // std::string setStatus();
        std::map<std::string, std::string> getHeaders() const;
        void parse_request_line(std::string &_rawReq);
        void parse_headers_body(std::string _rawReq);
        // int check_req_format();
        // std::vector<std::string> split(const std::string& input, char delimiter);

    private:
        std::string _header;
        std::map<std::string, std::string> _headers;
        std::string _lineRequest;
        std::string _body;
        std::string _method;
        std::string _URL;
        std::string http_version;
        std::string _rawReq;
        // int _status;
        // std::string _statusMsg;

        
};