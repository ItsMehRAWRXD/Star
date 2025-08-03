#!/usr/bin/env python3
"""
Simple test script for encryption/decryption functionality
Tests both XOR and AES encryption methods
"""

import os
import secrets
import sys

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

def xor_crypt(data: bytes, key: bytes) -> bytes:
    """XOR encryption/decryption"""
    klen = len(key)
    return bytes([b ^ key[i % klen] for i, b in enumerate(data)])

def test_xor_encryption():
    """Test XOR encryption and decryption"""
    print("Testing XOR encryption...")
    
    # Test data
    test_data = b"Hello, World! This is a test message for encryption."
    print(f"Original data: {test_data}")
    
    # Encrypt
    encrypted = xor_crypt(test_data, KEY_DEOBFUSCATED)
    print(f"Encrypted (hex): {encrypted.hex()}")
    
    # Decrypt (XOR is symmetric)
    decrypted = xor_crypt(encrypted, KEY_DEOBFUSCATED)
    print(f"Decrypted: {decrypted}")
    
    # Verify
    if test_data == decrypted:
        print("✓ XOR encryption/decryption test PASSED")
        return True
    else:
        print("✗ XOR encryption/decryption test FAILED")
        return False

def test_aes_encryption():
    """Test AES encryption and decryption"""
    print("\nTesting AES encryption...")
    
    try:
        from Crypto.Cipher import AES  # pycryptodome
    except ImportError:
        try:
            from Cryptodome.Cipher import AES  # pycryptodomex
        except ImportError:
            print("✗ AES test SKIPPED - PyCryptodome not available")
            print("  Install with: pip install pycryptodome")
            return False
    
    # Test data
    test_data = b"Hello, World! This is a test message for AES encryption."
    print(f"Original data: {test_data}")
    
    # Expand/truncate key to 16 bytes for AES-128
    key16 = (KEY_DEOBFUSCATED * (16 // len(KEY_DEOBFUSCATED) + 1))[:16]
    
    # Generate nonce (AES-CTR typically uses 8-12 bytes for nonce)
    nonce = secrets.token_bytes(8)
    
    # Encrypt
    cipher = AES.new(key16, AES.MODE_CTR, nonce=nonce)
    encrypted_data = cipher.encrypt(test_data)
    
    # Prepend nonce to encrypted data
    encrypted_with_nonce = nonce + encrypted_data
    print(f"Encrypted with nonce (hex): {encrypted_with_nonce.hex()}")
    
    # Decrypt
    nonce_from_file = encrypted_with_nonce[:8]
    encrypted_data_from_file = encrypted_with_nonce[8:]
    
    cipher_decrypt = AES.new(key16, AES.MODE_CTR, nonce=nonce_from_file)
    decrypted = cipher_decrypt.encrypt(encrypted_data_from_file)  # CTR mode is symmetric
    
    print(f"Decrypted: {decrypted}")
    
    # Verify
    if test_data == decrypted:
        print("✓ AES encryption/decryption test PASSED")
        return True
    else:
        print("✗ AES encryption/decryption test FAILED")
        return False

def test_file_operations():
    """Test file encryption/decryption"""
    print("\nTesting file operations...")
    
    # Create test file
    test_content = b"This is a test file content for encryption testing."
    test_file = "test_file.txt"
    encrypted_file = "test_file.enc"
    decrypted_file = "test_file.dec"
    
    try:
        # Write test file
        with open(test_file, "wb") as f:
            f.write(test_content)
        print(f"Created test file: {test_file}")
        
        # Encrypt file using XOR
        with open(test_file, "rb") as f:
            data = f.read()
        
        encrypted_data = xor_crypt(data, KEY_DEOBFUSCATED)
        
        with open(encrypted_file, "wb") as f:
            f.write(encrypted_data)
        print(f"Created encrypted file: {encrypted_file}")
        
        # Decrypt file
        with open(encrypted_file, "rb") as f:
            encrypted_data = f.read()
        
        decrypted_data = xor_crypt(encrypted_data, KEY_DEOBFUSCATED)
        
        with open(decrypted_file, "wb") as f:
            f.write(decrypted_data)
        print(f"Created decrypted file: {decrypted_file}")
        
        # Verify
        if test_content == decrypted_data:
            print("✓ File encryption/decryption test PASSED")
            
            # Clean up
            os.remove(test_file)
            os.remove(encrypted_file)
            os.remove(decrypted_file)
            print("Cleaned up test files")
            return True
        else:
            print("✗ File encryption/decryption test FAILED")
            return False
            
    except Exception as e:
        print(f"✗ File operations test FAILED: {e}")
        return False

def main():
    """Run all tests"""
    print("=== Encryption/Decryption Test Suite ===")
    print(f"Using key: {KEY_DEOBFUSCATED.hex()}")
    print()
    
    tests_passed = 0
    total_tests = 3
    
    # Test XOR encryption
    if test_xor_encryption():
        tests_passed += 1
    
    # Test AES encryption
    if test_aes_encryption():
        tests_passed += 1
    
    # Test file operations
    if test_file_operations():
        tests_passed += 1
    
    print(f"\n=== Test Results ===")
    print(f"Tests passed: {tests_passed}/{total_tests}")
    
    if tests_passed == total_tests:
        print("✓ All tests PASSED - Encryption system is working correctly!")
        return 0
    else:
        print("✗ Some tests FAILED - Please check the issues above")
        return 1

if __name__ == "__main__":
    sys.exit(main())