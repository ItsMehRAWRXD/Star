# Makefile for File Encryption Project
# Native implementation only (zero dependencies)

CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -pedantic

# Native targets (zero dependencies)
NATIVE_TARGETS = native_encryptor native_xll_dropper native_gui native_stub_generator

# Default target
all: native

# Build native implementation (zero dependencies)
native: $(NATIVE_TARGETS)

# Native encryptor (AES-128-CTR encryption)
native_encryptor: native_encryptor.cpp
	$(CXX) $(CXXFLAGS) -o native_encryptor native_encryptor.cpp

# Native XLL dropper (AES-128-CTR decryption)
native_xll_dropper: native_xll_dropper.cpp
	$(CXX) $(CXXFLAGS) -o native_xll_dropper native_xll_dropper.cpp

# Native GUI (console-based interface)
native_gui: native_gui.cpp
	$(CXX) $(CXXFLAGS) -o native_gui native_gui.cpp

# Native Stub Generator (unlimited stub creation)
native_stub_generator: native_stub_generator.cpp
	$(CXX) $(CXXFLAGS) -o native_stub_generator native_stub_generator.cpp

# Test target
test: native
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

# Clean target
clean:
	rm -f $(NATIVE_TARGETS)
	rm -f *.o *.exe
	rm -f test_*.txt test_*.enc test_*.dec

# Install target
install: native
	@echo "Installing native components..."
	@mkdir -p /usr/local/bin
	@cp native_encryptor /usr/local/bin/
	@cp native_xll_dropper /usr/local/bin/
	@cp native_gui /usr/local/bin/
	@cp native_stub_generator /usr/local/bin/
	@chmod +x /usr/local/bin/native_encryptor
	@chmod +x /usr/local/bin/native_xll_dropper
	@chmod +x /usr/local/bin/native_gui
	@chmod +x /usr/local/bin/native_stub_generator
	@echo "Native components installed to /usr/local/bin/"

# Uninstall target
uninstall:
	@echo "Uninstalling native components..."
	@rm -f /usr/local/bin/native_encryptor
	@rm -f /usr/local/bin/native_xll_dropper
	@rm -f /usr/local/bin/native_gui
	@rm -f /usr/local/bin/native_stub_generator
	@echo "Native components uninstalled"

# Documentation
docs:
	@echo "=== File Encryption Project Documentation ==="
	@echo ""
	@echo "🟢 NATIVE IMPLEMENTATION (Zero Dependencies):"
	@echo "  native_encryptor  - AES-128-CTR file encryption"
	@echo "  native_xll_dropper - AES-128-CTR file decryption"
	@echo "  native_gui        - Console-based GUI interface"
	@echo "  native_stub_generator - Stub generation utility"
	@echo ""
	@echo "📋 BUILD TARGETS:"
	@echo "  make              - Build all native components"
	@echo "  make native       - Build native implementation"
	@echo ""
	@echo "🧪 TEST TARGETS:"
	@echo "  make test         - Test native implementation"
	@echo ""
	@echo "📦 INSTALL TARGETS:"
	@echo "  make install      - Install native components"
	@echo "  make uninstall    - Remove native components"
	@echo ""
	@echo "🔧 USAGE EXAMPLES:"
	@echo "  Encryption: ./native_encryptor <input> <output>"
	@echo "  Decryption: ./native_xll_dropper <input> <output>"
	@echo "  GUI:       ./native_gui"
	@echo "  Stub Gen:  ./native_stub_generator"
	@echo ""

# Help target
help:
	@echo "Available targets:"
	@echo ""
	@echo "🟢 NATIVE IMPLEMENTATION (Zero Dependencies):"
	@echo "  native           - Build all native components"
	@echo "  native_encryptor - Build native encryptor"
	@echo "  native_xll_dropper - Build native XLL dropper"
	@echo "  native_gui       - Build native GUI"
	@echo "  native_stub_generator - Build native stub generator"
	@echo "  test             - Test native components"
	@echo ""
	@echo "📦 INSTALLATION:"
	@echo "  install          - Install native components"
	@echo "  uninstall        - Remove native components"
	@echo ""
	@echo "🔧 UTILITIES:"
	@echo "  clean            - Remove all built files"
	@echo "  docs             - Show documentation"
	@echo "  help             - Show this help"

.PHONY: all native clean install uninstall test docs help