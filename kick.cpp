/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: osabir <osabir@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/17 13:30:05 by mlahlafi          #+#    #+#             */
/*   Updated: 2024/10/07 19:38:16 by osabir           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"


std::vector<std::string> get_tokensK(std::string str)
{
	std::vector<std::string> tokens;
    std::string token;

	size_t first = str.find_first_not_of(" \t\n");
    if (first != std::string::npos) {
        str = str.substr(first);
    }

    std::stringstream ss(str);
    std::getline(ss, token, ' ');
    tokens.push_back(token);

    str = str.substr(tokens[0].size());
    first = str.find_first_not_of(" \t\n");
    if (first != std::string::npos) {
        str = str.substr(first);
    }

    std::stringstream ll(str);
    std::getline(ll, token, ' ');
	if (token.empty() || (token.length() == 1 && token[0] == ':'))
		return tokens;
    tokens.push_back(token);

    str = str.substr(tokens[1].size());
    first = str.find_first_not_of(" \t\n");
    if (first != std::string::npos) {
        str = str.substr(first);

        if (str[0] != ':') {
            std::stringstream ss(str);
            std::getline(ss, token, ' ');
            tokens.push_back(token);
        } else {
            str = str.substr(1);
            tokens.push_back(str);
        }
    }

    return tokens;
}

void Server::kick(int &c, std::string &str) 
{
	if (!(_client[c].registered))
		return ;
	if (str.empty())
    {
        reply(_clients[c].fd, ERR_NEEDMOREPARAMS(_client[c].nickname, "KICK"));
        return ;
    }
	std::vector<std::string> tokens = get_tokensK(str);
	std::map<std::string, channel>::iterator it;
	std::map<std::string, std::pair<client *, bool> >::iterator itt;
	std::map<std::string, client *>::iterator itr;

	if (tokens.size() == 1)
		reply(_client[c].fd, ERR_NEEDMOREPARAMS(_client[c].nickname, "KICK"));
	else
	{
		std::vector<std::string> targets = get_targets(tokens[1]);
		if ((it = channels.find(lowerCASE(tokens[0]))) == channels.end())
		{
			reply(_client[c].fd, ERR_NOSUCHCHANNEL(_client[c].nickname, tokens[0]));
		}
		else
		{
			if (it->second.clientMap.find(_client[c].nickname) != it->second.clientMap.end())
			{
				if (it->second.clientMap[_client[c].nickname].second == 1)
				{
					for (unsigned long i = 0; i < targets.size(); i++)
					{
						if ((it->second.clientMap.erase(targets[i])) != 0)
						{
							it->second.count--;
							itr = _nicknames.find(targets[i]);
							std::vector<std::string>::iterator get = std::find(itr->second->_channel.begin(), itr->second->_channel.end(), lowerCASE(tokens[0]));
							if (get != itr->second->_channel.end())
								itr->second->_channel.erase(get);
							if (tokens.size() != 3)
								reply(_nicknames[targets[i]]->fd, KICK_MSG(_client[c].nickname, _client[c].username, _client[c].ip, tokens[0], targets[i], targets[i]));
							else
								reply(_nicknames[targets[i]]->fd, KICK_MSG(_client[c].nickname, _client[c].username, _client[c].ip, tokens[0], targets[i], tokens[2]));
							for (itt = it->second.clientMap.begin(); itt != it->second.clientMap.end(); itt++)
							{
								if (tokens.size() != 3)
									reply(itt->second.first->fd, KICK_MSG(_client[c].nickname, _client[c].username, _client[c].ip, tokens[0], targets[i], targets[i]));
								else
									reply(itt->second.first->fd, KICK_MSG(_client[c].nickname, _client[c].username, _client[c].ip, tokens[0], targets[i], tokens[2]));
							}
							if (!it->second.count)
							{
								it->second.ClearChannel();
								channels.erase(it);
							}
							if (it->second.clientMap.find(_client[c].nickname) == it->second.clientMap.end())
							{
								if (i + 1 != targets.size())
									reply(_client[c].fd, ERR_NOTONCHANNEL(_client[c].nickname, (tokens[0])));
								return ;
							}
								
						}
						else
						{
							reply(_client[c].fd, ERR_USERNOTINCHANNEL(_client[c].nickname, targets[i], (tokens[0])));
						}
					}
				}
				else
				{
					reply(_client[c].fd, ERR_CHANOPRIVSNEEDED(_client[c].nickname, tokens[0]));
				}
			}
			else
				reply(_client[c].fd, ERR_NOTONCHANNEL(_client[c].nickname, (tokens[0])));
		}
	}
}