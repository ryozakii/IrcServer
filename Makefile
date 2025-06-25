# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mlahlafi <mlahlafi@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/02/20 17:28:08 by fel-hazz          #+#    #+#              #
#    Updated: 2024/09/18 09:54:56 by mlahlafi         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME= ircserv

SRC=  Ircserv.cpp  authentication.cpp invite.cpp  join.cpp  kick.cpp  main.cpp  mode.cpp  part.cpp  privmsg.cpp  topic.cpp tools.cpp channel.cpp
OBJS = ${SRC:.cpp=.o}

SRC_BONUS = ircbot.cpp

OBJ_BONUS = ${SRC_BONUS:.cpp=.o}

NAME_BONUS = ircbot
INCLUDES = Ircserv.hpp channel.hpp client.hpp reply.hpp

CC = c++ 

CFLAGS = -Wall -Wextra -Werror -std=c++98 

%.o: %.cpp ${INCLUDES}
	${CC} ${CFLAGS} -c $< -o $@

all : ${NAME}

${NAME} : ${OBJS}
	 ${CC} ${CFLAGS} ${OBJS} -o ${NAME}

bonus : ${NAME_BONUS}

${NAME_BONUS} : ${OBJ_BONUS}
		${CC} ${CFLAGS} ${OBJ_BONUS} -o ${NAME_BONUS}
clean :
	@rm -rf ${OBJS} ${OBJ_BONUS}

fclean : clean
	@rm -rf ${NAME} ${NAME_BONUS}

re : fclean ${NAME}

.PHONY: clean fclean 