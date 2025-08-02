# Advanced AES Encryption & Polymorphic File Dropper

This project has been enhanced with AES-256-CBC encryption, advanced file dropping capabilities, and polymorphic/metamorphic stub generation for maximum stealth and evasion.

## Features

### 🔐 **AES-256-CBC Encryption**
- Industry-standard encryption replacing simple XOR
- Proper key derivation and initialization vectors
- Enhanced security with OpenSSL integration

### 📁 **Advanced File Dropper**
- Multiple drop locations (Temp, AppData, Desktop, Documents)
- Stealth mode with random filenames
- Hidden execution capabilities
- Automatic cleanup options

### 🎭 **Polymorphic Stub Generator**
- Generates unlimited unique stubs
- Variable name randomization
- Instruction substitution
- Junk code injection
- Control flow obfuscation

### 🦠 **Metamorphic Engine**
- Advanced code transformation
- String obfuscation
- Multiple execution patterns
- Anti-detection techniques

## Components

### 1. **Enhanced Encryptor** (`encryptor.cpp`)
- **Encryption**: `./encryptor input_file output_file`
- **Decryption**: `./encryptor --decrypt encrypted_file [--execute]`
- **Drop & Execute**: `./encryptor input_file output_file --drop`

### 2. **File Dropper** (`filedropper.cpp`)
```bash
./filedropper encrypted_file [options]
Options:
  --location <0-4>    Drop location (0=temp, 1=local, 2=roaming, 3=desktop, 4=documents)
  --stealth          Use random filename and hide path
  --hidden           Execute in hidden mode
  --no-execute       Only drop file, don't execute
  --locations        List available drop locations
```

### 3. **Polymorphic Stub Generator** (`stub_generator.cpp`)
```bash
./stub_generator <count> [prefix]     # Generate multiple stubs
./stub_generator --single filename    # Generate single stub
```

### 4. **Metamorphic Engine** (`metamorphic_engine.cpp`)
```bash
./metamorphic_engine <count> [prefix]     # Generate metamorphic stubs
./metamorphic_engine --single filename    # Generate single metamorphic stub
```

## Building

### Windows (Visual Studio)
1. Install OpenSSL development libraries
2. Open `Star.sln` in Visual Studio
3. Build the solution

### Linux/Unix
```bash
# Install OpenSSL development package
sudo apt-get install libssl-dev  # Ubuntu/Debian
# or
sudo yum install openssl-devel   # CentOS/RHEL

# Build all components
make
```

## Usage Examples

### Basic Encryption & Dropping
```bash
# Encrypt a payload
./encryptor payload.exe encrypted_payload.bin

# Drop and execute encrypted payload
./filedropper encrypted_payload.bin --stealth --hidden

# Generate 10 polymorphic stubs
./stub_generator 10 my_stubs

# Generate 5 metamorphic stubs
./metamorphic_engine 5 meta_stubs
```

### Advanced Operations
```bash
# Encrypt and immediately drop/execute
./encryptor payload.exe encrypted.bin --drop

# Drop to specific location with stealth
./filedropper encrypted.bin --location 1 --stealth --hidden

# Generate unlimited unique stubs
./stub_generator 1000 unlimited_stubs
```

## Security Features

### 🔒 **Encryption**
- **AES-256-CBC**: Military-grade encryption
- **Key Derivation**: Secure key generation from obfuscated source
- **IV Randomization**: Prevents pattern analysis

### 🥷 **Stealth**
- **Polymorphic Code**: Each stub is unique
- **Metamorphic Engine**: Advanced code transformation
- **Junk Code**: Anti-analysis techniques
- **Variable Randomization**: Obfuscated identifiers

### 📍 **Dropping Locations**
- **Temp Directory**: Standard location
- **AppData Local**: User-specific hidden location
- **AppData Roaming**: Network-synced location
- **Desktop**: Visible but unsuspicious
- **Documents**: Common user directory

### 🎯 **Execution Modes**
- **Normal**: Standard execution
- **Hidden**: Background execution
- **Stealth**: Random filenames + hidden execution
- **No Execute**: Drop only, manual execution

## Anti-Detection Techniques

### **Code Obfuscation**
- Variable name randomization
- Instruction substitution
- Control flow obfuscation
- String obfuscation
- Junk code injection

### **Behavioral Evasion**
- Random delays and checks
- Environment detection
- Anti-debugging techniques
- Process injection capabilities

### **File System Evasion**
- Random filename generation
- Multiple drop locations
- Automatic cleanup
- Stealth file operations

## Dependencies

- **OpenSSL 1.1.1+**: Cryptographic operations
- **C++17**: Modern C++ features
- **Windows API**: File operations and execution
- **Linux/Unix**: Cross-platform support

## Notes

- Each generated stub is cryptographically unique
- Metamorphic engine provides maximum obfuscation
- File dropper supports multiple evasion techniques
- All components maintain AES encryption compatibility
- Unlimited stub generation capability