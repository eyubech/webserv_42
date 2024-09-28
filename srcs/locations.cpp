/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   locations.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maddou <maddou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/19 11:22:03 by ayyouub.py        #+#    #+#             */
/*   Updated: 2024/09/27 16:05:15 by maddou           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/locations.hpp"
#include <vector>

Locations::Locations() : _path(""), _return(""), default_file(""), directory(""), cgi_bin(""), cgi_extension(""), cgi_support(false), directory_listing(false), root("") {}

void Locations::setPath(std::string path) {
    _path = path;
}

void Locations::setReturn(std::string ret) {
    _return = ret;
}

void Locations::setDefaultFile(std::string file) {
    default_file = file;
}

void Locations::setDirectory(std::string dir) {
    directory = dir;
}

void Locations::setCgiBin(std::string bin) {
    cgi_bin = bin;
}

void Locations::setCgiExtension(std::string ext) {
    cgi_extension = ext;
}

void Locations::setCgiSupport(bool support) {
    cgi_support = support;
}

void Locations::setMethods(std::vector<std::string> methods) {
    _methods = methods;
}

void Locations::setDirectoryListing(bool listing) {
    directory_listing = listing;
}

void Locations::setIndexFiles(std::vector<std::string> indexFiles) {
    _indexFiles = indexFiles;
}


std::string Locations::getRoot() {
    return root;
}

std::vector<std::string> Locations::getIndexFiles() {
    return _indexFiles;
}

bool Locations::getDirectoryListing() {
    return directory_listing;
}

std::vector<std::string> Locations::getMethods() {
    return _methods;
}

std::string Locations::getPath() {
    return _path;
}

std::string Locations::getReturn() {
    return _return;
}

std::string Locations::getDefaultFile() {
    return default_file;
}

std::string Locations::getDirectory() {
    return directory;
}

std::string Locations::getCgiBin() {
    return cgi_bin;
}

std::string Locations::getCgiExtension() {
    return cgi_extension;
}

bool Locations::getCgiSupport() {
    return cgi_support;
}

void Locations::setRoot(std::string root) {
    this->root = root;
}
