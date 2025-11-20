#pragma once

// Architecture-dependent type definitions for cross-platform compatibility

#include <stdint.h>

// Detect architecture
#if defined(_M_IX86) || defined(__i386__) || defined(i386) || defined(_X86_)
    #define ARCH_X86 1
#elif defined(_M_AMD64) || defined(__x86_64__) || defined(_AMD64_)
    #define ARCH_X64 1
#elif defined(_M_ARM64) || defined(__aarch64__)
    #define ARCH_ARM64 1
#endif

// Define architecture-specific types
#ifdef ARCH_X86
    // 32-bit architecture types
    typedef uint32_t cell_t;      // AMX cell type (32-bit)
    typedef int32_t scell_t;       // Signed AMX cell type
    typedef uint32_t ptr_t;        // Pointer type (32-bit)
    typedef uint32_t uptr_t;       // Unsigned pointer type
    typedef int32_t sptr_t;        // Signed pointer type
#elif defined(ARCH_X64)
    // 64-bit architecture types
    typedef uint64_t cell_t;       // AMX cell type (64-bit)
    typedef int64_t scell_t;       // Signed AMX cell type
    typedef uint64_t ptr_t;        // Pointer type (64-bit)
    typedef uint64_t uptr_t;       // Unsigned pointer type
    typedef int64_t sptr_t;        // Signed pointer type
#elif defined(ARCH_ARM64)
    // ARM64 architecture types
    typedef uint64_t cell_t;       // AMX cell type (64-bit)
    typedef int64_t scell_t;       // Signed AMX cell type
    typedef uint64_t ptr_t;        // Pointer type (64-bit)
    typedef uint64_t uptr_t;       // Unsigned pointer type
    typedef int64_t sptr_t;        // Signed pointer type
#else
    #error "Unsupported architecture"
#endif

// Helper macros for architecture-specific code
#define CELL_SIZE sizeof(cell_t)
#define PTR_SIZE sizeof(ptr_t)
