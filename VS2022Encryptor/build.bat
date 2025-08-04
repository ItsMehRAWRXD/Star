@echo off
echo ========================================
echo Visual Studio 2022 Encryptor Builder
echo ========================================

echo.
echo Building for x64 Release...
msbuild VS2022Encryptor.vcxproj /p:Configuration=Release /p:Platform=x64 /p:OutDir=..\bin\x64\Release\

if %ERRORLEVEL% EQU 0 (
    echo.
    echo x64 Release build successful!
    echo Executable location: ..\bin\x64\Release\VS2022Encryptor.exe
) else (
    echo.
    echo x64 Release build failed!
    goto :error
)

echo.
echo Building for x86 Release...
msbuild VS2022Encryptor.vcxproj /p:Configuration=Release /p:Platform=Win32 /p:OutDir=..\bin\x86\Release\

if %ERRORLEVEL% EQU 0 (
    echo.
    echo x86 Release build successful!
    echo Executable location: ..\bin\x86\Release\VS2022Encryptor.exe
) else (
    echo.
    echo x86 Release build failed!
    goto :error
)

echo.
echo ========================================
echo All builds completed successfully!
echo ========================================
echo.
echo Available executables:
echo   x64: ..\bin\x64\Release\VS2022Encryptor.exe
echo   x86: ..\bin\x86\Release\VS2022Encryptor.exe
echo.
echo Usage examples:
echo   VS2022Encryptor.exe input.exe output.bin
echo   VS2022Encryptor.exe input.exe output.bin chacha20
echo.
goto :end

:error
echo.
echo ========================================
echo Build failed! Please check the errors above.
echo ========================================
echo.
echo Common solutions:
echo 1. Make sure Visual Studio 2022 is installed
echo 2. Run this from a Visual Studio Developer Command Prompt
echo 3. Ensure Windows SDK is installed
echo.

:end
pause