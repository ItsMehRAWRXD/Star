#!/usr/bin/env python3
"""
Advanced Packer - Compress and encrypt files with reversible encryption
Supports compression using zlib and encryption using XOR or AES
"""

import os
import sys
import struct
import zlib
import secrets
from typing import Optional

# Key handling (shared with C++ code)
XOR_OBFUSCATE_KEY = 0x5A
ENC_KEY = bytes([0x39, 0x39, 0x08, 0x0F, 0x0F, 0x38, 0x08, 0x31, 0x38, 0x32, 0x38])

def get_deobfuscated_key():
    """Get the deobfuscated encryption key"""
    env_key = os.getenv("ENCRYPTION_KEY")
    if env_key and len(env_key) >= len(ENC_KEY):
        return env_key[:len(ENC_KEY)].encode('utf-8')
    else:
        # Fallback to obfuscated key
        return bytes([b ^ XOR_OBFUSCATE_KEY for b in ENC_KEY])

KEY_DEOBFUSCATED = get_deobfuscated_key()

# File header structure
PACK_MAGIC = 0x4B434150  # "PACK"
HEADER_SIZE = 16

class PackHeader:
    """Header structure for packed files"""
    def __init__(self, magic: int = 0, original_size: int = 0, 
                 compressed_size: int = 0, encryption_type: int = 0):
        self.magic = magic
        self.original_size = original_size
        self.compressed_size = compressed_size
        self.encryption_type = encryption_type
        self.reserved = b'\x00\x00\x00'  # 3 bytes reserved
    
    def pack(self) -> bytes:
        """Pack header into bytes"""
        return struct.pack('<IIIB', self.magic, self.original_size, 
                          self.compressed_size, self.encryption_type) + self.reserved
    
    @classmethod
    def unpack(cls, data: bytes) -> 'PackHeader':
        """Unpack header from bytes"""
        if len(data) < HEADER_SIZE:
            raise ValueError("Header data too short")
        
        magic, original_size, compressed_size, encryption_type = \
            struct.unpack('<IIIB', data[:13])
        
        header = cls(magic, original_size, compressed_size, encryption_type)
        header.reserved = data[13:16] if len(data) >= 16 else b'\x00\x00\x00'
        return header

def xor_crypt(data: bytes, key: bytes) -> bytes:
    """XOR encryption/decryption"""
    klen = len(key)
    return bytes([b ^ key[i % klen] for i, b in enumerate(data)])

def aes_crypt(data: bytes, key: bytes, encrypt: bool = True) -> bytes:
    """AES encryption/decryption using CTR mode"""
    try:
        from Crypto.Cipher import AES
    except ImportError:
        try:
            from Cryptodome.Cipher import AES
        except ImportError:
            raise ImportError("PyCryptodome not available. Install with: pip install pycryptodome")
    
    # Expand/truncate key to 16 bytes for AES-128
    key16 = (key * (16 // len(key) + 1))[:16]
    
    if encrypt:
        # Generate nonce for encryption
        nonce = secrets.token_bytes(8)
        cipher = AES.new(key16, AES.MODE_CTR, nonce=nonce)
        encrypted = cipher.encrypt(data)
        return nonce + encrypted
    else:
        # Extract nonce for decryption
        if len(data) < 8:
            raise ValueError("Encrypted data too short")
        nonce = data[:8]
        encrypted = data[8:]
        cipher = AES.new(key16, AES.MODE_CTR, nonce=nonce)
        return cipher.encrypt(encrypted)  # CTR mode is symmetric

def compress_data(data: bytes) -> bytes:
    """Compress data using zlib"""
    return zlib.compress(data, level=zlib.Z_BEST_COMPRESSION)

def decompress_data(data: bytes) -> bytes:
    """Decompress data using zlib"""
    return zlib.decompress(data)

class AdvancedPacker:
    """Advanced packer with compression and encryption capabilities"""
    
    def __init__(self):
        self.key = KEY_DEOBFUSCATED
    
    def pack_file(self, input_file: str, output_file: str, 
                  encryption_type: int = 1) -> bool:
        """
        Pack a file: compress + encrypt
        
        Args:
            input_file: Path to input file
            output_file: Path to output file
            encryption_type: 0=none, 1=XOR, 2=AES
        
        Returns:
            True if successful, False otherwise
        """
        try:
            # Read input file
            with open(input_file, 'rb') as f:
                data = f.read()
            
            original_size = len(data)
            print(f"Original size: {original_size} bytes")
            
            # Compress data
            compressed = compress_data(data)
            compressed_size = len(compressed)
            compression_ratio = (100.0 * compressed_size / original_size)
            print(f"Compressed size: {compressed_size} bytes")
            print(f"Compression ratio: {compression_ratio:.1f}%")
            
            # Encrypt if requested
            processed = compressed
            if encryption_type == 1:
                processed = xor_crypt(compressed, self.key)
                print("Applied XOR encryption")
            elif encryption_type == 2:
                processed = aes_crypt(compressed, self.key, encrypt=True)
                print("Applied AES encryption")
            
            # Create header
            header = PackHeader(
                magic=PACK_MAGIC,
                original_size=original_size,
                compressed_size=compressed_size,
                encryption_type=encryption_type
            )
            
            # Write output file
            with open(output_file, 'wb') as f:
                f.write(header.pack())
                f.write(processed)
            
            print(f"Successfully packed to: {output_file}")
            return True
            
        except Exception as e:
            print(f"Packing failed: {e}")
            return False
    
    def unpack_file(self, input_file: str, output_file: str) -> bool:
        """
        Unpack a file: decrypt + decompress
        
        Args:
            input_file: Path to input file
            output_file: Path to output file
        
        Returns:
            True if successful, False otherwise
        """
        try:
            # Read input file
            with open(input_file, 'rb') as f:
                header_data = f.read(HEADER_SIZE)
                processed = f.read()
            
            # Parse header
            header = PackHeader.unpack(header_data)
            
            if header.magic != PACK_MAGIC:
                print("Invalid packed file format")
                return False
            
            print("Packed file info:")
            print(f"  Original size: {header.original_size} bytes")
            print(f"  Compressed size: {header.compressed_size} bytes")
            print(f"  Encryption type: {header.encryption_type}")
            
            # Decrypt if needed
            compressed = processed
            if header.encryption_type == 1:
                compressed = xor_crypt(processed, self.key)
                print("Applied XOR decryption")
            elif header.encryption_type == 2:
                compressed = aes_crypt(processed, self.key, encrypt=False)
                print("Applied AES decryption")
            
            # Decompress
            data = decompress_data(compressed)
            print(f"Decompressed size: {len(data)} bytes")
            
            # Write output file
            with open(output_file, 'wb') as f:
                f.write(data)
            
            print(f"Successfully unpacked to: {output_file}")
            return True
            
        except Exception as e:
            print(f"Unpacking failed: {e}")
            return False
    
    def list_file(self, input_file: str) -> bool:
        """
        List packed file info without unpacking
        
        Args:
            input_file: Path to input file
        
        Returns:
            True if successful, False otherwise
        """
        try:
            with open(input_file, 'rb') as f:
                header_data = f.read(HEADER_SIZE)
            
            header = PackHeader.unpack(header_data)
            
            if header.magic != PACK_MAGIC:
                print("Invalid packed file format")
                return False
            
            print(f"Packed file: {input_file}")
            print(f"  Magic: 0x{header.magic:08X}")
            print(f"  Original size: {header.original_size} bytes")
            print(f"  Compressed size: {header.compressed_size} bytes")
            
            if header.original_size > 0:
                compression_ratio = (100.0 * header.compressed_size / header.original_size)
                print(f"  Compression ratio: {compression_ratio:.1f}%")
            
            encryption_names = {0: "None", 1: "XOR", 2: "AES"}
            encryption_name = encryption_names.get(header.encryption_type, "Unknown")
            print(f"  Encryption type: {header.encryption_type} ({encryption_name})")
            
            return True
            
        except Exception as e:
            print(f"Listing failed: {e}")
            return False

def print_usage(program_name: str):
    """Print usage information"""
    print("Advanced Packer - Compress and encrypt files with reversible encryption")
    print("Usage:")
    print(f"  {program_name} pack <input> <output> [encryption_type]")
    print(f"  {program_name} unpack <input> <output>")
    print(f"  {program_name} list <input>")
    print("\nEncryption types:")
    print("  0 - No encryption (compression only)")
    print("  1 - XOR encryption (default)")
    print("  2 - AES encryption")
    print("\nExamples:")
    print(f"  {program_name} pack document.txt document.pak 1")
    print(f"  {program_name} unpack document.pak document_restored.txt")
    print(f"  {program_name} list document.pak")

def main():
    """Main function"""
    if len(sys.argv) < 3:
        print_usage(sys.argv[0])
        return 1
    
    command = sys.argv[1]
    packer = AdvancedPacker()
    
    if command == "pack":
        if len(sys.argv) < 4:
            print("Error: pack command requires input and output files")
            return 1
        
        input_file = sys.argv[2]
        output_file = sys.argv[3]
        encryption_type = int(sys.argv[4]) if len(sys.argv) > 4 else 1
        
        if not packer.pack_file(input_file, output_file, encryption_type):
            return 1
    
    elif command == "unpack":
        if len(sys.argv) < 4:
            print("Error: unpack command requires input and output files")
            return 1
        
        input_file = sys.argv[2]
        output_file = sys.argv[3]
        
        if not packer.unpack_file(input_file, output_file):
            return 1
    
    elif command == "list":
        if len(sys.argv) < 3:
            print("Error: list command requires input file")
            return 1
        
        input_file = sys.argv[2]
        
        if not packer.list_file(input_file):
            return 1
    
    else:
        print(f"Unknown command: {command}")
        print_usage(sys.argv[0])
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())