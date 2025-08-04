@echo off
echo Building VS2022 Universal PE Packer...
echo.

echo Building x64 Release...
msbuild vs2022_encryptor.sln /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v143
if %ERRORLEVEL% NEQ 0 (
    echo Build failed for x64 Release
    pause
    exit /b 1
)

echo Building x86 Release...
msbuild vs2022_encryptor.sln /p:Configuration=Release /p:Platform=Win32 /p:PlatformToolset=v143
if %ERRORLEVEL% NEQ 0 (
    echo Build failed for x86 Release
    pause
    exit /b 1
)

echo.
echo Build completed successfully!
echo Executables created in:
echo   x64\Release\VS2022Encryptor.exe
echo   Win32\Release\VS2022Encryptor.exe
echo.
pause