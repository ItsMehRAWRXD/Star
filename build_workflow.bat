@echo off
echo Building Workflow Triple Encryptor...
echo.

g++ -std=c++17 -O2 -s -static -o WorkflowTripleEncryptor.exe WorkflowTripleEncryptor.cpp

if exist WorkflowTripleEncryptor.exe (
    echo ✓ Build successful: WorkflowTripleEncryptor.exe
    echo.
    echo Usage:
    echo   WorkflowTripleEncryptor.exe input_file output_file
    echo.
    echo Example:
    echo   WorkflowTripleEncryptor.exe document.pdf encrypted_document.bin
) else (
    echo ✗ Build failed!
    echo Please ensure g++ is installed and in PATH
)

pause