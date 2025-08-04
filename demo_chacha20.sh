#!/bin/bash

echo "=== ChaCha20 Encryption Demo ==="
echo

# Create a test file
echo "Creating test file..."
echo "This is a test file for ChaCha20 encryption demonstration." > demo_input.txt
echo "It contains multiple lines of text to test the encryption." >> demo_input.txt
echo "ChaCha20 is a modern, high-performance stream cipher." >> demo_input.txt

echo "Original file content:"
cat demo_input.txt
echo

# Test ChaCha20 encryption
echo "=== Testing ChaCha20 Encryption ==="
./chacha20_encryptor demo_input.txt demo_chacha20_encrypted.bin
echo

# Test hybrid encryptor with ChaCha20
echo "=== Testing Hybrid Encryptor with ChaCha20 ==="
./hybrid_encryptor demo_input.txt demo_hybrid_chacha20.bin chacha20
echo

# Test hybrid encryptor with AES
echo "=== Testing Hybrid Encryptor with AES ==="
./hybrid_encryptor demo_input.txt demo_hybrid_aes.bin aes
echo

# Show file sizes
echo "=== File Size Comparison ==="
echo "Original file: $(wc -c < demo_input.txt) bytes"
echo "ChaCha20 encrypted: $(wc -c < demo_chacha20_encrypted.bin) bytes"
echo "Hybrid ChaCha20: $(wc -c < demo_hybrid_chacha20.bin) bytes"
echo "Hybrid AES: $(wc -c < demo_hybrid_aes.bin) bytes"
echo

# Show hex dump of encrypted files (first 64 bytes)
echo "=== Hex Dump of Encrypted Files (first 64 bytes) ==="
echo "ChaCha20 encrypted file header:"
hexdump -C demo_chacha20_encrypted.bin | head -2
echo

echo "Hybrid ChaCha20 file header:"
hexdump -C demo_hybrid_chacha20.bin | head -2
echo

echo "Hybrid AES file header:"
hexdump -C demo_hybrid_aes.bin | head -2
echo

# Clean up
echo "=== Cleaning up ==="
rm -f demo_input.txt demo_chacha20_encrypted.bin demo_hybrid_chacha20.bin demo_hybrid_aes.bin
echo "Demo completed!"