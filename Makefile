NAME = ircserv
VERSION = $(shell git describe --tags --always --dirty=* --broken=#)

SRC = \
	class/Channel.cpp \
	class/Client.cpp \
	class/Command.cpp \
	class/Command/hk.cpp \
	class/Command/invite.cpp \
	class/Command/join.cpp \
	class/Command/kick.cpp \
	class/Command/mode.cpp \
	class/Command/motd.cpp \
	class/Command/names.cpp \
	class/Command/nick.cpp \
	class/Command/part.cpp \
	class/Command/pass.cpp \
	class/Command/ping.cpp \
	class/Command/privmsg.cpp \
	class/Command/quit.cpp \
	class/Command/topic.cpp \
	class/Command/user.cpp \
	class/Command/who.cpp \
	class/Server.cpp \
	lib/log.cpp \
	lib/match_mask.cpp \
	lib/split.cpp \
	lib/to_lower.cpp \
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

.PHONY: all bonus clean fclean re run

all: $(NAME)

bonus:
	@$(MAKE) -C bot

clean:
	$(RM) $(BUILD_DIR)

fclean:
	$(RM) $(BUILD_DIR) $(NAME)

re: fclean
	@$(MAKE)

run: $(NAME)
	./$< $(ARGS)
