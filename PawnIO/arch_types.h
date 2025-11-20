// PawnIO - Input-output driver
// Copyright (C) 2023  namazso <admin@namazso.eu>
//
// Architecture-dependent type definitions for x86/x64/ARM64 support
// This file provides a unified type system that adapts to the target architecture

#pragma once

#include "arch_detect.h"
#include <cstdint>
#include <array>

// ============================================================================
// ARCHITECTURE-DEPENDENT CELL TYPES
// ============================================================================

#ifdef ARCH_X86
// 32-bit architecture (x86)
using cell_t = uint32_t;
using scell_t = int32_t;
using ptr_t = uint32_t;
using uptr_t = uint32_t;

#define CELL_SIZE 4
#define PTR_SIZE 4
#define CELL_BITS 32
#define ARCH_NAME "x86"

#else
// 64-bit architectures (x64, ARM64)
using cell_t = uint64_t;
using scell_t = int64_t;
using ptr_t = uint64_t;
using uptr_t = uint64_t;

#define CELL_SIZE 8
#define PTR_SIZE 8
#define CELL_BITS 64
#ifdef ARCH_X64
#define ARCH_NAME "x64"
#elif ARCH_A64
#define ARCH_NAME "ARM64"
#else
#define ARCH_NAME "unknown64"
#endif

#endif

// Verify type sizes at compile time
static_assert(sizeof(cell_t) == CELL_SIZE, "cell_t size mismatch");
static_assert(sizeof(scell_t) == CELL_SIZE, "scell_t size mismatch");
static_assert(sizeof(ptr_t) == PTR_SIZE, "ptr_t size mismatch");
static_assert(sizeof(void*) == PTR_SIZE, "pointer size mismatch");
static_assert(sizeof(cell_t) == sizeof(void*), "cell_t must match pointer size");

// ============================================================================
// ARCHITECTURE-DEPENDENT STORAGE TYPES
// ============================================================================

// CPU affinity storage - GROUP_AFFINITY size varies by architecture
// x86: GROUP_AFFINITY is 12 bytes, need 3 cells (3*4=12)
// x64: GROUP_AFFINITY is 16 bytes, need 2 cells (2*8=16)
#ifdef ARCH_X86
constexpr size_t AFFINITY_CELL_COUNT = 3;
#else
constexpr size_t AFFINITY_CELL_COUNT = 2;
#endif

using affinity_storage_t = std::array<cell_t, AFFINITY_CELL_COUNT>;

// ============================================================================
// ARCHITECTURE-DEPENDENT CALLING CONVENTIONS
// ============================================================================

// Kernel mode calling conventions
#ifdef ARCH_X86
// x86 kernel mode uses __stdcall
#define NATIVE_CALL __stdcall
#define CALLBACK_CALL __stdcall
#else
// x64/ARM64 use default calling convention
#define NATIVE_CALL
#define CALLBACK_CALL
#endif

// ============================================================================
// ARCHITECTURE-DEPENDENT ALIGNMENT
// ============================================================================

#ifdef ARCH_X86
constexpr size_t CACHE_LINE_SIZE = 32;  // Typical for older x86
constexpr size_t MEMORY_ALIGNMENT = 4;
#else
constexpr size_t CACHE_LINE_SIZE = 64;  // Modern x64/ARM64
constexpr size_t MEMORY_ALIGNMENT = 8;
#endif

// ============================================================================
// HELPER MACROS FOR CONDITIONAL COMPILATION
// ============================================================================

// Use these macros for architecture-specific code blocks
#ifdef ARCH_X86
#define IF_X86(x) x
#define IF_X64(x)
#define IF_A64(x)
#define IF_64BIT(x)
#define IF_32BIT(x) x
#elif defined(ARCH_X64)
#define IF_X86(x)
#define IF_X64(x) x
#define IF_A64(x)
#define IF_64BIT(x) x
#define IF_32BIT(x)
#elif defined(ARCH_A64)
#define IF_X86(x)
#define IF_X64(x)
#define IF_A64(x) x
#define IF_64BIT(x) x
#define IF_32BIT(x)
#endif

// ============================================================================
// TYPE CONVERSION HELPERS
// ============================================================================

// Safe conversion from pointer to cell_t
template<typename T>
inline cell_t ptr_to_cell(T* ptr) {
    return static_cast<cell_t>(reinterpret_cast<uptr_t>(ptr));
}

// Safe conversion from cell_t to pointer
template<typename T>
inline T* cell_to_ptr(cell_t cell) {
    return reinterpret_cast<T*>(static_cast<uptr_t>(cell));
}

// Safe conversion between cell types (with sign extension for signed)
inline cell_t scell_to_cell(scell_t value) {
    return static_cast<cell_t>(value);
}

inline scell_t cell_to_scell(cell_t value) {
    return static_cast<scell_t>(value);
}

// ============================================================================
// ARCHITECTURE INFORMATION
// ============================================================================

// Get architecture ID at runtime
inline constexpr cell_t get_architecture_id() {
#ifdef ARCH_X86
    return 0;  // x86 = 0
#elif defined(ARCH_X64)
    return 1;  // x64 = 1
#elif defined(ARCH_A64)
    return 2;  // ARM64 = 2
#else
    return 0xFF;  // Unknown
#endif
}

// Get architecture name
inline constexpr const char* get_architecture_name() {
    return ARCH_NAME;
}

// Check if running on 32-bit architecture
inline constexpr bool is_32bit_arch() {
#ifdef ARCH_X86
    return true;
#else
    return false;
#endif
}

// Check if running on 64-bit architecture
inline constexpr bool is_64bit_arch() {
    return !is_32bit_arch();
}

// ============================================================================
// COMPATIBILITY LAYER FOR LEGACY CODE
// ============================================================================

// For gradual migration, provide aliases to old names
// TODO: Remove these once all code is updated
using cell = cell_t;
using scell = scell_t;

// ============================================================================
// DEBUG HELPERS
// ============================================================================

#ifdef DBG
// Compile-time architecture verification
#pragma message("Building for architecture: " ARCH_NAME)
#pragma message("Cell size: " CELL_SIZE " bytes")
#pragma message("Pointer size: " PTR_SIZE " bytes")
#endif
