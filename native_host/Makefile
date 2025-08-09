CXX := g++
CXXFLAGS := -std=gnu++17 -O2 -Wall -Wextra -Wpedantic
INC := -I../self_learning_agent/src
SRC := host.cpp
AGENT_SRC := \
  ../self_learning_agent/src/agent.cpp \
  ../self_learning_agent/src/strategies.cpp \
  ../self_learning_agent/src/http.cpp \
  ../self_learning_agent/src/yaml_min.cpp
TARGET := bin/self_learning_host

.PHONY: all clean dirs

all: dirs $(TARGET)

$(TARGET): $(SRC) $(AGENT_SRC)
	$(CXX) $(CXXFLAGS) $(INC) $^ -o $@

dirs:
	mkdir -p bin

clean:
	rm -rf bin