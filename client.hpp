/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlahlafi <mlahlafi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/14 10:45:05 by osabir            #+#    #+#             */
/*   Updated: 2024/10/04 18:08:51 by mlahlafi         ###   ########.fr       */
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
#include "reply.hpp"

struct CaseInsensitiveCompare {
    bool operator()(const std::string& a, const std::string& b) const {
        std::string a_lower = a;
        std::string b_lower = b;
        std::transform(a_lower.begin(), a_lower.end(), a_lower.begin(), ::tolower);
        std::transform(b_lower.begin(), b_lower.end(), b_lower.begin(), ::tolower);
        return a_lower < b_lower;
    }
};

typedef struct t_client
{
    int                 fd;
    bool                registered;
    unsigned char       is_set;
    unsigned short      port;
    std::string			ip;
    std::string         buffer;
    std::string         nickname;
    std::string         username;
    std::string         realname;
    std::string         password;
    std::string         hostname;
    std::vector<std::string> _channel;
    void clear();
    void set(int &fd, struct sockaddr_in &);
} client;
