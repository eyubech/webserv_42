/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   servers.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-gand <mel-gand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/19 11:08:28 by ayyouub.py        #+#    #+#             */
/*   Updated: 2024/09/30 05:40:16 by mel-gand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERS_HPP
# define SERVERS_HPP


#include "includes_util.hpp"
#include "locations.hpp"


class Locations;
class Servers
{
       private:
                std::string host;
                std::vector<int> ports;
                size_t client_body_size;
                std::vector<std::string> server_names;
                std::vector<std::string> roots;
                bool default_server;
                std::vector<std::map<std::string, std::string> > error_pages;
                std::vector<std::string> indexFiles;
                // routers or locations
                std::vector<Locations> locations;
                std::vector<std::string> _indexFiles;
                std::vector<Servers> servers;

        public:
            Servers();
            ~Servers();
            std::vector<Servers> get_servers();
            void set_servers(Servers server);
            void set_host(std::string host);
            void set_ports(std::vector<int> ports);
            void set_client_body_size(size_t client_body_size);
            void set_server_names(std::vector<std::string> server_names);
            void set_roots(std::vector<std::string> roots);
            void set_default_server(bool default_server);
            void set_error_pages(std::vector<std::map<std::string, std::string> > error_pages);
            void set_locations(std::vector<Locations> locations);
            void set_indexFiles(std::vector<std::string> indexFiles);
            std::string get_host();
            std::vector<int> get_ports();
            size_t get_client_body_size();
            std::vector<std::string> get_server_names();
            std::vector<std::string> get_roots();
            bool get_default_server();
            std::vector<std::map<std::string, std::string> > get_error_pages();
            std::vector<Locations> &get_locations();
            std::vector<std::string> get_indexFiles();
        
            
            

};


#endif