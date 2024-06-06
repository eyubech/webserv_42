#include "StaticFileHandler.hpp"
#include "Client.hpp"

StaticFileHandler::StaticFileHandler()
{}

StaticFileHandler::~StaticFileHandler()
{}

void StaticFileHandler::handleRequest(Client* cli)
{
    try 
    {
        req_uri_location(cli);
        is_location_have_redirection(cli);
        is_method_allowed_in_location(cli);
        check_requested_method(cli);
    } 
    catch (...)
    {
    }
    setStatusMessage(cli);
}