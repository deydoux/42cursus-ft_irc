NAME = ircserv

SRC = \
	class/Channel.cpp \
	class/Client.cpp \
	class/Server.cpp \
	lib/log.cpp \
	lib/to_string.cpp \
	main.cpp

SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

CXX = c++
CXXFLAGS = -I$(INC_DIR) -MD -Wall -Wextra -Werror -std=c++98 -g

RM = rm -rf
MKDIR = mkdir -p

OBJ = $(addprefix $(BUILD_DIR)/, $(SRC:.cpp=.o))
DEP = $(OBJ:.o=.d)

.DEFAULT_GOAL = all

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(MKDIR) $(@D)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

-include $(DEP)

.PHONY: all clean fclean re

ARGS = -v 6697 slay
run: $(NAME)
	./$< $(ARGS)

all: $(NAME)

clean:
	$(RM) $(BUILD_DIR)

fclean:
	$(RM) $(BUILD_DIR) $(NAME)

re: fclean
	@$(MAKE)
