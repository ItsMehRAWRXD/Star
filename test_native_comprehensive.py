#!/usr/bin/env python3
"""
Comprehensive test script for native encryption implementation
Tests all components with various scenarios
"""

import os
import sys
import subprocess
import tempfile
import shutil

def run_command(cmd, input_data=None):
    """Run a command and return result"""
    try:
        if input_data:
            result = subprocess.run(cmd, input=input_data, capture_output=True, text=True)
        else:
            result = subprocess.run(cmd, capture_output=True, text=True)
        return result.returncode == 0, result.stdout, result.stderr
    except Exception as e:
        return False, "", str(e)

def test_native_encryptor():
    """Test native encryptor functionality"""
    print("Testing native encryptor...")
    
    # Create test file
    test_content = "This is a test file for native encryption.\n" * 100
    with open("test_native_enc.txt", "w") as f:
        f.write(test_content)
    
    # Test encryption
    success, stdout, stderr = run_command(["./native_encryptor", "test_native_enc.txt", "test_native_enc.enc"])
    
    if success:
        print("✓ Native encryptor encryption PASSED")
        
        # Check if encrypted file was created and has content
        if os.path.exists("test_native_enc.enc") and os.path.getsize("test_native_enc.enc") > 16:
            print("✓ Encrypted file created successfully")
            return True
        else:
            print("✗ Encrypted file is missing or too small")
            return False
    else:
        print(f"✗ Native encryptor encryption FAILED: {stderr}")
        return False

def test_native_xll_dropper():
    """Test native XLL dropper functionality"""
    print("\nTesting native XLL dropper...")
    
    # Test decryption of the file we just encrypted
    success, stdout, stderr = run_command(["./native_xll_dropper", "test_native_enc.enc", "test_native_enc.dec"])
    
    if success:
        print("✓ Native XLL dropper decryption PASSED")
        
        # Verify content
        with open("test_native_enc.txt", "r") as f:
            original = f.read()
        with open("test_native_enc.dec", "r") as f:
            decrypted = f.read()
        
        if original == decrypted:
            print("✓ Content verification PASSED")
            return True
        else:
            print("✗ Content verification FAILED")
            return False
    else:
        print(f"✗ Native XLL dropper decryption FAILED: {stderr}")
        return False

def test_native_gui():
    """Test native GUI functionality"""
    print("\nTesting native GUI...")
    
    # Test GUI with exit option
    success, stdout, stderr = run_command(["./native_gui"], input_data="5\n")
    
    if success and "Goodbye!" in stdout:
        print("✓ Native GUI basic functionality PASSED")
        return True
    else:
        print(f"✗ Native GUI basic functionality FAILED: {stderr}")
        return False

def test_error_handling():
    """Test error handling"""
    print("\nTesting error handling...")
    
    # Test with non-existent file
    success, stdout, stderr = run_command(["./native_encryptor", "non_existent.txt", "output.enc"])
    if not success:
        print("✓ Non-existent file handling PASSED")
    else:
        print("✗ Non-existent file handling FAILED")
        return False
    
    # Test with invalid arguments
    success, stdout, stderr = run_command(["./native_encryptor"])
    if not success:
        print("✓ Invalid arguments handling PASSED")
    else:
        print("✗ Invalid arguments handling FAILED")
        return False
    
    return True

def test_large_file():
    """Test with large file"""
    print("\nTesting large file handling...")
    
    # Create a large test file (1MB)
    large_content = "Large file test content. " * 50000  # ~1MB
    with open("test_large.txt", "w") as f:
        f.write(large_content)
    
    # Encrypt large file
    success, stdout, stderr = run_command(["./native_encryptor", "test_large.txt", "test_large.enc"])
    if success:
        print("✓ Large file encryption PASSED")
        
        # Decrypt large file
        success, stdout, stderr = run_command(["./native_xll_dropper", "test_large.enc", "test_large.dec"])
        if success:
            print("✓ Large file decryption PASSED")
            
            # Verify content
            with open("test_large.txt", "r") as f:
                original = f.read()
            with open("test_large.dec", "r") as f:
                decrypted = f.read()
            
            if original == decrypted:
                print("✓ Large file content verification PASSED")
                return True
            else:
                print("✗ Large file content verification FAILED")
                return False
        else:
            print(f"✗ Large file decryption FAILED: {stderr}")
            return False
    else:
        print(f"✗ Large file encryption FAILED: {stderr}")
        return False

def test_binary_file():
    """Test with binary file"""
    print("\nTesting binary file handling...")
    
    # Create a binary test file
    binary_data = bytes([i % 256 for i in range(1000)])
    with open("test_binary.bin", "wb") as f:
        f.write(binary_data)
    
    # Encrypt binary file
    success, stdout, stderr = run_command(["./native_encryptor", "test_binary.bin", "test_binary.enc"])
    if success:
        print("✓ Binary file encryption PASSED")
        
        # Decrypt binary file
        success, stdout, stderr = run_command(["./native_xll_dropper", "test_binary.enc", "test_binary.dec"])
        if success:
            print("✓ Binary file decryption PASSED")
            
            # Verify content
            with open("test_binary.bin", "rb") as f:
                original = f.read()
            with open("test_binary.dec", "rb") as f:
                decrypted = f.read()
            
            if original == decrypted:
                print("✓ Binary file content verification PASSED")
                return True
            else:
                print("✗ Binary file content verification FAILED")
                return False
        else:
            print(f"✗ Binary file decryption FAILED: {stderr}")
            return False
    else:
        print(f"✗ Binary file encryption FAILED: {stderr}")
        return False

def test_environment_key():
    """Test environment variable key support"""
    print("\nTesting environment variable key support...")
    
    # Set environment variable
    os.environ["ENCRYPTION_KEY"] = "test_key_12345"
    
    # Create test file
    test_content = "Test with environment key"
    with open("test_env.txt", "w") as f:
        f.write(test_content)
    
    # Encrypt with environment key
    success, stdout, stderr = run_command(["./native_encryptor", "test_env.txt", "test_env.enc"])
    if success:
        print("✓ Environment key encryption PASSED")
        
        # Decrypt with environment key
        success, stdout, stderr = run_command(["./native_xll_dropper", "test_env.enc", "test_env.dec"])
        if success:
            print("✓ Environment key decryption PASSED")
            
            # Verify content
            with open("test_env.txt", "r") as f:
                original = f.read()
            with open("test_env.dec", "r") as f:
                decrypted = f.read()
            
            if original == decrypted:
                print("✓ Environment key content verification PASSED")
                return True
            else:
                print("✗ Environment key content verification FAILED")
                return False
        else:
            print(f"✗ Environment key decryption FAILED: {stderr}")
            return False
    else:
        print(f"✗ Environment key encryption FAILED: {stderr}")
        return False

def cleanup():
    """Clean up test files"""
    test_files = [
        "test_native_enc.txt", "test_native_enc.enc", "test_native_enc.dec",
        "test_large.txt", "test_large.enc", "test_large.dec",
        "test_binary.bin", "test_binary.enc", "test_binary.dec",
        "test_env.txt", "test_env.enc", "test_env.dec"
    ]
    
    for file in test_files:
        if os.path.exists(file):
            os.remove(file)

def main():
    """Run all tests"""
    print("=== Native Implementation Comprehensive Test Suite ===")
    print()
    
    tests = [
        ("Native Encryptor", test_native_encryptor),
        ("Native XLL Dropper", test_native_xll_dropper),
        ("Native GUI", test_native_gui),
        ("Error Handling", test_error_handling),
        ("Large File", test_large_file),
        ("Binary File", test_binary_file),
        ("Environment Key", test_environment_key)
    ]
    
    passed = 0
    total = len(tests)
    
    for test_name, test_func in tests:
        try:
            if test_func():
                passed += 1
        except Exception as e:
            print(f"✗ {test_name} test FAILED with exception: {e}")
    
    print(f"\n=== Test Results ===")
    print(f"Tests passed: {passed}/{total}")
    
    cleanup()
    
    if passed == total:
        print("✓ All native implementation tests PASSED!")
        print("✓ Native implementation is working correctly!")
        return 0
    else:
        print("✗ Some tests FAILED - Please check the issues above")
        return 1

if __name__ == "__main__":
    sys.exit(main())