# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/01/19 09:38:21 by zkerriga          #+#    #+#              #
#    Updated: 2021/02/11 11:41:59 by matrus           ###   ########.fr        #
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
						ReplyList.cpp \
						Version.cpp \
						Squit.cpp \
						Nick.cpp \
						UserCmd.cpp

INTERFACES_DIR = interfaces
INTERFACES_FILES =		ACommand.cpp

SSL_DIR = 		ssl
SSL_FILES =		SSLConnection.cpp
SSL_LIBS =		mbedtls \
				mbedcrypto \
				mbedx509
SSL_LIB_INCLUDE_DIR = ./mbedtls/include/
SSL_LIB_DIR = ./mbedtls/library/

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
all: $(BIN_DIRECTORIES) mbedtls $(NAME)
	@echo
	@echo "\033[32m[+] The $(NAME) assembled!\033[0m\n"

$(BIN_DIRECTORIES):
	@mkdir -p $(BIN_DIRECTORIES)

$(NAME): $(OBJECTS)
	@$(CC) $(FLAGS) $(OBJECTS) $(addprefix -L./, $(LIBS_DIR)) $(addprefix -l, $(LIBS)) -o $(NAME)

$(OBJECTS): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@/bin/echo -n $<
	@$(CC) $(FLAGS) -MMD -c $< -o $@
	@/bin/bash -c "echo -en \"\033[2K\033[0G\""
include $(wildcard $(OBJ_DIR)/*.d $(OBJ_DIR)/*/*.d)

.PHONY: mbedtls
mbedtls:
	@/bin/echo "Assembling mbedtls library ..."
	@$(MAKE) no_test -C ./mbedtls --silent --no-print-directory --quiet -s
	@echo
	@echo "\033[32m[+] The mbedtls assembled!\033[0m\n"

.PHONY: clean
clean:
	@rm -rf $(OBJ_DIR)

.PHONY: fclean
fclean: clean
	@$(MAKE) -C ./mbedtls --silent clean
	@rm -f $(NAME)

.PHONY: re
re: fclean all

CERTIFICATE_DIR = certs
CERTIFICATE_CRT = localhost.crt
CERTIFICATE_KEY = localhost.key
CERTIFICATES = $(addprefix $(CERTIFICATE_DIR)/, $(CERTIFICATE_CRT) $(CERTIFICATE_KEY))

.PHONY: certs
certs: $(CERTIFICATES)
	@echo "\033[32m[+] Certificates were generated!\033[0m\n"

$(CERTIFICATES):
	mkdir -p $(CERTIFICATE_DIR)
	openssl req \
		-newkey rsa:2048 -nodes -keyout $(CERTIFICATE_DIR)/$(CERTIFICATE_KEY) \
		-x509 -days 365 -out $(CERTIFICATE_DIR)/$(CERTIFICATE_CRT) -subj \
		"/C=RU/ST=RT/L=Kazan/O=21 School/OU=students/emailAddress=matrus@student.21-school.ru/CN=localhost"

PYTHON_TEST_DIR = server_test

.PHONY: copy_certs_to_test
copy_certs_to_test: $(CERTIFICATES)
	cp -r $(CERTIFICATE_DIR) $(PYTHON_TEST_DIR)

.PHONY: clean_certs
clean_certs:
	rm -rf $(CERTIFICATE_DIR) $(PYTHON_TEST_DIR)/$(CERTIFICATE_DIR)
