# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/01/19 09:38:21 by zkerriga          #+#    #+#              #
#    Updated: 2021/01/30 23:24:37 by matrus           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

OBJ_DIR = bin
SRC_DIR = src

CC = clang++
DIRECTORIES = $(INTERFACES_DIR) $(LOGGERS_DIR) $(MODELS_DIR) $(TYPES_DIR) $(COMMANDS_DIR) $(PARSING_DIR) $(TOOLS_DIR) $(CONFIGURATION_DIR)
BIN_DIRECTORIES = $(addprefix $(OBJ_DIR)/, $(DIRECTORIES))
INCLUDES = $(addprefix $(SRC_DIR)/, $(DIRECTORIES)) $(SRC_DIR)
FLAGS = -Wall -Wextra -Werror -Wconversion -Wno-unused-private-field -Wno-unused-parameter -O2 $(addprefix -I./, $(INCLUDES))
# todo: remove extra flags

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
		$(addprefix $(INTERFACES_DIR)/, $(INTERFACES_FILES))

OBJECTS = $(addprefix $(OBJ_DIR)/, $(FILES:.cpp=.o))

.PHONY: all
all: $(BIN_DIRECTORIES) $(NAME)
	@echo
	@echo "\033[32m[+] The $(NAME) assembled!\033[0m\n"

$(BIN_DIRECTORIES):
	@mkdir -p $(BIN_DIRECTORIES)

$(NAME): $(OBJECTS)
	@$(CC) $(FLAGS) $(OBJECTS) -o $(NAME)

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
