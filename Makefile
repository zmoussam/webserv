# Colors for terminal output
GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
NC = \033[0m # No Color

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g
LDFLAGS =

SRC_DIR = src
INC_DIR = inc
OBJ_DIR = obj
BIN_DIR = bin

TARGET = webserv

SRCS := $(shell find $(SRC_DIR) -name "*.cpp")
OBJS := $(addprefix $(OBJ_DIR)/,$(patsubst $(SRC_DIR)/%,%,$(SRCS:.cpp=.o)))

all: $(TARGET)

# Check if the target file exists before linking
$(TARGET): $(OBJS)

	@mkdir -p $(BIN_DIR)
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $(BIN_DIR)/$@ $^
	@printf "$(GREEN)[SUCCESS]$(NC) Compiled $@\n"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)/$(*D)
	@$(CC) $(CFLAGS) -I$(INC_DIR) -c -o $@ $<
	@printf "$(GREEN)[OK]$(NC) Compiled $<\n"

clean:
	@rm -rf $(OBJ_DIR)
	@printf "$(RED)[CLEAN]$(NC) Removed object files\n"

fclean: clean
	@rm -rf $(BIN_DIR)
	@printf "$(RED)[FCLEAN]$(NC) Removed binaries\n"

re: fclean all

.PHONY: all clean fclean re
