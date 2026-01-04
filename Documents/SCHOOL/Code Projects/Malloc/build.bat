@echo off
REM Build script for Windows - tries g++ (MinGW) first, then MSVC cl

echo Building custom memory allocator...

REM Try g++ first (MinGW/GCC on Windows)
where g++ >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo Using g++ compiler...
    g++ -std=c++11 -Wall -Wextra -O2 -o allocator.exe allocator.cpp main.cpp
    if %ERRORLEVEL% EQU 0 (
        echo Build successful! Run with: allocator.exe
        exit /b 0
    ) else (
        echo Build failed with g++!
        exit /b 1
    )
)

REM Try cl (MSVC)
where cl >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo Using MSVC cl compiler...
    cl /EHsc /std:c++11 /W4 /O2 allocator.cpp main.cpp /Fe:allocator.exe
    if %ERRORLEVEL% EQU 0 (
        echo Build successful! Run with: allocator.exe
        exit /b 0
    ) else (
        echo Build failed with cl!
        exit /b 1
    )
)

REM Try clang++
where clang++ >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo Using clang++ compiler...
    clang++ -std=c++11 -Wall -Wextra -O2 -o allocator.exe allocator.cpp main.cpp
    if %ERRORLEVEL% EQU 0 (
        echo Build successful! Run with: allocator.exe
        exit /b 0
    ) else (
        echo Build failed with clang++!
        exit /b 1
    )
)

echo ERROR: No C++ compiler found!
echo Please install one of the following:
echo   - MinGW-w64 (provides g++)
echo   - Microsoft Visual Studio (provides cl)
echo   - LLVM/Clang (provides clang++)
echo.
echo Or compile manually:
echo   g++ -std=c++11 -Wall -Wextra -O2 -o allocator.exe allocator.cpp main.cpp
exit /b 1


