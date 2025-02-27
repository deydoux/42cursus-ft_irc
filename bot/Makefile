NAME = ircbot

SRC = \
	main.cpp

SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

CURL_VERSION = 8.12.1
CURL_NAME = curl-$(CURL_VERSION)
CURL_TARBALL = $(CURL_NAME).tar.gz
CURL_URL = https://curl.se/download/$(CURL_TARBALL)
CURL_DIR = $(BUILD_DIR)/$(CURL_NAME)
CURL_LIB = $(CURL_DIR)/lib/.libs/libcurl.a
CURL_INC = $(CURL_DIR)/include

CXX = c++
CXXFLAGS = -I$(INC_DIR) -I$(CURL_INC) -MD -Wall -Wextra -Werror -std=c++98 -g

CURL = curl
MKDIR = mkdir -p
RM = rm -rf
TAR = tar

OBJ = $(addprefix $(BUILD_DIR)/, $(SRC:.cpp=.o))
DEP = $(OBJ:.o=.d)

.DEFAULT_GOAL = all

$(NAME): $(OBJ) $(CURL_LIB)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp Makefile
	@$(MKDIR) $(@D)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

-include $(DEP)

$(BUILD_DIR)/$(CURL_TARBALL):
	@$(MKDIR) $(@D)
	$(CURL) -L $(CURL_URL) -o $@

$(CURL_DIR): $(BUILD_DIR)/$(CURL_TARBALL)
	$(TAR) -xzf $< -C $(BUILD_DIR)

$(CURL_DIR)/Makefile: $(CURL_DIR) Makefile
	cd $< && test -f Makefile || ./configure --with-openssl --without-libpsl

$(CURL_LIB): $(CURL_DIR)/Makefile
	$(MAKE) -C $(CURL_DIR)

.PHONY: all clean fclean re

all: $(NAME)

clean:
	$(RM) $(BUILD_DIR)

fclean:
	$(RM) $(BUILD_DIR) $(NAME)

re: fclean all
