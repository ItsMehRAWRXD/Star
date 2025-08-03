# Makefile for Native Encryption Project
# Pure C++ implementation with no external dependencies

CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -pedantic
TARGETS = native_encryptor native_xll_dropper native_gui

# Default target
all: $(TARGETS)

# Native encryptor (AES-128-CTR encryption)
native_encryptor: native_encryptor.cpp
	$(CXX) $(CXXFLAGS) -o native_encryptor native_encryptor.cpp

# Native XLL dropper (AES-128-CTR decryption)
native_xll_dropper: native_xll_dropper.cpp
	$(CXX) $(CXXFLAGS) -o native_xll_dropper native_xll_dropper.cpp

# Native GUI (console-based interface)
native_gui: native_gui.cpp
	$(CXX) $(CXXFLAGS) -o native_gui native_gui.cpp

# Legacy components (for compatibility)
encryptor: encryptor.cpp
	$(CXX) $(CXXFLAGS) -o encryptor encryptor.cpp -lssl -lcrypto

xll_dropper: xll_dropper.cpp
	$(CXX) $(CXXFLAGS) -o xll_dropper xll_dropper.cpp -lssl -lcrypto

packer: packer.cpp
	$(CXX) $(CXXFLAGS) -o packer packer.cpp

# Test targets
test_native: all
	@echo "Testing native components..."
	@echo "Creating test file..."
	@echo "Test content for native encryption" > test_native.txt
	@echo "Encrypting with native AES..."
	@./native_encryptor test_native.txt test_native.enc
	@echo "Decrypting with native XLL dropper..."
	@./native_xll_dropper test_native.enc test_native.dec
	@echo "Verifying content..."
	@if cmp -s test_native.txt test_native.dec; then \
		echo "✓ Native encryption/decryption test PASSED"; \
	else \
		echo "✗ Native encryption/decryption test FAILED"; \
	fi
	@rm -f test_native.txt test_native.enc test_native.dec

# Clean targets
clean:
	rm -f $(TARGETS) encryptor xll_dropper packer
	rm -f *.o *.exe
	rm -f test_*.txt test_*.enc test_*.dec

# Install targets
install: all
	@echo "Installing native components..."
	@mkdir -p /usr/local/bin
	@cp native_encryptor /usr/local/bin/
	@cp native_xll_dropper /usr/local/bin/
	@cp native_gui /usr/local/bin/
	@chmod +x /usr/local/bin/native_encryptor
	@chmod +x /usr/local/bin/native_xll_dropper
	@chmod +x /usr/local/bin/native_gui
	@echo "Native components installed to /usr/local/bin/"

# Uninstall targets
uninstall:
	@echo "Uninstalling native components..."
	@rm -f /usr/local/bin/native_encryptor
	@rm -f /usr/local/bin/native_xll_dropper
	@rm -f /usr/local/bin/native_gui
	@echo "Native components uninstalled"

# Documentation
docs:
	@echo "=== Native Encryption Project Documentation ==="
	@echo ""
	@echo "Components:"
	@echo "  native_encryptor  - AES-128-CTR file encryption"
	@echo "  native_xll_dropper - AES-128-CTR file decryption"
	@echo "  native_gui        - Console-based GUI interface"
	@echo ""
	@echo "Usage:"
	@echo "  ./native_encryptor <input_file> <output_file>"
	@echo "  ./native_xll_dropper <encrypted_file> [output_file]"
	@echo "  ./native_gui"
	@echo ""
	@echo "Features:"
	@echo "  - Pure C++ implementation"
	@echo "  - No external dependencies"
	@echo "  - AES-128-CTR encryption"
	@echo "  - XOR encryption (fallback)"
	@echo "  - Environment variable key support"
	@echo "  - Streaming file processing"
	@echo ""

# Help target
help:
	@echo "Available targets:"
	@echo "  all              - Build all native components"
	@echo "  native_encryptor - Build native encryptor"
	@echo "  native_xll_dropper - Build native XLL dropper"
	@echo "  native_gui       - Build native GUI"
	@echo "  test_native      - Test native components"
	@echo "  clean            - Remove all built files"
	@echo "  install          - Install to /usr/local/bin"
	@echo "  uninstall        - Remove from /usr/local/bin"
	@echo "  docs             - Show documentation"
	@echo "  help             - Show this help"

.PHONY: all clean install uninstall test_native docs help