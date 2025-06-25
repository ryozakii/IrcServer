/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: osabir <osabir@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 09:28:40 by osabir            #+#    #+#             */
/*   Updated: 2024/10/07 18:08:06 by osabir           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

void Server::sendJoinResponse(int index, const std::string& channelName, const std::string& listClient)
{
    std::string lowChan = lowerCASE(channelName);
    reply(_client[index].fd, JOIN_CHANNEL(_client[index].nickname, _client[index].username, channels[lowChan].real_name, _client[index].ip));
    if (channels[lowChan].topicSet)
    {
        std::stringstream x;
        x << channels[lowChan].topicCreationTime;
        reply(_client[index].fd, RPL_TOPIC(_client[index].nickname, channels[lowChan].real_name, channels[channelName].channelTopic));
        reply(_client[index].fd, RPL_TOPICWHOTIME(_client[index].nickname, channels[lowChan].topicSetter, channels[lowChan].topicSetter_username, channels[channelName].real_name, x.str(), _client[index].ip));
    }
    reply(_client[index].fd, RPL_CHANNELMODEIS(_client[index].nickname, channels[lowChan].real_name, channels[channelName].mode));
    reply(_client[index].fd, RPL_NAMREPLY(_client[index].nickname, channels[lowChan].real_name, listClient));
    reply(_client[index].fd, RPL_ENDOFNAMES(_client[index].nickname, channels[lowChan].real_name));
}


void Server::to_add_channel(const std::string& namechannel, int index)
{
    channel newChannel;
    std::string lowChan = lowerCASE(namechannel);
    newChannel.real_name = namechannel;
    newChannel.channelname = lowChan;
    newChannel.invite = 0;
    newChannel.limit = 0;
    newChannel.count = 1;
    newChannel.key = 0;
    newChannel.topic = 1;
    newChannel.limitclient = 0;
    newChannel.channelTopic = "";
    newChannel.topicSet = 0;
    newChannel.mode = "+t";
    newChannel.creationtime = time(0);
    newChannel.clientMap[_client[index].nickname].first = &_client[index];
    newChannel.clientMap[_client[index].nickname].second = 1;
    
    channels[lowChan] = newChannel;
    _client[index]._channel.push_back(lowChan);
    
    std::string listClient = "@" + _client[index].nickname;
    sendJoinResponse(index, namechannel, listClient);
}

std::string Server::buildClientList(const std::string& channelName, int index)
{
    std::string listClient;
    std::string lowChan = lowerCASE(channelName);
    (void)index;
    std::map<std::string, std::pair<client*, bool> >::iterator it;
    for (it = channels[lowChan].clientMap.begin(); it != channels[lowChan].clientMap.end(); it++)
    {
        client* client = it->second.first;
        if (_client[index].nickname != client->nickname)
            reply(client->fd, JOIN_CHANNEL(_client[index].nickname, _client[index].username, channels[lowChan].real_name, _client[index].ip));
        if (channels[lowChan].clientMap[client->nickname].second)
            listClient += " @" + client->nickname;
        else
            listClient += " " + client->nickname;
    }

    return !listClient.empty() ? listClient.substr(1) : listClient;
}

void Server::joinResponse(const std::string& namechannel, int index)
{
    std::string lowChan = lowerCASE(namechannel);
    channels[lowChan].clientMap[_client[index].nickname].first = &_client[index];
    channels[lowChan].clientMap[_client[index].nickname].second = 0;
    _client[index]._channel.push_back(channels[lowChan].channelname);
    channels[lowChan].count++;

    std::string listClient = buildClientList(namechannel, index);
    sendJoinResponse(index, namechannel, listClient);
}

void Server::prototypeChanel(int index, std::string chennalname, std::string chennalKey)
{
    std::string lowChan = lowerCASE(chennalname);
    if (chennalname.find("#") != 0)
        reply(_client[index].fd, ERR_NOSUCHCHANNEL(_client[index].nickname, chennalname));
    else if (chennalname.find(7) != std::string::npos || chennalname.find(32) != std::string::npos || chennalname.find(44) != std::string::npos)
        reply(_client[index].fd, ERR_BADCHANNAME(_client[index].nickname, chennalname));
        
    else if (chennalname.find("#") == 0 && channels.find(lowChan) == channels.end())
        to_add_channel(chennalname, index);
    else if (channels[lowChan].clientMap.find(_client[index].nickname) != channels[lowChan].clientMap.end())
        return ;
    else if (channels[lowChan].invite && !check_invit(_client[index].fd, lowChan))
        reply(_client[index].fd, ERR_INVITEONLYCHAN(_client[index].nickname, chennalname));
    else if (channels[lowChan].key && channels[lowChan].channelkey != chennalKey)
        reply(_client[index].fd, ERR_BADCHANNELKEY(_client[index].nickname, chennalname));
    else if (channels[lowChan].limit && channels[lowChan].count >= channels[lowChan].limitclient)
        reply(_client[index].fd, ERR_CHANNELISFULL(_client[index].nickname, chennalname));
    else
    {
        std::vector<int>::iterator get = std::find(channels[lowChan].ClientInvit.begin(), channels[lowChan].ClientInvit.end(), _client[index].fd);
        if (get != channels[lowChan].ClientInvit.end())
            channels[lowChan].ClientInvit.erase(get);
        joinResponse(chennalname, index);
    }
}

void Server::join(int& index,std::string &buff)
{
    std::vector<std::string>	command;
    
    if (!(_client[index].registered))
        return;

    command = command_split(buff);
    if (command.size() < 1)
    {
        reply(_client[index].fd, ERR_NEEDMOREPARAMS(_client[index].nickname, "JOIN"));
        return ;
    }
    if (buff[0] == ':')
    {
        if (buff.length() == 2 && buff[1] == '0')
        {
            for (size_t i = 0; i < _client[index]._channel.size(); i++)
                prototypePart(_client[index]._channel[i], index, "");
            return ;
        }
        else if (buff[0] == ':' && buff.length() > 1)
        {
            prototypeChanel(index, buff.c_str() + 1, "");
            return ;
        }
        
    }
    if (buff[0] == '0' && buff.length() == 1)
    {
        while (_client[index]._channel.size())
            prototypePart(_client[index]._channel[0], index, "");
        return ;
    }
    std::vector<std::string> channels = split(command[0], ',');
    std::vector<std::string> keys;

    if (command.size() >= 2)
        keys = split(command[1], ',');

    for (size_t j = 0; j < channels.size(); ++j)
    {
        std::string key = j < keys.size() ? keys[j] : "";
        prototypeChanel(index, channels[j], key);
    }
}

