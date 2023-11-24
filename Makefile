# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: zjaddad <zjaddad@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/10/11 16:00:46 by enja              #+#    #+#              #
#    Updated: 2023/11/22 17:32:48 by zjaddad          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv

SRCS = Web_serv/main.cpp Web_serv/ConfigParser/ConfigParser.cpp Web_serv/ConfigParser/MakeSockets.cpp Multiplexing/multiplexing.cpp \
	   Request_pars/Request_parsing.cpp Request_pars/Request_Handler.cpp Methods/DELETE.cpp Methods/GET.cpp Methods/POST.cpp \
	   Cgi-Call/CGI.cpp
OBJS = $(SRCS:.cpp=.o)
HEADERS =  ./Headers/includes.hpp ./Headers/ConfigParser.hpp

CXX = c++
RM = rm -rf
CXXFLAGS = -Wall -Wextra -Werror -g3 -std=c++98

all: $(NAME)

$(NAME): $(OBJS) $(HEADERS)
	@$(CXX) $(CXXFLAGS) $(SRCS) -o $(NAME)

clean:
	@$(RM) $(OBJS)

fclean: clean
	@$(RM) $(NAME)
	@$(RM) .vscode
	@$(RM) webserv.dSYM
	@$(RM) .DS_Store

re: fclean all