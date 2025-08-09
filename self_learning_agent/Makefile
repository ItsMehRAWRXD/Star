CXX := g++
CXXFLAGS := -std=gnu++17 -O2 -Wall -Wextra -Wpedantic
LDFLAGS := 
PREFIX := bin
TARGET := $(PREFIX)/agent
SRC := src/main.cpp src/agent.cpp src/strategies.cpp src/http.cpp src/yaml_min.cpp
INC := -Isrc

.PHONY: all clean dirs

all: dirs $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(INC) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(PREFIX)

dirs:
	mkdir -p $(PREFIX)
	mkdir -p src