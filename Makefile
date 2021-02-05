# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/01/19 09:38:21 by zkerriga          #+#    #+#              #
#    Updated: 2021/02/05 12:34:08 by matrus           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

OBJ_DIR = bin
SRC_DIR = src

CC = clang++
DIRECTORIES = $(SSL_DIR) $(INTERFACES_DIR) $(LOGGERS_DIR) $(MODELS_DIR) $(TYPES_DIR) $(COMMANDS_DIR) $(PARSING_DIR) $(TOOLS_DIR) $(CONFIGURATION_DIR)
BIN_DIRECTORIES = $(addprefix $(OBJ_DIR)/, $(DIRECTORIES))
INCLUDES = $(addprefix $(SRC_DIR)/, $(DIRECTORIES)) $(SRC_DIR) $(SSL_LIB_INCLUDE_DIR)
LIBS = $(SSL_LIBS)
LIBS_DIR = $(SSL_LIB_DIR)
FLAGS = -Wall -Wextra -Werror -Wconversion -Wno-unused-private-field -Wno-unused-parameter -O2 $(addprefix -I./, $(INCLUDES))


# todo: remove extra flags
# todo: add ssl lib

LOGGERS_DIR = loggers
LOGGERS_FILES =			BigLogger.cpp \
						NickLogger.cpp \
						RxTxLogger.cpp \
						ServerLogger.cpp \
						ServerSelfInfo.cpp

MODELS_DIR = models
MODELS_FILES =			ServerInfo.cpp \
						Service.cpp \
						StandardChannel.cpp \
						User.cpp \
						UserMods.cpp \
						ChannelMods.cpp \
						UserChannelPrivileges.cpp \
						RequestForConnect.cpp

TYPES_DIR = types
TYPES_FILES =			Wildcard.cpp

TOOLS_DIR = tools
TOOLS_FILES =			tools.cpp

PARSING_DIR = parsing
PARSING_FILES =			Parser.cpp

CONFIGURATION_DIR = configuration
CONFIGURATION_FILES =	Configuration.cpp

COMMANDS_DIR = command
COMMANDS_FILES =		Pass.cpp \
						Error.cpp \
						Ping.cpp \
						Pong.cpp \
						ServerCmd.cpp \
						ReplyList.cpp

INTERFACES_DIR = interfaces
INTERFACES_FILES =		ACommand.cpp

SSL_DIR = 		ssl
SSL_FILES =		SSLConnection.cpp
SSL_LIBS =		mbedtls \
				mbedcrypto \
				mbedx509
SSL_LIB_INCLUDE_DIR = .
SSL_LIB_DIR = .

MAIN_DIR = .
MAIN_FILES =			main.cpp \
						Server.cpp


FILES =	$(addprefix $(MAIN_DIR)/, $(MAIN_FILES)) \
		$(addprefix $(LOGGERS_DIR)/, $(LOGGERS_FILES)) \
		$(addprefix $(MODELS_DIR)/, $(MODELS_FILES)) \
		$(addprefix $(TYPES_DIR)/, $(TYPES_FILES)) \
		$(addprefix $(TOOLS_DIR)/, $(TOOLS_FILES)) \
		$(addprefix $(PARSING_DIR)/, $(PARSING_FILES)) \
		$(addprefix $(CONFIGURATION_DIR)/, $(CONFIGURATION_FILES)) \
		$(addprefix $(COMMANDS_DIR)/, $(COMMANDS_FILES)) \
		$(addprefix $(INTERFACES_DIR)/, $(INTERFACES_FILES)) \
		$(addprefix $(SSL_DIR)/, $(SSL_FILES))

OBJECTS = $(addprefix $(OBJ_DIR)/, $(FILES:.cpp=.o))

.PHONY: all
all: $(BIN_DIRECTORIES) $(NAME)
	@echo
	@echo "\033[32m[+] The $(NAME) assembled!\033[0m\n"

$(BIN_DIRECTORIES):
	@mkdir -p $(BIN_DIRECTORIES)

$(NAME): $(OBJECTS)
	@$(CC) $(FLAGS) $(addprefix -L./, $(LIBS_DIR)) $(addprefix -l, $(LIBS)) $(OBJECTS) -o $(NAME)

$(OBJECTS): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@/bin/echo -n $^
	@$(CC) $(FLAGS) -MMD -c $< -o $@
	@/bin/bash -c "echo -en \"\033[2K\033[0G\""
include $(wildcard $(OBJ_DIR)/*.d $(OBJ_DIR)/*/*.d)

.PHONY: clean
clean:
	@rm -rf $(OBJ_DIR)

.PHONY: fclean
fclean: clean
	@rm -f $(NAME)

.PHONY: re
re: fclean all
