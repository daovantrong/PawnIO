// PawnIO - Input-output driver
// Copyright (C) 2023  namazso <admin@namazso.eu>
//
// AMX Architecture Wrapper - Provides architecture-dependent AMX types
// This wrapper allows PawnPP to work with both 32-bit and 64-bit architectures

#pragma once

#include "arch_types.h"

// Define LITTLE_ENDIAN if not already defined (required by PawnPP)
#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN
#endif

#include "../PawnPP/amx.h"
#include "../PawnPP/amx_loader.h"

// ============================================================================
// ARCHITECTURE-DEPENDENT AMX TYPES
// ============================================================================

#ifdef ARCH_X86
// 32-bit AMX configuration
using amx_cell_type = uint32_t;
using amx_type = amx::amx<
    uint32_t, 
    amx::memory_manager_harvard<
        amx::memory_backing_contignous_buffer, 
        amx::memory_backing_paged_buffers<5>
    >
>;
using amx_loader_type = amx::loader<amx_type>;

// Verify cell type matches
static_assert(sizeof(amx_cell_type) == sizeof(cell_t), "AMX cell type mismatch");
static_assert(std::is_same_v<cell_t, amx_type::cell>, "cell_t must match amx_type::cell");

#else
// 64-bit AMX configuration (x64, ARM64)
using amx_cell_type = uint64_t;
using amx_type = amx::amx<
    uint64_t, 
    amx::memory_manager_harvard<
        amx::memory_backing_contignous_buffer, 
        amx::memory_backing_paged_buffers<5>
    >
>;
using amx_loader_type = amx::loader<amx_type>;

// Verify cell type matches
static_assert(sizeof(amx_cell_type) == sizeof(cell_t), "AMX cell type mismatch");
static_assert(std::is_same_v<cell_t, amx_type::cell>, "cell_t must match amx_type::cell");

#endif

// ============================================================================
// LEGACY ALIASES (for backward compatibility)
// ============================================================================

// These aliases maintain compatibility with existing code
using amx64 = amx_type;
using amx64_loader = amx_loader_type;

// Note: Despite the name "amx64", these types are now architecture-aware
// and will be 32-bit on x86 and 64-bit on x64/ARM64

// ============================================================================
// TYPE VERIFICATION
// ============================================================================

// Compile-time verification that our types are correct
namespace amx_wrapper_verify {
    // Verify cell sizes
    static_assert(sizeof(cell_t) == sizeof(amx_type::cell), 
        "cell_t size must match AMX cell size");
    
    // Verify pointer sizes
    static_assert(sizeof(void*) == sizeof(cell_t), 
        "Pointer size must match cell_t size");
    
    // Verify alignment
    static_assert(alignof(amx_type) <= CACHE_LINE_SIZE, 
        "AMX type alignment exceeds cache line size");
    
    // Verify loader size is reasonable
    static_assert(sizeof(amx_loader_type) < 1024 * 1024, 
        "AMX loader size is unexpectedly large");
}

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

// Get AMX architecture information
inline constexpr const char* get_amx_arch_name() {
#ifdef ARCH_X86
    return "AMX32";
#else
    return "AMX64";
#endif
}

// Get AMX cell size in bytes
inline constexpr size_t get_amx_cell_size() {
    return sizeof(amx_cell_type);
}

// Check if AMX is using 32-bit cells
inline constexpr bool is_amx_32bit() {
    return sizeof(amx_cell_type) == 4;
}

// Check if AMX is using 64-bit cells
inline constexpr bool is_amx_64bit() {
    return sizeof(amx_cell_type) == 8;
}

// ============================================================================
// DEBUG INFORMATION
// ============================================================================

#ifdef DBG
#pragma message("AMX Configuration:")
#pragma message("  Architecture: " ARCH_NAME)
#pragma message("  Cell type: " #amx_cell_type)
#ifdef ARCH_X86
#pragma message("  AMX Mode: 32-bit")
#else
#pragma message("  AMX Mode: 64-bit")
#endif
#endif
