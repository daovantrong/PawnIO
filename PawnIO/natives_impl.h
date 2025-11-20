// PawnIO - Input-output driver
// Copyright (C) 2023  namazso <admin@namazso.eu>
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// 
// Linking PawnIO statically or dynamically with other modules is making a
// combined work based on PawnIO. Thus, the terms and conditions of the GNU
// General Public License cover the whole combination.
// 
// In addition, as a special exception, the copyright holders of PawnIO give
// you permission to combine PawnIO program with free software programs or
// libraries that are released under the GNU LGPL and with independent modules
// that communicate with PawnIO solely through the device IO control
// interface. You may copy and distribute such a system following the terms of
// the GNU GPL for PawnIO and the licenses of the other code concerned,
// provided that you include the source code of that other code when and as
// the GNU GPL requires distribution of source code.
// 
// Note that this exception does not include programs that communicate with
// PawnIO over the Pawn interface. This means that all modules loaded into
// PawnIO must be compatible with this licence, including the earlier
// exception clause. We recommend using the GNU Lesser General Public License
// version 2.1 to fulfill this requirement.
// 
// For alternative licensing options, please contact the copyright holder at
// admin@namazso.eu.
// 
// Note that people who make modified versions of PawnIO are not obligated to
// grant this special exception for their modified versions; it is their
// choice whether to do so. The GNU General Public License gives permission
// to release a modified version without this exception; this exception also
// makes it possible to release a modified version which carries forward this
// exception.

#pragma once

#include "arch_types.h"

// Architecture information
cell_t get_arch();

// CPU management
cell_t cpu_count();
cell_t cpu_set_affinity(cell_t which, affinity_storage_t& old);
cell_t cpu_restore_affinity(affinity_storage_t old);

// MSR operations
cell_t msr_read(cell_t msr, cell_t& value);
cell_t msr_write(cell_t msr, cell_t value);

// Interrupt control
void interrupts_disable();
void interrupts_enable();

// Physical memory operations
cell_t physical_read_byte(cell_t pa, cell_t& value);
cell_t physical_read_word(cell_t pa, cell_t& value);
cell_t physical_read_dword(cell_t pa, cell_t& value);
cell_t physical_read_qword(cell_t pa, cell_t& value);

cell_t physical_write_byte(cell_t pa, cell_t value);
cell_t physical_write_word(cell_t pa, cell_t value);
cell_t physical_write_dword(cell_t pa, cell_t value);
cell_t physical_write_qword(cell_t pa, cell_t value);

// IO space mapping
cell_t io_space_map(cell_t pa, cell_t size);
void io_space_unmap(cell_t va, cell_t size);

// Virtual memory operations
cell_t virtual_read_byte(cell_t va, cell_t& value);
cell_t virtual_read_word(cell_t va, cell_t& value);
cell_t virtual_read_dword(cell_t va, cell_t& value);
cell_t virtual_read_qword(cell_t va, cell_t& value);

cell_t virtual_write_byte(cell_t va, cell_t value);
cell_t virtual_write_word(cell_t va, cell_t value);
cell_t virtual_write_dword(cell_t va, cell_t value);
cell_t virtual_write_qword(cell_t va, cell_t value);

// Atomic operations
cell_t virtual_cmpxchg_byte2(cell_t va, cell_t exchange, cell_t comparand);
cell_t virtual_cmpxchg_word2(cell_t va, cell_t exchange, cell_t comparand);
cell_t virtual_cmpxchg_dword2(cell_t va, cell_t exchange, cell_t comparand);
cell_t virtual_cmpxchg_qword2(cell_t va, cell_t exchange, cell_t comparand);

// Memory allocation
cell_t virtual_alloc(cell_t size);
void virtual_free(cell_t va);

// PCI configuration space
cell_t pci_config_read_byte(cell_t bus, cell_t device, cell_t function, cell_t offset, cell_t& value);
cell_t pci_config_read_word(cell_t bus, cell_t device, cell_t function, cell_t offset, cell_t& value);
cell_t pci_config_read_dword(cell_t bus, cell_t device, cell_t function, cell_t offset, cell_t& value);
cell_t pci_config_read_qword(cell_t bus, cell_t device, cell_t function, cell_t offset, cell_t& value);

cell_t pci_config_write_byte(cell_t bus, cell_t device, cell_t function, cell_t offset, cell_t value);
cell_t pci_config_write_word(cell_t bus, cell_t device, cell_t function, cell_t offset, cell_t value);
cell_t pci_config_write_dword(cell_t bus, cell_t device, cell_t function, cell_t offset, cell_t value);
cell_t pci_config_write_qword(cell_t bus, cell_t device, cell_t function, cell_t offset, cell_t value);

// Kernel functions
cell_t get_proc_address(const char* name);

// Function invocation
cell_t invoke(
  cell_t address,
  cell_t& retval,
  cell_t a0,
  cell_t a1,
  cell_t a2,
  cell_t a3,
  cell_t a4,
  cell_t a5,
  cell_t a6,
  cell_t a7,
  cell_t a8,
  cell_t a9,
  cell_t a10,
  cell_t a11,
  cell_t a12,
  cell_t a13,
  cell_t a14,
  cell_t a15
);

// Timing
cell_t microsleep(cell_t us);
cell_t microsleep2(cell_t us);

cell_t qpc(cell_t& frequency);

#if defined(ARCH_A64)
// ARM64-specific functions (if any)

#elif defined(ARCH_X64) || defined(ARCH_X86)
// x86/x64-specific functions

// Dell SMM interface (order: eax ecx edx ebx esi edi)
cell_t query_dell_smm(std::array<cell_t, 6> in, std::array<cell_t, 6>& out);

// Port I/O
void io_out_byte(cell_t port, cell_t value);
void io_out_word(cell_t port, cell_t value);
void io_out_dword(cell_t port, cell_t value);

cell_t io_in_byte(cell_t port);
cell_t io_in_word(cell_t port);
cell_t io_in_dword(cell_t port);

// LWPCB (Lightweight Profiling)
void llwpcb(cell_t addr);
cell_t slwpcb();

// CPUID (order: eax ebx ecx edx)
void cpuid(cell_t leaf, cell_t subleaf, std::array<cell_t, 4>& out);

// Control registers
cell_t cr_read(cell_t cr);
void cr_write(cell_t cr, cell_t value);

// Debug registers
cell_t dr_read(cell_t dr);
void dr_write(cell_t dr, cell_t value);

// Extended control registers
cell_t xcr_read(cell_t xcr, cell_t& value);
cell_t xcr_write(cell_t xcr, cell_t value);

// TLB management
void invlpg(cell_t va);
void invpcid(cell_t type, cell_t descriptor);

// Performance monitoring
cell_t readpmc(cell_t pmc, cell_t& value);

// Time stamp counter
cell_t rdtsc();
cell_t rdtscp(cell_t& pid);

// Random number generation
cell_t rdrand(cell_t& v);
cell_t rdseed(cell_t& v);

// Descriptor tables
void lidt(cell_t limit, cell_t base);
void sidt(cell_t& limit, cell_t& base);

void lgdt(cell_t limit, cell_t base);
void sgdt(cell_t& limit, cell_t& base);

// MXCSR register
cell_t mxcsr_read();
void mxcsr_write(cell_t v);

// Supervisor mode access prevention
void stac();
void clac();

// System instructions
void halt();
void ud2();
void int3();
void int2c();
void wbinvd();

#endif
