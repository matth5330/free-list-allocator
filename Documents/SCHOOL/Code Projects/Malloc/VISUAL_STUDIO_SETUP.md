# Using the Custom Allocator in Visual Studio Community

## Quick Start (Easiest Method)

1. **Double-click `allocator.sln`** in Windows Explorer
   - This will open the solution directly in Visual Studio

2. **Select a configuration** (Debug or Release) from the dropdown at the top
   - Choose **x64** or **x86** (Win32) based on your preference

3. **Build the project**:
   - Press `Ctrl+Shift+B`, or
   - Go to `Build` → `Build Solution`, or
   - Press `F7`

4. **Run the program**:
   - Press `F5` to run with debugging, or
   - Press `Ctrl+F5` to run without debugging, or
   - Go to `Debug` → `Start Debugging` / `Start Without Debugging`

The executable will be in:
- `x64/Debug/allocator.exe` or `x64/Release/allocator.exe`
- `Debug/allocator.exe` or `Release/allocator.exe` (for x86)

## Alternative: Create New Project in Visual Studio

If you prefer to create a new project:

1. **Open Visual Studio Community**

2. **Create a new project**:
   - Click "Create a new project"
   - Select "Console App" (C++)
   - Click "Next"
   - Name it "allocator" (or any name you prefer)
   - Choose a location
   - Click "Create"

3. **Add existing files**:
   - Right-click on "Source Files" in Solution Explorer
   - Select "Add" → "Existing Item..."
   - Navigate to your project folder
   - Select `allocator.cpp` and `main.cpp`
   - Click "Add"
   - Right-click on "Header Files" in Solution Explorer
   - Select "Add" → "Existing Item..."
   - Select `allocator.h`
   - Click "Add"

4. **Remove the default files**:
   - Delete the default `main.cpp` or `*.cpp` that Visual Studio created (if any)
   - Keep only your allocator files

5. **Configure project settings** (optional):
   - Right-click the project in Solution Explorer
   - Select "Properties"
   - Go to `C/C++` → `Language`
   - Set "C++ Language Standard" to "ISO C++11" or higher
   - Click "OK"

6. **Build and run** as described above

## Project Configuration Details

The provided `.vcxproj` file is configured with:

- **C++11 Standard**: Uses ISO C++11
- **Warning Level**: Level 4 (all warnings enabled)
- **Platforms**: x64 and x86 (Win32)
- **Configurations**: Debug and Release
- **Console Application**: Outputs to console window
- **Optimizations**: Enabled in Release mode

## Debugging

Visual Studio provides excellent debugging features:

- **Breakpoints**: Click in the left margin or press `F9` to set breakpoints
- **Step Over**: `F10` - Execute current line
- **Step Into**: `F11` - Step into function calls
- **Step Out**: `Shift+F11` - Step out of current function
- **Watch Window**: Monitor variables and expressions
- **Call Stack**: See the function call hierarchy
- **Memory Window**: View raw memory (useful for debugging allocators!)

## Tips

- Use **Debug** configuration for development (includes debug symbols)
- Use **Release** configuration for performance testing (optimizations enabled)
- The `print_heap_state()` function is great for debugging - you can call it at breakpoints
- Check the Output window (View → Output) for build messages and errors

## Troubleshooting

**"Cannot open source file" errors:**
- Make sure `allocator.h` is in the "Header Files" folder in Solution Explorer
- Check that file paths are correct

**Build errors about C++11:**
- Right-click project → Properties → C/C++ → Language
- Set "C++ Language Standard" to "ISO C++11" or "ISO C++14/17/20"

**Can't find the executable:**
- Check the Output window for the build output path
- Usually in `x64/Debug/` or `x64/Release/` subdirectory

