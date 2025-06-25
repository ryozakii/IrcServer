/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ircserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: osabir <osabir@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/13 03:11:56 by fel-hazz          #+#    #+#             */
/*   Updated: 2024/10/03 20:45:32 by osabir           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <exception>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <map>
#include <vector>
#include <netdb.h>
#include <ctime>
#include <sstream>
#include <limits.h>
#include <iostream>
#include <set>
#include <stack>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <cctype>
#include <map>
#include "client.hpp"
#include "channel.hpp"
#include "reply.hpp"



class Server
{
    private:
    std::map<std::string, channel> channels;
    std::map<std::string, void (Server::*)(int &,std::string &) > cmds;
    std::set<int> _blocks;
    std::map<std::string, client *,CaseInsensitiveCompare> _nicknames;
    std::string         _date;
    struct sockaddr_in  _server;
    int                 _server_fd;
    std::string         _password;
    int                 _fd_count;
    struct pollfd       _clients[MAX_CLIENTS];
    std::vector<client> _client;
    public:
    Server(int port, std::string password);
    void authenticate(int &i);
    void parsing(int &id, std::string &buff);
    void close_client(int &i);
    void add_client(void);
    void read_client(int &i);
    void run();
    bool blind_find(std::string &buff,std::string str);
    bool blind_find_1(std::string &buff,std::string str);
    void first_word(std::string &str);
    void trim(std::string &str);
    void ltrim(std::string &str);
    void rtrim(std::string &str);
    void adjust(std::string &str);

    void parse_password(int &i, std::string &password);
    void parse_nick(int &i, std::string &nickname);
    void parse_user(int &i, std::string &user);
    void join(int &i, std::string &cmd);
    void invit_pars(int &i, std::string &cmd);
    void privmsg(int &i, std::string &cmd);
    void kick(int &i, std::string &cmd);
    void topic(int &i, std::string &cmd);
    void pars_mode(int &i, std::string &cmd);
    void part_pars(int &i, std::string &cmd);

    void to_add_channel(const std::string& namechannel, int index);

    bool check_invit(int client, std::string chaneel);
    void check_mode(std::string channelname);
    std::vector<std::string> get_targets(std::string targets);
    std::vector<std::string> get_tokens(std::string str);
    void joinResponse(const std::string& namechannel, int index);
    void sendJoinResponse(int index, const std::string& channelName, const std::string& listClient);
    void reply(int fd, const std::string& str);
    std::string buildClientList(const std::string& channelName, int index);
    void prototypeChanel(int index, std::string chennalname, std::string chennalKey);
    void prototypeMode(std::vector<std::string> &command, nodes &node, int &index);
    void prototypePart(std::string &chennalname, int &index, std::string reason);
    ~Server() {}
};
int parseport(const std::string &str);
std::vector<std::string> split(const std::string &string, char delimiter);
long atolong(const std::string &str);
std::vector<std::string> command_split(std::string &buff);
std::string lowerCASE(const std::string& str);
