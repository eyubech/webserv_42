/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-gand <mel-gand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 15:28:06 by aech-che          #+#    #+#             */
/*   Updated: 2024/10/06 16:08:18 by mel-gand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/includes.hpp"
#include "../includes/utils.hpp"
#include "../includes/errors.hpp"
#include <vector>

Parsing::Parsing() {
}


Parsing::~Parsing(){
}


std::map<int, std::vector<Servers> >  Parsing::parse_file(char *filename, std::vector<std::string> &data, std::vector<Servers> &serversvec)
{
    (void)data;
    std::ifstream infile_count(filename);
    if (!infile_count.is_open()) {
        throw("Error: Could not open file for counting servers!");
    }

    Utils::count_servers(infile_count);
    infile_count.close();

    __NO_SERVERS__ = __NUMBER_OF_SERVERS__;
    std::cout << "\n[INFO] : We got " << __NUMBER_OF_SERVERS__ << " servers" << std::endl;

    if(!__NO_SERVERS__) {
        throw("No server detected!");
        
    }

    std::ifstream infile(filename);
    if (!infile.is_open()) {
        throw("Error: Could not open file for parsing!");
        
    }

    std::string buff;
    int switch_server = 0;
    int line = 0;
    std::string save_arg = "";
    std::vector<Locations> locationsvec;
    Servers server;
    while (true) {
        
        if(save_arg == "")
        {
            if(!std::getline(infile, buff))
                break;
        }
        else
        {
            buff = save_arg;
            save_arg = "";
        }
        buff = Utils::strtrim(buff);
        if (buff == "router:" || buff == "location:") {
            __IN_ROUTER__ = 1;
        } else if (buff == "server:") {
            __IN_ROUTER__ = 0;
            switch_server += 1;
        }

        if(switch_server % 2 == 0) {
            for(size_t i = 0; i < locationsvec.size(); i += 1){
                if(locationsvec[i].getPath() == "/"){
                    server.set_rootlocation(true);
                }
            }

            if(!server.get_rootlocation()){
                    Locations location;
                    location.setPath("/");
                    locationsvec.push_back(location);
                    reverse(locationsvec.begin(), locationsvec.end());
                
            }
            server.set_locations(locationsvec);
            if(!Errors::valid_server_data(server)){
                infile.close();
            }
            serversvec.push_back(server);
            server = Servers();
            switch_server += 1;
            locationsvec.clear();
        }
        else {
            std::string arg;
            std::vector<std::string> data_splited = Utils::split(buff, " ");
            
            arg = data_splited[0];
            if (arg == "host:") {
                if (data_splited.size() == 2) {
                    server.set_host(data_splited[1]);
                }
                else {
                    infile.close();
                    throw( " [ERROR] : Error in host");
                    
                }
                
            }
            else if (arg == "port:") {
                std::vector<std::string> data_splited_2 = Utils::split(Utils::split(buff, ":")[1], ", ");
                std::vector<int> ports;
                for (std::vector<std::string>::iterator it = data_splited_2.begin(); it != data_splited_2.end(); ++it) {
                    if (!Errors::valid_port(*it)) {
                        infile.close();
                        throw( " [ERROR] : Error in port");
                    }
                        ports.push_back(std::atoi(it->c_str()));
                }
                server.set_ports(ports);
            }
            else if (arg == "client_body_size:") {
                if (data_splited.size() != 2) {
                    infile.close();
                    throw( "[ERROR] : Error in client_body_size, too many arguments");
                }
                    server.set_client_body_size(std::atoi(Utils::strtrim(data_splited[1]).c_str()));
            }
            else if (arg == "server_names:") {
                std::vector<std::string> server_names = Utils::split(Utils::split(buff, ":")[1], ", ");
                for (std::vector<std::string>::iterator it = server_names.begin(); it != server_names.end(); ++it) {
                    if (!Errors::valid_servername(Utils::strtrim(*it))) {
                        infile.close();
                        throw( "[ERROR] : Error in server names");
                    }
                }
                server.set_server_names(server_names);
            }
            else if (arg == "root:") {
                std::string root = Utils::strtrim(Utils::split(buff, ": ")[1]);
                if (!Errors::valid_root(Utils::strtrim(root))) {
                infile.close();
                    throw( "[ERROR] : Error in root");
                }
                root.erase(0, 1);
                root.erase(root.size() - 1, 1);
                server.set_root(root);
            }
            else if (arg == "default_server:") {
                if (Utils::strtrim(data_splited[1]) == "true")
                    server.set_default_server(true);
                else
                    server.set_default_server(false);
            }
            else if(arg == "index:")
            {
                std::vector<std::string> index_files = Utils::split(Utils::split(buff, ":")[1], ", ");
                for (std::vector<std::string>::iterator it = index_files.begin(); it != index_files.end(); ++it) {
                    *it = Utils::strtrim(*it);
                    if (!Errors::valid_servername(*it)) {
                      infile.close();
                        throw( "[ERROR] : Error in index files");
                    }
                }
                
                server.set_indexFiles(index_files);
            }
            else if (arg == "error_pages") {
                std::map<std::string, std::string> error_pages;
                while (std::getline(infile, buff)) {
                    line += 1;
                    buff = Utils::strtrim(buff);
                    if(buff == "{")
                        continue;
                    if (buff == "}")
                    {
                        server.set_error_pages(error_pages);
                        save_arg = buff;
                        break;
                    }
                    std::vector<std::string> error_data = Utils::split(buff, " ");
                    if (error_data.size() != 2) {
                        infile.close();
                        throw( "[ERROR] : Error in error pages, check arguments");
                    }
                    if (!Errors::valid_errorpage(error_data[0])) {
                       infile.close();
                        throw( "[ERROR] : Error in error pages");
                    }
                    error_data[1].erase(0, 1);
                    error_data[1].erase(error_data[1].size() - 1, 1);
                    error_data[0] = error_data[0].substr(0, error_data[0].size() - 1);
                    std::cout << error_data[0] << " : " << error_data[1] << std::endl;
                    error_pages[error_data[0]] = error_data[1];
                }
                   
            }
            else if (arg == "router" || arg == "locations") {
                int return_flag = 0;
                int count = 0;
                Locations location;
                while (std::getline(infile, buff)) {
                    line += 1;
                    buff = Utils::strtrim(buff);
                    if (buff == "}")
                    {
                        locationsvec.push_back(location);
                        save_arg = buff;
                        break;
                    }
                    if(buff == "{")
                        continue;
                    std::vector<std::string> location_data = Utils::split(buff, ": ");
                    arg = Utils::strtrim(location_data[0]);
                    if(arg == "path") {
                        location_data = Utils::split(buff, " ");
                        if (location_data.size() != 2){
                            infile.close();
                            throw( "[ERROR] : Error in path, check arguments");

                        }
                        arg = Utils::strtrim(location_data[1]);
                        if(!Errors::valid_path(arg)) {
                            infile.close();
                            throw( "[ERROR] : Error in path");

                        }
                        arg.erase(0, 1);
                        arg.erase(arg.size() - 1, 1);
                        location.setPath(arg);
                    }
                    else if(arg == "return") {
                        if(return_flag) {
                            infile.close();
                            throw( "[ERROR] : Error in return, already set");

                        }
                        if(count > 0) {
                            infile.close();
                            throw( "[ERROR] : Error in return, only return in location");

                        }
                        return_flag += 1;
                        location_data = Utils::split(buff, " ");
                        if (location_data.size() != 2){
                            infile.close();
                            throw( "[ERROR] : Error in return, check arguments");

                        }
                        arg = Utils::strtrim(location_data[1]);
                        if(!Errors::valid_return(arg)) {
                            infile.close();
                            throw( "[ERROR] : Error in return");

                        }
                        arg.erase(0, 1);
                        arg.erase(arg.size() - 1, 1);
                        location.setReturn(arg);
                    }
                    else if(arg == "methods"){
                        count += 1;
                        if(return_flag > 0) {
                            infile.close();
                            throw( "[ERROR] : Error in return, only return in location");

                        }
                        if (location_data.size() != 2){
                            infile.close();
                            throw( "[ERROR] : Error in methods, check arguments");

                        }
                        std::vector<std::string> methods = Utils::split(location_data[1], ", ");
                        std::vector<std::string> methods_data;
                        for (std::vector<std::string>::iterator it = methods.begin(); it != methods.end(); ++it) {
                            if (!Errors::valid_method(Utils::strtrim(*it))) {
                                infile.close();
                                throw( "[ERROR] : Error in methods");
    
                            }
                            methods_data.push_back(Utils::strtrim(*it));
                        }
                        location.setMethods(methods_data);
                    }
                    else if(arg == "index"){
                        count += 1;
                        if(return_flag > 0) {
                            infile.close();
                            throw( "[ERROR] : Error in return, only return in location");

                        }
                        if (location_data.size() != 2){
                            infile.close();
                            throw( "[ERROR] : Error in index files, check arguments");

                        }
                        std::vector<std::string> indexfiles = Utils::split(location_data[1], ", ");
                        std::vector<std::string> indexfiles_data;
                        for (std::vector<std::string>::iterator it = indexfiles.begin(); it != indexfiles.end(); ++it) {
                            if (!Errors::valid_indexfiles(Utils::strtrim(*it))) {
                                infile.close();
                                throw( "[ERROR] : Error in indexfiles");
    
                            }
                            indexfiles_data.push_back(Utils::strtrim(*it));
                        }
                        location.setIndexFiles(indexfiles_data);
                    }
                    else if(arg == "directory_listing") {
                        count += 1;
                        if(return_flag > 0) {
                            infile.close();
                            throw( "[ERROR] : Error in return, only return in location");

                        }
                        location_data = Utils::split(buff, " ");
                        if (location_data.size() != 2){
                            infile.close();
                            throw( "[ERROR] : Error in directory listing, check arguments");

                        }
                        arg = Utils::strtrim(location_data[1]);
                        if(!Errors::valid_directory_listing(arg)) {
                            infile.close();
                            throw( "[ERROR] : Error in directory");

                        }
                        if (arg == "on")
                            location.setDirectoryListing(true);
                        else
                            location.setDirectoryListing(false);
                    }
                    else if(arg == "cgi_support") {
                        count += 1;
                        if(return_flag > 0) {
                            infile.close();
                            throw( "[ERROR] : Error in return, only return in location");

                        }
                        location_data = Utils::split(buff, " ");
                        if (location_data.size() != 2){
                            infile.close();
                            throw( "[ERROR] : Error in cgi support, check arguments");

                        }
                        arg = Utils::strtrim(location_data[1]);
                        if(!Errors::valid_cgi_support(arg)) {
                            infile.close();
                            throw( "[ERROR] : Error in cgi support");

                        }
                        if (arg == "on")
                            location.setCgiSupport(true);
                        else
                            location.setCgiSupport(false);
                    }
                    else if(arg == "cgi_bin") {
                        count += 1;
                        if(return_flag > 0) {
                            infile.close();
                            throw( "[ERROR] : Error in return, only return in location");

                        }
                        location_data = Utils::split(buff, " ");
                        if (location_data.size() != 2){
                            infile.close();
                            throw( "[ERROR] : Error in cgi bin, check arguments");

                        }
                        arg = Utils::strtrim(location_data[1]);
                        if(!Errors::valid_cgi_bin(arg)) {
                            infile.close();
                            throw( "[ERROR] : Error in cgi bin");

                        }
                        location.setCgiBin(arg);
                    }
                    else if(arg == "upload_dir") {
                        count += 1;
                        if(return_flag > 0) {
                            infile.close();
                            throw( "[ERROR] : Error in return, only return in location");

                        }
                        location_data = Utils::split(buff, " ");
                        if (location_data.size() != 2){
                            infile.close();
                            throw( "[ERROR] : Error in upload dir, check arguments");

                        }
                        arg = Utils::strtrim(location_data[1]);
                        if(!Errors::valid_upload_dir(arg)) {
                            infile.close();
                            throw( "[ERROR] : Error in upload dir");

                        }
                        location.setUploadDir(arg);

                        

                    }
                    else if(arg == "root") {
                        count += 1;
                        if(return_flag > 0) {
                            infile.close();
                            throw( "[ERROR] : Error in return, only return in location");

                        }
                        location_data = Utils::split(buff, " ");
                        if (location_data.size() != 2){
                            infile.close();
                            throw( "[ERROR] : Error in root location, check arguments");

                        }
                        arg = Utils::strtrim(location_data[1]);
                        if(!Errors::valid_root(arg)) {
                            infile.close();
                            throw( "[ERROR] : Error in root location");

                        }
                        arg.erase(0, 1);
                        arg.erase(arg.size() - 1, 1);
                        location.setRoot(arg);
                    }
                    else if(arg == "cgi_extension") {
                        count += 1;
                        if(return_flag > 0) {
                            infile.close();
                            throw( "[ERROR] : Error in return, only return in location");

                        }
                        location_data = Utils::split(buff, " ");
                        if (location_data.size() != 2){
                            infile.close();
                            throw( "[ERROR] : Error in cgi extension, check arguments");

                        }
                        arg = Utils::strtrim(location_data[1]);
                        if(!Errors::valid_cgi_extension(arg)) {
                            infile.close();
                            throw( "[ERROR] : Error in cgi extension");

                        }
                        location.setCgiExtension(arg);
                    }
                    
                    else{
                        if(arg == "{" || arg == "}")
                            continue;
                        Errors::you_mean(arg, 1);
                    }
                    
                }
            }
            else{
                if(arg == "server:" || arg == "{" || arg == "}")
                    continue;
                Errors::you_mean (Utils::strtrim(arg), 0);
            }
        }
        line += 1;
    }
    
    // modify location vec and add root location
    for(size_t i = 0; i < locationsvec.size(); i += 1){
        if(locationsvec[i].getPath() == "/"){
            server.set_rootlocation(true);
        }
    }

    if(!server.get_rootlocation()){
            Locations location;
            location.setPath("/");
            std::vector<std::string> methodsvec;
            methodsvec.push_back("GET");
            methodsvec.push_back("POST");
            methodsvec.push_back("DELETE");
            location.setMethods(methodsvec);
            locationsvec.push_back(location);
            reverse(locationsvec.begin(), locationsvec.end());
        
    }
    // end
    server.set_locations(locationsvec);
    if(!Errors::valid_server_data(server)){
        infile.close();
    }
    serversvec.push_back(server);
    std::map<int, std::vector<Servers> > groupedServer;
    for (size_t i = 0; i < serversvec.size(); i++) {
        std::vector<int> ports = serversvec[i].get_ports();
        for (size_t j = 0; j < ports.size(); j++) {
            groupedServer[ports[j]].push_back(serversvec[i]);
        }
    }

    infile.close();
    return groupedServer;
}

std::vector<std::vector<Servers> > parse_server(std::vector<Servers> serversvec)
{
    std::vector<std::vector<Servers> > result;
    std::vector<bool> visited(serversvec.size(), false);

    for (std::size_t i = 0; i < serversvec.size(); ++i)
    {
        if (visited[i])
            continue;

        std::vector<Servers> group;
        group.push_back(serversvec[i]);
        visited[i] = true;

        for (std::size_t j = i + 1; j < serversvec.size(); ++j)
        {
            if (visited[j])
                continue;

            if (serversvec[i].get_host() == serversvec[j].get_host())
            {
                group.push_back(serversvec[j]);
                visited[j] = true;
            }
        }
        result.push_back(group);
    }

    return result;
}