/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: osabir <osabir@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 14:24:17 by osabir            #+#    #+#             */
/*   Updated: 2024/10/04 10:44:52 by osabir           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

void Server::invit_pars(int &index,std::string &buff)
{
    std::vector<std::string>	commad;
    
    if (!(_client[index].registered))
        return ;
    commad = command_split(buff);
    if (commad.size() < 2)
    {
        reply(_client[index].fd, ERR_NEEDMOREPARAMS(_client[index].nickname, "INVITE"));
        return ;
    }
    std::string &target = commad[0];
    std::string &namechannel = commad[1];
    std::string lowChan = lowerCASE(namechannel);

    if (namechannel.find("#") != 0 || channels.find(lowChan) == channels.end())
        reply(_client[index].fd, ERR_NOSUCHCHANNEL(_client[index].nickname, "INVITE"));
    else if (channels[lowChan].clientMap.find(_client[index].nickname) == channels[lowChan].clientMap.end())
        reply(_client[index].fd, ERR_NOTONCHANNEL(_client[index].nickname, namechannel));
    else if (channels[lowChan].invite && !channels[lowChan].clientMap[_client[index].nickname].second)
        reply(_client[index].fd, ERR_CHANOPRIVSNEEDED(_client[index].nickname, namechannel));
    else if (_nicknames.find(target) == _nicknames.end()) 
        reply(_client[index].fd, ERR_NOSUCHNICK(_client[index].nickname, target));
    else if (channels[lowChan].clientMap.find(target) != channels[lowChan].clientMap.end())
        reply(_client[index].fd, ERR_USERONCHANNEL(_client[index].nickname, namechannel));
    else
    {
        if (channels[lowChan].invite)
            channels[lowChan].ClientInvit.push_back(_nicknames[target]->fd);
        reply(_client[index].fd, RPL_INVITING(_client[index].nickname, target, namechannel));
        reply(_nicknames[target]->fd, INVITE(_client[index].nickname, _client[index].username, target, channels[lowChan].real_name, _client[index].ip));
    }
}