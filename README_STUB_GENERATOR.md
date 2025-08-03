# Native Stub Generator - Unlimited Stub Creation

## 🎯 Overview

The **Native Stub Generator** is a pure C++ tool that creates unlimited stubs on the fly for packing using the native_encryptor. It embeds encrypted payloads into self-contained C++ executables with zero external dependencies.

## 🚀 Features

### **✅ Core Features**
- **Pure C++ Implementation** - Zero external dependencies
- **AES-128-CTR Encryption** - Industry-standard encryption
- **Unlimited Stub Generation** - Create as many stubs as needed
- **Multiple Stub Types** - Basic, Advanced, and Minimal variants
- **Obfuscated Keys** - Keys and nonces are obfuscated in generated stubs
- **Random Key Generation** - Each stub gets unique encryption keys
- **Environment Variable Support** - Use ENCRYPTION_KEY for consistent keys

### **🔧 Stub Types**

#### **1. Basic Stub**
- Standard AES-128-CTR decryption
- Full S-box implementation
- Clean, readable code structure
- Suitable for most use cases

#### **2. Advanced Stub**
- Anti-debugging techniques
- Polymorphic decryption
- Timing-based detection
- Random output filenames
- Enhanced obfuscation

#### **3. Minimal Stub**
- Smallest possible stub size
- Simplified decryption
- Minimal dependencies
- Fast compilation and execution

## 🛠️ Installation & Building

### **Build the Stub Generator**
```bash
# Build all native components (including stub generator)
make native

# Or build just the stub generator
make native_stub_generator
```

### **Install System-Wide**
```bash
# Install to /usr/local/bin
make install
```

## 📖 Usage

### **Basic Usage**
```bash
# Generate a basic stub
./native_stub_generator <input_file> <output_stub.cpp> [stub_type] [use_random_key]

# Examples
./native_stub_generator payload.bin stub.cpp
./native_stub_generator payload.bin stub.cpp advanced
./native_stub_generator payload.bin stub.cpp minimal 0
```

### **Command Line Arguments**
- **`input_file`** - File to embed in the stub (required)
- **`output_stub.cpp`** - Generated C++ stub file (required)
- **`stub_type`** - Type of stub: `basic`, `advanced`, `minimal` (default: `basic`)
- **`use_random_key`** - Use random key (1) or environment variable (0) (default: 1)

### **Environment Variables**
```bash
# Set encryption key for consistent stubs
export ENCRYPTION_KEY="your-secure-key-here"

# Generate stub with environment key
./native_stub_generator payload.bin stub.cpp basic 0
```

## 🔄 Complete Workflow

### **Step 1: Create Your Payload**
```bash
# Create a test payload
echo "This is my secret payload" > my_payload.txt
```

### **Step 2: Generate the Stub**
```bash
# Generate a basic stub
./native_stub_generator my_payload.txt my_stub.cpp basic

# Generate an advanced stub
./native_stub_generator my_payload.txt my_stub_advanced.cpp advanced
```

### **Step 3: Compile the Stub**
```bash
# Compile the generated stub
g++ -std=c++17 -o my_stub my_stub.cpp

# Compile advanced stub
g++ -std=c++17 -o my_stub_advanced my_stub_advanced.cpp
```

### **Step 4: Execute the Stub**
```bash
# Run the stub to extract payload
./my_stub

# Check the extracted file
cat decrypted_output.bin
```

## 📊 Stub Comparison

| Feature | Basic | Advanced | Minimal |
|---------|-------|----------|---------|
| **Size** | ~3.5KB | ~4.2KB | ~1.8KB |
| **Anti-Debug** | ❌ | ✅ | ❌ |
| **Polymorphic** | ❌ | ✅ | ❌ |
| **Obfuscation** | Basic | Advanced | Minimal |
| **Compilation Speed** | Fast | Medium | Very Fast |
| **Execution Speed** | Fast | Medium | Very Fast |
| **Security Level** | High | Very High | Medium |

## 🔐 Security Features

### **Encryption**
- **AES-128-CTR** - Industry-standard encryption
- **Random Nonces** - Each stub gets unique nonce
- **Key Obfuscation** - Keys are obfuscated in hex format
- **Variable Obfuscation** - Random variable names

### **Anti-Analysis**
- **Anti-Debugging** - Timing-based detection
- **Code Obfuscation** - Random variable names and structure
- **Polymorphic Decryption** - Multiple decryption layers
- **Random Output Names** - Dynamic filename generation

## 🧪 Testing

### **Test Stub Generation**
```bash
# Create test payload
echo "Test payload content" > test_payload.txt

# Generate different stub types
./native_stub_generator test_payload.txt test_basic.cpp basic
./native_stub_generator test_payload.txt test_advanced.cpp advanced
./native_stub_generator test_payload.txt test_minimal.cpp minimal

# Compile and test
g++ -std=c++17 -o test_basic test_basic.cpp
g++ -std=c++17 -o test_advanced test_advanced.cpp
g++ -std=c++17 -o test_minimal test_minimal.cpp

# Execute stubs
./test_basic
./test_advanced
./test_minimal
```

### **Verify Output**
```bash
# Check extracted files
cat decrypted_output.bin  # Basic stub output
ls output_*.bin          # Advanced stub output
cat output.bin           # Minimal stub output
```

## 🔧 Integration with Native Encryptor

### **Workflow Integration**
```bash
# 1. Encrypt file with native_encryptor
./native_encryptor secret_file.txt secret_file.enc

# 2. Generate stub with encrypted file
./native_stub_generator secret_file.enc stub.cpp basic

# 3. Compile and distribute stub
g++ -std=c++17 -o secret_stub stub.cpp
```

### **Batch Processing**
```bash
# Process multiple files
for file in *.txt; do
    ./native_stub_generator "$file" "${file%.txt}_stub.cpp" basic
done
```

## 📋 Examples

### **Example 1: Basic Stub**
```bash
# Generate basic stub
./native_stub_generator document.pdf doc_stub.cpp basic

# Compile
g++ -std=c++17 -o doc_stub doc_stub.cpp

# Execute
./doc_stub
# Output: decrypted_output.bin (contains document.pdf)
```

### **Example 2: Advanced Stub**
```bash
# Generate advanced stub
./native_stub_generator malware.exe malware_stub.cpp advanced

# Compile
g++ -std=c++17 -o malware_stub malware_stub.cpp

# Execute
./malware_stub
# Output: output_[timestamp].bin (contains malware.exe)
```

### **Example 3: Minimal Stub**
```bash
# Generate minimal stub
./native_stub_generator config.json config_stub.cpp minimal

# Compile
g++ -std=c++17 -o config_stub config_stub.cpp

# Execute
./config_stub
# Output: output.bin (contains config.json)
```

## 🚨 Important Notes

### **Security Considerations**
- **Key Management** - Store keys securely
- **Stub Distribution** - Protect generated stubs
- **Payload Size** - Large payloads increase stub size
- **Compilation** - Compile on target platform for best compatibility

### **Best Practices**
- **Use Advanced Stubs** for sensitive payloads
- **Use Environment Keys** for consistent encryption
- **Test Thoroughly** before deployment
- **Monitor Execution** for anti-debugging triggers

### **Limitations**
- **Stub Size** - Increases with payload size
- **Compilation Time** - Depends on payload size
- **Platform Dependency** - Compile on target platform
- **Memory Usage** - Loads entire payload into memory

## 🔄 Troubleshooting

### **Common Issues**

#### **Compilation Errors**
```bash
# Missing cstdint header
# Solution: Stub generator includes this automatically

# Variable name errors
# Solution: Fixed in latest version - ensures valid C++ names
```

#### **Decryption Issues**
```bash
# Wrong key/nonce
# Solution: Use same key for encryption and stub generation

# File corruption
# Solution: Verify input file integrity
```

#### **Execution Issues**
```bash
# Permission denied
# Solution: chmod +x stub_executable

# Missing dependencies
# Solution: Stubs have zero dependencies
```

## 📞 Support

### **Help Command**
```bash
./native_stub_generator
# Shows usage information and examples
```

### **Debug Mode**
```bash
# Compile with debug information
g++ -std=c++17 -g -o debug_stub stub.cpp
```

### **Verbose Output**
```bash
# Check stub generation details
./native_stub_generator payload.txt stub.cpp basic
# Shows key, nonce, and file sizes
```

---

**The Native Stub Generator provides unlimited, secure stub creation with zero dependencies. Perfect for embedding encrypted payloads into self-contained executables!**