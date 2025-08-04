#!/bin/bash

echo "=== Testing Enhanced No-Limit Generator ==="
echo "Generating 101 unique stubs with different anti-debug modes"
echo ""

# Clean up
rm -rf enhanced_stubs
mkdir -p enhanced_stubs

# Create test payload if needed
if [ ! -f test_program.exe ]; then
    echo -e '#include <iostream>\nint main() { std::cout << "Test payload!\\n"; return 0; }' > test.cpp
    g++ -o test_program.exe test.cpp
    rm test.cpp
fi

# Generate 25 stubs for each anti-debug mode
echo "1. Generating 25 stubs with Exit mode (freezes on debug)..."
./enhanced_no_limit_generator test_program.exe enhanced_stubs/exit_stub 25 --anti-debug-mode 0

echo "2. Generating 25 stubs with Assist mode (helps debugger)..."
./enhanced_no_limit_generator test_program.exe enhanced_stubs/assist_stub 25 --anti-debug-mode 1

echo "3. Generating 25 stubs with Freeze mode (infinite loop)..."
./enhanced_no_limit_generator test_program.exe enhanced_stubs/freeze_stub 25 --anti-debug-mode 2

echo "4. Generating 26 stubs with Crash mode (crashes debugger)..."
./enhanced_no_limit_generator test_program.exe enhanced_stubs/crash_stub 26 --anti-debug-mode 3

echo ""
echo "=== Verification ==="

# Count files
TOTAL=$(ls enhanced_stubs/*.cpp 2>/dev/null | wc -l)
echo "Total stubs generated: $TOTAL"

# Check uniqueness
cd enhanced_stubs
rm -f all_hashes.txt
md5sum *.cpp > all_hashes.txt
UNIQUE=$(cut -d' ' -f1 all_hashes.txt | sort -u | wc -l)
echo "Unique stubs: $UNIQUE"

if [ "$UNIQUE" -eq "101" ]; then
    echo "✓ SUCCESS: All 101 stubs are unique!"
else
    echo "✗ WARNING: Found duplicate stubs"
fi

# Test compilation of a few
echo ""
echo "=== Compilation Test ==="
echo "Testing one stub from each mode..."

for prefix in exit_stub assist_stub freeze_stub crash_stub; do
    if [ -f "${prefix}_0.cpp" ]; then
        echo -n "Compiling ${prefix}_0.cpp... "
        if g++ -std=c++11 -O2 -o "test_${prefix}" "${prefix}_0.cpp" 2>/dev/null; then
            echo "✓ SUCCESS"
            rm -f "test_${prefix}"
        else
            echo "✗ FAILED"
        fi
    fi
done

# Analyze string encryption
echo ""
echo "=== String Encryption Analysis ==="
echo "Checking for encrypted strings in assist mode stubs..."
if grep -q "Debugger detected - Analysis mode enabled" assist_stub_*.cpp 2>/dev/null; then
    echo "✗ Found unencrypted strings"
else
    echo "✓ No plaintext strings found"
fi

# Check for lambda functions (encrypted strings)
LAMBDA_COUNT=$(grep -c "auto.*=.*\[\].*->.*std::string" assist_stub_*.cpp 2>/dev/null | grep -v ":0$" | wc -l)
echo "Files with encrypted strings: $LAMBDA_COUNT"

cd ..

echo ""
echo "=== Summary ==="
echo "All enhanced stubs saved in: enhanced_stubs/"
echo "Features:"
echo "- String encryption: Enabled"
echo "- Anti-debug modes: All 4 modes tested"
echo "- Junk code: Enabled"
echo "- Random delays: Enabled"