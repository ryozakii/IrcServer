/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: osabir <osabir@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/14 10:44:08 by osabir            #+#    #+#             */
/*   Updated: 2024/09/15 19:38:28 by osabir           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "channel.hpp"

void t_channel::ClearChannel()
{
    ClientInvit.clear();
    clientMap.clear();
    channelname.clear();
    channelkey.clear();
    channelTopic.clear();
    topicSetter.clear();
    mode.clear();
}