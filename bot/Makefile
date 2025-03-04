NAME = ircbot

SRC = \
	class/Curl.cpp \
	class/JSON.cpp \
	class/Ollama.cpp \
	main.cpp

SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
TEST_DIR = test

CURL_VERSION = 8.12.1
CURL_NAME = curl-$(CURL_VERSION)
CURL_TARBALL = $(CURL_NAME).tar.gz
CURL_URL = https://curl.se/download/$(CURL_TARBALL)
CURL_DIR = $(BUILD_DIR)/$(CURL_NAME)
CURL_LIB = $(CURL_DIR)/lib/.libs
CURL_INC = $(CURL_DIR)/include

CXX = c++
CXXFLAGS = -I$(INC_DIR) -I$(CURL_INC) -MD -Wall -Wextra -Werror -std=c++98 -g
LFLAGS = -L$(CURL_LIB) -lcurl

CURL = curl
MKDIR = mkdir -p
RM = rm -rf
TAR = tar

OBJ = $(addprefix $(BUILD_DIR)/, $(SRC:.cpp=.o))
DEP = $(OBJ:.o=.d)

ifeq ($(shell uname), Darwin)
CURL_CONFIG = --with-openssl=/opt/homebrew/opt/openssl --without-libpsl
else
CURL_CONFIG = --with-openssl --without-libpsl
endif

.DEFAULT_GOAL = all

$(BUILD_DIR)/$(CURL_TARBALL):
	$(MKDIR) $(@D)
	$(CURL) -L $(CURL_URL) -o $@

$(CURL_DIR) $(CURL_INC): $(BUILD_DIR)/$(CURL_TARBALL)
	$(TAR) -xzf $< -C $(BUILD_DIR)

$(CURL_DIR)/Makefile: Makefile | $(CURL_DIR)
	cd $(@D) && ./configure $(CURL_CONFIG)

$(CURL_LIB): $(CURL_DIR)/Makefile
	$(MAKE) -C $(CURL_DIR) --no-print-directory

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp Makefile | $(CURL_INC)
	$(MKDIR) $(@D)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(BUILD_DIR)/$(TEST_DIR)/%.o: $(TEST_DIR)/%.cpp Makefile
	$(MKDIR) $(@D)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(NAME): $(OBJ) $(CURL_LIB)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) $(LFLAGS)

json_test: $(BUILD_DIR)/class/JSON.o $(BUILD_DIR)/$(TEST_DIR)/json.o
	$(CXX) $(CXXFLAGS) -o $@ $^

ollama-cli_test: $(BUILD_DIR)/class/Curl.o $(BUILD_DIR)/class/JSON.o $(BUILD_DIR)/class/Ollama.o $(BUILD_DIR)/$(TEST_DIR)/ollama-cli.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LFLAGS)

trivia_test: $(BUILD_DIR)/class/Curl.o $(BUILD_DIR)/class/JSON.o $(BUILD_DIR)/$(TEST_DIR)/trivia.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LFLAGS)

-include $(DEP)

.PHONY: all clean fclean re

all: $(NAME)

clean:
	$(RM) $(BUILD_DIR)

fclean:
	$(RM) $(BUILD_DIR) $(NAME) json_test ollama-cli_test trivia_test

re: fclean all
