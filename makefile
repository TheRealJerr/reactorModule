CXX := g++
CXXFLAGS := -std=c++17 -Wall -g

SRC_DIR := src
OBJ_DIR := obj
TEST_DIR := test

# 获取所有源文件
SRC := $(wildcard $(SRC_DIR)/*.cpp)

# 生成目标文件列表（.cpp → .o）
OBJ := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))

# 默认目标
all: server client

# server 可执行文件
server: $(TEST_DIR)/server.cpp $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# client 可执行文件
client: $(TEST_DIR)/client.cpp $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 编译 .cpp → .o 的规则
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 创建 obj 目录（如果不存在）
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# 清理
clean:
	rm -rf $(OBJ_DIR) server client

.PHONY: all clean

