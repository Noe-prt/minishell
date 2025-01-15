# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nopareti <nopareti@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/01/14 04:20:50 by nopareti          #+#    #+#              #
#    Updated: 2025/01/14 04:20:50 by nopareti         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

# Compilateur et flags
CC = cc
CFLAGS = -Wall -Wextra -Werror -g
LDFLAGS = -lreadline

# Chemins
SRC_DIR = src
INC_DIR = include
LIBFT_DIR = Libft

# Sources
SRC_FILES = main.c init.c free.c utils.c debug.c parsing/parsing.c exec/exec.c
SRCS = $(addprefix $(SRC_DIR)/, $(SRC_FILES))
OBJS = $(SRCS:.c=.o)

# Libft
LIBFT = $(LIBFT_DIR)/libft.a

# Headers
INCLUDES = -I$(INC_DIR) -I$(LIBFT_DIR)

all: $(LIBFT) $(NAME)

$(LIBFT):
	@make -C $(LIBFT_DIR)

$(NAME): $(OBJS)
	$(CC) $(OBJS) -L$(LIBFT_DIR) -lft $(LDFLAGS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@make -C $(LIBFT_DIR) clean
	rm -f $(OBJS)

fclean: clean
	@make -C $(LIBFT_DIR) fclean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
