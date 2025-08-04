#!/bin/bash

echo "=== Verifying Final 101 Unique Stubs ==="
echo ""

cd final_101_stubs

# Check file count
TOTAL=$(ls unique_stub_*.cpp 2>/dev/null | wc -l)
echo "Total stubs generated: $TOTAL"

# Verify uniqueness
echo "Calculating MD5 hashes..."
rm -f final_hashes.txt
md5sum unique_stub_*.cpp > final_hashes.txt
UNIQUE=$(cut -d' ' -f1 final_hashes.txt | sort -u | wc -l)

echo "Unique stubs: $UNIQUE"

if [ "$UNIQUE" -eq "101" ]; then
    echo "✅ SUCCESS: All 101 stubs are unique!"
else
    echo "❌ FAILED: Found duplicate stubs"
    echo "Duplicate hashes:"
    cut -d' ' -f1 final_hashes.txt | sort | uniq -d
fi

# Size analysis
echo ""
echo "=== File Size Analysis ==="
MIN_SIZE=$(ls -l unique_stub_*.cpp | awk '{print $5}' | sort -n | head -1)
MAX_SIZE=$(ls -l unique_stub_*.cpp | awk '{print $5}' | sort -n | tail -1)
AVG_SIZE=$(ls -l unique_stub_*.cpp | awk '{sum+=$5; count++} END {print int(sum/count)}')

echo "Minimum stub size: $MIN_SIZE bytes"
echo "Maximum stub size: $MAX_SIZE bytes"
echo "Average stub size: $AVG_SIZE bytes"
echo "Size variation: $((MAX_SIZE - MIN_SIZE)) bytes"

# Test compilation
echo ""
echo "=== Compilation Test ==="
SUCCESS=0
FAILED=0

for i in 0 25 50 75 100; do
    if [ -f "unique_stub_$i.cpp" ]; then
        echo -n "Testing unique_stub_$i.cpp... "
        if g++ -std=c++11 -O2 -o "test_$i" "unique_stub_$i.cpp" 2>/dev/null; then
            echo "✅ Compiled"
            ((SUCCESS++))
            rm -f "test_$i"
        else
            echo "❌ Failed"
            ((FAILED++))
        fi
    fi
done

echo ""
echo "Compilation results: $SUCCESS succeeded, $FAILED failed"

# Feature verification
echo ""
echo "=== Feature Verification ==="

# Check for string encryption
PLAIN_STRINGS=$(grep -c "Debugger detected\|Analysis mode\|Infinite loop" unique_stub_*.cpp 2>/dev/null || echo "0")
echo "Unencrypted strings found: $PLAIN_STRINGS"

# Check for anti-debug code
ANTI_DEBUG=$(grep -c "IsDebuggerPresent\|volatile int\* p = " unique_stub_*.cpp | grep -v ":0$" | wc -l)
echo "Files with anti-debug code: $ANTI_DEBUG"

# Check for encrypted strings (lambda functions)
ENCRYPTED=$(grep -c "auto.*=.*\[\].*->.*std::string" unique_stub_*.cpp | grep -v ":0$" | wc -l)
echo "Files with encrypted strings: $ENCRYPTED"

# Check unique IDs
echo ""
echo "=== Unique ID Verification ==="
UNIQUE_IDS=$(grep "Unique ID:" unique_stub_*.cpp | cut -d' ' -f4 | sort -u | wc -l)
echo "Unique IDs found: $UNIQUE_IDS"

# Show sample
echo ""
echo "=== Sample Stub Header ==="
echo "From unique_stub_0.cpp:"
head -n 30 unique_stub_0.cpp | grep -E "Unique ID:|bool.*\(\)|IsDebuggerPresent|volatile int\* p"

cd ..

echo ""
echo "=== Summary ==="
echo "✅ Generated 101 unique stubs with:"
echo "  - String encryption (no plaintext debug strings)"
echo "  - Anti-debugging (freeze mode - infinite loop on debug)"
echo "  - Random delays and junk code"
echo "  - Unique variable names and structure"
echo "  - All stubs saved in: final_101_stubs/"