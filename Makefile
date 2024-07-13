# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: xabaudhu <xabaudhu@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/04/19 15:55:54 by xabaudhu          #+#    #+#              #
#    Updated: 2024/07/12 14:26:24 by xabaudhu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


################################################################################
#                                 CONFIGURATION                                #
################################################################################

NAME					=	webserv

CXX						=	c++

CXXFLAGS			=	-Wall -Werror -Wextra -MMD -MP -std=c++98 -g3

debug 				= 	0

print				=	0

ifeq ($(debug), 1)
	CXX = g++
	CXXFLAGS += -g
endif

ifeq ($(debug), 2)
	CXX = g++
	CXXFLAGS += -g2
endif

ifeq ($(debug), 3)
	CXX = g++
	CXXFLAGS += -g3
endif

ifeq ($(debug), 4)
	CXX = g++
	CXXFLAGS += -g3 -fsanitize=address
endif

ifeq ($(print), 1)
	CXXFLAGS += -D PRINT=1
endif

ifeq ($(print), 2)
	CXXFLAGS += -D PRINT=2
endif

ifeq ($(print), 3)
	CXXFLAGS += -D PRINT=3
endif

TXT						=	compile_flags.txt

HEADER				=	-I./include/headers_hpp/ \
								
DEP_PATH			=	dep/


HEADER_FILES	=	 Config.hpp \
					 Webserv.hpp \
					 ServerConf.hpp \
					 Error.hpp \
					 SubServ.hpp \
					 Typedef.hpp \
					 Utils.hpp \
					 Client.hpp \
					 SubServ.hpp \
					 sockets.hpp 

GREEN					=	\033[0;32m
RED						=	\033[0;31m
BLUE					=	\033[0;34m
YELLOW    		=	\033[;33m
BWHITE    		=	\033[1;37m
RESET					=	\033[0m
ITALIC				=	\e[3m
BOLD					=	\e[1m
NEW						=	\r\033[K

SRC_PATH			=	./srcs/

OBJ_PATH			=	obj/

RM						=	rm -rf

AR						=	ar rcs

################################################################################
#                                    SOURCES                                   #
################################################################################

SRCS_MAIN		=	main/main.cpp \
					main/Webserv.cpp \
					main/handleSignal.cpp \
					exec/SubServ.cpp \
					parser/Config.cpp \
					parser/Split.cpp \
					parser/CreateSocket.cpp \
					parser/Parser.cpp \
					parser/ServerConf.cpp \
					parser/ServerConfGetter.cpp \
					parser/UtilsParser.cpp \
					parser/Location.cpp \
					parser/ParserLocation.cpp \
					parser/Printer.cpp \
					sockets/createServerSocket.cpp \
					sockets/addSocketToEpoll.cpp \
					sockets/protectedClose.cpp \
					sockets/changeEpollEvents.cpp \
					error_logs/logs.cpp \
					error_pages/singleton.cpp \
					request_parsing/Client.cpp \

REQUEST = requester

SRCS_REQUEST 	=		main/Webserv.cpp \
					exec/SubServ.cpp \
					parser/Config.cpp \
					parser/Split.cpp \
					parser/CreateSocket.cpp \
					parser/Parser.cpp \
					parser/ServerConf.cpp \
					parser/ServerConfGetter.cpp \
					parser/UtilsParser.cpp \
					parser/Location.cpp \
					parser/ParserLocation.cpp \
					parser/Printer.cpp \
					sockets/createServerSocket.cpp \
					sockets/addSocketToEpoll.cpp \
					sockets/protectedClose.cpp \
					sockets/changeEpollEvents.cpp \
					error_logs/logs.cpp \
					error_pages/singleton.cpp \
									request_parsing/Client.cpp \
									request_parsing/main.cpp 


OBJS			=	$(addprefix ${OBJ_PATH}, ${SRCS_MAIN:.cpp=.o}) \

OBJS_REQUEST	=	$(addprefix ${OBJ_PATH}, ${SRCS_REQUEST:.cpp=.o}) \

################################################################################
#                                    TESTS                                     #
################################################################################

SRCS_TESTS		=	main/unitTests.cpp \
					exec/SubServ.cpp \
					sockets/createServerSocket.cpp \
					main/Webserv.cpp \
					parser/Config.cpp \
					parser/Split.cpp \
					parser/CreateSocket.cpp \
					parser/Parser.cpp \
					parser/ServerConf.cpp \
					parser/ServerConfGetter.cpp \
					parser/UtilsParser.cpp \
					parser/Location.cpp \
					parser/ParserLocation.cpp \
					parser/Printer.cpp \
					error_logs/logs.cpp \

OBJS_TESTS		=	$(addprefix ${OBJ_PATH}, ${SRCS_TESTS:.cpp=.o}) \

################################################################################
#                                 RULES                                        #
################################################################################
					
all:			${NAME}

${NAME}:		${OBJS} ${TXT} Makefile
		@${CXX} ${CXXFLAGS} -o ${NAME} ${OBJS} ${HEADER}
		@printf "${NEW}${YELLOW}${NAME}${RESET}${GREEN}${BOLD} Compiled\n${RESET}${GREEN}compiled with:${RESET} ${CXX} ${CXXFLAGS}\n"

request: ${REQUEST}

${REQUEST}: ${OBJS_REQUEST} ${TXT} Makefile
		@${CXX} ${CXXFLAGS} -o ${REQUEST} ${OBJS_REQUEST} ${HEADER}
		@printf "${NEW}${YELLOW}${REQUEST}${RESET}${GREEN}${BOLD} Compiled\n${RESET}${GREEN}compiled with:${RESET} ${CXX} ${CXXFLAGS}\n"
		
		

${OBJ_PATH}%.o:	${SRC_PATH}%.cpp
		@mkdir -p $(dir $@)
		@${CXX} ${CXXFLAGS} ${HEADER} -c $< -o $@
		@printf "${NEW}${YELLOW} ${NAME} ${GREEN}Building: ${RESET}${CXX} ${CXXFLAGS} ${ITALIC}${BOLD}$<${RESET}"

txt:			${TXT}

compile:	${OBJS}
		@printf "\n"

${TXT}:
		@echo "-Iinclude/headers_hpp\n-I/include/headers_h\n-Wall -Werror -Wextra -std=c++98" > compile_flags.txt

test:		${OBJS_TESTS} ${TXT} Makefile
		@${CXX} ${CXXFLAGS} -o ${NAME} ${OBJS_TESTS} ${HEADER}
		@printf "${NEW}${YELLOW}${NAME}${RESET}${GREEN}${BOLD} Compiled\n${RESET}${GREEN}compiled with:${RESET} ${CXX} ${CXXFLAGS}\n"

clean:	
		${RM}  ${OBJ_PATH}

fclean:		clean
		${RM} ${NAME} ${REQUEST} ${SRC_PATH}${TXT} ${TXT}

re:			fclean all

.PHONY:		all fclean clean re txt compile
