#!/bin/bash

echo "Checking uniqueness of generated stubs..."

# Generate MD5 hashes
for i in $(seq 0 24); do
    md5sum mirc_unlimited_$i.cpp >> stub_hashes.txt
done

# Check for duplicates
total=$(cat stub_hashes.txt | wc -l)
unique=$(cat stub_hashes.txt | cut -d' ' -f1 | sort -u | wc -l)

echo "Total stubs: $total"
echo "Unique stubs: $unique"

if [ "$total" -eq "$unique" ]; then
    echo "✓ All stubs are unique!"
else
    echo "✗ Found duplicate stubs!"
    echo "Duplicates:"
    cat stub_hashes.txt | cut -d' ' -f1 | sort | uniq -d
fi

# Check Unique IDs
echo ""
echo "Checking Unique IDs in stubs..."
grep "Unique ID:" mirc_unlimited_*.cpp | cut -d':' -f3 | sort | uniq -d > duplicate_ids.txt

if [ ! -s duplicate_ids.txt ]; then
    echo "✓ All Unique IDs are different!"
else
    echo "✗ Found duplicate IDs:"
    cat duplicate_ids.txt
fi

# Sample code differences
echo ""
echo "Sample variable names from first 3 stubs:"
for i in 0 1 2; do
    echo "Stub $i variables:"
    grep -o "[a-zA-Z_][a-zA-Z0-9_]*[0-9]\{3,5\}" mirc_unlimited_$i.cpp | head -5
done

# Cleanup
rm -f stub_hashes.txt duplicate_ids.txt