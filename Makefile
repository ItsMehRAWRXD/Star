# Autonomous Unlimited Code Generator Makefile
# ============================================

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -pthread
INCLUDES = -I.
LDFLAGS = -pthread
TARGET = autocoder
SOURCES = autonomous_code_generator_cli.cpp autonomous_unlimited_code_generator.cpp

# Debug build
DEBUG_TARGET = autocoder_debug
DEBUG_CXXFLAGS = -std=c++17 -Wall -Wextra -g -O0 -pthread -DDEBUG

# Default target
all: $(TARGET)

# Release build
$(TARGET): $(SOURCES)
	@echo "Building Autonomous Unlimited Code Generator (Release)..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(SOURCES) $(LDFLAGS)
	@echo "Build complete! Run with: ./$(TARGET) --help"

# Debug build
debug: $(DEBUG_TARGET)

$(DEBUG_TARGET): $(SOURCES)
	@echo "Building Autonomous Unlimited Code Generator (Debug)..."
	$(CXX) $(DEBUG_CXXFLAGS) $(INCLUDES) -o $(DEBUG_TARGET) $(SOURCES) $(LDFLAGS)
	@echo "Debug build complete! Run with: ./$(DEBUG_TARGET) --help"

# Install target
install: $(TARGET)
	@echo "Installing autocoder to /usr/local/bin..."
	sudo cp $(TARGET) /usr/local/bin/
	sudo chmod +x /usr/local/bin/$(TARGET)
	@echo "Installation complete! You can now run 'autocoder' from anywhere."

# Clean target
clean:
	@echo "Cleaning build artifacts..."
	rm -f $(TARGET) $(DEBUG_TARGET) *.o
	rm -rf generated/
	@echo "Clean complete."

# Test target
test: $(TARGET)
	@echo "Running basic tests..."
	./$(TARGET) generate -l python -p api "Simple REST API for testing"
	./$(TARGET) generate -l javascript -p web "Basic web application"
	./$(TARGET) generate -l cpp -p cli "Command line utility"
	./$(TARGET) stats
	@echo "Test complete! Check ./generated/ for output."

# Example usage
examples: $(TARGET)
	@echo "Running example generations..."
	@mkdir -p examples
	
	# Python REST API
	./$(TARGET) generate -l python -p api -o examples/python_api "User management REST API with authentication"
	
	# JavaScript Web App
	./$(TARGET) generate -l javascript -p web -o examples/js_webapp "E-commerce web application with shopping cart"
	
	# C++ CLI Tool
	./$(TARGET) generate -l cpp -p cli -o examples/cpp_cli "File processing command line tool"
	
	# Go Microservice
	./$(TARGET) generate -l go -p microservice -o examples/go_microservice "User authentication microservice"
	
	# Rust Game Engine
	./$(TARGET) generate -l rust -p game -o examples/rust_game "2D platformer game engine"
	
	# Java Desktop App
	./$(TARGET) generate -l java -p desktop -o examples/java_desktop "Database management desktop application"
	
	@echo "Examples generated in ./examples/ directory"

# Interactive demo
demo: $(TARGET)
	@echo "Starting interactive demo..."
	@echo "This will launch the interactive mode for demonstration."
	./$(TARGET) interactive

# Batch processing example
batch-demo: $(TARGET)
	@echo "Creating batch file for demonstration..."
	@echo "# Batch processing example" > batch_demo.txt
	@echo "python:api:3:User management REST API" >> batch_demo.txt
	@echo "javascript:web:2:Simple blog website" >> batch_demo.txt
	@echo "cpp:cli:4:Advanced file compression tool" >> batch_demo.txt
	@echo "go:microservice:3:Order processing service" >> batch_demo.txt
	@echo "rust:game:5:3D graphics engine" >> batch_demo.txt
	
	@echo "Running batch processing..."
	./$(TARGET) batch batch_demo.txt
	@echo "Batch processing complete!"

# Project scaffolding demo
scaffold-demo: $(TARGET)
	@echo "Demonstrating project scaffolding..."
	./$(TARGET) scaffold -l javascript -p web -o scaffold_demo "Complete e-commerce platform"
	@echo "Project scaffolding complete! Check ./scaffold_demo/"

# Show help
help: $(TARGET)
	./$(TARGET) --help

# Development targets
format:
	@echo "Formatting code..."
	clang-format -i *.cpp *.hpp
	@echo "Code formatting complete."

lint:
	@echo "Running static analysis..."
	cppcheck --enable=all --std=c++17 *.cpp *.hpp
	@echo "Static analysis complete."

# Package for distribution
package: $(TARGET)
	@echo "Creating distribution package..."
	@mkdir -p dist/autocoder
	cp $(TARGET) dist/autocoder/
	cp README.md dist/autocoder/ 2>/dev/null || echo "# Autonomous Unlimited Code Generator" > dist/autocoder/README.md
	cp autonomous_unlimited_code_generator.hpp dist/autocoder/
	@echo "#!/bin/bash" > dist/autocoder/install.sh
	@echo "sudo cp autocoder /usr/local/bin/" >> dist/autocoder/install.sh
	@echo "sudo chmod +x /usr/local/bin/autocoder" >> dist/autocoder/install.sh
	@echo "echo 'Autocoder installed successfully!'" >> dist/autocoder/install.sh
	chmod +x dist/autocoder/install.sh
	cd dist && tar czf autocoder.tar.gz autocoder/
	@echo "Package created: dist/autocoder.tar.gz"

# Check dependencies
check-deps:
	@echo "Checking system dependencies..."
	@which g++ >/dev/null || (echo "ERROR: g++ not found. Please install g++." && exit 1)
	@echo "g++ found: $(shell g++ --version | head -1)"
	@echo "C++17 support: $(shell g++ -std=c++17 -x c++ -E - < /dev/null >/dev/null 2>&1 && echo 'OK' || echo 'MISSING')"
	@echo "Dependencies check complete."

# Benchmark performance
benchmark: $(TARGET)
	@echo "Running performance benchmarks..."
	@time ./$(TARGET) generate -l python -p api "Performance test API"
	@time ./$(TARGET) generate -l cpp -p cli "Performance test CLI"
	@time ./$(TARGET) generate -l javascript -p web "Performance test web app"
	@echo "Benchmark complete."

# Create sample batch file
create-batch-sample:
	@echo "Creating sample batch file..."
	@echo "# Sample batch file for Autonomous Code Generator" > sample_batch.txt
	@echo "# Format: language:pattern:complexity:description" >> sample_batch.txt
	@echo "" >> sample_batch.txt
	@echo "python:api:3:User authentication REST API" >> sample_batch.txt
	@echo "javascript:web:2:Simple portfolio website" >> sample_batch.txt
	@echo "cpp:cli:4:High-performance data processor" >> sample_batch.txt
	@echo "java:desktop:3:Customer management system" >> sample_batch.txt
	@echo "go:microservice:4:Payment processing service" >> sample_batch.txt
	@echo "rust:game:5:Real-time strategy game engine" >> sample_batch.txt
	@echo "" >> sample_batch.txt
	@echo "# Use with: ./autocoder batch sample_batch.txt" >> sample_batch.txt
	@echo "Sample batch file created: sample_batch.txt"

# Show version info
version:
	@echo "Autonomous Unlimited Code Generator v1.0.0"
	@echo "Built with $(CXX) $(shell $(CXX) --version | head -1)"
	@echo "C++ Standard: C++17"
	@echo "Build date: $(shell date)"

# Uninstall
uninstall:
	@echo "Uninstalling autocoder..."
	sudo rm -f /usr/local/bin/autocoder
	@echo "Uninstall complete."

.PHONY: all debug install clean test examples demo batch-demo scaffold-demo help format lint package check-deps benchmark create-batch-sample version uninstall