# Makefile for File Encryption Project
# Native implementation only (zero dependencies)

CXX = g++
CXXFLAGS = -std=c++11 -O2 -Wall -Wextra

# Targets
all: chacha20_encryptor hybrid_encryptor chacha20_test_stub hybrid_stub

# ChaCha20 encryptor
chacha20_encryptor: chacha20_encryptor.cpp chacha20.h
	$(CXX) $(CXXFLAGS) -o chacha20_encryptor chacha20_encryptor.cpp

# Hybrid encryptor (supports both AES and ChaCha20)
hybrid_encryptor: hybrid_encryptor.cpp chacha20.h
	$(CXX) $(CXXFLAGS) -o hybrid_encryptor hybrid_encryptor.cpp

# ChaCha20 test stub
chacha20_test_stub: chacha20_test_stub.cpp chacha20.h
	$(CXX) $(CXXFLAGS) -o chacha20_test_stub chacha20_test_stub.cpp

# Hybrid stub (supports both AES and ChaCha20)
hybrid_stub: hybrid_stub.cpp chacha20.h
	$(CXX) $(CXXFLAGS) -o hybrid_stub hybrid_stub.cpp

# Original AES encryptor (for comparison)
native_encryptor: native_encryptor.cpp
	$(CXX) $(CXXFLAGS) -o native_encryptor native_encryptor.cpp

# Test targets
test_chacha20: chacha20_encryptor
	@echo "Testing ChaCha20 encryption..."
	@echo "Hello, ChaCha20!" > test_input.txt
	./chacha20_encryptor test_input.txt test_chacha20_output.bin
	@echo "ChaCha20 encryption test completed"

test_hybrid: hybrid_encryptor
	@echo "Testing hybrid encryption..."
	@echo "Hello, Hybrid!" > test_input.txt
	./hybrid_encryptor test_input.txt test_aes_output.bin aes
	./hybrid_encryptor test_input.txt test_chacha20_output.bin chacha20
	@echo "Hybrid encryption test completed"

# Clean up
clean:
	rm -f chacha20_encryptor hybrid_encryptor chacha20_test_stub hybrid_stub
	rm -f native_encryptor
	rm -f test_*.txt test_*.bin

# Install (copy to /usr/local/bin)
install: all
	sudo cp chacha20_encryptor hybrid_encryptor /usr/local/bin/
	@echo "ChaCha20 tools installed to /usr/local/bin/"

# Uninstall
uninstall:
	sudo rm -f /usr/local/bin/chacha20_encryptor /usr/local/bin/hybrid_encryptor
	@echo "ChaCha20 tools uninstalled"

.PHONY: all clean test_chacha20 test_hybrid install uninstall