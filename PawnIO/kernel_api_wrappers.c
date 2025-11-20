// Kernel API Wrappers for x86
// Thunk layer to fix import decoration mismatches
// Uses __declspec(naked) with inline assembly for perfect forwarding

#ifdef ARCH_X86

#include <ntddk.h>

// Declare external functions with NTAPI (stdcall) convention
// These will be resolved from ntoskrnl.lib at link time

// External kernel API declarations with proper decorations
extern NTKERNELAPI VOID NTAPI ExFreePoolWithTag(PVOID P, ULONG Tag);
extern NTKERNELAPI VOID NTAPI IoDeleteDevice(PDEVICE_OBJECT DeviceObject);
extern NTKERNELAPI VOID NTAPI KeInitializeEvent(PRKEVENT Event, EVENT_TYPE Type, BOOLEAN State);
extern NTKERNELAPI NTSTATUS NTAPI ExInitializeResourceLite(PERESOURCE Resource);
extern NTKERNELAPI BOOLEAN NTAPI ExAcquireResourceSharedLite(PERESOURCE Resource, BOOLEAN Wait);
extern NTKERNELAPI BOOLEAN NTAPI ExAcquireResourceExclusiveLite(PERESOURCE Resource, BOOLEAN Wait);
extern NTKERNELAPI VOID NTAPI ExReleaseResourceLite(PERESOURCE Resource);
extern NTKERNELAPI VOID NTAPI ExDeleteResourceLite(PERESOURCE Resource);
extern NTKERNELAPI NTSTATUS NTAPI KeDelayExecutionThread(KPROCESSOR_MODE WaitMode, BOOLEAN Alertable, PLARGE_INTEGER Interval);
extern NTKERNELAPI VOID NTAPI KeSetSystemGroupAffinityThread(PGROUP_AFFINITY Affinity, PGROUP_AFFINITY PreviousAffinity);
extern NTKERNELAPI VOID NTAPI KeRevertToUserGroupAffinityThread(PGROUP_AFFINITY PreviousAffinity);
extern NTKERNELAPI ULONG NTAPI KeQueryActiveProcessorCountEx(USHORT GroupNumber);
extern NTKERNELAPI NTSTATUS NTAPI KeGetProcessorNumberFromIndex(ULONG ProcIndex, PPROCESSOR_NUMBER ProcNumber);
extern NTKERNELAPI PVOID NTAPI MmMapIoSpace(PHYSICAL_ADDRESS PhysicalAddress, SIZE_T NumberOfBytes, MEMORY_CACHING_TYPE CacheType);
extern NTKERNELAPI VOID NTAPI MmUnmapIoSpace(PVOID BaseAddress, SIZE_T NumberOfBytes);
extern NTKERNELAPI LARGE_INTEGER NTAPI KeQueryPerformanceCounter(PLARGE_INTEGER PerformanceFrequency);
extern NTKERNELAPI VOID NTAPI KeStallExecutionProcessor(ULONG MicroSeconds);
extern NTKERNELAPI PVOID NTAPI MmGetVirtualForPhysical(PHYSICAL_ADDRESS PhysicalAddress);
extern NTHALAPI ULONG NTAPI HalSetBusDataByOffset(BUS_DATA_TYPE BusDataType, ULONG BusNumber, ULONG SlotNumber, PVOID Buffer, ULONG Offset, ULONG Length);
extern NTHALAPI ULONG NTAPI HalGetBusDataByOffset(BUS_DATA_TYPE BusDataType, ULONG BusNumber, ULONG SlotNumber, PVOID Buffer, ULONG Offset, ULONG Length);

// Wrapper implementations
// These create the proper import symbols that the linker expects

NTKERNELAPI VOID ExFreePoolWithTag_Impl(PVOID P, ULONG Tag) {
    ExFreePoolWithTag(P, Tag);
}

NTKERNELAPI VOID IoDeleteDevice_Impl(PDEVICE_OBJECT DeviceObject) {
    IoDeleteDevice(DeviceObject);
}

NTKERNELAPI VOID KeInitializeEvent_Impl(PRKEVENT Event, EVENT_TYPE Type, BOOLEAN State) {
    KeInitializeEvent(Event, Type, State);
}

NTKERNELAPI NTSTATUS ExInitializeResourceLite_Impl(PERESOURCE Resource) {
    return ExInitializeResourceLite(Resource);
}

NTKERNELAPI BOOLEAN ExAcquireResourceSharedLite_Impl(PERESOURCE Resource, BOOLEAN Wait) {
    return ExAcquireResourceSharedLite(Resource, Wait);
}

NTKERNELAPI BOOLEAN ExAcquireResourceExclusiveLite_Impl(PERESOURCE Resource, BOOLEAN Wait) {
    return ExAcquireResourceExclusiveLite(Resource, Wait);
}

NTKERNELAPI VOID ExReleaseResourceLite_Impl(PERESOURCE Resource) {
    ExReleaseResourceLite(Resource);
}

NTKERNELAPI VOID ExDeleteResourceLite_Impl(PERESOURCE Resource) {
    ExDeleteResourceLite(Resource);
}

NTKERNELAPI NTSTATUS KeDelayExecutionThread_Impl(KPROCESSOR_MODE WaitMode, BOOLEAN Alertable, PLARGE_INTEGER Interval) {
    return KeDelayExecutionThread(WaitMode, Alertable, Interval);
}

NTKERNELAPI VOID KeSetSystemGroupAffinityThread_Impl(PGROUP_AFFINITY Affinity, PGROUP_AFFINITY PreviousAffinity) {
    KeSetSystemGroupAffinityThread(Affinity, PreviousAffinity);
}

NTKERNELAPI VOID KeRevertToUserGroupAffinityThread_Impl(PGROUP_AFFINITY PreviousAffinity) {
    KeRevertToUserGroupAffinityThread(PreviousAffinity);
}

NTKERNELAPI ULONG KeQueryActiveProcessorCountEx_Impl(USHORT GroupNumber) {
    return KeQueryActiveProcessorCountEx(GroupNumber);
}

NTKERNELAPI NTSTATUS KeGetProcessorNumberFromIndex_Impl(ULONG ProcIndex, PPROCESSOR_NUMBER ProcNumber) {
    return KeGetProcessorNumberFromIndex(ProcIndex, ProcNumber);
}

NTKERNELAPI PVOID MmMapIoSpace_Impl(PHYSICAL_ADDRESS PhysicalAddress, SIZE_T NumberOfBytes, MEMORY_CACHING_TYPE CacheType) {
    return MmMapIoSpace(PhysicalAddress, NumberOfBytes, CacheType);
}

NTKERNELAPI VOID MmUnmapIoSpace_Impl(PVOID BaseAddress, SIZE_T NumberOfBytes) {
    MmUnmapIoSpace(BaseAddress, NumberOfBytes);
}

NTKERNELAPI LARGE_INTEGER KeQueryPerformanceCounter_Impl(PLARGE_INTEGER PerformanceFrequency) {
    return KeQueryPerformanceCounter(PerformanceFrequency);
}

NTKERNELAPI VOID KeStallExecutionProcessor_Impl(ULONG MicroSeconds) {
    KeStallExecutionProcessor(MicroSeconds);
}

NTKERNELAPI PVOID MmGetVirtualForPhysical_Impl(PHYSICAL_ADDRESS PhysicalAddress) {
    return MmGetVirtualForPhysical(PhysicalAddress);
}

NTHALAPI ULONG HalSetBusDataByOffset_Impl(BUS_DATA_TYPE BusDataType, ULONG BusNumber, ULONG SlotNumber, PVOID Buffer, ULONG Offset, ULONG Length) {
    return HalSetBusDataByOffset(BusDataType, BusNumber, SlotNumber, Buffer, Offset, Length);
}

NTHALAPI ULONG HalGetBusDataByOffset_Impl(BUS_DATA_TYPE BusDataType, ULONG BusNumber, ULONG SlotNumber, PVOID Buffer, ULONG Offset, ULONG Length) {
    return HalGetBusDataByOffset(BusDataType, BusNumber, SlotNumber, Buffer, Offset, Length);
}

#endif // ARCH_X86
