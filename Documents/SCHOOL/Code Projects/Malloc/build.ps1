# Build script for Windows (PowerShell)

Write-Host "Building custom memory allocator..." -ForegroundColor Cyan

# Try g++ first (MinGW/GCC on Windows)
$gpp = Get-Command g++ -ErrorAction SilentlyContinue
if ($gpp) {
    Write-Host "Using g++ compiler..." -ForegroundColor Yellow
    g++ -std=c++11 -Wall -Wextra -O2 -o allocator.exe allocator.cpp main.cpp
    if ($LASTEXITCODE -eq 0) {
        Write-Host "Build successful! Run with: .\allocator.exe" -ForegroundColor Green
        exit 0
    } else {
        Write-Host "Build failed with g++!" -ForegroundColor Red
        exit 1
    }
}

# Try cl (MSVC)
$cl = Get-Command cl -ErrorAction SilentlyContinue
if ($cl) {
    Write-Host "Using MSVC cl compiler..." -ForegroundColor Yellow
    cl /EHsc /std:c++11 /W4 /O2 allocator.cpp main.cpp /Fe:allocator.exe
    if ($LASTEXITCODE -eq 0) {
        Write-Host "Build successful! Run with: .\allocator.exe" -ForegroundColor Green
        exit 0
    } else {
        Write-Host "Build failed with cl!" -ForegroundColor Red
        exit 1
    }
}

# Try clang++
$clangpp = Get-Command clang++ -ErrorAction SilentlyContinue
if ($clangpp) {
    Write-Host "Using clang++ compiler..." -ForegroundColor Yellow
    clang++ -std=c++11 -Wall -Wextra -O2 -o allocator.exe allocator.cpp main.cpp
    if ($LASTEXITCODE -eq 0) {
        Write-Host "Build successful! Run with: .\allocator.exe" -ForegroundColor Green
        exit 0
    } else {
        Write-Host "Build failed with clang++!" -ForegroundColor Red
        exit 1
    }
}

Write-Host ""
Write-Host "ERROR: No C++ compiler found!" -ForegroundColor Red
Write-Host "Please install one of the following:" -ForegroundColor Yellow
Write-Host "  - MinGW-w64 (provides g++)" -ForegroundColor White
Write-Host "  - Microsoft Visual Studio (provides cl)" -ForegroundColor White
Write-Host "  - LLVM/Clang (provides clang++)" -ForegroundColor White
Write-Host ""
Write-Host "Or compile manually:" -ForegroundColor Yellow
Write-Host "  g++ -std=c++11 -Wall -Wextra -O2 -o allocator.exe allocator.cpp main.cpp" -ForegroundColor Cyan
exit 1

