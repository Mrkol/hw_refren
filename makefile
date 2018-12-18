CXX = clang++-6.0
CXXFLAGS = -Wall -Wextra -Werror -ggdb -std=c++17

SRC_DIR = ./src
BIN_DIR = ./bin

REP_NAME = https://github.com/Mrkol/hw_refren
AUTOGEN_MSG = // You are NOT supposed to look at this automatically generated file!!!\n// Please, refer to this repository instead: $(REP_NAME).

SOURCES = $(SRC_DIR)/SuffixAutomaton.cpp $(SRC_DIR)/SuffixTree.cpp

main: init $(SRC_DIR)/main.cpp $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SRC_DIR)/main.cpp $(SOURCES) -o $(BIN_DIR)/main

test: init $(SRC_DIR)/test.cpp $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SRC_DIR)/test.cpp $(SOURCES) -o $(BIN_DIR)/test

singlefile: init
	echo "$(AUTOGEN_MSG)" > $(BIN_DIR)/main.cpp
	cat $(SRC_DIR)/SuffixAutomaton.hpp $(SRC_DIR)/SuffixAutomaton.cpp\
		$(SRC_DIR)/SuffixTree.hpp $(SRC_DIR)/SuffixTree.cpp\
		$(SRC_DIR)/RefrainCalculator.hpp\
		$(SRC_DIR)/main.cpp\
		>> $(BIN_DIR)/main.cpp

	sed -i '/#include "/d' $(BIN_DIR)/main.cpp
	sed -i '/#pragma/d' $(BIN_DIR)/main.cpp

init:
	mkdir -p $(BIN_DIR)