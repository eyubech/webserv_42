/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maddou <maddou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/09 14:39:01 by aech-che          #+#    #+#             */
/*   Updated: 2024/05/20 20:49:08 by maddou           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../includes/utils.hpp"


Utils::Utils() 
{
    
    


}


Utils::~Utils(){
}





// void Utils::count_servers(std::ifstream &infile)
// {
//     std::string buff;
//     while ( infile )
//     {
//         std::getline(infile, buff);
//         if (buff == "server:")
//             __NUMBER_OF_SERVERS__ += 1;
//     }
// }






std::vector<std::string> Utils::split(std::string s, std::string c)
{
    std::vector<std::string> vec;
    size_t pos = 0;
    for(;pos < s.size();)
    {
        pos = s.find(c);
        if ((int)pos == -1)
            break;
        vec.push_back(s.substr(0,pos));
        s.erase(0, pos + 1);
        pos = 0;
    }
    vec.push_back(s);
    return(vec);
}


