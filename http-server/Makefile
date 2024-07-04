.PHONY: all setup clean

CL := clang++
CXXFLAGS := -Werror -O2 -std=c++20
SRCS 		:= $(wildcard *.cpp)
# strip .cpp from the src files
BARE_SRC := $(SRCS:%.cpp=%)
INC_DIR	:= ./inc
BIN_DIR 	:= ./builds
BIN  		:= $(BIN_DIR)/server

all: setup $(BIN)

setup:
	mkdir -p $(BIN_DIR)

$(BIN): $(SRCS:%.cpp=$(BIN_DIR)/%.o)
	$(CL) -o $(BIN) $<

$(BIN_DIR)/%.o: %.cpp
	${CL} ${CXXFLAGS} -include $(INC_DIR)/$(<:%.cpp=%.h) -c $< -o $@

clean:
	rm -rf $(BIN_DIR)
