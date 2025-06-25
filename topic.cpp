/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: osabir <osabir@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/22 13:14:20 by mlahlafi          #+#    #+#             */
/*   Updated: 2024/10/07 16:02:27 by osabir           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

void	t_channel::ClearTopic(std::map<std::string, t_channel>::iterator &it)
{
    it->second.channelTopic = "";
	it->second.topicSet = 0;
	it->second.topicSetter = "";
	it->second.topicSetter_username = "";
	it->second.topicCreationTime = 0;
}

void Server::topic(int &c, std::string &str)
{
	if (!(_client[c].registered))
		return;
	if (str.empty())
	{
		reply(_client[c].fd, ERR_NEEDMOREPARAMS(_client[c].nickname, "TOPIC"));
		return;
	}
	std::vector<std::string> tokens = get_tokens(str);
	std::map<std::string, channel>::iterator it;
	std::map<std::string, std::pair<client *, bool> >::iterator itt;
	std::stringstream x;

	if (tokens.size() == 0 || (tokens.size() == 1 && tokens[0][0] == ':'))
		reply(_client[c].fd, ERR_NEEDMOREPARAMS(_client[c].nickname, "TOPIC"));
	else
	{
		if ((it = channels.find(lowerCASE(tokens[0]))) == channels.end())
		{
			reply(_client[c].fd, ERR_T_NOSUCHNICK(tokens[0]));
		}
		else
		{
			if (it->second.clientMap.find(_client[c].nickname) != it->second.clientMap.end())
			{
				if (tokens.size() == 1)
				{
					if (it->second.topicSet)
					{
						x << it->second.topicCreationTime;
						reply(_client[c].fd, RPL_TOPIC(_client[c].nickname, it->second.real_name, it->second.channelTopic));
						reply(_client[c].fd, RPL_TOPICWHOTIME(_client[c].nickname, it->second.topicSetter, it->second.topicSetter_username, it->second.real_name, x.str(), _client[c].ip));
					}
					else
						reply(_client[c].fd, RPL_NOTOPIC(_client[c].nickname, tokens[0]));
				}
				else if (it->second.clientMap[_client[c].nickname].second == 1 || !it->second.topic)
				{
					if (tokens[1].size() == 0)
					{
						it->second.ClearTopic(it);
						for (itt = it->second.clientMap.begin(); itt != it->second.clientMap.end(); itt++)
						{
							reply(itt->second.first->fd, TOPIC_CHANGED(_client[c].nickname, _client[c].username, it->second.real_name, _client[c].ip, tokens[1]));
						}
						return;
					}
					it->second.channelTopic = tokens[1];
					it->second.topicSet = 1;
					it->second.topicSetter = _client[c].nickname;
					it->second.topicSetter_username = _client[c].username;
					it->second.topicCreationTime = time(0);
					for (itt = it->second.clientMap.begin(); itt != it->second.clientMap.end(); itt++)
					{
						reply(itt->second.first->fd, TOPIC_CHANGED(_client[c].nickname, _client[c].username, it->second.real_name, _client[c].ip, tokens[1]));
					}
				}
				else
				{
					reply(_client[c].fd, ERR_CHANOPRIVSNEEDED(_client[c].nickname, tokens[0]));
				}
			}
			else
				reply(_client[c].fd, ERR_NOTONCHANNEL(_client[c].nickname, tokens[0]));
		}
	}
}