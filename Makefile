# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/01/19 09:38:21 by zkerriga          #+#    #+#              #
#    Updated: 2021/02/18 13:08:30 by matrus           ###   ########.fr        #
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
FLAGS = -Wall -Wextra -Werror -Wconversion -Wno-unused-private-field -Wno-unused-parameter -Wno-unused-variable -O2 $(addprefix -I./, $(INCLUDES))
# todo: remove extra flags

LOGGERS_DIR = loggers
LOGGERS_FILES =			BigLogger.cpp \
						NickLogger.cpp \
						RxTxLogger.cpp \
						ServerLogger.cpp \
						ServerSelfInfo.cpp

MODELS_DIR = models
MODELS_FILES =			ServerInfo.cpp \
						StandardChannel.cpp \
						User.cpp \
						Modes.cpp \
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
						UserCmd.cpp \
						Join.cpp \
						Oper.cpp \
						Time.cpp \
						Info.cpp \
						Connect.cpp \
						Motd.cpp \
						Links.cpp \
						Quit.cpp \
						Mode.cpp

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


################
# CERTIFICATES #
################

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

.PHONY: certs_clean
certs_clean:
	rm -rf $(CERTIFICATE_DIR) $(PYTHON_TEST_DIR)/$(CERTIFICATE_DIR)


#############
# NET SETUP #
#############

NET_DIR = net_test
NET_SCRIPT = start.sh
CONFIG = $(NAME).conf

.PHONY: net
net: $(NAME) ircserv.conf
	@mkdir -p $(NET_DIR)/left $(NET_DIR)/center $(NET_DIR)/right

	cp $(NAME) $(NET_DIR)/left
	cp $(NAME) $(NET_DIR)/center
	cp $(NAME) $(NET_DIR)/right
	cp motd $(NET_DIR)/left
	cp motd $(NET_DIR)/center
	cp motd $(NET_DIR)/right

	sed -e "s/zkerriga.matrus.cgarth.com/left.net/; s/.\/certs\//..\/..\/certs\//g; s/;Port = 6697/Port = 6697/" $(CONFIG) > $(NET_DIR)/left/$(CONFIG)
	sed -e "s/zkerriga.matrus.cgarth.com/center.net/; s/.\/certs\//..\/..\/certs\//g; s/;Port = 6697/Port = 6698/" $(CONFIG) > $(NET_DIR)/center/$(CONFIG)
	sed -e "s/zkerriga.matrus.cgarth.com/right.net/; s/.\/certs\//..\/..\/certs\//g; s/;Port = 6697/Port = 6699/" $(CONFIG) > $(NET_DIR)/right/$(CONFIG)

	@printf '#!/bin/zsh\n\nfunction start_server() {\n  cd $$1\n  ./ircserv $$5 $$2 $$3 ' > $(NET_DIR)/$(NET_SCRIPT)
ifeq ($(shell uname),Linux)
#	@printf '| sed -ru "s/[\\x10-\\x1F]\[.{1,2}m//g"' >> $(NET_DIR)/$(NET_SCRIPT
else
#	@printf '| tr -u -d '\033' | sed -El -e "s/\[.{1,2}m//g"' >> $(NET_DIR)/$(NET_SCRIPT
endif
	@printf ' > server.log &\n  echo "[+] The $$4 server has pid =" $$!\n}\n\n' >> $(NET_DIR)/$(NET_SCRIPT)
	@printf 'start_server ./left 6667 pass left\n' >> $(NET_DIR)/$(NET_SCRIPT)
	@printf 'start_server ../center 6668 pass center 127.0.0.1:6667:pass\n' >> $(NET_DIR)/$(NET_SCRIPT)
	@printf 'start_server ../right 6669 pass right 127.0.0.1:6668:pass\n' >> $(NET_DIR)/$(NET_SCRIPT)
	@printf '\necho "[+] Deployment is complete!"\n' >> $(NET_DIR)/$(NET_SCRIPT)
	@chmod +x $(NET_DIR)/$(NET_SCRIPT)

.PHONY: net_clean
net_clean:
	rm -rf $(NET_DIR)

.PHONY: net_re
net_re: net_clean net

.PHONY: kill
kill:
	pkill ircserv &
	pkill ngircd &

TERMINAL = ./iterm.sh
CUR_DIR = $(shell pwd)
SLEEP = sleep 0.5

.PHONY: net_setup
.ONESHELL:
net_setup: kill net_re
	touch $(NET_DIR)/left/server.log $(NET_DIR)/center/server.log $(NET_DIR)/right/server.log
	$(TERMINAL) "cd $(CUR_DIR)/$(NET_DIR)/left; tail -f server.log"
	$(SLEEP)
	$(TERMINAL) "cd $(CUR_DIR)/$(NET_DIR)/center; tail -f server.log"
	$(SLEEP)
	$(TERMINAL) "cd $(CUR_DIR)/$(NET_DIR)/right; tail -f server.log"
	cd $(NET_DIR) && ./$(NET_SCRIPT)

.PHONY: net_big
net_big: $(NAME) ircserv.conf
	./$(PYTHON_TEST_DIR)/setup_ngircd.sh
	@mkdir -p $(NET_DIR)/irc1 $(NET_DIR)/irc2 $(NET_DIR)/irc3 $(NET_DIR)/irc5

	cp $(NAME) $(NET_DIR)/irc1
	cp $(NAME) $(NET_DIR)/irc2
	cp $(NAME) $(NET_DIR)/irc3
	cp $(NAME) $(NET_DIR)/irc5

	sed -e "s/zkerriga.matrus.cgarth.com/irc1.net/; s/.\/certs\//..\/..\/certs\//g; s/;Port = 6697/Port = 6697/" $(CONFIG) > $(NET_DIR)/irc1/$(CONFIG)
	sed -e "s/zkerriga.matrus.cgarth.com/irc2.net/; s/.\/certs\//..\/..\/certs\//g; s/;Port = 6697/Port = 6698/" $(CONFIG) > $(NET_DIR)/irc2/$(CONFIG)
	sed -e "s/zkerriga.matrus.cgarth.com/irc3.net/; s/.\/certs\//..\/..\/certs\//g; s/;Port = 6697/Port = 6699/" $(CONFIG) > $(NET_DIR)/irc3/$(CONFIG)
	sed -e "s/zkerriga.matrus.cgarth.com/irc5.net/; s/.\/certs\//..\/..\/certs\//g; s/;Port = 6697/Port = 6700/" $(CONFIG) > $(NET_DIR)/irc5/$(CONFIG)

	@printf '#!/bin/zsh\n\nfunction start_server() {\n  cd $$1\n  ./ircserv $$5 $$2 $$3 ' > $(NET_DIR)/$(NET_SCRIPT)
	@printf ' > server.log &\n  echo "[+] The $$4 server has pid =" $$!\n}\n\n' >> $(NET_DIR)/$(NET_SCRIPT)
	@printf 'start_server ./irc1 6667 pass irc1\n' >> $(NET_DIR)/$(NET_SCRIPT)
	@printf 'start_server ../irc2 6668 pass irc2 127.0.0.1:6667:pass\n' >> $(NET_DIR)/$(NET_SCRIPT)
	@printf 'start_server ../irc3 6669 pass irc3 127.0.0.1:6668:pass\n' >> $(NET_DIR)/$(NET_SCRIPT)
	@printf 'start_server ../irc5 6671 pass irc5 127.0.0.1:6670:pass\n' >> $(NET_DIR)/$(NET_SCRIPT)
	@printf '\necho "[+] Deployment is complete!"\n' >> $(NET_DIR)/$(NET_SCRIPT)
	@chmod +x $(NET_DIR)/$(NET_SCRIPT)

.PHONY: net_big_re
net_big_re: net_clean net_big

.PHONY: net_big_setup
.ONESHELL:
net_big_setup: kill net_big_re
	touch $(NET_DIR)/irc1/server.log $(NET_DIR)/irc2/server.log $(NET_DIR)/irc3/server.log $(NET_DIR)/irc5/server.log
	$(TERMINAL) "cd $(CUR_DIR)/$(NET_DIR)/irc1; tail -f server.log | sed s/^/1\:\ /"
	$(SLEEP)
	$(TERMINAL) "cd $(CUR_DIR)/$(NET_DIR)/irc2; tail -f server.log | sed s/^/2\:\ /"
	$(SLEEP)
	$(TERMINAL) "cd $(CUR_DIR)/$(NET_DIR)/irc3; tail -f server.log | sed s/^/3\:\ /"
	$(SLEEP)
	$(TERMINAL) "cd $(CUR_DIR)/$(NET_DIR)/irc5; tail -f server.log | sed s/^/5\:\ /"
	$(SLEEP)
	cd $(NET_DIR) && ./$(NET_SCRIPT)
	$(TERMINAL) "~/.brew/sbin/ngircd -n | sed s/^/4\:\ /"
