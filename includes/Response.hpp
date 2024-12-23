#pragma once

#include "includes_util.hpp"

class Client;
typedef std::map<std::string, std::string>::iterator it_Header;
class Response {
public:
    Response();
    ~Response();
    template<typename T>
    void setHeader(const std::string& key,  const T& value)
    {
        std::ostringstream oss;
            oss << value;
        _headers[key] = oss.str();
    }
    void setStatus(const int &status);
    void setStatusMsg(const std::string &Msg);
    void setBody(const std::string& body);
    void setAppendBody(const char *buffer);
    int getStatus() const;
    std::string getStatusMsg() const;
    std::string getBody() const;
    it_Header getHeader(const std::string& key);
    void generateErrorPage(std::string root, std::string error_page);
    void generateHeaderResponse();
    std::string generateHTML_file(std::string print, bool type, int status);
    void generate_ErrorPages(Client *cli);
    void Send(int cli_fd);

    /**************** Response *********** */
    std::string getResponse() const 
    {
        return _Response;
    }
    /**************** Response *********** */

private:
    
    int _status;
    std::string _Msg;
    std::string _Response;
    std::map<std::string, std::string> _headers;
    std::string _body;
};
