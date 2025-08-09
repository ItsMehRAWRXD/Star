# Makefile for File Encryption Project
# Native implementation only (zero dependencies)

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I.
LDFLAGS = -pthread

# Targets
TARGETS = test_plugin_system unified_stub_generator_with_plugins

# Plugin sources
PLUGIN_SOURCES = plugins/chacha20_plugin.cpp \
                 plugins/aes_plugin.cpp \
                 plugins/xor_plugin.cpp

# Common headers
HEADERS = encryption_plugin_interface.h \
          encryption_plugin_manager.h

# Common objects
COMMON_OBJS = encryption_plugin_manager.o

all: $(TARGETS)

# Build plugin manager
encryption_plugin_manager.o: encryption_plugin_manager.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build test program
test_plugin_system: test_plugin_system.cpp $(COMMON_OBJS) $(HEADERS) $(PLUGIN_SOURCES)
	$(CXX) $(CXXFLAGS) $< $(COMMON_OBJS) -o $@ $(LDFLAGS)

# Build unified stub generator with plugins
unified_stub_generator_with_plugins: unified_stub_generator_with_plugins.cpp $(COMMON_OBJS) $(HEADERS) $(PLUGIN_SOURCES)
	$(CXX) $(CXXFLAGS) $< $(COMMON_OBJS) -o $@ $(LDFLAGS)

# Create example payload for testing
test_payload:
	printf '#include <stdio.h>\nint main() { printf("Hello from payload!\\n"); return 0; }' > test_payload.c
	gcc -o test_payload test_payload.c
	rm test_payload.c

# Run tests
test: test_plugin_system test_payload
	./test_plugin_system
	@echo ""
	@echo "Testing stub generation:"
	./unified_stub_generator_with_plugins test_payload test_stub_output.cpp --list-plugins
	@echo ""
	@echo "Generating stub with all plugins:"
	./unified_stub_generator_with_plugins test_payload test_stub_output.cpp

# Clean build files
clean:
	rm -f $(TARGETS) $(COMMON_OBJS) test_payload test_stub_output.cpp *.o

# Install (optional)
install:
	@echo "Installation not implemented. Run from build directory."

.PHONY: all clean test test_payload install