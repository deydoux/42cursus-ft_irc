NAME = ircserv
VERSION = $(shell git describe --tags --always --dirty=* --broken=#)

SRC = \
	class/Channel.cpp \
	class/Client.cpp \
	class/Command.cpp \
	class/Server.cpp \
	lib/log.cpp \
	lib/to_lower.cpp \
	lib/split.cpp \
	lib/match_mask.cpp \
	lib/unix_timestamp.cpp \
	main.cpp

SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

CXX = c++
CXXFLAGS = -D 'VERSION="ft_irc-$(VERSION)"' -I$(INC_DIR) -MD -Wall -Wextra -Werror -std=c++98 -g

RM = rm -rf
MKDIR = mkdir -p

OBJ = $(addprefix $(BUILD_DIR)/, $(SRC:.cpp=.o))
DEP = $(OBJ:.o=.d)

.DEFAULT_GOAL = all

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp Makefile
	@$(MKDIR) $(@D)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

-include $(DEP)

.PHONY: all clean fclean re run

all: $(NAME)

clean:
	$(RM) $(BUILD_DIR)

fclean:
	$(RM) $(BUILD_DIR) $(NAME)

re: fclean
	@$(MAKE)

run: $(NAME)
	./$< $(ARGS)
