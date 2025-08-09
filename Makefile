CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2
TARGET = code_assistant

all: $(TARGET)

$(TARGET): code_assistant.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)

run:
	./$(TARGET)

.PHONY: all clean run