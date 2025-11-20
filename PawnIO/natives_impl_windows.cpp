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

#include "stdafx.h"

#include "natives_impl.h"

#pragma warning(disable: 4309)

cell_t get_arch() {
  return get_architecture_id();
}

cell_t cpu_count() {
  return (cell_t)KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);
}

cell_t cpu_set_affinity(cell_t which, affinity_storage_t& old) {
  PROCESSOR_NUMBER pnum{};
  const auto status = KeGetProcessorNumberFromIndex((ULONG)which, &pnum);
  if (!NT_SUCCESS(status))
    return (cell_t)(scell_t)status;

  GROUP_AFFINITY ga{}, old_ga{};
  ga.Group = pnum.Group;
  ga.Mask = 1ull << pnum.Number;
  KeSetSystemGroupAffinityThread(&ga, &old_ga);
  static_assert(sizeof(old_ga) <= sizeof(affinity_storage_t), "GROUP_AFFINITY too large");
  memcpy(old.data(), &old_ga, sizeof(old_ga));
  return (cell_t)(scell_t)(old_ga.Group == 0 && old_ga.Mask == 0 ? STATUS_UNSUCCESSFUL : STATUS_SUCCESS);
}

cell_t cpu_restore_affinity(affinity_storage_t old) {
  GROUP_AFFINITY ga{};
  static_assert(sizeof(ga) <= sizeof(affinity_storage_t), "GROUP_AFFINITY too large");
  memcpy(&ga, old.data(), sizeof(ga));
  if (ga.Group == 0 && ga.Mask == 0)
    return (cell_t)(scell_t)STATUS_UNSUCCESSFUL; // some idiot passed in the output of a failed cpu_set_affinity
  KeRevertToUserGroupAffinityThread(&ga);
  return (cell_t)(scell_t)STATUS_SUCCESS;
}

void interrupts_disable() { _disable(); }
void interrupts_enable() { _enable(); }

template <typename T>
static cell_t physical_read(cell_t pa, cell_t& v) {
  PHYSICAL_ADDRESS phys;
  phys.QuadPart = (LONGLONG)pa;
  const auto va = MmGetVirtualForPhysical(phys);
  if (!va)
    return (cell_t)(scell_t)STATUS_UNSUCCESSFUL;
  __try {
    v = (cell_t)*(T*)va;
    return (cell_t)(scell_t)STATUS_SUCCESS;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return (cell_t)(scell_t)GetExceptionCode();
  }
}

template <typename T>
static cell_t physical_write(cell_t pa, cell_t v) {
  PHYSICAL_ADDRESS phys;
  phys.QuadPart = (LONGLONG)pa;
  const auto va = MmGetVirtualForPhysical(phys);
  if (!va)
    return (cell_t)(scell_t)STATUS_UNSUCCESSFUL;
  __try {
    *(T*)va = (T)v;
    return (cell_t)(scell_t)STATUS_SUCCESS;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return (cell_t)(scell_t)GetExceptionCode();
  }
}

cell_t physical_read_byte(cell_t pa, cell_t& value) { return physical_read<UCHAR>(pa, value); }
cell_t physical_read_word(cell_t pa, cell_t& value) { return physical_read<USHORT>(pa, value); }
cell_t physical_read_dword(cell_t pa, cell_t& value) { return physical_read<ULONG>(pa, value); }
cell_t physical_read_qword(cell_t pa, cell_t& value) { return physical_read<ULONG64>(pa, value); }

cell_t physical_write_byte(cell_t pa, cell_t value) { return physical_write<UCHAR>(pa, value); }
cell_t physical_write_word(cell_t pa, cell_t value) { return physical_write<USHORT>(pa, value); }
cell_t physical_write_dword(cell_t pa, cell_t value) { return physical_write<ULONG>(pa, value); }
cell_t physical_write_qword(cell_t pa, cell_t value) { return physical_write<ULONG64>(pa, value); }

cell_t io_space_map(cell_t pa, cell_t size) {
  PHYSICAL_ADDRESS physical;
  physical.QuadPart = (scell_t)pa;
  return (cell_t)MmMapIoSpace(physical, (SIZE_T)size, MmNonCached);
}

void io_space_unmap(cell_t va, cell_t size) {
  MmUnmapIoSpace((PVOID)(uptr_t)va, (SIZE_T)size);
}

template <typename T>
static cell_t virtual_read(cell_t va, cell_t& v) {
  __try {
    v = (cell_t)*(T*)(uptr_t)va;
    return (cell_t)(scell_t)STATUS_SUCCESS;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return (cell_t)(scell_t)GetExceptionCode();
  }
}

template <typename T>
static cell_t virtual_write(cell_t va, cell_t v) {
  __try {
    *(T*)(uptr_t)va = (T)v;
    return (cell_t)(scell_t)STATUS_SUCCESS;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return (cell_t)(scell_t)GetExceptionCode();
  }
}

template <typename T>
static cell_t virtual_cmpxchg(cell_t va, cell_t exchange, cell_t comparand) {
  __try {
    bool success = false;
    switch (sizeof(T)) {
    case 1:
      success = (char)comparand == _InterlockedCompareExchange8((char volatile*)(uptr_t)va, (char)exchange, (char)comparand);
      break;
    case 2:
      success = (short)comparand == _InterlockedCompareExchange16((short volatile*)(uptr_t)va, (short)exchange, (short)comparand);
      break;
    case 4:
      success = (long)comparand == _InterlockedCompareExchange((long volatile*)(uptr_t)va, (long)exchange, (long)comparand);
      break;
    case 8:
      success = (int64_t)comparand == _InterlockedCompareExchange64((int64_t volatile*)(uptr_t)va, (int64_t)exchange, (int64_t)comparand);
      break;
    default:
      break;
    }
    return (cell_t)(scell_t)(success ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL);
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return (cell_t)(scell_t)GetExceptionCode();
  }
}

cell_t virtual_read_byte(cell_t va, cell_t& value) { return virtual_read<UCHAR>(va, value); }
cell_t virtual_read_word(cell_t va, cell_t& value) { return virtual_read<USHORT>(va, value); }
cell_t virtual_read_dword(cell_t va, cell_t& value) { return virtual_read<ULONG>(va, value); }
cell_t virtual_read_qword(cell_t va, cell_t& value) { return virtual_read<ULONG64>(va, value); }

cell_t virtual_write_byte(cell_t va, cell_t value) { return virtual_write<UCHAR>(va, value); }
cell_t virtual_write_word(cell_t va, cell_t value) { return virtual_write<USHORT>(va, value); }
cell_t virtual_write_dword(cell_t va, cell_t value) { return virtual_write<ULONG>(va, value); }
cell_t virtual_write_qword(cell_t va, cell_t value) { return virtual_write<ULONG64>(va, value); }

cell_t virtual_cmpxchg_byte2(cell_t va, cell_t exchange, cell_t comparand) { return virtual_cmpxchg<UCHAR>(va, exchange, comparand); }
cell_t virtual_cmpxchg_word2(cell_t va, cell_t exchange, cell_t comparand) { return virtual_cmpxchg<USHORT>(va, exchange, comparand); }
cell_t virtual_cmpxchg_dword2(cell_t va, cell_t exchange, cell_t comparand) { return virtual_cmpxchg<ULONG>(va, exchange, comparand); }
cell_t virtual_cmpxchg_qword2(cell_t va, cell_t exchange, cell_t comparand) { return virtual_cmpxchg<ULONG64>(va, exchange, comparand); }

cell_t virtual_alloc(cell_t size) {
  return (cell_t)ExAllocatePoolZero(NonPagedPoolNx, size, 'nwaP');
}

void virtual_free(cell_t va) {
  ExFreePoolWithTag((PVOID)va, 'nwaP');
}

#pragma warning(push)
#pragma warning(disable: 4996)

static NTSTATUS pci_config_read_raw(ULONG bus, ULONG device, ULONG function, ULONG offset, PVOID buffer, ULONG length) {
  if (length == 0)
    return STATUS_INVALID_PARAMETER;

  PCI_SLOT_NUMBER slot{};
  slot.u.bits.DeviceNumber = device;
  slot.u.bits.FunctionNumber = function;

  USHORT vendor_id{};
  auto result = HalGetBusDataByOffset(
    PCIConfiguration,
    bus,
    slot.u.AsULONG,
    &vendor_id,
    0,
    sizeof(vendor_id)
  );

  if (result == 0)
    return STATUS_NOT_FOUND;

  if (result == 2 && vendor_id == PCI_INVALID_VENDORID)
    return STATUS_DEVICE_DOES_NOT_EXIST;

  result = HalGetBusDataByOffset(
    PCIConfiguration,
    bus,
    slot.u.AsULONG,
    buffer,
    offset,
    length
  );

  if (result == 0)
    return STATUS_NOT_FOUND;

  if (result == 2 && length != 2)
    return STATUS_DEVICE_DOES_NOT_EXIST;

  if (result != length)
    return STATUS_UNSUCCESSFUL;

  return STATUS_SUCCESS;
}

static NTSTATUS pci_config_write_raw(ULONG bus, ULONG device, ULONG function, ULONG offset, PVOID buffer, ULONG length) {
  if (length == 0)
    return STATUS_INVALID_PARAMETER;

  PCI_SLOT_NUMBER slot{};
  slot.u.bits.DeviceNumber = device;
  slot.u.bits.FunctionNumber = function;

  USHORT vendor_id{};
  auto result = HalGetBusDataByOffset(
    PCIConfiguration,
    bus,
    slot.u.AsULONG,
    &vendor_id,
    0,
    sizeof(vendor_id)
  );

  if (result == 0)
    return STATUS_NOT_FOUND;

  if (result == 2 && vendor_id == PCI_INVALID_VENDORID)
    return STATUS_DEVICE_DOES_NOT_EXIST;

  result = HalSetBusDataByOffset(
    PCIConfiguration,
    bus,
    slot.u.AsULONG,
    buffer,
    offset,
    length
  );

  if (result != length)
    return STATUS_UNSUCCESSFUL;

  return STATUS_SUCCESS;
}

#pragma warning(pop)

template <typename T>
FORCEINLINE static cell_t pci_config_read(cell_t bus, cell_t device, cell_t function, cell_t offset, cell_t& value) {
  T t{};
  const auto status = (cell_t)(scell_t)pci_config_read_raw((ULONG)bus, (ULONG)device, (ULONG)function, (ULONG)offset, &t, sizeof(t));
  value = (cell_t)t;
  return status;
}

template <typename T>
FORCEINLINE static cell_t pci_config_write(cell_t bus, cell_t device, cell_t function, cell_t offset, cell_t value) {
  T t{(T)value};
  return (cell_t)(scell_t)pci_config_write_raw((ULONG)bus, (ULONG)device, (ULONG)function, (ULONG)offset, &t, sizeof(t));
}

cell_t pci_config_read_byte(cell_t bus, cell_t device, cell_t function, cell_t offset, cell_t& value) { return pci_config_read<UCHAR>(bus, device, function, offset, value); }
cell_t pci_config_read_word(cell_t bus, cell_t device, cell_t function, cell_t offset, cell_t& value) { return pci_config_read<USHORT>(bus, device, function, offset, value); }
cell_t pci_config_read_dword(cell_t bus, cell_t device, cell_t function, cell_t offset, cell_t& value) { return pci_config_read<ULONG>(bus, device, function, offset, value); }
cell_t pci_config_read_qword(cell_t bus, cell_t device, cell_t function, cell_t offset, cell_t& value) { return pci_config_read<ULONG64>(bus, device, function, offset, value); }

cell_t pci_config_write_byte(cell_t bus, cell_t device, cell_t function, cell_t offset, cell_t value) { return pci_config_write<UCHAR>(bus, device, function, offset, value); }
cell_t pci_config_write_word(cell_t bus, cell_t device, cell_t function, cell_t offset, cell_t value) { return pci_config_write<USHORT>(bus, device, function, offset, value); }
cell_t pci_config_write_dword(cell_t bus, cell_t device, cell_t function, cell_t offset, cell_t value) { return pci_config_write<ULONG>(bus, device, function, offset, value); }
cell_t pci_config_write_qword(cell_t bus, cell_t device, cell_t function, cell_t offset, cell_t value) { return pci_config_write<ULONG64>(bus, device, function, offset, value); }

cell_t get_proc_address(const char* name) {
  const auto len = strlen(name);
  wchar_t name_w[1024]{};
  const auto maxlen = min(len, std::size(name_w) - 1);
  for (size_t i = 0; i < maxlen; ++i)
    name_w[i] = name[i];
  UNICODE_STRING ustr;
  RtlInitUnicodeString(&ustr, name_w);
  return (cell_t)MmGetSystemRoutineAddress(&ustr);
}

 __declspec(guard(nocf)) cell_t invoke(
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
) {
  const auto p = (uintptr_t(*)(uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t))address;
  __try {
    retval = p(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15);
    return (cell_t)(scell_t)STATUS_SUCCESS;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return (cell_t)(scell_t)GetExceptionCode();
  }
}

cell_t microsleep(cell_t us) {
  LARGE_INTEGER li;
  li.QuadPart = (scell_t)us * -10;
  return (cell_t)(scell_t)KeDelayExecutionThread(KernelMode, FALSE, &li);
}

cell_t microsleep2(cell_t us) {
  KeStallExecutionProcessor((ULONG)us);
  return (cell_t)(scell_t)STATUS_SUCCESS;
}

cell_t qpc(cell_t& frequency) {
  LARGE_INTEGER freq{};
  const auto v = KeQueryPerformanceCounter(&freq).QuadPart;
  frequency = (cell_t)freq.QuadPart;
  return (cell_t)v;
}

#if defined(ARCH_A64)

unsigned arm_mrs(unsigned instruction);
void arm_msr(unsigned instruction, unsigned v);

cell_t msr_read(cell_t msr, cell_t& value) {
  value = 0;
  if ((msr & 0xFFFFFFFFFFF00000) != 0xD5300000) {
    return (cell_t)(scell_t)STATUS_INVALID_PARAMETER;
  }
  __try {
    value = (cell_t)arm_mrs((ULONG)msr);
    return (cell_t)(scell_t)STATUS_SUCCESS;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return (cell_t)(scell_t)GetExceptionCode();
  }
}

cell_t msr_write(cell_t msr, cell_t value) {
  if ((msr & 0xFFFFFFFFFFF00000) != 0xD5300000) {
    return (cell_t)(scell_t)STATUS_INVALID_PARAMETER;
  }

  __try {
    arm_msr((ULONG)msr, (ULONG)value);
    return (cell_t)(scell_t)STATUS_SUCCESS;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return (cell_t)(scell_t)GetExceptionCode();
  }
}

#endif

#if defined(ARCH_X64) || defined(ARCH_X86)

#ifdef ARCH_X86
extern "C" ULONG __stdcall _dell(ULONG* smm); // Compiler adds _ prefix -> __dell@4
#else
extern "C" ULONG __fastcall _dell(ULONG* smm);
#endif

cell_t query_dell_smm(std::array<cell_t, 6> in, std::array<cell_t, 6>& out) {
  ULONG regs[6];
  for (auto i = 0; i < 6; ++i)
    regs[i] = (ULONG)in[i];
  const auto result = _dell(regs);
  for (auto i = 0; i < 6; ++i)
    out[i] = regs[i];
  return (result == 0 && (regs[0] & 0xFFFF) != 0xFFFF && regs[0] != (ULONG)in[0]);
}

void io_out_byte(cell_t port, cell_t value) { __outbyte((USHORT)port, (UCHAR)value); }
void io_out_word(cell_t port, cell_t value) { __outword((USHORT)port, (USHORT)value); }
void io_out_dword(cell_t port, cell_t value) { __outdword((USHORT)port, (ULONG)value); }

cell_t io_in_byte(cell_t port) { return __inbyte((USHORT)port); }
cell_t io_in_word(cell_t port) { return __inword((USHORT)port); }
cell_t io_in_dword(cell_t port) { return __indword((USHORT)port); }

void llwpcb(cell_t addr) { __llwpcb((void*)(uptr_t)addr); }
cell_t slwpcb() { return (cell_t)(uptr_t)__slwpcb(); }

cell_t msr_read(cell_t msr, cell_t& value) {
  // clamp
  msr = (ULONG)msr;

  value = 0;
  __try {
    value = (cell_t)__readmsr((ULONG)msr);
    return (cell_t)(scell_t)STATUS_SUCCESS;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return (cell_t)(scell_t)GetExceptionCode();
  }
}

cell_t msr_write(cell_t msr, cell_t value) {
  // clamp
  msr = (ULONG)msr;

  __try {
    __writemsr((ULONG)msr, value);
    return (cell_t)(scell_t)STATUS_SUCCESS;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return (cell_t)(scell_t)GetExceptionCode();
  }
}

void cpuid(cell_t leaf, cell_t subleaf, std::array<cell_t, 4>& out) {
  int out32[4];
  __cpuidex(out32, (int)leaf, (int)subleaf);
  for (size_t i = 0; i < 4; ++i)
    out[i] = out32[i];
}

extern "C" cell_t _crdr(cell_t id, cell_t v);

static cell_t crdr_wrap(cell_t v, cell_t idx, bool is_cr, bool is_wr) {
  return _crdr((idx & 0xF) << 3 | (cell_t)is_cr << 7 | (cell_t)is_wr << 8, v);
}

cell_t cr_read(cell_t cr) { return crdr_wrap(0, cr, true, false); }
void cr_write(cell_t cr, cell_t value) { crdr_wrap(value, cr, true, true); }

cell_t dr_read(cell_t dr) { return crdr_wrap(0, dr, false, false); }
void dr_write(cell_t dr, cell_t value) { crdr_wrap(value, dr, false, true); }

cell_t xcr_read(cell_t xcr, cell_t& value) {
  value = 0;
  __try {
    value = (cell_t)_xgetbv((ULONG)xcr);
    return (cell_t)(scell_t)STATUS_SUCCESS;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return (cell_t)(scell_t)GetExceptionCode();
  }
}

cell_t xcr_write(cell_t xcr, cell_t value) {
  __try {
    _xsetbv((ULONG)xcr, value);
    return (cell_t)(scell_t)STATUS_SUCCESS;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return (cell_t)(scell_t)GetExceptionCode();
  }
}

void invlpg(cell_t va) { __invlpg((void*)(uptr_t)va); }
void invpcid(cell_t type, cell_t descriptor) { _invpcid((unsigned)type, (void*)(uptr_t)descriptor); }

cell_t readpmc(cell_t pmc, cell_t& value) {
  value = 0;
  __try {
    value = (cell_t)__readpmc((ULONG)pmc);
    return (cell_t)(scell_t)STATUS_SUCCESS;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return (cell_t)(scell_t)GetExceptionCode();
  }
}

cell_t rdtsc() { return (cell_t)__rdtsc(); }

cell_t rdtscp(cell_t& pid) {
  unsigned _pid{};
  const auto res = __rdtscp(&_pid);
  pid = (cell_t)_pid;
  return (cell_t)res;
}

#ifdef ARCH_X86
cell_t rdrand(cell_t& v) { return _rdrand32_step((unsigned int*)&v); }
cell_t rdseed(cell_t& v) { return _rdseed32_step((unsigned int*)&v); }
#else
cell_t rdrand(cell_t& v) { return _rdrand64_step((unsigned __int64*)&v); }
cell_t rdseed(cell_t& v) { return _rdseed64_step((unsigned __int64*)&v); }
#endif

__pragma(pack(push, 1))

struct idtrgdtr {
  uint16_t limit;
  uintptr_t base;
} __pragma(pack(pop));

void lidt(cell_t limit, cell_t base) {
  idtrgdtr v{};
  v.limit = (uint16_t)limit;
  v.base = base;
  __lidt(&v);
}

void sidt(cell_t& limit, cell_t& base) {
  idtrgdtr v{};
  __sidt(&v);
  limit = v.limit;
  base = v.base;
}

void lgdt(cell_t limit, cell_t base) {
  idtrgdtr v;
  v.limit = (uint16_t)limit;
  v.base = base;
  _lgdt(&v);
}

void sgdt(cell_t& limit, cell_t& base) {
  idtrgdtr v{};
  _sgdt(&v);
  limit = v.limit;
  base = v.base;
}

cell_t mxcsr_read() { return _mm_getcsr(); }
void mxcsr_write(cell_t v) { _mm_setcsr((unsigned)v); }

void stac() { _stac(); }
void clac() { _clac(); }

void halt() { __halt(); }

void ud2() { __ud2(); }

void int3() { __debugbreak(); }

void int2c() { __int2c(); }

void wbinvd() { __wbinvd(); }

#endif
