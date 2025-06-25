/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: osabir <osabir@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/07 12:50:58 by osabir            #+#    #+#             */
/*   Updated: 2024/10/07 17:17:52 by osabir           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"


std::vector<std::string> command_split(std::string &buff)
{
    std::stringstream			split(buff);
    std::string                 cmd;
    std::vector<std::string>	command;
    
    while (split >> cmd)
        command.push_back(cmd);
    return command;
}

void Server::prototypePart(std::string &chennalname, int &index, std::string reason)
{
    std::string lowChan = lowerCASE(chennalname);
    if (chennalname.find("#") != 0)
        reply(_clients[index].fd, ERR_NOSUCHCHANNEL(_client[index].nickname, chennalname));
    else if (channels.find(lowChan) == channels.end())
        reply(_clients[index].fd, ERR_NOSUCHCHANNEL(_client[index].nickname, chennalname));
    else if (channels[lowChan].clientMap.find(_client[index].nickname) == channels[lowChan].clientMap.end())
        reply(_clients[index].fd, ERR_NOTONCHANNEL(_client[index].nickname, chennalname));
    else
    {
        reply(_clients[index].fd, (reason.empty() ? PART_CHANNEL(_client[index].nickname, _client[index].username, channels[lowChan].real_name, _client[index].ip) \
                : PART_CHANNEL_REASON(_client[index].nickname, _client[index].username, channels[lowChan].real_name, _client[index].ip, reason)));
        std::map<std::string, std::pair<client*, bool> >::iterator iter;

        channels[lowChan].count--;
        channels[lowChan].clientMap.erase(_client[index].nickname);
        std::vector<std::string>::iterator get = std::find(_client[index]._channel.begin(), _client[index]._channel.end(), chennalname);
        if (get != _client[index]._channel.end())
            _client[index]._channel.erase(get);
        if (channels[lowChan].count == 0)
        {
            channels[lowChan].ClearChannel();
            channels.erase(lowChan);
            return ;
        }
        for (iter = channels[lowChan].clientMap.begin(); iter != channels[lowChan].clientMap.end(); iter++)
            reply(iter->second.first->fd, (reason.empty() ? PART_CHANNEL(_client[index].nickname, _client[index].username, channels[lowChan].real_name, _client[index].ip) \
                : PART_CHANNEL_REASON(_client[index].nickname, _client[index].username, channels[lowChan].real_name, _client[index].ip, reason)));
    }
}


void Server::part_pars(int &index,std::string &buff)
{
    if (!(_client[index].registered))
        return ;
        std::string buf = buff;
    std::vector<std::string>	command = command_split(buff);
    if (command.size() < 1 || (command.size() == 1 && command[0][0] == ':'))
    {
        reply(_clients[index].fd, ERR_NEEDMOREPARAMS(_client[index].nickname, "PART"));
        return ;
    }
    if (command.size() > 1)
    {
        while (buf.find(' ') != std::string::npos)
            buf = buf.substr(buff.find(' ') + 1);
        if (buf[0] == ':')
            buf = buf.c_str() + 1;
    }
    std::vector<std::string> channels = split(command[0], ',');
    for (size_t j = 0; j < channels.size(); ++j)
        prototypePart(channels[j], index, (command.size() > 1 ? buf : ""));
}
