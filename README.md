# Unified Malware Toolkit v3.0

Complete multi-format malware generation framework supporting C++, MIRC, and PowerShell formats with advanced evasion techniques.

## 🔧 Build Requirements

- **Windows OS** (7/8/10/11)
- **Visual Studio** (2019/2022) OR **MinGW-w64**
- **C++11 compatible compiler**
- **WinINet library** (included with Windows)

## 🚀 Quick Build

```batch
git clone [workspace]
cd workspace
build.bat
```

## 📋 Toolkit Features

### ✅ Multi-Format Generation
- **C++ XOR encrypted stubs** - Rolling key XOR encryption
- **C++ ChaCha20 encrypted stubs** - Modern stream cipher encryption
- **MIRC bot scripts** - IRC-based command and control
- **PowerShell droppers** - Script-based payload delivery

### ✅ Advanced Evasion Techniques
- **Anti-debugging** (IsDebuggerPresent, CheckRemoteDebuggerPresent)
- **VM detection** (Timing-based analysis)
- **Analysis tool detection** (OllyDbg, WinDbg window detection)
- **Network connectivity checks** (Internet connection validation)
- **Random delays** (2-5 second randomized delays)

### ✅ Persistence Mechanisms
- **Registry Run key** modification
- **Mutex single instance** prevention
- **Self-replication** capabilities
- **Stealth execution** modes

### ✅ Memory Operations
- **VirtualAlloc RWX** (executable memory allocation)
- **In-memory decryption** (runtime payload decryption)
- **Clean memory management** (proper resource cleanup)

## 🎯 Usage

### Generate All Formats
```batch
unified_malware_toolkit.exe
```

### Custom Payload Integration
```batch
unified_malware_toolkit.exe custom_payload.bin
```

### Generated Files
- `xor_encrypted_stub.cpp` - XOR encrypted C++ stub
- `chacha_encrypted_stub.cpp` - ChaCha20 encrypted C++ stub  
- `mirc_bot_dropper.mrc` - MIRC script dropper
- `powershell_dropper.ps1` - PowerShell dropper

### Compile Generated Stubs
```batch
g++ -o xor_stub.exe xor_encrypted_stub.cpp -lwininet -static
g++ -o chacha_stub.exe chacha_encrypted_stub.cpp -lwininet -static
```

## 📁 File Structure

```
workspace/
├── unified_malware_toolkit.cpp    # Main toolkit source
├── build.bat                     # Compilation script
├── README.md                     # This file
├── unified_malware_toolkit.exe   # Generated toolkit (after build)
├── xor_encrypted_stub.cpp        # Generated XOR stub (after run)
├── chacha_encrypted_stub.cpp     # Generated ChaCha stub (after run)
├── mirc_bot_dropper.mrc          # Generated MIRC script (after run)
└── powershell_dropper.ps1        # Generated PowerShell script (after run)
```

## 🔍 Detailed Features

### C++ Stub Capabilities
```cpp
// Each generated stub includes:
- Anti-debugging detection
- VM timing analysis  
- Analysis tool window detection
- Network connectivity validation
- Registry persistence establishment
- Mutex single-instance prevention
- Executable memory allocation (RWX)
- Runtime payload decryption
- Clean resource management
```

### MIRC Bot Script
```mirc
; Features included:
- IRC channel-based command reception
- Hex-encoded payload transmission
- Automatic payload decoding
- Temporary file execution
- Self-cleanup mechanisms
```

### PowerShell Dropper
```powershell
# Capabilities:
- Hex string payload decoding
- XOR decryption with position-dependent keys
- Temporary file creation and execution
- Silent process execution
- Automatic cleanup
```

## 🧪 Testing Workflow

### 1. Generate Toolkit
```batch
build.bat
```

### 2. Create Stubs
```batch
unified_malware_toolkit.exe
```

### 3. Compile Stubs
```batch
g++ -o test1.exe xor_encrypted_stub.cpp -lwininet -static
g++ -o test2.exe chacha_encrypted_stub.cpp -lwininet -static
```

### 4. VirusTotal Analysis
- Upload `test1.exe` and `test2.exe` to VirusTotal
- Analyze detection rates across different engines
- Review behavioral analysis reports
- Test evasion technique effectiveness

### 5. Advanced Testing
- Run in sandboxes (Cuckoo, Any.run, Joe Sandbox)
- Monitor anti-analysis behavior
- Test persistence mechanisms
- Validate network connectivity checks

## 🔒 Encryption Methods

### XOR with Rolling Key
```cpp
for (size_t i = 0; i < data.size(); i++) {
    data[i] ^= key + (i % 256);
    key = (key << 1) | (key >> 7);  // Rotate key
}
```

### ChaCha20-Style Encryption
```cpp
uint32_t key[8] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574,
                   random1, random2, random3, random4};
                   
for (size_t i = 0; i < size; i += 4) {
    uint32_t keystream = key[i % 8] ^ (i * 0x9E3779B9);
    // Apply keystream to data blocks
}
```

## 🎯 Advanced Customization

### Custom Payload Integration
```cpp
// Replace test payload with your binary:
std::vector<uint8_t> custom_payload = {
    0x90, 0x90, 0x90, 0x90,  // Your shellcode here
    // ... additional payload bytes
};
```

### Additional Evasion Techniques
```cpp
// Add new anti-analysis checks:
- Registry key detection
- File system artifacts
- Process enumeration
- Hardware fingerprinting
```

### Persistence Extensions
```cpp
// Additional persistence locations:
- Windows services
- Scheduled tasks
- WMI events
- Browser extensions
```

## 📊 Expected Results

### Detection Analysis
- **Static analysis** - Encrypted payloads resist signature detection
- **Behavioral analysis** - Anti-VM may cause sandbox failures
- **Network analysis** - Connectivity checks may trigger alerts
- **Memory analysis** - RWX allocation may be flagged

### Evasion Effectiveness
- **VM detection** - Timing-based checks identify virtual environments
- **Debugger detection** - Multiple detection methods reduce analysis
- **Tool detection** - Window-based detection identifies analysis tools
- **Network validation** - Connectivity checks prevent offline analysis

## ⚠️ Legal Notice

This toolkit is designed for **educational and research purposes only**. It demonstrates:

- **Advanced malware generation techniques**
- **Multi-format payload delivery methods**
- **Anti-analysis and evasion mechanisms**
- **Persistence and stealth capabilities**

**Use only in authorized, controlled environments for legitimate security research.**

## 🔬 Research Applications

### Malware Analysis Education
- Study multi-format malware generation
- Analyze evasion technique implementation
- Research persistence mechanism effectiveness
- Evaluate detection system capabilities

### Security Testing
- Red team operation tools
- Penetration testing frameworks
- Security control validation
- Incident response training

### Academic Research
- Cybersecurity education demonstrations
- Malware family classification studies
- Defense mechanism development
- Behavioral analysis research