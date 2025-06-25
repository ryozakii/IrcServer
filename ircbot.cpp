#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sstream>
#define REDB "\033[31m"

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

void trim(std::string &str)
{
    size_t start = 0;
    while (str[start] == ' ')
        start++;
    size_t end = str.size() - 1;
    while (str[end] == ' ')
        end--;
    str = str.substr(start, end - start + 1);
}

void calculate(std::string &operation)
{
    std::string result;
    std::string num1, num2;
    char op;
    size_t pos = 0;
    if ((pos = operation.find('+')) != std::string::npos) {
        op = '+';
    } else if ((pos = operation.find('-')) != std::string::npos) {
        op = '-';
    } else if ((pos = operation.find('*')) != std::string::npos) {
        op = '*';
    } else if ((pos = operation.find('/')) != std::string::npos) {
        op = '/';
    } else {
        operation = "Usage: <num1> <op> <num2> || Supported <op> : + | - | * | /\n";
        return ;
    }
        num1 = operation.substr(0, pos);
        num2 = operation.substr(pos + 1);
        trim(num1); trim(num2);
    std::stringstream s;
    double n1,n2;
    s << num1;
    s >> n1;
    s.clear();
    s << num2;
    s >> n2;
    s.clear();
    double r;
    std::string tmp;
    switch (op) {
        case '+':
            r = n1 + n2;
            s << r;
            s >> tmp;
            result += tmp;
            break;
        case '-':
            r = n1 - n2;
            s << r;
            s >> tmp;
            result += tmp;
            break;
        case '*':
            r = n1 * n2;
            s << r;
            s >> tmp;
            result += tmp;
            break;
        case '/':
            r = n1 / n2;
            s << r;
            s >> tmp;
            result += tmp;
            break;
    }

    operation = result;

}

int main(int argc, char **argv) {
    if (argc < 3) {
        std::cerr << REDB << "Usage: ./ircbot <port> <password> <ip = localhost> <name = ircbot>" << std::endl;
        exit(EXIT_FAILURE);
    }


    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        std::cerr << REDB << "Error creating the bot socket!" << std::endl;
        exit(EXIT_FAILURE);
    }

    int port = parseport(argv[1]);
    if (port == -1) {
        std::cerr << REDB << "Port number is invalid" << std::endl;
        close(fd);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_info;
    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_info.sin_family = AF_INET;
    if (argc >= 4)
    {
        std::string d = argv[3];
        if (d == "localhost")
            server_info.sin_addr.s_addr = INADDR_ANY;
        else
        {
        if (inet_pton(AF_INET, argv[3], &server_info.sin_addr) <= 0)
        {
            std::cerr << REDB << "Invalid ip address" << std::endl;
            exit(EXIT_FAILURE);
        }
        }
    }
    else
        server_info.sin_addr.s_addr = INADDR_ANY;
    std::string botname;
    if (argc == 5)
        botname = argv[4];
    else
        botname = "ircbot";
    server_info.sin_port = htons(port);
    if (connect(fd, (struct sockaddr *)&server_info, sizeof(server_info)) < 0) {
        std::cerr << REDB << "Error connecting to the server!" << std::endl;
        close(fd);
        exit(EXIT_FAILURE);
    }

    std::string m = argv[2];
    bool f = m.find(' ') != std::string::npos;
    std::string passCmd = "PASS " + ( f ? std::string(":") : std::string("")) + m + "\nNICK " + botname + "\nUSER ircbot 0 * 0 bot\n";
    if (send(fd, passCmd.c_str(), passCmd.size(),0) < 0) {
        std::cerr << REDB << "Error writing to the socket!" << std::endl;
        close(fd);
        exit(EXIT_FAILURE);
    }

    char buffer[1024];
    recv(fd, buffer, sizeof(buffer) - 1,0);
    std::string s = buffer;
    if (s.find("001") == std::string::npos)
    {
        std::cerr << REDB << "Error connecting to the server\n";
        return -1;
    }
    else
        std::cout << "botot device soxasfolly connected\n";
    while (true) {
        int bytesRead = recv(fd, buffer, sizeof(buffer) - 1,0);
        if (bytesRead < 0) {
            std::cerr << REDB << "Error reading from the socket!" << std::endl;
            break;
        }
        if (bytesRead == 0) {
            break;
        }
        buffer[bytesRead] = '\0'; 
        std::string tmp(buffer);
        if (tmp.find(':') == std::string::npos || tmp.find(':') == std::string::npos || tmp.find('@') == std::string::npos || tmp.find(':') == tmp.size() - 1)
            continue;
        size_t start = tmp.find(':');
        std::string parseme = tmp.substr(tmp.find('@'));
        parseme = parseme.substr(parseme.find(':') + 1);
        calculate(parseme);
        if (start != std::string::npos) {
            std::string nickname = tmp.substr(start + 1, tmp.find('!')  - start - 1);
            std::string msg = "PRIVMSG " + nickname + " :" + parseme + "\n";
            send(fd, msg.c_str(), msg.size(),0);
        }
    }
    close(fd);
    return 0;
}