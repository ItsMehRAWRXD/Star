CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g
INCLUDES = -I.
LIBS = -lpthread

# Source files
BRANCH_PLUGIN_SOURCES = branch_style_plugins.cpp
DEMO_SOURCES = branch_style_demo.cpp
ENHANCED_GENERATOR_SOURCES = enhanced_stub_generator_with_branches.cpp
NATIVE_TETRIS_SOURCES = native_branch_tetris.cpp
BUSINESS_COPILOT_SOURCES = business_copilot.cpp
PURE_COPILOT_SOURCES = pure_business_copilot.cpp

# Object files
BRANCH_PLUGIN_OBJECTS = $(BRANCH_PLUGIN_SOURCES:.cpp=.o)
DEMO_OBJECTS = $(DEMO_SOURCES:.cpp=.o)
ENHANCED_GENERATOR_OBJECTS = $(ENHANCED_GENERATOR_SOURCES:.cpp=.o)
NATIVE_TETRIS_OBJECTS = $(NATIVE_TETRIS_SOURCES:.cpp=.o)
BUSINESS_COPILOT_OBJECTS = $(BUSINESS_COPILOT_SOURCES:.cpp=.o)
PURE_COPILOT_OBJECTS = $(PURE_COPILOT_SOURCES:.cpp=.o)

# Executable names
DEMO_EXEC = branch_style_demo
ENHANCED_GENERATOR_EXEC = enhanced_stub_generator_with_branches
NATIVE_TETRIS_EXEC = native_branch_tetris
BUSINESS_COPILOT_EXEC = business_copilot
PURE_COPILOT_EXEC = pure_business_copilot

# Default target
all: $(PURE_COPILOT_EXEC)

# Build the demo executable
$(DEMO_EXEC): $(BRANCH_PLUGIN_OBJECTS) $(DEMO_OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

# Build the enhanced generator executable
$(ENHANCED_GENERATOR_EXEC): $(BRANCH_PLUGIN_OBJECTS) $(ENHANCED_GENERATOR_OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

# Build the native tetris executable
$(NATIVE_TETRIS_EXEC): $(NATIVE_TETRIS_OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

# Build the business copilot executable
$(BUSINESS_COPILOT_EXEC): $(BUSINESS_COPILOT_OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Run the business copilot
business: $(BUSINESS_COPILOT_EXEC)
	./$(BUSINESS_COPILOT_EXEC)

# Run the native tetris
tetris: $(NATIVE_TETRIS_EXEC)
	./$(NATIVE_TETRIS_EXEC)

# Run the demo
demo: $(DEMO_EXEC)
	./$(DEMO_EXEC)

# Run the enhanced generator demo
generator-demo: $(ENHANCED_GENERATOR_EXEC)
	./$(ENHANCED_GENERATOR_EXEC) demo

# Generate a basic stub
generate: $(ENHANCED_GENERATOR_EXEC)
	./$(ENHANCED_GENERATOR_EXEC) generate

# Generate a complex stub
complex: $(ENHANCED_GENERATOR_EXEC)
	./$(ENHANCED_GENERATOR_EXEC) complex

# Test all functionality
test: business tetris demo

# Clean build artifacts
clean:
	rm -f *.o $(DEMO_EXEC) $(ENHANCED_GENERATOR_EXEC) $(NATIVE_TETRIS_EXEC) $(BUSINESS_COPILOT_EXEC) $(PURE_COPILOT_EXEC)
	rm -f enhanced_stub_with_branches.cpp complex_mixed_stub.cpp

# Help target
help:
	@echo "Available targets:"
	@echo "  all           - Build all executables"
	@echo "  business      - Run the business copilot"
	@echo "  tetris        - Run the native tetris system"
	@echo "  demo          - Run the branch style demo"
	@echo "  test          - Run all tests"
	@echo "  clean         - Remove build artifacts"
	@echo "  help          - Show this help message"

# Declare phony targets
.PHONY: all business tetris demo test clean help