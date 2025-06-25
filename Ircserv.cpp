/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ircserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: osabir <osabir@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/14 06:29:31 by fel-hazz          #+#    #+#             */
/*   Updated: 2024/10/07 19:37:58 by osabir           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Ircserv.hpp"


void Server::reply(int fd, const std::string& str)
{
	send(fd, str.c_str(), str.size(),0);
}

void client::clear()
{
    std::cout << REDB <<"----------------------------------------------------------------------------------------------->\n";
    std::cout << "Client disconnected: \nIp: " << ip << "\nHostname: " << hostname << "\nPort: " << port  <<std::endl;
    std::cout << "----------------------------------------------------------------------------------------------->\n" << RESET;
    close(fd);
    fd = -10;
    registered = false;
    is_set = 0;
    ip.clear();
    buffer.clear();
    nickname.clear();
    username.clear();
    password.clear();
    hostname.clear();
    _channel.clear();    
}

void client::set(int &client_fd,struct sockaddr_in  &addr)
{
    fd = client_fd;
    registered = false;
    is_set = 0;
    ip = inet_ntoa(addr.sin_addr);
    port = ntohs(addr.sin_port);
    char host[NI_MAXHOST];
    getnameinfo((struct sockaddr *)&addr, sizeof(addr), host, NI_MAXHOST, NULL, 0, 0);
    hostname = host;
    std::cout << GREEN <<"----------------------------------------------------------------------------------------------->\n";
    std::cout << "Client connected: \nIp: " << ip << "\nHostname: " << hostname << "\nPort: " << port  <<std::endl;
    std::cout << "----------------------------------------------------------------------------------------------->\n" << RESET;

}
int parseport(const std::string &str)
{
    size_t i = 0;
    if (str.length() == 0)
        return -1;
    int number = 0;
    while (std::isdigit(str[i]))
    {
        number = number * 10 + (str[i] - '0');
        if (number > 65535)
            return -1;
        i++;
    }
    if (str[i])
        return -1;
    return number;
}

Server::Server(int port, std::string password)
{
    if (port < 0)
        throw std::runtime_error("Invalid port number");
    if (password.length() > 64)
        throw std::runtime_error("Password too long");
    else if (password.length() == 0)
        throw std::runtime_error("Password can't be empty");
    _password = password;
    _server.sin_family = AF_INET;
    _server.sin_port = htons(port);
    _server.sin_addr.s_addr = INADDR_ANY;
    // inet_pton(AF_INET, "10.11.2.12", &_server.sin_addr);
    _fd_count = 1;
    unsigned int len = 1;
    signal(SIGPIPE, SIG_IGN);
    if ((_server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        throw std::runtime_error("socket");
    fcntl(_server_fd, F_SETFL, O_NONBLOCK);
    if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &len, sizeof(int)) < 0)
        throw std::runtime_error("setsockopt");
    if (bind(_server_fd, (struct sockaddr *)&_server, sizeof(_server)) < 0)
        throw std::runtime_error("bind");
    if (listen(_server_fd, SOMAXCONN ) < 0)
        throw std::runtime_error("listen");
    len = sizeof(_server);
    if (getsockname(_server_fd, (struct sockaddr *)&_server, (socklen_t *)&len) < 0)
        throw std::runtime_error("getsockname");
    _client.resize(MAX_CLIENTS);

    time_t now = time(0);
    _date = ctime(&now);
    _clients[0].fd = _server_fd;
    _clients[0].events = POLLIN;
    std::cout << "Port: " << ntohs(_server.sin_port) << std::endl;
    cmds["PASS"] = &Server::parse_password;
    cmds["NICK"] = &Server::parse_nick;
    cmds["USER"] = &Server::parse_user;
    cmds["PRIVMSG"] = &Server::privmsg;
    cmds["KICK"] = &Server::kick;
    cmds["TOPIC"] = &Server::topic;
    cmds["JOIN"] = &Server::join;
    cmds["INVITE"] = &Server::invit_pars;
    cmds["MODE"] = &Server::pars_mode;
    cmds["PART"] = &Server::part_pars;
}

void Server::close_client(int &i)
{
    if (_client[i].registered)
    {
        for (std::map<std::string, channel>::iterator it = channels.begin(); it != channels.end(); it++)
        {
            std::vector<int>::iterator index = std::find(it->second.ClientInvit.begin(), it->second.ClientInvit.end(), _client[i].fd);
            if (index != it->second.ClientInvit.end())
                it->second.ClientInvit.erase(index);
        }
        for (size_t iters = 0; iters < _client[i]._channel.size(); iters++)
        {
            channel &it = channels[_client[i]._channel[iters]];
            if (it.clientMap.find(_client[i].nickname) != it.clientMap.end())
            {
                it.count--;
                it.clientMap.erase(_client[i].nickname);
                if (it.count == 0)
                {
                    channels[_client[i]._channel[iters]].ClearChannel();
                    channels.erase(_client[i]._channel[iters]);
                }
                else
                {
                    std::map<std::string, std::pair<client*, bool> >::iterator iter;
                    for (iter = channels[_client[i]._channel[iters]].clientMap.begin(); iter != channels[_client[i]._channel[iters]].clientMap.end(); iter++)
                        reply(iter->second.first->fd, QUIT_SERVER(_client[i].nickname, _client[i].username , _client[i].ip));
                }
            }
            

        }
        _nicknames.erase(_client[i].nickname);
    }
    _blocks.insert(i);
    _client[i].clear();
}

void Server::add_client(void)
{
    int client_fd;
    struct sockaddr_in  addr;
    int len = sizeof(addr);
    while ((client_fd = accept(_server_fd, (struct sockaddr *)&(addr), (socklen_t *)&len)) > 0)
    {
        fcntl(client_fd, F_SETFL, O_NONBLOCK);
        if (client_fd < 0)
            throw std::runtime_error("accept");
        if (_fd_count - _blocks.size() >= MAX_CLIENTS)
        {
            close(client_fd);
            return ;
        }
        if (_blocks.empty())
        {
            _client[_fd_count].set(client_fd,addr);
            _clients[_fd_count].fd = client_fd; 
            _clients[_fd_count].events = POLLIN;
            _clients[_fd_count].revents = 0;
            _fd_count++;
        }
        else
        {
            int index = *_blocks.begin();
            _blocks.erase(index);
            _client[index].set(client_fd,addr);
            _clients[index].fd = client_fd; 
            _clients[index].events = POLLIN;
            _clients[index].revents = 0;
        }
        std::cout << "clients = " << _fd_count - _blocks.size() - 1<< '\n';
    }
}

void Server::read_client(int &i)
{
    char buffer[512];
    int ret = recv(_clients[i].fd, buffer, 512,0);
    if (ret <= 0)
    {
        if (ret < 0)
            perror("recv");
        close_client(i);
        return ;
    }
    buffer[ret] = '\0';
    _client[i].buffer += buffer;
    std::string buff;
    std::string s = _client[i].buffer;
    _client[i].buffer.clear();
    for (int j = 0; j < (int)s.size();j++)
    {
        buff.clear();
        while (s[j] == ' ')
            j++;
        while (s[j] && s[j] != '\n')
        {
            buff += s[j];
            j++;
        }
        if (s[j] == '\n')
        {
            if (buff[buff.size() - 1] == '\r')
                buff.pop_back();
            if ((buff.size() > 510) || (buff.size() == 0))
            {
                if (buff.size() > 510)
                    send(_clients[i].fd, ERR_INPUTTOOLONG(_client[i].nickname).c_str(), ERR_INPUTTOOLONG(_client[i].nickname).size(), 0);
                return ;
            }
            std::cout << buff << '\n';
            parsing(i, buff);
        }
        else
        {
            _client[i].buffer = buff;
            return ;
        }
    }
}

void Server::run()
{
    std::string buff;
    while (1)
    {
        if (poll(_clients, _fd_count, -1) < 0)
            throw std::runtime_error("poll");
        if (_clients[0].revents & POLLIN)
            add_client();
        for (int i = 1; i < _fd_count; i++)
        {
            if (_client[i].fd == -10)
                continue ;
            else if ((_clients[i].revents & POLLERR) || (_clients[i].revents & POLLNVAL) || (_clients[i].revents & POLLHUP))
            {
                 if ((_clients[i].revents & POLLERR) || (_clients[i].revents & POLLNVAL))
                    std::cout << "some poll error occurred\n";
                close_client(i);
            }else if (_clients[i].revents & POLLIN)
                read_client(i);
        }
    }
}

void Server::first_word (std::string &str)
{
    size_t end = 0;
    while (str[end] != ' ' && str[end])
        end++;
    str = str.substr(0, end);
}

void Server::ltrim(std::string &str)
{
    size_t start = 0;
    while (str[start] == ' ')
        start++;
    str =  str.substr(start);

}

void Server::rtrim(std::string &str)
{
    size_t end = str.size() - 1;
    while (str[end] == ' ')
        end--;
    str =  str.substr(0, end + 1);

}
void Server::trim(std::string &str)
{
    size_t start = 0;
    while (str[start] == ' ')
        start++;
    size_t end = str.size() - 1;
    while (str[end] == ' ')
        end--;
    str = str.substr(start, end - start + 1);
}

long atolong(const std::string &str)
{
    size_t i = 0;
    bool entre = 0;
    if (str.length() == 0)
        return 0;
    long number = 0;
    while (std::isdigit(str[i]))
    {
        number = number * 10 + (str[i] - '0');
        if (number > INT_MAX)
            return 0;
        i++;
        entre = 1;
    }
    if (str[i] && !entre)
        return 0;
    return number;
}

void Server::parsing(int &i, std::string &buff)
{
    std::string cmd;

    int j = 0;
    for (; j < (int)buff.size() && buff[j] != ' ';j++)
        cmd += buff[j];
    while (buff[j] == ' ')
        j++;
    buff = buff.substr(j);
    for (j = 0 ; j < (int)cmd.size();j++)
        cmd[j] = toupper(cmd[j]);
    std::map<std::string, void (Server::*)(int &,std::string &) >::iterator it;
    it = cmds.find(cmd);
    if (it == cmds.end())
        reply(_clients[i].fd, ERR_UNKOWNCOMMAND(_client[i].nickname, cmd));
    else
        (this->*cmds[it->first])(i,buff);
}

void Server::adjust(std::string &str)
{
    if (str[0] != ':')
        first_word(str);
    else
        str = str.substr(1);
}



