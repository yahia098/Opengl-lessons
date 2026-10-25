CXX      := g++
CC       := gcc

TARGET   := build/main
BUILD    := build
SRC_DIR  := src
INC_DIRS := include
LIB_DIRS := lib

CXXFLAGS := -Wall -Wextra -g -O0
CFLAGS   := -Wall -Wextra -g -O0
CPPFLAGS := $(addprefix -I,$(INC_DIRS))
LDFLAGS  := $(addprefix -L,$(LIB_DIRS))
LDLIBS   := -lglfw -lGL

CPP_SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
C_SOURCES   := $(wildcard $(SRC_DIR)/*.c)

OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD)/%.o,$(CPP_SOURCES)) \
           $(patsubst $(SRC_DIR)/%.c,$(BUILD)/%.o,$(C_SOURCES))

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) $(LDLIBS) -o $@

$(BUILD)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(BUILD)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD)
