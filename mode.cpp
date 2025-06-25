    /* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: osabir <osabir@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/18 09:38:59 by osabir            #+#    #+#             */
/*   Updated: 2024/04/26 17:48:53 by osabir           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"

void    Server::prototypeMode(std::vector<std::string> &command, nodes &node, int &index)
{
    long lim = 0;
    if(node.flagsarg[node.i] == 'i')
    {
        channels[node.lowChan].invite = node.sign;
        node.tmp += 'i';
    }
    else if (node.flagsarg[node.i] == 't')
    {
        channels[node.lowChan].topic = node.sign;
        node.tmp += 't';
    }
    else if (node.flagsarg[node.i] == 'l')
    {
        if (node.sign)
        {
            lim = atolong(command[node.flag]);
            if (lim <= 0)
                return ;
            channels[node.lowChan].limitclient = lim;
            std::stringstream x;
            x << lim;
            node.args += " " + x.str();

        }
        channels[node.lowChan].limit = node.sign;
        node.tmp += 'l';
    }
    else if (node.flagsarg[node.i] == 'o')
    {
        if (_nicknames.find(command[node.flag]) == _nicknames.end())
            reply(_client[index].fd, ERR_NOSUCHNICK(_client[index].nickname, command[node.flag]));
        else if (channels[node.lowChan].clientMap.find(command[node.flag]) == channels[node.lowChan].clientMap.end())
            reply(_client[index].fd, ERR_USERNOTINCHANNEL(_client[index].nickname, command[node.flag], node.channelarg));
        else
        {
            channels[node.lowChan].clientMap[command[node.flag]].second = node.sign;
            node.tmp += 'o';
            node.args += " " + command[node.flag];
        }
    }
    else if (node.flagsarg[node.i] == 'k')
    {
        std::string password; 
        if (node.sign)
        {
            channels[node.lowChan].key = node.sign;
            if (command[node.flag][0] == ':')
            {
                while (node.flag < command.size())
                    password += command[node.flag++];
                password = password.c_str() + 1;
            }
            else
                password = command[node.flag];
            channels[node.lowChan].channelkey = password;
            node.args += " " + password;
        }
        else if (!node.sign && channels[node.lowChan].key)
        {
            channels[node.lowChan].key = node.sign;
            node.args += " *";
        }
        node.tmp += 'k';
    }
}

void Server::pars_mode(int &index,std::string &buff)
{
    std::vector<std::string>	command;
    if (!(_client[index].registered))
        return ;
    command = command_split(buff);
    if (command.size() < 1)
    {
        reply(_client[index].fd, ERR_NEEDMOREPARAMS(_client[index].nickname, "MODE"));
        return ;
    }
    nodes node;
    node.channelarg = command[0];
    node.lowChan = lowerCASE(node.channelarg);
    node.i = 0;
    node.flag = 2;
    if ((node.channelarg.find("#") != 0) || channels.find(node.lowChan) == channels.end())
        reply(_clients[index].fd, ERR_NOSUCHCHANNEL(_client[index].nickname, node.channelarg));
    else if (command.size() == 1)
    {
        std::stringstream x;
        x << channels[node.lowChan].creationtime;
        reply(_clients[index].fd, RPL_CHANNELMODEIS(_client[index].nickname, node.channelarg, channels[node.lowChan].mode));
        reply(_clients[index].fd, RPL_CREATIONTIME(_client[index].nickname, node.channelarg, x.str()));
    }
    else
    {
        node.flagsarg = command[1];
        if (command.size() >= 2)
        {
            size_t r = node.flagsarg.find_first_not_of("-+ikolt");
            if (r != std::string::npos)
            {
                reply(_client[index].fd, ERR_UNKNOWNMODE(_client[index].nickname, node.flagsarg[r]));
                return ;
            }
            
            while (node.i < node.flagsarg.length())
            {
                while (node.i < node.flagsarg.length() && (node.flagsarg[node.i] == '-' || node.flagsarg[node.i] == '+'))
                    node.i++;
                if (node.i == 0 || node.flagsarg[node.i - 1] == '+')
                    node.sign = 1;
                else
                    node.sign = 0;
                while (node.i < node.flagsarg.length())
                {
                    if ((node.flagsarg[node.i] == 'o' || (node.sign && node.flagsarg[node.i] == 'l')) && node.flag >= command.size())
                    {
                        reply(_client[index].fd, ERR_NEEDMOREPARAMS(_client[index].nickname, "MODE"));
                        return ;
                    }
                    {
                        if (node.flagsarg[node.i] == 'k' && command[node.flag][0] == ':')
                        {
                            while (node.flag < command.size())
                                node.flag++;
                        }
                        else if (((node.flagsarg[node.i] == 'o' || node.flagsarg[node.i] == 'k')) || (node.sign && (node.flagsarg[node.i] == 'l')))
                            node.flag++;
                    }
                    if (node.flagsarg[node.i] == '+'|| node.flagsarg[node.i] == '-')
                        break ;
                    node.i++;
                }
            }

            if (channels[node.lowChan].clientMap.find(_client[index].nickname) == channels[node.lowChan].clientMap.end() || channels[node.lowChan].clientMap[_client[index].nickname].second == false)
            {
                send(_client[index].fd, ERR_CHANOPRIVSNEEDED(_client[index].nickname, node.channelarg).c_str(), ERR_CHANOPRIVSNEEDED(_client[index].nickname, node.channelarg).size(), 0);
                return ;
            }
            node.i = 0;
            node.flag = 2;
            while (node.i < node.flagsarg.length() && node.flag <= command.size())
            {
                node.tmp.clear();
                while (node.i < node.flagsarg.length() && (node.flagsarg[node.i] == '-' || node.flagsarg[node.i] == '+'))
                    node.i++;
                if (node.i == 0 || node.flagsarg[node.i - 1] == '+')
                {
                    node.sign = 1;
                    node.add = '+';
                }
                else
                {
                    node.sign = 0;
                    node.add = '-';
                }
                while (node.i < node.flagsarg.length() && node.flag <= command.size())
                {
                    if (node.flagsarg[node.i] == 'i' && ((!node.sign && channels[node.channelarg].invite) || (node.sign && !channels[node.channelarg].invite)))
                        prototypeMode(command, node, index);
                    else if (node.flagsarg[node.i] == 't' && ((!node.sign && channels[node.channelarg].topic) || (node.sign && !channels[node.channelarg].topic)))
                        prototypeMode(command, node, index);
                    else if (node.flagsarg[node.i] == 'l' && ((!node.sign && channels[node.channelarg].limit) || node.sign))
                        prototypeMode(command, node, index);
                    else if (node.flagsarg[node.i] == 'o')
                        prototypeMode(command, node, index);
                    else if (node.flagsarg[node.i] == 'k' && (((!node.sign || node.sign) && channels[node.channelarg].key) || (node.sign && !channels[node.channelarg].key)))
                        prototypeMode(command, node, index);
                    else if (node.flagsarg[node.i] == '+'|| node.flagsarg[node.i] == '-')
                        break ;
                    if (((node.flagsarg[node.i] == 'o' || node.flagsarg[node.i] == 'k')) || (node.sign && (node.flagsarg[node.i] == 'l')))
                        node.flag++;
                    node.i++;
                }
                if (!node.tmp.empty())
                    node.mode += node.add + node.tmp;
            }
            node.mode += node.args;
            check_mode(node.lowChan);
            if (!node.mode.empty())
            {
                std::map<std::string, std::pair<client*, bool> >::iterator it;
                for (it = channels[node.lowChan].clientMap.begin(); it != channels[node.lowChan].clientMap.end(); it++)
                    reply(it->second.first->fd, SEND_MODE(_client[index].nickname, _client[index].username, channels[node.lowChan].real_name, node.mode, _client[index].ip));
            }
        }
    }
}

void Server::check_mode(std::string channelname)
{
    channels[channelname].mode = "";

    if (channels[channelname].invite)
        channels[channelname].mode += 'i';
    if (channels[channelname].topic)
        channels[channelname].mode += 't';
    if (channels[channelname].key)
        channels[channelname].mode += 'k';
    if (channels[channelname].limit)
        channels[channelname].mode += 'l';

    if (!channels[channelname].mode.empty())
        channels[channelname].mode = "+" + channels[channelname].mode;
}
