TARGET_EXEC  := Arjin.exe
BUILD_DIR    := build
SRC_DIR      := src
INC_DIRS     := include deps/jsoncpp deps/sfml/include
SFML_LIB_DIR := deps/sfml/lib
SFML_LIBS    := sfml-system sfml-window sfml-graphics

SOURCES      := $(wildcard $(SRC_DIR)/*.cpp)  deps/jsoncpp/jsoncpp.cpp
OBJECTS      := $(SOURCES:%=$(BUILD_DIR)/%.o)

CFLAGS       := $(addprefix -I,$(INC_DIRS)) -Wall -Werror -std=c++17
LDFLAGS      := -L$(SFML_LIB_DIR) $(addprefix -l,$(SFML_LIBS))

$(BUILD_DIR)/$(TARGET_EXEC): $(SOURCES)
	g++ $(SOURCES) -o $@ $(CFLAGS) $(LDFLAGS)




