CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2 -pthread
TARGETS = autonomous_code_generator unlimited_generator

all: $(TARGETS)

autonomous_code_generator: autonomous_code_generator.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

unlimited_generator: unlimited_generator.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)
	rm -rf generated_code unlimited_output

run-simple:
	./autonomous_code_generator

run-unlimited:
	./unlimited_generator

.PHONY: all clean run-simple run-unlimited