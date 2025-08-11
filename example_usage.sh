#!/bin/bash

echo "=== Encryption Tools Usage Example ==="
echo

# Step 1: Create a test payload
echo "Step 1: Creating test payload..."
cat > test_payload.cpp << 'EOF'
#include <iostream>
int main() {
    std::cout << "Hello from encrypted payload!" << std::endl;
    std::cout << "This was decrypted and executed successfully." << std::endl;
    return 0;
}
EOF

g++ -o test_payload.exe test_payload.cpp
echo "✓ Test payload created"
echo

# Step 2: Encrypt the payload
echo "Step 2: Encrypting payload..."
if [ -f universal_encryptor ]; then
    ./universal_encryptor test_payload.exe encrypted_payload.bin
    echo "✓ Payload encrypted"
else
    echo "Error: universal_encryptor not found. Compile it first:"
    echo "g++ -O2 -o universal_encryptor universal_encryptor.cpp"
    exit 1
fi
echo

# Step 3: Generate a standalone stub
echo "Step 3: Generating standalone stub..."
if [ -f native_stub_generator ]; then
    ./native_stub_generator --standalone my_stub.cpp basic
    echo "✓ Standalone stub generated"
else
    echo "Error: native_stub_generator not found. Compile it first:"
    echo "g++ -O2 -o native_stub_generator native_stub_generator.cpp"
    exit 1
fi
echo

# Step 4: Link the payload with the stub
echo "Step 4: Linking encrypted payload with stub..."
if [ -f stub_linker ]; then
    ./stub_linker my_stub.cpp encrypted_payload.bin final_dropper.cpp
    echo "✓ Payload linked with stub"
else
    echo "Error: stub_linker not found. Compile it first:"
    echo "g++ -O2 -o stub_linker stub_linker.cpp"
    exit 1
fi
echo

# Step 5: Compile the final dropper
echo "Step 5: Compiling final dropper..."
if command -v g++ &> /dev/null; then
    g++ -O2 -o final_dropper final_dropper.cpp -pthread
    echo "✓ Final dropper compiled"
else
    echo "Error: g++ compiler not found"
    exit 1
fi
echo

echo "=== Success! ==="
echo "You can now run: ./final_dropper"
echo "This will decrypt and execute the payload."
echo

# Alternative: One-step generation
echo "=== Alternative: One-Step Generation ==="
echo "You can also use the unified generator for a simpler workflow:"
echo "./unified_stub_generator test_payload.exe one_step_dropper.cpp --config"
echo

# Cleanup option
read -p "Clean up test files? (y/n) " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    rm -f test_payload.cpp test_payload.exe encrypted_payload.bin encrypted_payload.bin.keys
    rm -f my_stub.cpp final_dropper.cpp
    echo "✓ Test files cleaned up"
fi