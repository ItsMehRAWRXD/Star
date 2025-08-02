CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
OPENSSL_LIBS = -lcrypto -lssl

# Default target
all: encryptor

# Compile encryptor
encryptor: encryptor.cpp
	$(CXX) $(CXXFLAGS) -o encryptor encryptor.cpp $(OPENSSL_LIBS)

# Clean build artifacts
clean:
	rm -f encryptor

.PHONY: all clean