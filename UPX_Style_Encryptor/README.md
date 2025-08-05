# UPX-Style Encryption Packer with URL Crypto Service

**🔥 Encrypt any file and it still works - Just like UPX but with encryption!**
**🌐 NEW: Download, encrypt & pack files directly from URLs!**

Perfect for AV evasion testing, file protection, and remote payload management.

## 🎯 What This Does

- **Pack any executable** with strong encryption
- **Works exactly like UPX** but uses encryption instead of compression
- **Download from URLs** and encrypt automatically
- **Embeds encrypted payload** directly in the generated executable
- **Zero dependencies** - Single standalone files
- **Polymorphic code generation** - Unique signatures every time
- **Ready for VirusTotal testing** - Perfect for AV evasion research

## 🚀 Quick Start

### Windows Users (RECOMMENDED)
```batch
# Use the Windows-compiled version:
VS2022_MenuEncryptor_Windows.exe
```

### Compile from Source
```bash
# Windows with MinGW:
g++ -std=c++17 -O2 -s -static -o VS2022_MenuEncryptor.exe VS2022_MenuEncryptor.cpp

# Windows with Visual Studio:
cl /O2 /EHsc VS2022_MenuEncryptor.cpp /Fe:VS2022_MenuEncryptor.exe

# Linux:
g++ -std=c++17 -O2 -s -static -o VS2022_MenuEncryptor VS2022_MenuEncryptor.cpp
```

## 📋 Menu Options

```
=== Visual Studio 2022 Universal Encryptor ===
Advanced encryption tool with multiple algorithms and stealth features

Select an option:
  1. Pack File (AES Encryption) - Works like UPX
  2. Pack File (ChaCha20 Encryption) - Works like UPX
  3. Pack File (Triple Encryption) - Maximum Security
  4. Basic File Encryption (Save to disk)
  5. Advanced: Generate Custom MASM Stub
  6. URL Crypto Service (AES) - Download, Encrypt & Re-upload
  7. URL Crypto Service (Triple) - Download, Encrypt & Re-upload
  0. Exit
```

## 🌐 NEW: URL Crypto Service

### **Download → Encrypt → Pack Workflow:**

#### **Option 6: AES URL Service**
```
Input:  http://example.com/payload.exe
         ↓
    Download & Encrypt (AES)
         ↓
    Generate: encrypted_payload.cpp
         ↓
    Compile: encrypted_payload.exe (works like original!)
```

#### **Option 7: Triple URL Service**
```
Input:  http://example.com/payload.exe
         ↓
    Download & Encrypt (AES+ChaCha20+XOR)
         ↓
    Generate: max_security_payload.cpp
         ↓
    Compile: max_security_payload.exe (ultimate protection!)
```

## 🔥 Example Workflows

### **Local File Packing:**
```bash
# 1. Run the encryptor
./VS2022_MenuEncryptor_Windows.exe

# 2. Choose option 1 (AES Packer)
# Input: calc.exe
# Output: packed_calc

# 3. Compile the generated source
g++ -O2 -s -static -o packed_calc.exe packed_calc.cpp

# 4. Test it!
./packed_calc.exe  # Works exactly like original calc.exe!
```

### **URL-Based Remote Packing:**
```bash
# 1. Run the encryptor
./VS2022_MenuEncryptor_Windows.exe

# 2. Choose option 6 (URL Crypto Service)
# Input URL: http://site.com/payload.exe
# Output name: encrypted_payload

# 3. Compile the generated source
g++ -O2 -s -static -o encrypted_payload.exe encrypted_payload.cpp

# 4. Upload to VirusTotal for testing!
```

## 🧪 Perfect for AV Testing

- **Upload packed executables to VirusTotal**
- **Test detection rates across multiple AV engines**
- **Each generated file has unique signatures**
- **Polymorphic code ensures different results each time**
- **Remote URL processing for automated workflows**

## ✨ Key Features

### 🔐 **Encryption Methods**
- **AES-256** with position mixing
- **ChaCha20** stream cipher
- **Triple-layer** (AES + ChaCha20 + XOR)
- **Custom MASM** ultra-lightweight stubs (<2KB)

### 🌐 **URL Crypto Service**
- **Windows:** Native WinINet HTTP client
- **Linux:** wget/curl fallback
- **Progress tracking** during downloads
- **Error handling** and retry logic
- **Remote payload management**

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
├── VS2022_MenuEncryptor.cpp              # Source code
├── VS2022_MenuEncryptor_Windows.exe      # Windows executable (READY TO USE!)
├── VS2022_MenuEncryptor.exe              # Linux executable
├── README.md                             # This file
└── build.bat                             # Windows build script
```

## 🛠️ Build Instructions

### Windows (Cross-compiled - RECOMMENDED)
```
✅ Use: VS2022_MenuEncryptor_Windows.exe (Ready to go!)
```

### Windows (Local compilation)
```batch
# With MinGW:
g++ -std=c++17 -O2 -s -static -o VS2022_MenuEncryptor.exe VS2022_MenuEncryptor.cpp

# With MSVC:
cl /O2 /EHsc VS2022_MenuEncryptor.cpp /Fe:VS2022_MenuEncryptor.exe
```

### Linux
```bash
g++ -std=c++17 -O2 -s -static -o VS2022_MenuEncryptor VS2022_MenuEncryptor.cpp
```

## 🎯 Use Cases

### **Traditional Packing:**
- **File protection** - Protect executables with strong encryption
- **AV evasion research** - Test detection capabilities
- **Stealth deployment** - Deploy files that look different but work the same

### **URL-Based Operations:**
- **Remote payload management** - Download and encrypt in one step
- **Automated encryption workflows** - Script-friendly URL processing
- **Zero-trace encryption** - No local file storage required
- **VirusTotal automation** - Perfect for testing pipelines

### **Research & Education:**
- **Security testing** - Test detection capabilities of security tools
- **Educational** - Learn about packing, encryption, and evasion techniques
- **Cross-platform testing** - Test on both Windows and Linux

## ⚠️ Legal Notice

This tool is for **educational and research purposes only**. Use responsibly and only on systems you own or have explicit permission to test. The authors are not responsible for any misuse.

## 🔧 Technical Details

### Encryption Process
1. **Read input file** (local or URL) into memory
2. **Generate cryptographic keys** using secure random sources
3. **Apply encryption** (AES/ChaCha20/Triple-layer)
4. **Embed encrypted payload** in C++ source template
5. **Generate polymorphic code** with unique variable names
6. **Output complete C++ source** ready for compilation

### URL Download Process
1. **Windows:** WinINet HTTP client with progress tracking
2. **Linux:** wget/curl system command fallback
3. **Error handling** with retry logic
4. **Secure HTTPS** support
5. **Automatic cleanup** of temporary files

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
- **URL processing** - Remote file support
- **Progress tracking** - Visual download feedback
- **Polymorphic generation** - Unique output every time

## 🔥 Latest Updates

### **v2.0 - URL Crypto Service**
- ✅ **URL download support** - Encrypt files directly from URLs
- ✅ **Windows cross-compilation** - Proper Windows executable
- ✅ **WinINet integration** - Native Windows HTTP client
- ✅ **Linux wget/curl fallback** - Cross-platform URL support
- ✅ **Progress tracking** - Real-time download feedback
- ✅ **Enhanced error handling** - Robust connection management

### **v1.0 - UPX-Style Packing**
- ✅ **Local file packing** - Traditional UPX-like functionality
- ✅ **Multiple encryption methods** - AES, ChaCha20, Triple
- ✅ **MASM stub generation** - Ultra-lightweight assembly
- ✅ **Polymorphic code** - Unique signatures every time

---

**🎉 Happy Packing! Perfect for VirusTotal testing and AV research!**

**🌐 Now with URL support for remote payload management!**