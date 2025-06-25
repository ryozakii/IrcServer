/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlahlafi <mlahlafi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/13 03:10:27 by fel-hazz          #+#    #+#             */
/*   Updated: 2024/09/18 11:28:24 by mlahlafi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserv.hpp"


int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cout << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }
    else
    {
        try
        {
            Server server(parseport(argv[1]), argv[2]);
            server.run();
        }
        catch (std::exception &e)
        {
            std::cerr << e.what() << std::endl;
            if (errno)
            perror("Error");
            return 1;
        }
    }
    
}
