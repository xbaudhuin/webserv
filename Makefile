# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: xabaudhu <xabaudhu@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/04/19 15:55:54 by xabaudhu          #+#    #+#              #
#    Updated: 2024/06/14 15:36:27 by xabaudhu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


################################################################################
#                                 CONFIGURATION                                #
################################################################################

NAME					=	webserv

CXX						=	c++

CXXFLAGS			=	-Wall -Werror -Wextra -MMD -MP -std=c++98 -g3

debug 				= 	0

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
					 SubServ.hpp \
					 sockets.hpp \

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
					error_logs/logs.cpp \

OBJS			=	$(addprefix ${OBJ_PATH}, ${SRCS_MAIN:.cpp=.o}) \

################################################################################
#                                    TESTS                                     #
################################################################################

SRCS_TESTS		=	main/unitTests.cpp \
					exec/SubServ.cpp \
					sockets/createServerSocket.cpp \

OBJS_TESTS		=	$(addprefix ${OBJ_PATH}, ${SRCS_TESTS:.cpp=.o}) \

################################################################################
#                                 RULES                                        #
################################################################################
					
all:			${NAME}

${NAME}:		${OBJS} ${TXT} Makefile
		@${CXX} ${CXXFLAGS} -o ${NAME} ${OBJS} ${HEADER}
		@printf "${NEW}${YELLOW}${NAME}${RESET}${GREEN}${BOLD} Compiled\n${RESET}${GREEN}compiled with:${RESET} ${CXX} ${CXXFLAGS}\n"

${OBJ_PATH}%.o:	${SRC_PATH}%.cpp
		@mkdir -p $(dir $@)
		@${CXX} ${CXXFLAGS} ${HEADER} -c $< -o $@
		@printf "${NEW}${YELLOW} ${NAME} ${GREEN}Building: ${RESET}${CXX} ${CXXFLAGS} ${ITALIC}${BOLD}$<${RESET}"

txt:			${TXT}

compile:	${OBJS}
		@printf "\n"

${TXT}:
		@echo "-Iinclude/\n-Wall -Werror -Wextra -std=c++98" > compile_flags.txt

test:		${OBJS_TESTS} ${TXT} Makefile
		@${CXX} ${CXXFLAGS} -o ${NAME} ${OBJS_TESTS} ${HEADER}
		@printf "${NEW}${YELLOW}${NAME}${RESET}${GREEN}${BOLD} Compiled\n${RESET}${GREEN}compiled with:${RESET} ${CXX} ${CXXFLAGS}\n"

clean:	
		${RM}  ${OBJ_PATH}

fclean:		clean
		${RM} ${NAME} ${SRC_PATH}${TXT} ${TXT}

re:			fclean all

.PHONY:		all fclean clean re txt compile
