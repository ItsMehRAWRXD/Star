# UPX-Style Encryption Packer

**🔥 Encrypt any file and it still works - Just like UPX but with encryption!**

Perfect for AV evasion testing, file protection, and transparent deployment.

## 🎯 What This Does

- **Pack any executable** with strong encryption
- **Works exactly like UPX** but uses encryption instead of compression
- **Embeds encrypted payload** directly in the generated executable
- **Zero dependencies** - Single standalone files
- **Polymorphic code generation** - Unique signatures every time
- **Ready for VirusTotal testing** - Perfect for AV evasion research

## 🚀 Quick Start

### Option 1: Use Pre-compiled Binary
```bash
# Just run it!
./VS2022_MenuEncryptor.exe
```

### Option 2: Compile from Source
```bash
# Compile the encryptor
g++ -std=c++17 -O2 -s -static -o VS2022_MenuEncryptor.exe VS2022_MenuEncryptor.cpp

# Run it
./VS2022_MenuEncryptor.exe
```

## 📋 Menu Options

```
Select an option:
  1. Pack File (AES Encryption) - Works like UPX
  2. Pack File (ChaCha20 Encryption) - Works like UPX  
  3. Pack File (Triple Encryption) - Maximum Security
  4. Basic File Encryption (Save to disk)
  5. Advanced: Generate Custom MASM Stub
  0. Exit
```

## 🔥 Example Workflow

```bash
# 1. Run the encryptor
./VS2022_MenuEncryptor.exe

# 2. Choose option 1 (AES Packer)
# Input: calc.exe
# Output: packed_calc

# 3. Compile the generated source
g++ -O2 -s -static -o packed_calc.exe packed_calc.cpp

# 4. Test it!
./packed_calc.exe  # Works exactly like original calc.exe!

# 5. Upload to VirusTotal for AV testing
```

## 🧪 Perfect for AV Testing

- **Upload packed executables to VirusTotal**
- **Test detection rates across multiple AV engines**
- **Each generated file has unique signatures**
- **Polymorphic code ensures different results each time**

## ✨ Key Features

### 🔐 **Encryption Methods**
- **AES-256** with position mixing
- **ChaCha20** stream cipher
- **Triple-layer** (AES + ChaCha20 + XOR)
- **Custom MASM** ultra-lightweight stubs (<2KB)

### 🎭 **Polymorphic Generation**
- **Unique variable names** every execution
- **Randomized function names**
- **Different code signatures** each time
- **Unlimited unique stubs**

### 🚀 **Deployment**
- **Zero dependencies** - Single file deployment
- **Cross-platform** - Windows & Linux
- **Self-contained** - No external files needed
- **Drop and run** - Works anywhere

## 📁 File Structure

```
UPX_Style_Encryptor/
├── VS2022_MenuEncryptor.cpp    # Source code
├── VS2022_MenuEncryptor.exe    # Pre-compiled binary
├── README.md                   # This file
└── build.bat                   # Windows build script
```

## 🛠️ Build Instructions

### Windows (with g++)
```batch
g++ -std=c++17 -O2 -s -static -o VS2022_MenuEncryptor.exe VS2022_MenuEncryptor.cpp
```

### Windows (with MSVC)
```batch
cl /O2 /EHsc VS2022_MenuEncryptor.cpp /Fe:VS2022_MenuEncryptor.exe
```

### Linux
```bash
g++ -std=c++17 -O2 -s -static -o VS2022_MenuEncryptor VS2022_MenuEncryptor.cpp
```

## 🎯 Use Cases

- **AV Evasion Research** - Test how different AV engines detect encrypted files
- **File Protection** - Protect executables with strong encryption
- **Stealth Deployment** - Deploy files that look different but work the same
- **Security Testing** - Test detection capabilities of security tools
- **Educational** - Learn about packing, encryption, and evasion techniques

## ⚠️ Legal Notice

This tool is for **educational and research purposes only**. Use responsibly and only on systems you own or have explicit permission to test. The authors are not responsible for any misuse.

## 🔧 Technical Details

### Encryption Process
1. **Read input file** into memory
2. **Generate cryptographic keys** using secure random sources
3. **Apply encryption** (AES/ChaCha20/Triple-layer)
4. **Embed encrypted payload** in C++ source template
5. **Generate polymorphic code** with unique variable names
6. **Output complete C++ source** ready for compilation

### Execution Process
1. **Decrypt embedded payload** in memory
2. **Write to temporary file** 
3. **Execute via CreateProcess/system**
4. **Clean up temporary file**
5. **Exit cleanly**

## 🚀 Advanced Features

- **Runtime-only operation** - Original files remain untouched
- **Memory-only decryption** - No permanent disk writes
- **Anti-debugging hooks** - Basic evasion techniques
- **Cross-platform compatibility** - Windows and Linux support
- **Optimized compilation** - Small, fast executables

---

**🎉 Happy Packing! Perfect for VirusTotal testing and AV research!**