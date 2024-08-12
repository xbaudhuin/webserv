# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: xabaudhu <xabaudhu@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/04/19 15:55:54 by xabaudhu          #+#    #+#              #
#    Updated: 2024/07/24 14:38:04 by xabaudhu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


################################################################################
#                                 CONFIGURATION                                #
################################################################################

NAME					=	webserv

CXX						=	c++

CXXFLAGS			=	-Wall -Werror -Wextra -MMD -MP -std=c++98

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

SPAM			=	spam.exe

TXT						=	compile_flags.txt

HEADER				=	-I./include/headers_hpp/ \
								
DEP_PATH			=	dep/

HEADER_FILES	=	 Webserv.hpp \
					 ServerConf.hpp \
					 Error.hpp \
					 Port.hpp \
					 Typedef.hpp \
					 Utils.hpp \
					 Client.hpp \
					 Response.hpp \
					 Port.hpp \
					 sockets.hpp \
					 Colors.hpp \
					 security_error.hpp \
					 cgiException.hpp \
					 bad_key_error.hpp \

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

LOG 					= log/

RM						=	rm -rf

AR						=	ar rcs

################################################################################
#                                    SOURCES                                   #
################################################################################

SRCS_MAIN		=	main/main.cpp \
					main/Webserv.cpp \
					signals/handleSignal.cpp \
					signals/getExitStatus.cpp \
					exec/Port.cpp \
					parser/Split.cpp \
					parser/Parser.cpp \
					parser/ServerConf.cpp \
					parser/ServerConfGetter.cpp \
					parser/Location.cpp \
					parser/LocationChecker.cpp \
					parser/LocationGetter.cpp \
					parser/GetPreciseLocation.cpp \
					parser/UtilsParser.cpp \
					parser/ParserLocation.cpp \
					parser/Printer.cpp \
					sockets/createServerSocket.cpp \
					sockets/addSocketToEpoll.cpp \
					sockets/protectedClose.cpp \
					sockets/changeEpollEvents.cpp \
					sockets/checkEvent.cpp \
					error_logs/logs.cpp \
					error_logs/Security_error.cpp \
					error_logs/cgiException.cpp \
					error_logs/bad_key_error.cpp \
					error_pages/singleton.cpp \
					client/Client.cpp \
					client/ClientConst.cpp \
					client/ClientResponse.cpp \
					client/Response.cpp \
					client/ClientParseRequest.cpp \
					client/ClientCgi.cpp \
					client/ClientMultipart.cpp \

SRCS_SPAM 		= 	custom_client/client_spam.cpp \

OBJS_SPAM 		= 	$(addprefix ${OBJ_PATH}, ${SRCS_SPAM:.cpp=.o}) \

OBJS			=	$(addprefix ${OBJ_PATH}, ${SRCS_MAIN:.cpp=.o}) \

################################################################################
#                                 RULES                                        #
################################################################################
					
all:			${NAME}

${NAME}:		${OBJS} ${TXT} Makefile ${LOG} 
		@${CXX} ${CXXFLAGS} -o ${NAME} ${OBJS} ${HEADER}
		@printf "${NEW}${YELLOW}${NAME}${RESET}${GREEN}${BOLD} Compiled\n${RESET}${GREEN}compiled with:${RESET} ${CXX} ${CXXFLAGS}\n"

${LOG}:
		mkdir -p ${LOG}

${OBJ_PATH}%.o:	${SRC_PATH}%.cpp
		@mkdir -p $(dir $@)
		@${CXX} ${CXXFLAGS} ${HEADER} -c $< -o $@
		@printf "${NEW}${YELLOW} ${NAME} ${GREEN}Building: ${RESET}${CXX} ${CXXFLAGS} ${ITALIC}${BOLD}$<${RESET}"

${SPAM}: Makefile ${OBJS_SPAM} ${TXT}
		@${CXX} ${CXXFLAGS} -o ${SPAM} ${OBJS_SPAM}
		@printf "${NEW}${YELLOW}${SPAM}${RESET}${GREEN}${BOLD} Compiled\n${RESET}${GREEN}compiled with:${RESET} ${CXX} ${CXXFLAGS}\n"

txt:			${TXT}

compile:	${OBJS}
		@printf "\n"

spam: ${SPAM}
		@printf "${GREEN}Client spam compiled${RESET}\n"
${TXT}:
		@echo "-Iinclude/headers_hpp\n-I/include/headers_h\n-Wall -Werror -Wextra -std=c++98" > compile_flags.txt

clean:	
		${RM}	${OBJ_PATH}
		${RM}	./log/*
		${RM}	html/upload/*

fclean:		clean
		${RM} ${NAME} ${SPAM} ${SRC_PATH}${TXT} ${TXT}

re:			fclean all

.PHONY:		all fclean clean re txt compile
