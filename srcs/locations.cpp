/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   locations.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-gand <mel-gand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/19 11:22:03 by ayyouub.py        #+#    #+#             */
/*   Updated: 2024/06/01 06:47:56 by mel-gand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/locations.hpp"
#include <vector>

Locations::Locations() 
{
    // True -> on
    directory_listing = true;
    cgi_bin = "/usr/bin/bash";
    cgi_extension = ".sh";
}


Locations::~Locations()
{
}


std::string Locations::getPath() const
{
    return (_path);
}

std::string Locations::getReturn() const
{
    return (_return);
}


std::vector<std::string> Locations::getAcceptedMethod() const
{
    return(methods);
}