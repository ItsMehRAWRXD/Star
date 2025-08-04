#!/usr/bin/env python3
"""
Test script for Advanced Packer functionality
Demonstrates compression and reversible encryption capabilities
"""

import os
import sys
import tempfile
import shutil
from advanced_packer import AdvancedPacker, xor_crypt, compress_data, decompress_data

def create_test_file(filename: str, size: int = 1024, pattern: str = "test") -> str:
    """Create a test file with specified size and pattern"""
    with open(filename, 'w') as f:
        # Create repetitive content for better compression
        content = pattern * (size // len(pattern)) + pattern[:size % len(pattern)]
        f.write(content)
    return filename

def test_basic_functionality():
    """Test basic pack/unpack functionality"""
    print("=== Testing Basic Functionality ===")
    
    packer = AdvancedPacker()
    
    # Create test file
    test_file = "test_basic.txt"
    create_test_file(test_file, 2048, "Hello World! This is a test file for the advanced packer. ")
    
    # Test packing with XOR encryption
    print("\n1. Testing XOR encryption:")
    if packer.pack_file(test_file, "test_basic_xor.pak", 1):
        packer.list_file("test_basic_xor.pak")
        if packer.unpack_file("test_basic_xor.pak", "test_basic_xor_restored.txt"):
            # Verify content
            with open(test_file, 'rb') as f1, open("test_basic_xor_restored.txt", 'rb') as f2:
                if f1.read() == f2.read():
                    print("✓ XOR encryption/decryption test PASSED")
                else:
                    print("✗ XOR encryption/decryption test FAILED")
    
    # Test packing without encryption
    print("\n2. Testing compression only:")
    if packer.pack_file(test_file, "test_basic_comp.pak", 0):
        packer.list_file("test_basic_comp.pak")
        if packer.unpack_file("test_basic_comp.pak", "test_basic_comp_restored.txt"):
            # Verify content
            with open(test_file, 'rb') as f1, open("test_basic_comp_restored.txt", 'rb') as f2:
                if f1.read() == f2.read():
                    print("✓ Compression only test PASSED")
                else:
                    print("✗ Compression only test FAILED")
    
    # Cleanup
    for f in [test_file, "test_basic_xor.pak", "test_basic_xor_restored.txt", 
              "test_basic_comp.pak", "test_basic_comp_restored.txt"]:
        if os.path.exists(f):
            os.remove(f)

def test_aes_encryption():
    """Test AES encryption functionality"""
    print("\n=== Testing AES Encryption ===")
    
    try:
        from Crypto.Cipher import AES
    except ImportError:
        print("PyCryptodome not available, skipping AES test")
        print("Install with: pip install pycryptodome")
        return
    
    packer = AdvancedPacker()
    
    # Create test file
    test_file = "test_aes.txt"
    create_test_file(test_file, 1024, "AES encryption test data with some repetitive content. ")
    
    # Test packing with AES encryption
    print("\nTesting AES encryption:")
    if packer.pack_file(test_file, "test_aes.pak", 2):
        packer.list_file("test_aes.pak")
        if packer.unpack_file("test_aes.pak", "test_aes_restored.txt"):
            # Verify content
            with open(test_file, 'rb') as f1, open("test_aes_restored.txt", 'rb') as f2:
                if f1.read() == f2.read():
                    print("✓ AES encryption/decryption test PASSED")
                else:
                    print("✗ AES encryption/decryption test FAILED")
    
    # Cleanup
    for f in [test_file, "test_aes.pak", "test_aes_restored.txt"]:
        if os.path.exists(f):
            os.remove(f)

def test_large_files():
    """Test with larger files to demonstrate compression effectiveness"""
    print("\n=== Testing Large Files ===")
    
    packer = AdvancedPacker()
    
    # Create a larger test file with repetitive content
    test_file = "test_large.txt"
    print(f"Creating large test file: {test_file}")
    
    # Create 100KB of repetitive content for good compression
    with open(test_file, 'w') as f:
        for i in range(1000):
            f.write(f"This is line {i:04d} with some repetitive content for testing compression. " * 10 + "\n")
    
    file_size = os.path.getsize(test_file)
    print(f"Original file size: {file_size:,} bytes")
    
    # Test packing
    print("\nTesting compression and XOR encryption:")
    if packer.pack_file(test_file, "test_large.pak", 1):
        packer.list_file("test_large.pak")
        
        # Unpack and verify
        if packer.unpack_file("test_large.pak", "test_large_restored.txt"):
            with open(test_file, 'rb') as f1, open("test_large_restored.txt", 'rb') as f2:
                if f1.read() == f2.read():
                    print("✓ Large file test PASSED")
                else:
                    print("✗ Large file test FAILED")
    
    # Cleanup
    for f in [test_file, "test_large.pak", "test_large_restored.txt"]:
        if os.path.exists(f):
            os.remove(f)

def test_binary_files():
    """Test with binary files"""
    print("\n=== Testing Binary Files ===")
    
    packer = AdvancedPacker()
    
    # Create a binary test file
    test_file = "test_binary.bin"
    print(f"Creating binary test file: {test_file}")
    
    # Create binary data with some patterns
    with open(test_file, 'wb') as f:
        # Write some structured binary data
        for i in range(1000):
            f.write(struct.pack('<I', i))  # 4 bytes per integer
            f.write(b'\x00' * 4)  # 4 bytes of zeros
    
    file_size = os.path.getsize(test_file)
    print(f"Original file size: {file_size:,} bytes")
    
    # Test packing
    print("\nTesting binary file compression and encryption:")
    if packer.pack_file(test_file, "test_binary.pak", 1):
        packer.list_file("test_binary.pak")
        
        # Unpack and verify
        if packer.unpack_file("test_binary.pak", "test_binary_restored.bin"):
            with open(test_file, 'rb') as f1, open("test_binary_restored.bin", 'rb') as f2:
                if f1.read() == f2.read():
                    print("✓ Binary file test PASSED")
                else:
                    print("✗ Binary file test FAILED")
    
    # Cleanup
    for f in [test_file, "test_binary.pak", "test_binary_restored.bin"]:
        if os.path.exists(f):
            os.remove(f)

def test_compression_effectiveness():
    """Test compression effectiveness with different content types"""
    print("\n=== Testing Compression Effectiveness ===")
    
    packer = AdvancedPacker()
    
    # Test 1: Highly compressible content
    print("\n1. Highly compressible content (repetitive):")
    test_file1 = "test_compressible.txt"
    with open(test_file1, 'w') as f:
        f.write("This is very repetitive content. " * 1000)
    
    size1 = os.path.getsize(test_file1)
    print(f"Original size: {size1:,} bytes")
    
    if packer.pack_file(test_file1, "test_compressible.pak", 0):
        packer.list_file("test_compressible.pak")
    
    # Test 2: Less compressible content (random-like)
    print("\n2. Less compressible content (random-like):")
    test_file2 = "test_random.txt"
    import random
    with open(test_file2, 'w') as f:
        for _ in range(1000):
            f.write(f"Random{random.randint(100000, 999999)} ")
    
    size2 = os.path.getsize(test_file2)
    print(f"Original size: {size2:,} bytes")
    
    if packer.pack_file(test_file2, "test_random.pak", 0):
        packer.list_file("test_random.pak")
    
    # Cleanup
    for f in [test_file1, test_file2, "test_compressible.pak", "test_random.pak"]:
        if os.path.exists(f):
            os.remove(f)

def test_error_handling():
    """Test error handling"""
    print("\n=== Testing Error Handling ===")
    
    packer = AdvancedPacker()
    
    # Test with non-existent file
    print("\n1. Testing with non-existent file:")
    if not packer.pack_file("nonexistent.txt", "output.pak"):
        print("✓ Correctly handled non-existent input file")
    
    # Test with invalid packed file
    print("\n2. Testing with invalid packed file:")
    with open("invalid.pak", 'wb') as f:
        f.write(b"Invalid data")
    
    if not packer.unpack_file("invalid.pak", "output.txt"):
        print("✓ Correctly handled invalid packed file")
    
    # Cleanup
    for f in ["invalid.pak"]:
        if os.path.exists(f):
            os.remove(f)

def main():
    """Main test function"""
    print("Advanced Packer Test Suite")
    print("=" * 50)
    
    # Import struct for binary file test
    global struct
    import struct
    
    try:
        test_basic_functionality()
        test_aes_encryption()
        test_large_files()
        test_binary_files()
        test_compression_effectiveness()
        test_error_handling()
        
        print("\n" + "=" * 50)
        print("All tests completed!")
        
    except Exception as e:
        print(f"\nTest suite failed with error: {e}")
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())