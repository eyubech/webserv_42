/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   locations.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayyouub.py <aech-che@127.0.0.1>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/19 11:21:18 by ayyouub.py        #+#    #+#             */
/*   Updated: 2024/10/05 21:23:45 by ayyouub.py       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONS_HPP
# define LOCATIONS_HPP

#include "includes_util.hpp"



class Locations
{
        private:
                std::string _path;
                std::string _return;
                std::string default_file;
                std::string directory;
                std::string cgi_bin;
                std::string cgi_extension;
                bool cgi_support;
                std::vector<std::string> _methods;
                bool directory_listing;
                std::vector<std::string> _indexFiles;
                std::string root;
                std::string return_;
                std::string upload_dir;


        public :      
                Locations();
                ~Locations();
                void setPath(std::string path);
                void setReturn(std::string ret);
                void setDefaultFile(std::string file);
                void setDirectory(std::string dir);
                void setCgiBin(std::string bin);
                void setCgiExtension(std::string ext);
                void setCgiSupport(bool support);
                void setMethods(std::vector<std::string> methods);
                void setDirectoryListing(bool listing);
                void setIndexFiles(std::vector<std::string> indexFiles);
                void setRoot(std::string root);
                std::string getRoot();
                std::vector<std::string> getIndexFiles();
                bool getDirectoryListing();
                std::vector<std::string> getMethods();
                std::string getPath();
                std::string getReturn();
                std::string getDefaultFile();
                std::string getDirectory();
                std::string getCgiBin();
                std::string getCgiExtension();
                bool getCgiSupport();
                std::string getUploadDir();
                void setUploadDir(std::string dir);


};



#endif