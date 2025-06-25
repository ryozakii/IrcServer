/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: osabir <osabir@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 10:08:50 by osabir            #+#    #+#             */
/*   Updated: 2024/09/09 10:14:07 by osabir           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

std::string lowerCASE(const std::string& str)
{
    std::string result = str;    
    for (size_t i = 0; i < result.length(); ++i)
        result[i] = std::tolower(result[i]);    
    return result;
}

std::vector<std::string> split(const std::string &string, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream tokenStream(string);
    
    while (std::getline(tokenStream, token, delimiter))
        tokens.push_back(token);
    return tokens;
}

bool Server::check_invit(int client, std::string chaneel)
{
    std::vector<int>::iterator get = std::find(channels[chaneel].ClientInvit.begin(), channels[chaneel].ClientInvit.end(), client);
    if (get != channels[chaneel].ClientInvit.end())
        return true;
    return false;
}


