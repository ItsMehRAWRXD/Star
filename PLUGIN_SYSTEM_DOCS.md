# Branch Style Encryption Plugin System

## Overview

The branch style plugin system provides a modular architecture for encryption algorithms in the stub generator. Each encryption method is implemented as a separate plugin that can be dynamically loaded and configured.

## Architecture

### Core Components

1. **IEncryptionPlugin Interface** (`encryption_plugin_interface.h`)
   - Base interface that all encryption plugins must implement
   - Defines methods for encryption, decryption, and code generation
   - Provides metadata methods (name, version, description)

2. **PluginManager** (`encryption_plugin_manager.h/cpp`)
   - Singleton manager for plugin registration and lifecycle
   - Handles plugin discovery and instantiation
   - Provides validation and testing utilities

3. **Encryption Plugins** (`plugins/` directory)
   - ChaCha20: Stream cipher with 256-bit key
   - AES: Enhanced stream mode with position dependency
   - XOR: Advanced XOR with avalanche effect

## Plugin Development

### Creating a New Plugin

1. Create a new class inheriting from `IEncryptionPlugin`:

```cpp
class MyCustomPlugin : public IEncryptionPlugin {
    // Implement all virtual methods
};
```

2. Implement required methods:
   - `getName()`: Return unique plugin identifier
   - `getKeySize()`: Return required key size in bytes
   - `encrypt()`: Implement encryption logic
   - `decrypt()`: Implement decryption logic
   - `generateStubCode()`: Generate C++ code for stubs

3. Register the plugin:

```cpp
REGISTER_ENCRYPTION_PLUGIN("MyCustom", MyCustomPlugin)
```

### Plugin Interface Methods

```cpp
// Metadata
virtual std::string getName() const = 0;
virtual std::string getDescription() const = 0;
virtual std::string getVersion() const = 0;

// Key requirements
virtual size_t getKeySize() const = 0;
virtual size_t getNonceSize() const = 0;
virtual bool requiresIV() const = 0;

// Operations
virtual bool initialize(const EncryptionContext& context) = 0;
virtual bool encrypt(std::vector<uint8_t>& data, const EncryptionContext& context) = 0;
virtual bool decrypt(std::vector<uint8_t>& data, const EncryptionContext& context) = 0;

// Code generation
virtual std::string generateStubCode(const std::string& varPrefix, bool useDecimalKeys) const = 0;
virtual std::string generateDecryptionCode(const std::string& dataVar, const std::string& keyVar, size_t keySize) const = 0;
```

## Usage

### Building

```bash
make all              # Build all components
make test            # Run tests
make clean           # Clean build files
```

### Command Line Usage

```bash
# List available plugins
./unified_stub_generator_with_plugins input.exe output.cpp --list-plugins

# Generate stub with specific plugins
./unified_stub_generator_with_plugins input.exe output.cpp --plugins ChaCha20,XOR

# Generate stub with all available plugins
./unified_stub_generator_with_plugins input.exe output.cpp
```

### Programmatic Usage

```cpp
// Get plugin manager instance
auto& pm = PluginManager::getInstance();

// Create a plugin
auto plugin = pm.createPlugin("ChaCha20");

// Set up encryption context
EncryptionContext ctx;
ctx.key = generateKey(plugin->getKeySize());
ctx.nonce = generateNonce(plugin->getNonceSize());

// Encrypt data
std::vector<uint8_t> data = loadData();
plugin->initialize(ctx);
plugin->encrypt(data, ctx);
```

## Built-in Plugins

### ChaCha20
- **Key Size**: 256 bits (32 bytes)
- **Nonce Size**: 96 bits (12 bytes)
- **Features**: Hardware-accelerated, constant-time operations
- **Use Case**: High-security applications requiring stream encryption

### AES (Enhanced Stream Mode)
- **Key Size**: 128 bits (16 bytes)
- **Nonce Size**: 128 bits (16 bytes)
- **Features**: Position-dependent encryption, S-box transformations
- **Use Case**: Standard encryption with added obfuscation

### XOR (Advanced)
- **Key Size**: Variable (16-64 bytes)
- **Nonce Size**: Not used
- **Features**: Avalanche effect, three-pass encryption
- **Use Case**: Lightweight encryption with strong diffusion

## Plugin Features

### Security Features
- Anti-debugging support
- Timing attack resistance
- Key obfuscation (decimal encoding)
- Variable name generation

### Performance Features
- Parallel processing support (plugin-specific)
- Optimal block size hints
- Memory-efficient implementations

### Code Generation
- Self-contained stub generation
- No external dependencies
- Cross-platform support (Windows/Linux)
- Customizable variable naming

## Extending the System

### Adding New Encryption Methods

1. Study existing plugins in `plugins/` directory
2. Implement the `IEncryptionPlugin` interface
3. Add unique features or optimizations
4. Test thoroughly with the validation system

### Custom Plugin Loaders

The system supports dynamic plugin loading:

```cpp
bool loadPluginFromFile(const std::string& filepath);
```

This allows loading plugins from external shared libraries (.dll/.so files).

## Best Practices

1. **Key Management**: Always use cryptographically secure random number generators
2. **Testing**: Use the built-in validation system before deployment
3. **Documentation**: Document any custom plugins thoroughly
4. **Security**: Follow cryptographic best practices for your algorithm
5. **Performance**: Profile your plugin with realistic workloads

## Examples

See `test_plugin_system.cpp` for comprehensive examples of:
- Plugin discovery and enumeration
- Encryption/decryption workflows
- Stub code generation
- Plugin validation

## Troubleshooting

### Common Issues

1. **Plugin not found**: Ensure plugin is registered with `REGISTER_ENCRYPTION_PLUGIN`
2. **Encryption fails**: Check key/nonce sizes match plugin requirements
3. **Linking errors**: Include all necessary plugin source files

### Debug Output

Enable verbose output by modifying plugin implementations to include debug logging.

## Future Enhancements

- Plugin configuration files
- Runtime plugin loading from directories
- Plugin chains and compositions
- Hardware acceleration support
- Additional encryption algorithms