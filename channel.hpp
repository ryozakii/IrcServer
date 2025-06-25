/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlahlafi <mlahlafi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/14 10:44:15 by osabir            #+#    #+#             */
/*   Updated: 2024/10/03 20:10:48 by mlahlafi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#pragma once
#include "client.hpp"


typedef struct t_channel
{
    std::vector<int>	ClientInvit;
    std::map<std::string, std::pair<client*, bool>,CaseInsensitiveCompare> clientMap;
    std::string			channelname;
    std::string			real_name;
    std::string			channelkey;
    std::string         mode;
    size_t              limitclient;
    size_t              count;
    bool                limit;
	bool				invite;
    bool                topic;
    bool				topicSet;
    long                topicCreationTime;
    std::string			channelTopic;
    std::string			topicSetter;
    std::string			topicSetter_username;
	bool				key;
    long                creationtime;
    void 				ClearChannel();
    void                ClearTopic(std::map<std::string, t_channel>::iterator &it);
} channel;

struct nodes
{
    std::string     flagsarg;
    std::string     mode;
    std::string     args;
    std::string     tmp;
    std::string     add;
    std::string     channelarg;
    std::string     lowChan;
    size_t          i;
    size_t          flag;
    bool            sign;

};