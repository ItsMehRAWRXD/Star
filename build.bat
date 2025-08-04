@echo off
echo ========================================
echo VS2022 Triple Encryptor Build Script
echo ========================================

REM Check if Visual Studio is available
where cl >nul 2>nul
if %errorlevel% neq 0 (
    echo Visual Studio compiler not found in PATH.
    echo Please run this from a Visual Studio Developer Command Prompt.
    pause
    exit /b 1
)

REM Create build directories
if not exist "bin" mkdir bin
if not exist "obj" mkdir obj

echo.
echo [*] Building VS2022 Triple Encryptor...
echo.

REM Compile with optimizations for release
cl /EHsc /O2 /MT /DWIN32_LEAN_AND_MEAN /DNDEBUG /D_CONSOLE /std:c++17 ^
   /Wall /WX- /Gm- /GS- /Gy /fp:fast /arch:AVX2 ^
   /Fe:bin\VS2022_TripleEncryptor.exe ^
   /Fo:obj\ ^
   VS2022_TripleEncryptor.cpp ^
   /link advapi32.lib kernel32.lib user32.lib ^
   /SUBSYSTEM:CONSOLE /OPT:REF /OPT:ICF /LTCG

if %errorlevel% equ 0 (
    echo.
    echo [+] Build successful!
    echo [+] Output: bin\VS2022_TripleEncryptor.exe
    echo.
    echo Usage:
    echo   bin\VS2022_TripleEncryptor.exe -e input.exe encrypted.bin
    echo   bin\VS2022_TripleEncryptor.exe -s payload.exe stub.cpp
    echo.
) else (
    echo.
    echo [-] Build failed!
    echo.
)

pause