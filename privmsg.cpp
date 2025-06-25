/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: osabir <osabir@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 03:21:10 by mlahlafi          #+#    #+#             */
/*   Updated: 2024/10/07 15:42:22 by osabir           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

std::vector<std::string> Server::get_tokens(std::string str)
{
	std::vector<std::string> tokens;

    std::string token;

    size_t first = str.find_first_not_of(" \t\n");
	if (first)
		str = str.substr(first);
	std::stringstream ss(str);
    std::getline(ss, token, ' ');
	tokens.push_back(token);
    str = str.substr(tokens[0].size());
    first = str.find_first_not_of(" \t\n");
    if (first != std::string::npos)
    {
		str = str.substr(first);
        if (str[0] != ':')
        {
            std::stringstream ss(str);
            std::getline(ss, token, ' ');
            str = token;
        }
        else
            str = str.substr(1);
        tokens.push_back(str);
    }
	return (tokens);
}

std::vector<std::string> Server::get_targets(std::string targets)
{
	std::vector<std::string> v;
	std::stringstream ss(targets);
	std::string token;
	while (std::getline(ss, token, ','))
	{
		v.push_back(token);
	}
	return (v);
}

void	Server::privmsg(int &c,std::string &str)
{
	if (!(_client[c].registered))
		return ;

	if (str.empty())
	{
            reply(_client[c].fd,ERR_NORECIPIENT(_client[c].nickname));
			return ;
	}

	std::map<std::string, channel >::iterator it;
    std::map<std::string, client *>::iterator itr;
	std::map<std::string, std::pair<client*, bool> >::iterator itt;
	std::vector<std::string> tokens = get_tokens(str);

	if (tokens.size() == 1)
		reply(_client[c].fd, ERR_NOTEXTTOSEND(_client[c].nickname));
	else
	{
		std::vector<std::string> targets = get_targets(tokens[0]);
		for(unsigned long i = 0; i < targets.size(); i++)
		{
			if (targets[i][0] == '#')
			{	
				if ((it = channels.find(lowerCASE(targets[i]))) == channels.end())
				{
					reply(_client[c].fd, ERR_NOSUCHNICK(_client[c].nickname,targets[i]));
				}
				else if (it->second.clientMap.find(_client[c].nickname) == it->second.clientMap.end())
					reply(_client[c].fd, ERR_CANNOTSENDTOCHAN(_client[c].nickname, targets[i]));
				else
				{
    				for (itt = it->second.clientMap.begin(); itt != it->second.clientMap.end(); itt++)
    				{
						if (itt->second.first->fd != _client[c].fd)
							reply(itt->second.first->fd, RPL_PRIVMSG(_client[c].nickname, _client[c].username, _client[c].ip, it->second.real_name, tokens[1]));
					}
				}
			}
			else if ((itr = _nicknames.find(targets[i])) == _nicknames.end())
			{
				reply(_client[c].fd, ERR_NOSUCHNICK(_client[c].nickname,targets[i]));
			}
			else
			{
				reply(itr->second->fd, RPL_PRIVMSG(_client[c].nickname, _client[c].username, _client[c].ip, targets[i], tokens[1]));
			}
		}
	}
}