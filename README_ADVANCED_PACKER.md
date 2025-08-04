# Advanced Packer

A sophisticated file packing utility that combines compression with reversible encryption. The Advanced Packer can compress files using zlib and encrypt them using XOR or AES encryption, providing both space savings and security.

## Features

- **Compression**: Uses zlib for efficient data compression
- **Reversible Encryption**: Supports XOR and AES encryption methods
- **Cross-Platform**: Available in both C++ and Python implementations
- **Flexible**: Choose between compression-only, XOR encryption, or AES encryption
- **Header-based**: Includes metadata for easy identification and unpacking
- **Key Management**: Supports environment variable-based key configuration

## Encryption Methods

### 1. XOR Encryption (Type 1)
- Fast and lightweight
- Symmetric encryption (same key for encrypt/decrypt)
- Good for basic obfuscation
- Uses the same key system as other components in this project

### 2. AES Encryption (Type 2)
- Industry-standard AES-128 encryption
- Uses CTR mode for symmetric operation
- More secure than XOR
- Requires PyCryptodome library (Python version)

### 3. No Encryption (Type 0)
- Compression only
- Useful when you need space savings without encryption

## File Format

Packed files use a custom format with the following structure:

```
[Header (16 bytes)] [Compressed/Encrypted Data]
```

### Header Structure
- **Magic Number** (4 bytes): `0x4B434150` ("PACK")
- **Original Size** (4 bytes): Size of the original uncompressed file
- **Compressed Size** (4 bytes): Size of the compressed data
- **Encryption Type** (1 byte): 0=none, 1=XOR, 2=AES
- **Reserved** (3 bytes): Reserved for future use

## Installation

### C++ Version
```bash
# Compile with zlib dependency
make advanced_packer

# Or compile manually
g++ -std=c++17 -O2 -Wall -o advanced_packer advanced_packer.cpp -lz
```

### Python Version
```bash
# No additional dependencies required for basic functionality
python3 advanced_packer.py

# For AES encryption support
pip install pycryptodome
```

## Usage

### Command Line Interface

#### Pack a file
```bash
# Pack with XOR encryption (default)
./advanced_packer pack input.txt output.pak

# Pack with specific encryption type
./advanced_packer pack input.txt output.pak 1  # XOR
./advanced_packer pack input.txt output.pak 2  # AES
./advanced_packer pack input.txt output.pak 0  # No encryption

# Python version
python3 advanced_packer.py pack input.txt output.pak 1
```

#### Unpack a file
```bash
./advanced_packer unpack input.pak output.txt
python3 advanced_packer.py unpack input.pak output.txt
```

#### List file information
```bash
./advanced_packer list input.pak
python3 advanced_packer.py list input.pak
```

### Programmatic Usage (Python)

```python
from advanced_packer import AdvancedPacker

# Create packer instance
packer = AdvancedPacker()

# Pack a file with XOR encryption
packer.pack_file("document.txt", "document.pak", encryption_type=1)

# Unpack a file
packer.unpack_file("document.pak", "document_restored.txt")

# List file information
packer.list_file("document.pak")
```

## Key Management

The packer uses the same key management system as other components in this project:

### Environment Variable
Set the `ENCRYPTION_KEY` environment variable to use a custom key:
```bash
export ENCRYPTION_KEY="your-secret-key-here"
./advanced_packer pack input.txt output.pak
```

### Default Key
If no environment variable is set, the packer uses a hardcoded obfuscated key that is deobfuscated at runtime.

## Examples

### Basic Usage
```bash
# Create a test file
echo "This is a test file with some content for compression." > test.txt

# Pack with XOR encryption
./advanced_packer pack test.txt test.pak

# List information
./advanced_packer list test.pak

# Unpack
./advanced_packer unpack test.pak test_restored.txt

# Verify
diff test.txt test_restored.txt
```

### Large File Compression
```bash
# Create a large file with repetitive content
for i in {1..1000}; do echo "This is line $i with repetitive content for testing compression."; done > large.txt

# Pack with compression only
./advanced_packer pack large.txt large.pak 0

# Check compression ratio
./advanced_packer list large.pak
```

### Binary File Handling
```bash
# Pack a binary file
./advanced_packer pack image.jpg image.pak

# Unpack the binary file
./advanced_packer unpack image.pak image_restored.jpg
```

## Testing

Run the comprehensive test suite:

```bash
# Python test suite
python3 test_advanced_packer.py

# Or test individual components
python3 -c "
from advanced_packer import AdvancedPacker
packer = AdvancedPacker()
packer.pack_file('test.txt', 'test.pak', 1)
packer.list_file('test.pak')
packer.unpack_file('test.pak', 'test_restored.txt')
"
```

## Performance

### Compression Ratios
- **Text files**: Typically 20-80% compression depending on content
- **Binary files**: Varies widely based on file type and content
- **Already compressed files**: Minimal additional compression

### Speed
- **XOR encryption**: Very fast, minimal overhead
- **AES encryption**: Fast with hardware acceleration
- **Compression**: Depends on file size and content

## Security Considerations

### XOR Encryption
- Provides basic obfuscation
- Not cryptographically secure
- Suitable for lightweight protection

### AES Encryption
- Cryptographically secure when used properly
- Uses CTR mode for symmetric operation
- Key management is critical for security

### General Security
- Keys should be kept secure
- Use environment variables for key storage
- Consider the security implications of your use case

## Troubleshooting

### Common Issues

1. **"zlib not found" (C++ version)**
   ```bash
   # Install zlib development package
   sudo apt-get install libz-dev  # Ubuntu/Debian
   sudo yum install zlib-devel    # CentOS/RHEL
   ```

2. **"PyCryptodome not available" (Python AES)**
   ```bash
   pip install pycryptodome
   ```

3. **"Invalid packed file format"**
   - Ensure the file is a valid packed file
   - Check if the file is corrupted
   - Verify the magic number in the header

4. **"Decompression failed"**
   - File may be corrupted
   - Check if the packed file is complete
   - Verify the original size in the header

### Debug Mode
For debugging, you can examine the packed file structure:
```bash
# View file header (first 16 bytes)
hexdump -C file.pak | head -1

# Expected header format for XOR encryption:
# 50 41 43 4B  [magic: "PACK"]
# XX XX XX XX  [original size]
# YY YY YY YY  [compressed size]
# 01           [encryption type: XOR]
# 00 00 00     [reserved]
```

## Integration

The Advanced Packer can be integrated with other components in this project:

- Use with `native_encryptor` for additional encryption layers
- Combine with `packer` for multi-stage packing
- Integrate with GUI components for user-friendly interfaces

## License

This component is part of the larger encryption project and follows the same licensing terms.

## Contributing

When contributing to the Advanced Packer:

1. Maintain compatibility with existing packed files
2. Test with various file types and sizes
3. Ensure proper error handling
4. Update documentation for new features
5. Add tests for new functionality