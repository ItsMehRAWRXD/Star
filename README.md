# File Encryption Project

A comprehensive file encryption/decryption system with both **pure native C++ implementation** (zero dependencies) and **legacy implementations** (with external dependencies).

## 🏗️ Project Structure

### **🟢 Native Implementation (Recommended)**
*Pure C++ with zero external dependencies*

- **`native_encryptor`** - AES-128-CTR file encryption
- **`native_xll_dropper`** - AES-128-CTR file decryption
- **`native_gui`** - Console-based user interface
- **`native_stub_generator`** - Unlimited stub creation for packing

### **🟡 Legacy Implementation (With Dependencies)**
*Original implementations requiring external libraries*

- **`encryptor`** - OpenSSL-based AES-128-CTR encryption
- **`xll_dropper`** - OpenSSL-based AES-128-CTR decryption
- **`dragdrop_encryptor_gui.py`** - PyQt5-based GUI application
- **`packer`** - File combination utility

## 🚀 Quick Start

### **Option 1: Native Implementation (Recommended)**

```bash
# Build native components (no dependencies required)
make all

# Encrypt a file
./native_encryptor your_file.txt your_file.enc

# Decrypt a file
./native_xll_dropper your_file.enc your_file_decrypted.txt

# Generate a stub for packing
./native_stub_generator your_file.enc stub.cpp basic

# Use interactive GUI
./native_gui
```

### **Option 2: Legacy Implementation**

```bash
# Install dependencies
sudo apt install libssl-dev python3-pycryptodome python3-pyqt5

# Build legacy components
make encryptor xll_dropper packer

# Use legacy components
./encryptor input.txt output.enc
./xll_dropper input.enc output.txt
python3 dragdrop_encryptor_gui.py
```

## 📁 File Organization

```
├── 🟢 NATIVE IMPLEMENTATION (Zero Dependencies)
│   ├── native_encryptor.cpp      # Pure C++ AES-128-CTR encryptor
│   ├── native_xll_dropper.cpp    # Pure C++ AES-128-CTR decryption
│   ├── native_gui.cpp           # Console-based GUI
│   ├── native_stub_generator.cpp # Unlimited stub generator
│   ├── README_NATIVE.md         # Native implementation docs
│   └── README_STUB_GENERATOR.md # Stub generator documentation
│
├── 🟡 LEGACY IMPLEMENTATION (With Dependencies)
│   ├── encryptor.cpp            # OpenSSL-based encryptor
│   ├── xll_dropper.cpp          # OpenSSL-based XLL dropper
│   ├── dragdrop_encryptor_gui.py # PyQt5 GUI application
│   ├── packer.cpp               # File combination utility
│   └── requirements.txt         # Python dependencies
│
├── 🧪 TESTING & DOCUMENTATION
│   ├── test_encryption.py       # Python test suite
│   ├── test_xll_dropper.py      # XLL dropper tests
│   ├── test_native_comprehensive.py # Native implementation tests
│   ├── test_results.md          # Test results documentation
│   └── xll_dropper_test_report.md # XLL dropper test report
│
├── 🔧 BUILD & CONFIGURATION
│   ├── Makefile                 # Build automation
│   ├── .gitignore              # Git ignore rules
│   └── .gitattributes          # Git attributes
│
└── 📚 DOCUMENTATION
    ├── README.md               # This file
    ├── README_NATIVE.md        # Native implementation guide
    ├── BUG_FIXES.md           # Bug fixes and improvements
    └── agent_generated_files/  # Project organization files
```

## 🎯 Which Implementation Should You Use?

### **Use Native Implementation If:**
- ✅ You want **zero dependencies**
- ✅ You need **easy deployment** (single binary)
- ✅ You want **cross-platform compatibility**
- ✅ You prefer **console-based interface**
- ✅ You want **faster startup times**
- ✅ You need **unlimited stub generation** for packing

### **Use Legacy Implementation If:**
- 🔄 You need **graphical user interface** (PyQt5)
- 🔄 You prefer **OpenSSL** for cryptography
- 🔄 You want **drag-and-drop functionality**
- 🔄 You're already using **Python ecosystem**

## 🔧 Building

### **Native Implementation (Recommended)**
```bash
# Build all native components
make all

# Or build individually
make native_encryptor
make native_xll_dropper
make native_gui
make native_stub_generator

# Test native implementation
make test_native
```

### **Legacy Implementation**
```bash
# Install dependencies first
sudo apt install libssl-dev python3-pycryptodome python3-pyqt5

# Build legacy components
make encryptor
make xll_dropper
make packer

# Test legacy implementation
python3 test_encryption.py
python3 test_xll_dropper.py
```

## 🎯 Usage Examples

### **Native Implementation**

```bash
# Encrypt a file
./native_encryptor secret.txt secret.enc

# Decrypt a file
./native_xll_dropper secret.enc secret_decrypted.txt

# Generate a stub for packing
./native_stub_generator secret.enc stub.cpp basic

# Use interactive GUI
./native_gui
```

### **Legacy Implementation**

```bash
# Encrypt a file
./encryptor secret.txt secret.enc

# Decrypt a file
./xll_dropper secret.enc secret_decrypted.txt

# Use graphical GUI
python3 dragdrop_encryptor_gui.py
```

## 🔐 Security Features

Both implementations provide:
- **AES-128-CTR encryption** (industry standard)
- **Secure nonce generation**
- **Environment variable key support**
- **Streaming file processing**
- **Memory-safe operations**

## 🧪 Testing

### **Test Native Implementation**
```bash
# Run comprehensive native tests
python3 test_native_comprehensive.py

# Or use Makefile
make test_native
```

### **Test Legacy Implementation**
```bash
# Test Python components
python3 test_encryption.py
python3 test_xll_dropper.py
```

## 📊 Performance Comparison

| Feature | Native | Legacy |
|---------|--------|--------|
| **Dependencies** | None | OpenSSL + Python |
| **Startup Time** | Instant | ~2-3 seconds |
| **Memory Usage** | ~4KB | ~50MB+ |
| **GUI** | Console | Graphical |
| **Deployment** | Single binary | Multiple packages |
| **Cross-platform** | Excellent | Good |

## 🚨 Important Notes

### **Key Management**
Both implementations support environment variable keys:
```bash
export ENCRYPTION_KEY="your-secure-key-here"
```

### **File Compatibility**
- **Native and Legacy implementations are compatible**
- Files encrypted with one can be decrypted with the other
- Both use the same AES-128-CTR algorithm

### **Production Use**
- **Native implementation recommended for production**
- **Legacy implementation good for development/testing**
- Both provide the same security level

## 🔄 Migration Guide

### **From Legacy to Native**
```bash
# 1. Build native implementation
make all

# 2. Test with existing encrypted files
./native_xll_dropper existing_file.enc decrypted_file.txt

# 3. Use native implementation going forward
./native_encryptor new_file.txt new_file.enc
```

## 📞 Support

### **For Native Implementation**
- See `README_NATIVE.md` for detailed documentation
- See `README_STUB_GENERATOR.md` for stub generator documentation
- Run `make docs` for build system help
- Use `python3 test_native_comprehensive.py` for testing

### **For Legacy Implementation**
- See `requirements.txt` for dependencies
- Run `python3 test_encryption.py` for testing
- Check `BUG_FIXES.md` for known issues

---

**Choose Native Implementation for production use - it's faster, more portable, and has zero dependencies!**
# Star-2
