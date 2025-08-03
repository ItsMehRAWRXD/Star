# 🦖 Rawr Quick Start Guide

> **Get up and running with Rawr in minutes!**

## 🚀 Quick Installation

### **Prerequisites**
- **C++17** compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- **CMake** 3.15 or later
- **Git** for version control
- **Make** or **Ninja** build system

### **Installation Steps**

1. **Clone the repository**
   ```bash
   git clone https://github.com/ItsMehRAWRXD/Rawr.git
   cd Rawr
   ```

2. **Build the project**
   ```bash
   # Using the build script (recommended)
   ./scripts/build.sh
   
   # Or manually
   mkdir build && cd build
   cmake ..
   make
   ```

3. **Run Rawr**
   ```bash
   # Show help
   ./build/bin/rawr --help
   
   # List available plugins
   ./build/bin/rawr --list-plugins
   
   # Run in interactive mode
   ./build/bin/rawr
   ```

## 🎯 First Steps

### **1. Explore the Framework**
```bash
# Check framework status
./build/bin/rawr --status

# View available plugins
./build/bin/rawr --list-plugins
```

### **2. Try a Plugin**
```bash
# Execute time manipulator plugin
./build/bin/rawr --execute time_manipulator

# Perform a random action
./build/bin/rawr --random-action

# Activate ultimate mode
./build/bin/rawr --ultimate-mode
```

### **3. Load Configuration**
```bash
# Load custom configuration
./build/bin/rawr --config configs/core_config.yaml
```

## 📚 Learning Path

### **Beginner (First Week)**
1. **Read the Documentation**
   - [Main README](README.md)
   - [Learning Roadmap](documentation/learning_paths/cybersecurity_roadmap.md)
   - [API Reference](documentation/api_reference/)

2. **Run Examples**
   ```bash
   # Run basic examples
   cd examples
   ./basic_plugin_example
   ```

3. **Create Your First Plugin**
   - Follow the [Plugin Development Guide](documentation/tutorials/plugin_development.md)
   - Use the [Time Manipulator](plugins/time_manipulator/) as a reference

### **Intermediate (First Month)**
1. **Study Advanced Plugins**
   - [Multiverse System](plugins/multiverse_system/)
   - [Offline AI Engine](plugins/offline_ai/)
   - [Predator Armor](plugins/predator_armor/)

2. **Contribute to the Project**
   - Fix bugs
   - Add new features
   - Improve documentation

3. **Join the Community**
   - Participate in discussions
   - Share your research
   - Help other users

### **Advanced (Ongoing)**
1. **Research and Innovation**
   - Conduct original research
   - Publish findings
   - Present at conferences

2. **Specialization**
   - Choose a domain focus
   - Develop expertise
   - Lead projects

## 🔧 Development Setup

### **IDE Configuration**
- **Visual Studio Code**: Install C++ extension
- **CLion**: Open the project directly
- **Vim/Emacs**: Use compile_commands.json

### **Debugging**
```bash
# Build in debug mode
./scripts/build.sh -d

# Run with debug output
./build/bin/rawr --debug
```

### **Testing**
```bash
# Run all tests
./scripts/build.sh -t

# Run specific tests
./build/bin/rawr --test-core
./build/bin/rawr --test-plugins
./build/bin/rawr --test-integration
```

## 🛠️ Common Tasks

### **Building**
```bash
# Clean build
./scripts/build.sh -c

# Debug build
./scripts/build.sh -d

# Build and install
./scripts/build.sh -i

# Build and package
./scripts/build.sh -p
```

### **Development**
```bash
# Format code
make format

# Run linting
make lint

# Generate documentation
make docs
```

### **Troubleshooting**
```bash
# Check dependencies
./scripts/build.sh --help

# Verbose build output
./scripts/build.sh -v

# Clean everything
make clean-all
```

## 📖 Next Steps

### **Documentation**
- [Complete Learning Roadmap](documentation/learning_paths/cybersecurity_roadmap.md)
- [API Reference](documentation/api_reference/)
- [Tutorials](documentation/tutorials/)
- [Examples](documentation/examples/)

### **Community**
- [GitHub Issues](https://github.com/ItsMehRAWRXD/Rawr/issues)
- [Discussions](https://github.com/ItsMehRAWRXD/Rawr/discussions)
- [Contributing Guide](CONTRIBUTING.md)

### **Resources**
- [Security Best Practices](documentation/security_guides/)
- [Research Papers](documentation/research/)
- [Conference Presentations](documentation/presentations/)

## 🎯 Success Metrics

### **Week 1**
- ✅ Successfully build and run Rawr
- ✅ Understand the plugin architecture
- ✅ Execute basic plugins
- ✅ Read core documentation

### **Month 1**
- ✅ Create a custom plugin
- ✅ Understand advanced concepts
- ✅ Contribute to the project
- ✅ Join the community

### **Year 1**
- ✅ Master the framework
- ✅ Conduct original research
- ✅ Publish findings
- ✅ Lead projects

## 🆘 Getting Help

### **Documentation**
- Check the [documentation](documentation/) first
- Search existing [issues](https://github.com/ItsMehRAWRXD/Rawr/issues)
- Read [FAQ](documentation/faq.md)

### **Community**
- Ask questions in [discussions](https://github.com/ItsMehRAWRXD/Rawr/discussions)
- Join our [Discord server](https://discord.gg/rawr)
- Attend [community events](documentation/events/)

### **Professional Support**
- For enterprise support, contact [rawr@security.com](mailto:rawr@security.com)
- For academic collaboration, see [research partnerships](documentation/research/)

---

**🦖 Welcome to Rawr!** 

You're now ready to begin your cybersecurity journey. Remember: knowledge is power, and Rawr is your gateway to that power.

*Happy learning! 🚀*