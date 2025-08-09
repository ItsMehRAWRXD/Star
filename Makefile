CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g
INCLUDES = -I.
LIBS = -lpthread

# Source files
BRANCH_PLUGIN_SOURCES = branch_style_plugins.cpp
DEMO_SOURCES = branch_style_demo.cpp
ENHANCED_GENERATOR_SOURCES = enhanced_stub_generator_with_branches.cpp

# Object files
BRANCH_PLUGIN_OBJECTS = $(BRANCH_PLUGIN_SOURCES:.cpp=.o)
DEMO_OBJECTS = $(DEMO_SOURCES:.cpp=.o)
ENHANCED_GENERATOR_OBJECTS = $(ENHANCED_GENERATOR_SOURCES:.cpp=.o)

# Executable names
DEMO_EXEC = branch_style_demo
ENHANCED_GENERATOR_EXEC = enhanced_stub_generator_with_branches

# Default target
all: $(DEMO_EXEC) $(ENHANCED_GENERATOR_EXEC)

# Build the demo executable
$(DEMO_EXEC): $(BRANCH_PLUGIN_OBJECTS) $(DEMO_OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

# Build the enhanced generator executable
$(ENHANCED_GENERATOR_EXEC): $(BRANCH_PLUGIN_OBJECTS) $(ENHANCED_GENERATOR_OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

# Compile source files to object files
%.o: %.cpp branch_style_plugins.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

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

# Benchmark branch styles
benchmark: $(ENHANCED_GENERATOR_EXEC)
	./$(ENHANCED_GENERATOR_EXEC) benchmark

# Test all functionality
test: demo generator-demo benchmark

# Build with Windows support (cross-compile)
windows: CXXFLAGS += -DWIN32 -static-libgcc -static-libstdc++
windows: CXX = x86_64-w64-mingw32-g++
windows: $(DEMO_EXEC) $(ENHANCED_GENERATOR_EXEC)

# Debug build
debug: CXXFLAGS += -DDEBUG -g3 -O0
debug: $(DEMO_EXEC) $(ENHANCED_GENERATOR_EXEC)

# Release build with optimizations
release: CXXFLAGS += -DNDEBUG -O3 -flto -march=native
release: $(DEMO_EXEC) $(ENHANCED_GENERATOR_EXEC)

# Profiling build
profile: CXXFLAGS += -pg -O2
profile: $(DEMO_EXEC) $(ENHANCED_GENERATOR_EXEC)

# Static analysis
analyze:
	@echo "Running static analysis..."
	cppcheck --enable=all --std=c++17 $(BRANCH_PLUGIN_SOURCES) $(DEMO_SOURCES) $(ENHANCED_GENERATOR_SOURCES)

# Format code
format:
	@echo "Formatting code..."
	clang-format -i *.cpp *.h

# Clean build artifacts
clean:
	rm -f *.o $(DEMO_EXEC) $(ENHANCED_GENERATOR_EXEC)
	rm -f enhanced_stub_with_branches.cpp complex_mixed_stub.cpp
	rm -f gmon.out

# Clean all generated files
distclean: clean
	rm -f *.exe

# Documentation generation
docs:
	@echo "Generating documentation..."
	doxygen Doxyfile 2>/dev/null || echo "Doxygen not available"

# Installation (copy to system directory)
install: $(DEMO_EXEC) $(ENHANCED_GENERATOR_EXEC)
	@echo "Installing branch style plugins..."
	install -d /usr/local/bin
	install -m 755 $(DEMO_EXEC) /usr/local/bin/
	install -m 755 $(ENHANCED_GENERATOR_EXEC) /usr/local/bin/
	install -d /usr/local/include/branch_style_plugins
	install -m 644 branch_style_plugins.h /usr/local/include/branch_style_plugins/

# Create example usage
examples: $(ENHANCED_GENERATOR_EXEC)
	@echo "Creating example outputs..."
	./$(ENHANCED_GENERATOR_EXEC) generate
	./$(ENHANCED_GENERATOR_EXEC) complex
	@echo "Examples created: enhanced_stub_with_branches.cpp, complex_mixed_stub.cpp"

# Performance test
perf-test: $(ENHANCED_GENERATOR_EXEC)
	@echo "Running performance tests..."
	time ./$(ENHANCED_GENERATOR_EXEC) benchmark
	@echo "Performance test completed"

# Memory leak check (requires valgrind)
memcheck: $(DEMO_EXEC)
	@echo "Running memory leak check..."
	valgrind --leak-check=full --show-leak-kinds=all ./$(DEMO_EXEC) 2>/dev/null || echo "Valgrind not available"

# Code coverage (requires gcov)
coverage: CXXFLAGS += --coverage
coverage: $(DEMO_EXEC)
	@echo "Running code coverage..."
	./$(DEMO_EXEC)
	gcov $(BRANCH_PLUGIN_SOURCES) $(DEMO_SOURCES) 2>/dev/null || echo "gcov not available"

# Help target
help:
	@echo "Available targets:"
	@echo "  all           - Build all executables"
	@echo "  demo          - Run the branch style demo"
	@echo "  generator-demo- Run the enhanced generator demo"
	@echo "  generate      - Generate a basic obfuscated stub"
	@echo "  complex       - Generate a complex mixed-style stub"
	@echo "  benchmark     - Benchmark all branch styles"
	@echo "  test          - Run all tests"
	@echo "  windows       - Cross-compile for Windows"
	@echo "  debug         - Build with debug symbols"
	@echo "  release       - Build optimized release version"
	@echo "  profile       - Build with profiling support"
	@echo "  analyze       - Run static analysis"
	@echo "  format        - Format source code"
	@echo "  clean         - Remove build artifacts"
	@echo "  distclean     - Remove all generated files"
	@echo "  docs          - Generate documentation"
	@echo "  install       - Install to system directories"
	@echo "  examples      - Create example output files"
	@echo "  perf-test     - Run performance tests"
	@echo "  memcheck      - Check for memory leaks"
	@echo "  coverage      - Generate code coverage report"
	@echo "  help          - Show this help message"

# Declare phony targets
.PHONY: all demo generator-demo generate complex benchmark test windows debug release profile analyze format clean distclean docs install examples perf-test memcheck coverage help