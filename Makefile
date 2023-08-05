CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 #-fsanitize=address -g
LDFLAGS =

SRC_DIR = src
INC_DIR = inc
OBJ_DIR = obj
BIN_DIR = bin

TARGET = webserv

SRCS := $(shell find $(SRC_DIR) -name "*.cpp")
OBJS := $(addprefix $(OBJ_DIR)/,$(patsubst $(SRC_DIR)/%,%,$(SRCS:.cpp=.o)))

DEPS := $(OBJS:.o=.d) # Dependency files

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BIN_DIR)/$@ $^

# Include the dependency files
-include $(DEPS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)/$(*D)
	$(CC) $(CFLAGS) -I$(INC_DIR) -MMD -c -o $@ $<

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -rf $(BIN_DIR)

re: fclean all

.PHONY: all clean fclean re
