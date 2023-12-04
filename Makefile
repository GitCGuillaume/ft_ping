TOOLS = ./srcs/tools

BONUS_MA = ./bonus/subMakefile

SRCS_TOOLS = icmpByCodeTools.c icmpTools.c tools.c

SRCS = icmpByCode.c icmpRequest.c icmpResponse.c

SRCS_MAIN = main.c

NAME = ft_ping

GCC = gcc

FLAGS = -Wall -Wextra -Werror

INCLUDES = -Iincludes -Isrcs/libft #-g3 -fsanitize=address

O_DIR = objs

OBJS_T =	$(addprefix $(O_DIR)/, $(SRCS_TOOLS:.c=.o))

OBJS_SRCS = $(addprefix $(O_DIR)/, $(SRCS:.c=.o))

OBJS_MAIN = $(addprefix $(O_DIR)/, $(SRCS_MAIN:.c=.o))

$(O_DIR)/%.o: $(TOOLS)/%.c
		@mkdir -p $(O_DIR)
		$(GCC) $(FLAGS) $(INCLUDES) -c $< -o $@

$(O_DIR)/%.o: ./srcs/%.c
		@mkdir -p $(O_DIR)
		$(GCC) $(FLAGS) $(INCLUDES) -c $< -o $@

$(O_DIR)/%.o: %.c
		@mkdir -p $(O_DIR)
		$(GCC) $(FLAGS) $(INCLUDES) -c $< -o $@

$(NAME):	$(OBJS_T) $(OBJS_SRCS) $(OBJS_MAIN)
	make all -C ./srcs/libft
	$(GCC) $(FLAGS) $(INCLUDES) -o $(NAME) \
		$(OBJS_T) $(OBJS_SRCS) $(OBJS_MAIN) \
		./srcs/libft/libft.a

all: $(NAME)

ifneq ($(BONUS_MA), "")
include $(BONUS_MA)
endif

clean:
	make clean -C ./srcs/libft
	rm -f $(OBJS_T) $(OBJS_SRCS) $(OBJS_MAIN)
	rm -f $(OBJS_T_BONUS) $(OBJS_SRCS_BONUS) $(OBJS_MAIN_BONUS)
	rm -rf $(O_DIR)
	rm -rf $(O_DIR_BONUS)

fclean:
	make fclean -C ./srcs/libft
	make clean
	rm -f $(NAME)
	rm -f $(NAME_BONUS)

re:
	make fclean
	make all

.PHONY: all clean fclean re bonus