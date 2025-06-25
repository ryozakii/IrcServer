/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reply.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: osabir <osabir@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/14 10:49:56 by osabir            #+#    #+#             */
/*   Updated: 2024/10/07 19:38:28 by osabir           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#define MAX_CLIENTS 10000
//turn pass to upper

#define PASS_IS_SET 1
#define NICK_IS_SET 2
#define USER_IS_SET 4


// MY ERROR

#define REDB "\033[31m\033[1m"
#define GREEN  "\033[32m\033[1m"
#define BOLD   "\033[1m"
#define RESET   "\033[0m\033[1m"
#define INVITE(nickname,username,nickinvite,channelname,hostname) (":" + nickname + "!~" + username + "@" + hostname + " INVITE " + nickinvite + " :" + channelname + "\r\n")
#define SEND_MODE(nickname,username,channelname,mode,hostname) (":" + nickname + "!~" + username + "@" + hostname + " MODE " + channelname + " " + mode + "\r\n")
#define RPL_CHANNELMODEIS(nickname,channelname,mode) ("234 " + nickname + " " + channelname + " " + mode + "\r\n")
#define RPL_CREATIONTIME(nickname,channelname,creattime) ("329 " + nickname + " " + channelname + " " + creattime + "\r\n")
#define ERR_USERNOTINCHANNEL(clientname,nickname,channelname) ("441 " + clientname + " " + nickname + " " + channelname + " :They aren't on that channel\r\n")
#define RPL_ENDOFNAMES(nickname,channelname) ("366 " + nickname + " " + channelname + " :End of /NAMES list\r\n")
#define ERR_UNKOWNCOMMAND(nickname,command) ("421 " + nickname + " " + command + " :Unknown command\r\n")
#define ERR_CHANNELISFULL(nickname,channelname) ("471 " + nickname + " " + channelname + " :Cannot join channel (+l)\r\n")
#define ERR_BADCHANNELKEYY(nickname,channelname) ("475 " + nickname + " " + channelname + " :Cannot join channel (+k) - bad key\r\n")
#define ERR_UNKNOWNMODE(nickname,character) ("472 " + nickname + " " + character + " :is unknown mode char to me\r\n")
#define RPL_NAMREPLY(nickname,channelname,listclient) ("353 " + nickname + " = " + channelname + " :" + listclient + "\r\n")
#define ERR_USERSDONTMATCH(nickname) ("501 " + nickname + " :Cant change mode for other users\r\n")
#define ERR_NOSUCHNICK(nickname,_nickname) ("401 " +  nickname + " " + _nickname + " :No such nick/channel\r\n")
#define RPL_INVITING(clientname,nickname,channelname) ("341 " + clientname + " " + nickname + " " + channelname + "\r\n")
#define ERR_USERONCHANNEL(nickname,channelname) ("443 " +  nickname + " " + channelname + " :is already on channel\r\n")
#define ERR_CHANOPRIVSNEEDED(nickname,channelname) ("482 " +  nickname + " " + channelname + " :You're not channel operator\r\n")
#define ERR_NOTONCHANNEL(nickname,channelname) ("442 " +  nickname + " " + channelname + " :You're not on that channel\r\n")
#define ERR_BADCHANNELKEY(nickname,channelname) ("475 " +  nickname + " " + channelname + " :Cannot join channel (+k)\r\n")
#define ERR_NOSUCHCHANNEL(nickname,channelname) ("403 " +  nickname + " " + channelname + " :No such channel\r\n")
#define ERR_INVITEONLYCHAN(nickname,channelname) ("473 " +  nickname + " " + channelname + " :Cannot join channel (+i)\r\n")
#define	JOIN_CHANNEL(nickname,username,channelname,hostname) (":" + nickname + "!~" + username + "@" + hostname + " JOIN " + channelname + "\r\n")
#define ERR_BADCHANNAME(nickname,channelname) ("479 " + nickname + " " + channelname + " :Illegal channel name\r\n")

// part channel
#define	PART_CHANNEL(nickname,username,channelname,hostname) (":" + nickname + "!~" + username + "@" + hostname + " PART " + channelname + "\r\n")
#define	PART_CHANNEL_REASON(nickname,username,channelname,hostname,reason) (":" + nickname + "!~" + username + "@" + hostname + " PART " + channelname + " :" + reason +"\r\n")
// quit channel
#define QUIT_SERVER(nickname,username,hostname) (":" + nickname + "!~" + username + "@" + hostname + " QUIT :Remote host closed the connection\r\n")
#define ERR_BADCHANNAME(nickname,channelname) ("479 " + nickname + " " + channelname + " :Illegal channel name\r\n")

//privmsg errors
#define RPL_PRIVMSG(nickname, username, ip , targets, msg)(":" + nickname + "!~"  + username + "@" + ip + " PRIVMSG " + targets + " :" +  msg + "\r\n")
#define ERR_NORECIPIENT(nickname)("411 " + nickname + " :No recipient given (PRIVMSG)\r\n")
#define ERR_NOTEXTTOSEND(nickname)("412 " + nickname + " :No text to send\r\n")
#define ERR_CANNOTSENDTOCHAN(nickname, channelname)("404 " + nickname + " " + channelname + " :Cannot send to nick/channel\r\n")

//kick errors
#define KICK_MSG(nickname, username, ip, channelname, target, comment)(":" + nickname + "!~" + username + "@" + ip + " KICK " + channelname + " " + target + " :" + comment + "\r\n")
#define KICK_MSG_C(nickname, username, ip, channelname, target, comment)(":" + nickname + "!~" + username + "@" + ip + " KICK " + channelname + " " + target + " :" + comment + "\r\n")
//ERR
//topic
#define ERR_T_NOSUCHNICK(nickname) ("401 " +  nickname + " :No such nick/channel\r\n")
#define RPL_TOPIC(nickname,channelname,topic) ("332 " + nickname + " " + channelname + " :" + topic + "\r\n")
#define RPL_NOTOPIC(nickname,channelname) ("331 " +  nickname + " " + channelname + " :No topic is set.\r\n")
#define RPL_TOPICWHOTIME(nickname, topicSetter, username, channelname, timed, ip) ("333 " + nickname + " " + channelname + " " + topicSetter + "!~" + username + "@" + ip + " " + timed + "\r\n")
#define	TOPIC_CHANGED(nickname,username,channelname, ip, chanTopic) (":" + nickname + "!~" + username + "@" + ip + " TOPIC " + channelname + " :" + chanTopic + "\r\n")

#define NETWORK_NAME "Ircserver Network "
#define SERVER_NAME "Ircserver"

#define RED "\e[1;31m"
#define ERR_INPUTTOOLONG(nickname) ("417 " + nickname +  " :Input line too long\r\n")
#define ERR_NEEDMOREPARAMS(nickname,cmd)("461 " + nickname + " " + cmd + " :Not enough parameters\r\n")
#define ERR_PASS(nickname,cmd)("461 " + nickname + " " + cmd + " :Not enough parameters, No password given\r\n")

#define ERR_ALREADYREGISTRED(nickname) ("462 " + nickname + " :You may not reregister\r\n")
#define ERR_ERRONEUSNICKNAME(nick) ("432 " +  nick + " :Erroneous nickname\r\n")
#define ERR_NICKNAMEINUSE(oldnickname,nick)("433 " + oldnickname + " " + nick + " :Nickname is already in use\r\n")
#define ERR_NICKNAMEINUSEE(nick)("433 " + nick + " :Nickname is already in use\r\n")
#define ERR_NONICKNAMEGIVEN(nickname) ("431 " + nickname + " :No nickname given\r\n")
#define ERR_PASSWDMISMATCH(nickname) ("464 " + nickname + " :Password incorrect\r\n")
//RPLY
#define RPL_WELCOME(nickname,username,hostname) ("001 " + nickname + " :Welcome to the " + NETWORK_NAME + nickname + "!" + username + "@" + hostname  + "\r\n")
#define RPL_YOURHOST(nickname) ("002 " + nickname + " :Your host is " + SERVER_NAME + ", running version 1.0\r\n")
#define RPL_CREATED(nickname) ("003 " + nickname + " :This server was created " + _date + "\r\n")


#define EPL_NICKNAMECHANGE(nicknameold,username,hostname,nicknamenew) (":" + nicknameold+ "!~"+ username + "@"+ hostname + " NICK :" + nicknamenew + "\r\n")
#define ERR_BADCHANNAME(nickname,channelname) ("479 " + nickname + " " + channelname + " :Illegal channel name\r\n")