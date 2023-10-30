TOOLS = ./srcs/tools

SRCS_TOOLS = icmpByCodeTools.c icmpTools.c tools.c

SRCS = icmpByCode.c icmpRequest.c icmpResponse.c

SRCS_MAIN = main.c

NAME = ft_ping

FLAGS = -Wall -Wextra -Werror

INCLUDES = -Iincludes -Isrcs/libft

O_DIR = objs

OBJS_T =	$(addprefix $(O_DIR)/, $(SRCS_TOOLS:.c=.o))

OBJS_SRCS = $(addprefix $(O_DIR)/, $(SRCS:.c=.o))

OBJS_MAIN = $(addprefix $(O_DIR)/, $(SRCS_MAIN:.c=.o))

$(O_DIR)/%.o: $(TOOLS)/%.c
		@mkdir -p $(O_DIR)
		gcc $(FLAGS) $(INCLUDES) -c $< -o $@

$(O_DIR)/%.o: ./srcs/%.c
		@mkdir -p $(O_DIR)
		gcc $(FLAGS) $(INCLUDES) -c $< -o $@

$(O_DIR)/%.o: %.c
		@mkdir -p $(O_DIR)
		gcc $(FLAGS) $(INCLUDES) -c $< -o $@

$(NAME):	$(OBJS_T) $(OBJS_SRCS) $(OBJS_MAIN)
	make all -C ./srcs/libft
	gcc $(FLAGS) $(INCLUDES) -o $(NAME) \
		$(OBJS_T) $(OBJS_SRCS) $(OBJS_MAIN) \
		./srcs/libft/libft.a

all: $(NAME)

clean:
	make clean -C ./srcs/libft
	rm -f $(OBJS_T) $(OBJS_SRCS) $(OBJS_MAIN)
	rm -rf $(O_DIR)

fclean:
	make fclean -C ./srcs/libft
	make clean
	rm -f $(NAME)

re:
	make fclean
	make all

.PHONY: all clean fclean re