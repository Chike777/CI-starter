CC = g++
CFLAGS = -Wall -Wextra -Wpedantic -std=c++17 -MMD

TEST_EXE = test
BUILD_DIR = build

# The generated files shall be in build directory
# clean: shall delete build
# The default target shall build the test
# check: shall run the test