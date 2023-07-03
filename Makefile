CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98
LDFLAGS =

SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

TARGET = webserv

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
DEPS = $(wildcard $(INC_DIR)/*.hpp)

all: $(BIN_DIR)/$(TARGET)

$(BIN_DIR)/$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPS)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

fclean: clean
	rm -f $(BIN_DIR)/$(TARGET)

re: fclean all

.PHONY: all clean fclean re
