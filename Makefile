# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: zkerriga <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/01/19 09:38:21 by zkerriga          #+#    #+#              #
#    Updated: 2021/01/20 10:42:50 by matrus           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

OBJ_DIR = bin
SRC_DIR = src

CC = clang++
DIRECTORIES = interfaces loggers models types command parsing tools configuration
BIN_DIRECTORIES = $(addprefix $(OBJ_DIR)/, $(DIRECTORIES))
INCLUDES = $(addprefix $(SRC_DIR)/, $(DIRECTORIES)) $(SRC_DIR)
FLAGS = -Wall -Wextra -Werror -Wconversion -O2 $(addprefix -I./, $(INCLUDES))

FILES =
OBJECTS = $(addprefix $(OBJ_DIR)/, $(FILES:.cpp=.o))

.PHONY: all
all: $(BIN_DIRECTORIES) $(NAME)
	@echo "\n\033[32m[+] The $(NAME) assembled!\033[0m\n"

$(BIN_DIRECTORIES):
	mkdir -p $@

$(NAME): $(OBJECTS)
	$(CC) $(FLAGS) $(OBJECTS) -o $(NAME)

$(OBJECTS): $(OBJ_DIR)/%.o: %.cpp
	@$(CC) $(FLAGS) -MMD -c $< -o $@
include $(wildcard $(OBJ_DIR)/*.d $(OBJ_DIR)/*/*.d $(OBJ_DIR)/*/*/*.d $(OBJ_DIR)/*/*/*/*.d)

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)

.PHONY: fclean
fclean: clean
	rm -f $(NAME)

.PHONY: re
re: fclean all
