# Makefile for Writing Make Files Example
 
# *****************************************************
# Variables to control Makefile operation
 
CXX = g++-11
CXXFLAGS  = -g -Wall -Wextra
CPPFLAGS := -Iinclude -MMD -MP 
LDFLAGS  := -L/usr/local/opt/llvm/lib -Wl,-rpath,/usr/local/opt/llvm/lib            
LDLIBS   := -lm   
SRC_DIR := src
BUILD_DIR := build2
OBJ_DIR := $(BUILD_DIR)/obj

TARGET = $(BUILD_DIR)/chip8
SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
.PHONEY: all clean
all: $(TARGET) 

$(TARGET): $(OBJ) | $(BUILD_DIR)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	$(RM) -rv $(BUILD_DIR)

-include $(OBJ:.o=.d)

# CXX = clang++ -std=c++17
# CXXFLAGS  = -g -Wall 
# CPPFLAGS := -Iinclude -MMD -MP
# LDFLAGS  := -L/usr/local/opt/llvm/lib -Wl,-rpath,/usr/local/opt/llvm/lib            
# LDLIBS   := -lm   
# SRC_DIR := src
# BUILD_DIR := build3
# OBJ_DIR := $(BUILD_DIR)/obj

# TARGET = $(BUILD_DIR)/main
# SRC := $(wildcard $(SRC_DIR)/*.cpp)
# OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
# .PHONEY: all clean
# all: $(TARGET) 

# $(TARGET): $(OBJ) | $(BUILD_DIR)
# 	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

# $(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
# 	$(CXX) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# $(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
# 	$(CXX) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# $(BUILD_DIR) $(OBJ_DIR):
# 	mkdir -p $@

# clean:
# 	$(RM) -rv $(BUILD_DIR)

# -include $(OBJ:.o=.d)