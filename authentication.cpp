/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   authentication.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: osabir <osabir@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/14 06:29:31 by fel-hazz          #+#    #+#             */
/*   Updated: 2024/10/07 19:37:13 by osabir           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Ircserv.hpp"

void Server::parse_password(int &i, std::string &password)
{
    if (_client[i].registered)
    {
        send(_clients[i].fd, ERR_ALREADYREGISTRED(_client[i].nickname).c_str(), ERR_ALREADYREGISTRED(_client[i].nickname).size(), 0);
        return;
    }
    adjust(password);
    if (password.size() == 0)
    {
        send(_clients[i].fd, ERR_NEEDMOREPARAMS(_client[i].nickname, "PASS").c_str(), ERR_NEEDMOREPARAMS(_client[i].nickname, "PASS").size(), 0);
        return;
    }
    if (password.size() > 64)  
    {
        send(_clients[i].fd, ERR_INPUTTOOLONG(_client[i].nickname).c_str(), ERR_INPUTTOOLONG(_client[i].nickname).size(), 0);
        return;
    }
    _client[i].password = password;
    _client[i].is_set |= PASS_IS_SET;
    authenticate(i);
}

void Server::parse_nick(int &i, std::string &nickname)
{
    if (!(_client[i].is_set & PASS_IS_SET))
    {
        send(_clients[i].fd, ERR_PASS(_client[i].nickname, "NICK").c_str(),ERR_PASS(_client[i].nickname, "NICK").size(), 0);
        return;
    }
    adjust(nickname);
    if (nickname.size() == 0)
    {
        send(_clients[i].fd, ERR_NONICKNAMEGIVEN(_client[i].nickname).c_str(), ERR_NONICKNAMEGIVEN(_client[i].nickname).size(), 0);
        return;
    }
    if (nickname.find_first_of(" ,*?!@.") != std::string::npos || nickname.substr(0, 1).find_first_of("0123456789$:#") != std::string::npos || (nickname.size() > 9))
    {
        send(_clients[i].fd, ERR_ERRONEUSNICKNAME(nickname).c_str(), ERR_ERRONEUSNICKNAME(nickname).size(), 0);
        return;
    }
    if (_client[i].registered)
    {
        std::map<std::string ,client *>::iterator it = _nicknames.find(nickname);
            if (it != _nicknames.end())
            {
                if (it->second != &_client[i])
                {
                    send(_clients[i].fd, ERR_NICKNAMEINUSE(_client[i].nickname, nickname).c_str(), ERR_NICKNAMEINUSE(_client[i].nickname, nickname).size(), 0);
                    return ;
                }
                else
                {
                    if (nickname == _client[i].nickname)
                        return ;
                }
            }
            send(_client[i].fd, EPL_NICKNAMECHANGE(_client[i].nickname, _client[i].username, _client[i].ip, nickname).c_str(), EPL_NICKNAMECHANGE(_client[i].nickname, _client[i].username, _client[i].ip, nickname).size(), 0  );
        for (std::vector<std::string>::iterator it = _client[i]._channel.begin(); it != _client[i]._channel.end();it++)
        {
            std::map<std::string, channel>::iterator it2 = channels.find(*it);
            if (it2 != channels.end())
            {
                std::map<std::string, std::pair<client *, bool> >::iterator it3 = it2->second.clientMap.begin();
                std::map<std::string, std::pair<client * , bool> >::iterator vc;

                for (; it3 != it2->second.clientMap.end();it3++)
                {
                    if (it3->second.first->fd != _client[i].fd)
                        send(it3->second.first->fd, EPL_NICKNAMECHANGE(_client[i].nickname, _client[i].username, _client[i].ip, nickname).c_str(), EPL_NICKNAMECHANGE(_client[i].nickname, _client[i].username, _client[i].ip, nickname).size(), 0);
                    else
                        vc = it3;
                }
                it2->second.clientMap[nickname] = vc->second;
                it2->second.clientMap.erase(_client[i].nickname);
            }
        }
        _nicknames.erase(_client[i].nickname);
        _client[i].nickname = nickname;
        _nicknames[nickname] = &(_client[i]);

    }
    _client[i].nickname = nickname;
    _client[i].is_set |= NICK_IS_SET;
    authenticate(i);
}

void Server::parse_user(int &i, std::string &user)
{
    if (!(_client[i].is_set & PASS_IS_SET))
    {
        send(_clients[i].fd, ERR_PASS(_client[i].nickname, "USER").c_str(),ERR_PASS(_client[i].nickname, "USER").size(), 0);
        return;
    }
    if (_client[i].registered)
    {
        send(_clients[i].fd, ERR_ALREADYREGISTRED(_client[i].nickname).c_str(), ERR_ALREADYREGISTRED(_client[i].nickname).size(), 0);
        return;
    }
    rtrim(user);
    size_t y = 0;
    for (; y <user.size() && user[y] != ' ';y++)
        _client[i].username.push_back(user[y]);
    int flag = 0;
    while (y < user.size() && user[y] == ' ')
        y++;
    while (y < user.size() && user[y] != ' ')
    {
        flag = 1;
        if (user[y] > '9' || user[y] < '0')
        {
            flag = -1;
            break;
        }
        y++;
    }
    if (flag != -1)
    {
        while (y < user.size() && user[y] == ' ')
            y++;
        while (y < user.size() && user[y] != ' ')
        {
            flag = 2;
            y++;
        }
        while (y < user.size() && user[y] == ' ')
            y++;
        while (y < user.size() && user[y] != ' ')
        {
            flag = 3;
            y++;
        }
    }
    if (flag != 3)
    {
        send(_clients[i].fd, ERR_NEEDMOREPARAMS(_client[i].nickname, "USER").c_str(),ERR_NEEDMOREPARAMS(_client[i].nickname, "USER").size(), 0);
        return;
    }   
    if (_client[i].username.find(" ") != std::string::npos || _client[i].username.size() > 9 || _client[i].username.size() == 0 )
        _client[i].username = _client[i].nickname;
    _client[i].is_set |= USER_IS_SET;
    authenticate(i);
}


void Server::authenticate(int &i)
{
    if (_client[i].registered)
        return ;
    if (_client[i].is_set == (PASS_IS_SET | NICK_IS_SET | USER_IS_SET))
    {
        if (_password != _client[i].password)
        {
            send(_clients[i].fd, ERR_PASSWDMISMATCH(_client[i].nickname).c_str(), ERR_PASSWDMISMATCH(_client[i].nickname).size(), 0);
            return;
        }
        std::string nickname = _client[i].nickname;
        if (_nicknames.find(nickname) != _nicknames.end())
        {
                send(_clients[i].fd, ERR_NICKNAMEINUSEE(nickname).c_str(), ERR_NICKNAMEINUSEE(nickname).size(), 0);
                return ;
        }
        _client[i].registered = true;
        _client[i].nickname = nickname;
        _nicknames[nickname] = &(_client[i]);
        send(_clients[i].fd, RPL_WELCOME(_client[i].nickname, _client[i].username, _client[i].hostname).c_str(), RPL_WELCOME(_client[i].nickname, _client[i].username, _client[i].hostname).size(), 0);
        send(_clients[i].fd, RPL_YOURHOST(_client[i].nickname).c_str(), RPL_YOURHOST(_client[i].nickname).size(), 0);
        send(_clients[i].fd, RPL_CREATED(_client[i].nickname).c_str(), RPL_CREATED(_client[i].nickname).size(), 0);
    }  
}