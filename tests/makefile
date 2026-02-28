CC = g++
GCC = gcc
CPPUTEST_HOME = /usr/share/cpputest

CFLAGS   = -Wall -g -I../include -I./ -I../
CPPFLAGS = -I$(CPPUTEST_HOME)/include 

LDLIBS   = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt

# Build directory
BUILD_DIR = build
# Test sources (C++)
TEST_SRCS = AllTests.cpp decoderTests.cpp
# Production sources (C)
SRC_SRCS  = ../src/video.c

# Objects
TEST_OBJS = $(TEST_SRCS:%.cpp=build/%.o)
SRC_OBJS  = $(SRC_SRCS:../src/%.c=build/%.o)

OBJS = $(TEST_OBJS) $(SRC_OBJS)
TARGET_NAME = AllTests
TARGET = $(BUILD_DIR)/$(TARGET_NAME)

.PHONY: all clean run

all: $(TARGET)

# Link final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

# Compile .cpp into build/
$(BUILD_DIR)/%.o: %.cpp | build
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Compile .c into build/
$(BUILD_DIR)/%.o: ../src/%.c | build
	$(GCC) $(CFLAGS) -c $< -o $@

# Ensure build dir exists
$(BUILD_DIR):
	mkdir -p build

# Clean build directory
clean:
	@echo "Cleaning..."
	@rm -rf $(BUILD_DIR)

run: 
	make all && cd $(BUILD_DIR)/ && ./$(TARGET_NAME)

