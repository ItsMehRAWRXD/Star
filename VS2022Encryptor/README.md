# Visual Studio 2022 Universal PE Packer

A professional-grade PE (Portable Executable) packer that creates self-decrypting executables with embedded encryption keys. Similar to commercial packers like Themida and VMProtect.

## 🚀 Features

### **Encryption Modes:**
- **Basic Encryption** - AES-128-CTR and ChaCha20 with headers
- **Raw Binary Output** - Pure encrypted data without headers
- **Stealth Triple Encryption** - 3-layer encryption (AES + ChaCha20 + XOR) with decimal key representation
- **PE-Aware Encryption** - Self-decrypting executables with embedded keys

### **Security Features:**
- **Triple-layer encryption** - Maximum protection against reverse engineering
- **Embedded keys** - Self-contained operation, no external key files
- **PE structure preservation** - Executables remain functional
- **Stealth key storage** - Decimal representation for obfuscation
- **Randomized encryption order** - Fisher-Yates shuffle for additional security

## 📋 Requirements

- **Visual Studio 2022** (Community, Professional, or Enterprise)
- **Windows SDK** (included with Visual Studio)
- **C++17** or later

## 🛠️ Building

### **Method 1: Visual Studio IDE**
1. Open `vs2022_encryptor.sln` in Visual Studio 2022
2. Select your target configuration (Debug/Release, x86/x64)
3. Build the solution (Ctrl+Shift+B)

### **Method 2: Command Line**
```batch
# Build for x64 Release
build.bat

# Or manually:
msbuild vs2022_encryptor.sln /p:Configuration=Release /p:Platform=x64
```

## 🎯 Usage

### **Interactive Menu:**
```
VS2022Encryptor.exe
```

**Available Options:**
1. **Basic Encryption (AES/ChaCha20)** - Standard encryption with algorithm headers
2. **Basic Encryption - Raw Binary Output** - Pure encrypted data, no headers
3. **Stealth Triple Encryption** - 3-layer encryption with decimal keys
4. **PE-Aware Encryption** - Self-decrypting executables (like Themida/VMProtect)

### **PE-Aware Encryption Example:**
```
Input: calc.exe
Output: encrypted_calc.exe
Result: Self-decrypting executable that runs normally
```

## 🔒 How It Works

### **PE-Aware Encryption Process:**
1. **Load PE file** - Parse DOS, NT, and section headers
2. **Identify code sections** - Find sections with `IMAGE_SCN_CNT_CODE` flag
3. **Apply triple encryption** - AES + ChaCha20 + XOR in randomized order
4. **Embed keys** - Store encryption keys in new PE section
5. **Preserve structure** - Maintain executable compatibility

### **Runtime Decryption:**
1. **Load self** - Executable reads its own encrypted code sections
2. **Extract keys** - Retrieve embedded keys from PE section
3. **Decrypt in memory** - Apply decryption layers in reverse order
4. **Execute normally** - Run the decrypted program

## 📁 Project Structure

```
VS2022Encryptor/
├── vs2022_encryptor.sln          # Visual Studio solution
├── VS2022Encryptor/
│   ├── VS2022Encryptor.vcxproj   # Project file
│   ├── main.cpp                  # Main application and menu
│   ├── encryptor.h               # Basic AES/ChaCha20 encryption
│   ├── stealth_triple_encryptor.h/.cpp  # Triple-layer encryption
│   ├── pe_encryptor.h/.cpp       # PE-aware encryption
│   ├── build.bat                 # Build script
│   ├── README.md                 # This file
│   └── .gitignore               # Git ignore rules
└── test_example.txt             # Test file for encryption
```

## 🔧 Technical Details

### **Encryption Algorithms:**
- **AES-128-CTR** - Advanced Encryption Standard in Counter Mode
- **ChaCha20** - High-speed stream cipher
- **XOR** - Simple byte-wise XOR with variable-length keys

### **Key Management:**
- **32-byte AES keys** - Extended key length for maximum security
- **32-byte ChaCha20 keys** - Standard ChaCha20 key size
- **Variable XOR keys** - 32-64 bytes for additional entropy
- **Decimal representation** - Keys stored as large decimal strings

### **PE Section Management:**
- **New .keys section** - Dedicated section for key storage
- **Header updates** - Automatic PE header modification
- **Size calculations** - Proper virtual and raw size management

## 🛡️ Security Features

### **Anti-Reverse Engineering:**
- **Code section encryption** - Protects program logic
- **Stealth key storage** - Keys hidden in decimal format
- **Randomized encryption order** - Prevents pattern analysis
- **PE structure preservation** - Maintains file integrity

### **Professional Quality:**
- **Self-contained operation** - No external dependencies
- **Universal compatibility** - Works with any Windows executable
- **Commercial-grade protection** - Similar to Themida/VMProtect

## 📝 Examples

### **Encrypting an Executable:**
```
VS2022Encryptor.exe
Select: 4. PE-Aware Encryption
Input: myapp.exe
Output: protected_myapp.exe
```

**Result:** `protected_myapp.exe` - Self-decrypting executable that runs normally but has encrypted code sections.

### **File Comparison:**
```
Original: myapp.exe (1.2 MB)
Encrypted: protected_myapp.exe (1.2 MB + key section)
```

## ⚠️ Important Notes

- **Backup original files** - Always keep unencrypted copies
- **Test thoroughly** - Verify encrypted executables work correctly
- **Legal use only** - Intended for software protection, not malicious purposes
- **Windows only** - PE format is Windows-specific

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## 📄 License

This project is for educational and legitimate software protection purposes only. Users are responsible for ensuring compliance with applicable laws and regulations.

## 🎯 Use Cases

- **Software protection** - Protect commercial applications
- **License management** - Add protection layers to licensed software
- **Anti-reverse engineering** - Prevent code analysis
- **Professional deployment** - Secure software distribution

---

**Built with ❤️ using Visual Studio 2022 and C++**