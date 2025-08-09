CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2
TARGET = code_gen

all: $(TARGET)

$(TARGET): code_gen.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)
	rm -f keygen.cpp passgen.cpp portscan.cpp encrypt.cpp hexdump.cpp
	rm -f keygen passgen portscan encrypt hexdump

run:
	./$(TARGET)

.PHONY: all clean run