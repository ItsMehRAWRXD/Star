CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2
TARGET = task_completing_assistant

all: $(TARGET)

$(TARGET): task_completing_assistant.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)
	rm -rf completed_projects

run:
	./$(TARGET)

.PHONY: all clean run