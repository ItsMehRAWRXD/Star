@echo off
echo.
echo ================================
echo  UPX-Style Encryptor Builder
echo ================================
echo.

echo Building VS2022_MenuEncryptor.exe...
g++ -std=c++17 -O2 -s -static -o VS2022_MenuEncryptor.exe VS2022_MenuEncryptor.cpp

if %errorlevel% equ 0 (
    echo.
    echo ✓ Build successful!
    echo ✓ Output: VS2022_MenuEncryptor.exe
    echo.
    echo Ready to pack files and test on VirusTotal!
    echo.
) else (
    echo.
    echo ✗ Build failed!
    echo Make sure you have g++ installed.
    echo.
)

pause