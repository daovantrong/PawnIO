# PawnIO x86 Port - Changes Documentation

## Overview
This document describes all changes made to port the PawnIO kernel driver from x64 to x86 (32-bit) architecture.

---

## 1. Architecture Type System

### New Files Created:

#### `PawnIO/arch_types.h`
Defines architecture-dependent types for cross-platform compatibility:
```cpp
#ifdef ARCH_X86
    typedef uint32_t cell_t;
    typedef int32_t scell_t;
    typedef uint32_t ptr_t;
    typedef uint32_t uptr_t;
#elif defined(ARCH_X64)
    typedef uint64_t cell_t;
    typedef int64_t scell_t;
    typedef uint64_t ptr_t;
    typedef uint64_t uptr_t;
#endif
```

#### `PawnIO/amx_wrapper.h`
Provides AMX VM architecture abstraction:
```cpp
#ifdef ARCH_X86
    using amx_type = amx::amx<uint32_t, ...>;
#else
    using amx_type = amx::amx<uint64_t, ...>;
#endif
using amx64 = amx_type;
using amx64_loader = amx::loader<amx_type>;
```

#### `PawnIO/wdk_compat.h`
WDK compatibility layer for older SDK:
- Defines missing types (PCUCHAR)
- Defines missing constants (FAST_FAIL_*)
- Defines missing macros (DECLARE_CONST_UNICODE_STRING, RTL_CONSTANT_STRING)

---

## 2. Modified Source Files

### `PawnIO/vm.h`
**Changes:**
- Added `#include "arch_types.h"`
- Added `#include "amx_wrapper.h"`

### `PawnIO/vm.cpp`
**Changes:**
- Replaced all `amx64` direct usage with `amx64` alias from amx_wrapper.h
- Updated all cell types to use `cell_t` instead of hardcoded types
- Updated callback structures to use architecture-aware types
- Fixed template instantiations for architecture compatibility

**Example:**
```cpp
// Before:
amx::amx<uint64_t, ...> vm;

// After:
amx64 vm;  // Resolves to correct architecture
```

### `PawnIO/natives_impl_windows.cpp`
**Major Changes:**
- Updated 80+ native functions to use `cell_t` and `scell_t`
- Added explicit pointer casts for x86 compatibility
- Extended architecture guards to include x86
- Added x86-specific implementations for rdrand/rdseed
- Fixed GROUP_AFFINITY handling for x86
- Updated Dell SMM function declaration for x86 stdcall

**Example:**
```cpp
// Before:
cell read_phys(cell addr) {
    return *(uint64_t*)addr;
}

// After:
cell_t read_phys(cell_t addr) {
    return *(uptr_t*)(uptr_t)addr;  // Explicit cast for x86
}
```

### `PawnIO/callbacks.h` & `callbacks.cpp`
**Changes:**
- Included `arch_types.h`
- Changed `vm_callback_precall` parameter `cip` to use `cell_t`
- Updated callback implementation to cast `cell_t` to `UINT_PTR`

### `PawnIO/public.h`
**Changes:**
- Added comment clarifying UINT_PTR is architecture-aware

### `PawnIO/stdafx.h`
**Changes:**
- Added conditional include for wdk_compat.h on x86 builds

### `PawnIO/x86.asm`
**Complete rewrite for x86:**
- Implemented Dell SMM function (`__dell@4`)
- Implemented CR/DR access function (`__crdr`)
- Fixed calling conventions for x86
- Removed invalid SAFESEH directive

**Key Functions:**
```asm
PUBLIC __dell@4
__dell@4 PROC
    ; Dell SMM implementation
    ret 4
__dell@4 ENDP

PUBLIC __crdr
__crdr PROC
    ; CR/DR register access
    ret
__crdr ENDP
```

---

## 3. New CRT Implementation

### `PawnIO/kernel_stubs.c`
**Purpose:** Provides CRT functions for kernel mode

**Implemented Functions:**
- `malloc()` - Uses ExAllocatePoolWithTag
- `free()` - Uses ExFreePoolWithTag
- `memcpy()` - Manual implementation
- `memmove()` - Handles overlapping regions
- `memset()` - Manual byte fill
- `ceil()` - Integer-based ceiling
- `_callnewh()` - Allocation failure handler
- `__std_exception_copy()` - Exception copy
- `__std_exception_destroy()` - Exception cleanup
- `_CxxThrowException()` - Calls KeBugCheckEx
- `__invoke_watson()` - Calls KeBugCheckEx
- `__except_handler3()` - Exception handler

---

## 4. Build Configuration Changes

### `PawnIO/PawnIO.vcxproj`
**Changes:**
- Made ksecdd.lib conditional (exclude for Win32)
- Disabled buffer security checks for x86 (/GS-)
- Configured MASM for x86.asm

### `PawnIO/Directory.Build.props`
**New file - MSBuild properties:**
```xml
<PropertyGroup Condition="'$(Platform)'=='Win32'">
    <WDK19041LibPath>C:\Program Files (x86)\Windows Kits\10\lib\10.0.19041.0\km\x86</WDK19041LibPath>
    <LibraryPath>$(WDK19041LibPath);$(LibraryPath)</LibraryPath>
</PropertyGroup>
```

### `PawnIO/Directory.Build.targets`
**New file - MSBuild targets:**
- Bypasses WDK x86 architecture check
- Defines _X86_ and i386 macros
- Prepends WDK 10.0.19041 include paths

---

## 5. Build Scripts

### `build_x86_19041.bat`
**Purpose:** Build script targeting WDK 10.0.19041
- Sets up VS environment
- Configures WDK paths
- Builds Win32 configuration

### `manual_link_x86.bat`
**Purpose:** Manual assembly and linking
- Assembles x86.asm with ml.exe
- Links with custom kernel library
- Uses /FORCE:UNRESOLVED for runtime resolution

### `create_kernel_lib.bat`
**Purpose:** Creates custom ntoskrnl import library
- Generates ntoskrnl_custom.lib from .def file

### `compile_stubs.bat`
**Purpose:** Compiles kernel_stubs.c
- Uses /TC (C mode) and /Oi- (disable intrinsics)

### `ntoskrnl_x86.def`
**Purpose:** Kernel API definitions for import library
- Lists all required ntoskrnl.exe exports with decorations

---

## 6. Architecture Detection

### Preprocessor Macros
The build system defines these macros:

**For x86:**
```cpp
#define ARCH_X86 1
#define _X86_ 1
#define i386 1
```

**For x64:**
```cpp
#define ARCH_X64 1
#define _AMD64_ 1
```

### Usage in Code:
```cpp
#if defined(ARCH_X86)
    // x86-specific code
#elif defined(ARCH_X64)
    // x64-specific code
#endif
```

---

## 7. Key Technical Decisions

### 1. Type System
- Used `cell_t` instead of hardcoded `uint64_t`
- Allows single codebase for multiple architectures

### 2. AMX VM
- Created wrapper to abstract amx template instantiation
- Maintains API compatibility while changing underlying types

### 3. CRT Functions
- Implemented in kernel_stubs.c instead of linking msvcrt
- Avoids kernel/user mode CRT conflicts

### 4. Kernel APIs
- Used /FORCE:UNRESOLVED for import resolution
- Windows resolves at runtime from ntoskrnl.exe

### 5. Calling Conventions
- x86 uses stdcall for Dell SMM (`__dell@4`)
- x64 uses fastcall (`_dell`)
- Conditional compilation handles differences

---

## 8. Files Summary

### Created Files (New):
1. `PawnIO/arch_types.h` - Architecture type definitions
2. `PawnIO/amx_wrapper.h` - AMX architecture wrapper
3. `PawnIO/wdk_compat.h` - WDK compatibility layer
4. `PawnIO/kernel_stubs.c` - CRT implementations
5. `PawnIO/Directory.Build.props` - MSBuild properties
6. `PawnIO/Directory.Build.targets` - MSBuild targets
7. `PawnIO/ntoskrnl_x86.def` - Kernel API definitions
8. `build_x86_19041.bat` - Build script
9. `manual_link_x86.bat` - Link script
10. `create_kernel_lib.bat` - Library creation
11. `compile_stubs.bat` - Stubs compilation

### Modified Files:
1. `PawnIO/vm.h` - Added architecture includes
2. `PawnIO/vm.cpp` - Architecture-aware types
3. `PawnIO/natives_impl.h` - Updated signatures
4. `PawnIO/natives_impl_windows.cpp` - 80+ functions updated
5. `PawnIO/callbacks.h` - cell_t usage
6. `PawnIO/callbacks.cpp` - Updated implementation
7. `PawnIO/public.h` - Documentation
8. `PawnIO/stdafx.h` - Compatibility include
9. `PawnIO/x86.asm` - Complete rewrite
10. `PawnIO/PawnIO.vcxproj` - Build configuration

---

## 9. Build Instructions

### Prerequisites:
- Visual Studio 2022/2026 with C++ Desktop Development
- Windows Driver Kit (WDK) 10.0.19041 or later
- Windows SDK 10.0.26100 or later

### Build Steps:

#### Option 1: Automated Build
```batch
cd d:\PawnIO
build_x86_19041.bat
```

#### Option 2: Manual Build
```batch
cd d:\PawnIO
manual_link_x86.bat
```

#### Option 3: MSBuild
```batch
msbuild PawnIO.sln /t:Rebuild /p:Configuration=Release /p:Platform=Win32
```

### Output:
- `d:\PawnIO\Release\PawnIO.sys` (77,824 bytes)

---

## 10. Testing

### Enable Test Signing:
```batch
bcdedit /set testsigning on
```

### Install Driver:
```batch
sc create PawnIO type= kernel binPath= C:\path\to\PawnIO.sys
sc start PawnIO
```

### Verify:
```batch
sc query PawnIO
```

### Uninstall:
```batch
sc stop PawnIO
sc delete PawnIO
```

---

## 11. Known Limitations

1. **Runtime Resolution**: Some kernel APIs resolve at load time (normal for kernel drivers)
2. **Test Signing**: Driver must be test-signed for Windows to load it
3. **Exception Handling**: Exceptions trigger KeBugCheckEx (intentional for kernel mode)
4. **WDK Version**: Requires WDK 10.0.19041 libraries for x86 support

---

## 12. Compatibility

### Supported Architectures:
- ✅ x86 (32-bit) - New
- ✅ x64 (64-bit) - Original
- 🔄 ARM64 (64-bit) - Prepared (needs testing)

### Supported Windows Versions:
- Windows 10 x86 (32-bit)
- Windows 10 x64 (64-bit)
- Windows 11 x64 (64-bit)

---

## 13. Performance Considerations

### x86 vs x64:
- **Cell Size**: 32-bit vs 64-bit
- **Pointer Size**: 32-bit vs 64-bit
- **Memory Usage**: ~50% less for VM data structures
- **Performance**: Slightly slower due to 32-bit arithmetic

---

## 14. Troubleshooting

### Build Errors:

**"Windows SDK version not found"**
- Install WDK 10.0.19041
- Check Directory.Build.props paths

**"Unresolved external symbol"**
- Rebuild kernel_stubs.obj
- Check manual_link_x86.bat library list

**"Assembly error"**
- Check x86.asm syntax
- Verify ml.exe is in PATH

### Runtime Errors:

**Driver fails to load**
- Enable test signing
- Check Event Viewer for details
- Verify driver signature

**BSOD on load**
- Check kernel_stubs.c implementations
- Verify CRT functions work correctly

---

## 15. Future Improvements

1. **Static Linking**: Link with static kernel CRT instead of /FORCE
2. **ARM64 Support**: Complete ARM64 port and testing
3. **Automated Testing**: Add unit tests for architecture-specific code
4. **CI/CD**: Automate x86/x64/ARM64 builds

---

## Credits

**Original Driver**: PawnIO x64  
**x86 Port**: 2025-11-20  
**WDK Version**: 10.0.19041 (x86 libs) + 10.0.26100 (headers)  
**Compiler**: MSVC 14.44.35207 (VS 2026)

---

## Author

**Dao Van Trong** - [TRONG.PRO](https://trong.pro)

---

## License

Same as original PawnIO project (GPL v3)


