#!/bin/bash

echo "=== Generating 101 Unique Stubs ==="
echo ""

# Clean up any previous runs
rm -rf stubs_output
mkdir -p stubs_output

# Use the test program as payload
if [ ! -f test_program.exe ]; then
    echo "Creating test payload..."
    g++ -o test_program.exe test_payload.cpp 2>/dev/null || {
        # Create a simple test program if compilation fails
        echo -e '#include <iostream>\nint main() { std::cout << "Test payload executed!\\n"; return 0; }' > simple_test.cpp
        g++ -o test_program.exe simple_test.cpp
    }
fi

# Generate 101 stubs
echo "Generating 101 stubs..."
./no_limit_unified_generator test_program.exe stubs_output/stub 101

echo ""
echo "=== Verifying Uniqueness ==="

# Calculate MD5 hashes for all generated stubs
echo "Calculating hashes..."
cd stubs_output

# Create hash list
rm -f hashes.txt
for i in $(seq 0 100); do
    if [ -f "stub_$i.cpp" ]; then
        md5sum "stub_$i.cpp" >> hashes.txt
    fi
done

# Count unique hashes
TOTAL_FILES=$(ls stub_*.cpp 2>/dev/null | wc -l)
UNIQUE_HASHES=$(cut -d' ' -f1 hashes.txt | sort -u | wc -l)

echo ""
echo "Total files generated: $TOTAL_FILES"
echo "Unique file hashes: $UNIQUE_HASHES"

if [ "$UNIQUE_HASHES" -eq "101" ]; then
    echo "✓ SUCCESS: All 101 stubs are unique!"
else
    echo "✗ WARNING: Some stubs may be identical"
    echo "Showing duplicate hashes:"
    cut -d' ' -f1 hashes.txt | sort | uniq -d
fi

# Show file size variation
echo ""
echo "=== File Size Analysis ==="
MIN_SIZE=$(ls -l stub_*.cpp | awk '{print $5}' | sort -n | head -1)
MAX_SIZE=$(ls -l stub_*.cpp | awk '{print $5}' | sort -n | tail -1)
AVG_SIZE=$(ls -l stub_*.cpp | awk '{sum+=$5; count++} END {print int(sum/count)}')

echo "Minimum stub size: $MIN_SIZE bytes"
echo "Maximum stub size: $MAX_SIZE bytes"
echo "Average stub size: $AVG_SIZE bytes"
echo "Size variation: $((MAX_SIZE - MIN_SIZE)) bytes"

# Sample content from a few stubs to show variation
echo ""
echo "=== Content Variation Sample ==="
echo "Showing first 5 lines from 3 random stubs:"
echo ""

for i in 0 50 100; do
    if [ -f "stub_$i.cpp" ]; then
        echo "--- stub_$i.cpp ---"
        head -n 5 "stub_$i.cpp"
        echo ""
    fi
done

# Count unique variable names
echo "=== Variable Name Analysis ==="
UNIQUE_VARS=$(grep -h "int \|char \|void \|BOOL \|DWORD " stub_*.cpp | grep -oE '[a-zA-Z0-9_]{8,}' | sort -u | wc -l)
echo "Unique variable names found: $UNIQUE_VARS"

# Check for compilation (test a few)
echo ""
echo "=== Compilation Test ==="
echo "Testing compilation of 3 random stubs..."

for i in 0 50 100; do
    if [ -f "stub_$i.cpp" ]; then
        echo -n "Compiling stub_$i.cpp... "
        if g++ -std=c++11 -O2 -o "test_stub_$i" "stub_$i.cpp" -w 2>/dev/null; then
            echo "✓ SUCCESS"
            rm -f "test_stub_$i"
        else
            echo "✗ FAILED"
        fi
    fi
done

cd ..

echo ""
echo "=== Generation Complete ==="
echo "All stubs saved in: stubs_output/"
echo ""

# Generate summary report
cat > stubs_output/generation_report.txt << EOF
Stub Generation Report
=====================
Date: $(date)
Total Stubs Generated: $TOTAL_FILES
Unique Stubs: $UNIQUE_HASHES
Success Rate: $((UNIQUE_HASHES * 100 / 101))%

File Size Statistics:
- Minimum: $MIN_SIZE bytes
- Maximum: $MAX_SIZE bytes  
- Average: $AVG_SIZE bytes
- Variation: $((MAX_SIZE - MIN_SIZE)) bytes

Unique Variable Names: $UNIQUE_VARS
EOF

echo "Report saved to: stubs_output/generation_report.txt"