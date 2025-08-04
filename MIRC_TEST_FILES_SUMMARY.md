# 🦠 **mIRC Test Files for WebSec Analysis**

## 📁 **Files Created for Testing**

### **1. Original mIRC File**
- **File**: `mirc_test.exe`
- **Size**: 52 bytes
- **Type**: Test mIRC executable file
- **Content**: Simple text file simulating mIRC.exe

### **2. Encrypted mIRC Payload**
- **File**: `mirc_encrypted.bin`
- **Size**: 68 bytes
- **Type**: AES-128-CTR encrypted mIRC payload
- **Key**: Randomly generated (different each time)
- **Nonce**: Randomly generated (different each time)

### **3. Basic mIRC Dropper**
- **File**: `mirc_dropper`
- **Size**: 30,160 bytes
- **Type**: Compiled C++ dropper for mIRC
- **Features**: 
  - AES-128-CTR decryption
  - Basic stub functionality
  - No anti-analysis features
- **Key Used**: `b0cb5029e0974a25dadb20d447252aa7`
- **Nonce Used**: `8dac75053f6166ad81fa07bb3024cb17`

### **4. Ultimate Stealth mIRC Stub**
- **File**: `ultimate_stealth_stub.cpp`
- **Size**: 6,905 bytes
- **Type**: XOR-obfuscated stealth stub
- **Features**:
  - XOR string obfuscation
  - Random function names
  - Stealth output
  - Anti-analysis techniques
- **Key**: `8dbe9e4bdfc4fe9931bcf0f42e3c6c50`
- **Nonce**: `f6f42376ae63e90295f6ac59dd22dc93`

## 🔍 **Expected WebSec Analysis Results**

### **Original File (`mirc_test.exe`)**
- **Expected Detection**: 0/100% (clean text file)
- **File Type**: Text file, not executable

### **Encrypted Payload (`mirc_encrypted.bin`)**
- **Expected Detection**: 0/100% (encrypted data)
- **File Type**: Binary data, no executable signature

### **Basic Dropper (`mirc_dropper`)**
- **Expected Detection**: 0-5/100% (depending on AV heuristics)
- **Detection Reasons**:
  - AES encryption/decryption routines
  - File I/O operations
  - Memory manipulation
  - Potential dropper behavior

### **Ultimate Stealth Stub (`ultimate_stealth_stub.cpp`)**
- **Expected Detection**: 0/100% (perfect stealth)
- **Features**:
  - XOR string obfuscation
  - Random function names
  - Stealth output
  - Anti-analysis techniques

## 📊 **File Size Analysis**

| File | Size | Type | Purpose |
|------|------|------|---------|
| `mirc_test.exe` | 52 bytes | Original | Test mIRC payload |
| `mirc_encrypted.bin` | 68 bytes | Encrypted | AES-128-CTR encrypted |
| `mirc_dropper` | 30,160 bytes | Dropper | Basic decryption stub |
| `ultimate_stealth_stub.cpp` | 6,905 bytes | Stub | XOR-obfuscated stealth |

## 🔧 **How to Test on WebSec**

### **Step 1: Upload Files**
1. Go to [websec.nl/scanner](https://websec.nl/scanner)
2. Upload each file individually:
   - `mirc_test.exe` (original)
   - `mirc_encrypted.bin` (encrypted)
   - `mirc_dropper` (basic dropper)
   - `ultimate_stealth_stub.cpp` (stealth stub)

### **Step 2: Analyze Results**
- Check detection rates
- Review detailed analysis
- Note any false positives
- Compare with previous calc.exe results

### **Step 3: Compare Results**
- Compare with calc.exe results
- Note differences in detection patterns
- Analyze mIRC-specific behavior
- Evaluate stealth effectiveness

## 🎯 **Expected Outcomes**

### **Low Detection Rate (0-5/100%)**
- Basic dropper with simple AES decryption
- No malicious behavior
- Standard file operations

### **Perfect Stealth (0/100%)**
- Ultimate stealth stub with XOR obfuscation
- Random function names
- Anti-analysis techniques
- Complete stealth

## 📈 **Success Metrics**

- **False Positive Rate**: < 10% detection
- **Functionality**: 100% decryption success
- **Performance**: Fast execution (< 1 second)
- **Stealth**: Minimal detection by AV

## 🔒 **Security Notes**

- These are **test files only**
- No malicious payloads included
- Used for educational/research purposes
- Always test in isolated environment
- Comply with local laws and regulations

## 🚀 **Testing Focus**

### **Key Questions to Answer:**
1. **Does mIRC filename affect detection?**
2. **Are IRC-related files treated differently?**
3. **How does XOR obfuscation perform on mIRC files?**
4. **Are there mIRC-specific detection patterns?**

### **Expected Insights:**
- **mIRC vs calc.exe** detection differences
- **Filename influence** on analysis
- **XOR obfuscation** effectiveness
- **Stealth technique** validation

---

**Files Ready for WebSec Testing** ✅
*Generated with Star-2 Native Encryption System*
*Testing mIRC-specific stealth patterns*