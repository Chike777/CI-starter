CC = g++
CFLAGS =  -Wall -Wextra -pthread
BUILD_DIR = build
TARGET = $(BUILD_DIR)/test
SRC_FILES = $(wildcard *.cpp)
OBJ_FILES = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES))

.PHONY: all test clean

all: $(BUILD_DIR) $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) $(CFLAGS) $< -lgtest -lgtest_main -lgmock -lgmock_main -pthread -o $@

$(BUILD_DIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

check: $(TARGET)
	./$<

clean:
	rm -rf $(BUILD_DIR)
	clear
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)