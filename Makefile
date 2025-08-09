CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2
TARGET = simple_code_writer

all: $(TARGET)

simple_code_writer: simple_code_writer.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)

run:
	./simple_code_writer

.PHONY: all clean run