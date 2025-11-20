// x86 Linker Fix - Resolves import decoration mismatches
// Uses /ALTERNATENAME to map decorated symbols to available imports

#ifdef ARCH_X86

// Tell linker to use alternate symbol names for x86 imports
// Format: /ALTERNATENAME:symbol1=symbol2
// Maps what compiler generates to what's available in kernel libs

// Kernel APIs - map undecorated __imp__ to decorated versions
#pragma comment(linker, "/ALTERNATENAME:__imp__ExFreePoolWithTag=__imp__ExFreePoolWithTag@8")
#pragma comment(linker, "/ALTERNATENAME:__imp__ExInitializeResourceLite=__imp__ExInitializeResourceLite@4")
#pragma comment(linker, "/ALTERNATENAME:__imp__ExAcquireResourceSharedLite=__imp__ExAcquireResourceSharedLite@8")
#pragma comment(linker, "/ALTERNATENAME:__imp__ExAcquireResourceExclusiveLite=__imp__ExAcquireResourceExclusiveLite@8")
#pragma comment(linker, "/ALTERNATENAME:__imp__ExReleaseResourceLite=__imp__ExReleaseResourceLite@4")
#pragma comment(linker, "/ALTERNATENAME:__imp__ExDeleteResourceLite=__imp__ExDeleteResourceLite@4")
#pragma comment(linker, "/ALTERNATENAME:__imp__IoDeleteDevice=__imp__IoDeleteDevice@4")
#pragma comment(linker, "/ALTERNATENAME:__imp__KeDelayExecutionThread=__imp__KeDelayExecutionThread@12")
#pragma comment(linker, "/ALTERNATENAME:__imp__KeSetSystemGroupAffinityThread=__imp__KeSetSystemGroupAffinityThread@8")
#pragma comment(linker, "/ALTERNATENAME:__imp__KeRevertToUserGroupAffinityThread=__imp__KeRevertToUserGroupAffinityThread@4")
#pragma comment(linker, "/ALTERNATENAME:__imp__KeQueryActiveProcessorCountEx=__imp__KeQueryActiveProcessorCountEx@2")
#pragma comment(linker, "/ALTERNATENAME:_KeGetProcessorNumberFromIndex=_KeGetProcessorNumberFromIndex@8")
#pragma comment(linker, "/ALTERNATENAME:__imp__MmMapIoSpace=__imp__MmMapIoSpace@12")
#pragma comment(linker, "/ALTERNATENAME:__imp__MmUnmapIoSpace=__imp__MmUnmapIoSpace@8")
#pragma comment(linker, "/ALTERNATENAME:__imp__KeQueryPerformanceCounter=__imp__KeQueryPerformanceCounter@4")
#pragma comment(linker, "/ALTERNATENAME:__imp__KeStallExecutionProcessor=__imp__KeStallExecutionProcessor@4")
#pragma comment(linker, "/ALTERNATENAME:__imp__MmGetVirtualForPhysical=__imp__MmGetVirtualForPhysical@8")
#pragma comment(linker, "/ALTERNATENAME:__imp__KeInitializeEvent=__imp__KeInitializeEvent@12")
#pragma comment(linker, "/ALTERNATENAME:__imp__HalSetBusDataByOffset=__imp__HalSetBusDataByOffset@24")
#pragma comment(linker, "/ALTERNATENAME:__imp__HalGetBusDataByOffset=__imp__HalGetBusDataByOffset@24")

// BCrypt APIs - map undecorated to decorated
#pragma comment(linker, "/ALTERNATENAME:_BCryptOpenAlgorithmProvider@16=_BCryptOpenAlgorithmProvider@16")
#pragma comment(linker, "/ALTERNATENAME:_BCryptGetProperty@24=_BCryptGetProperty@24")
#pragma comment(linker, "/ALTERNATENAME:_BCryptCloseAlgorithmProvider@8=_BCryptCloseAlgorithmProvider@8")
#pragma comment(linker, "/ALTERNATENAME:_BCryptImportKeyPair@28=_BCryptImportKeyPair@28")
#pragma comment(linker, "/ALTERNATENAME:_BCryptDestroyKey@4=_BCryptDestroyKey@4")
#pragma comment(linker, "/ALTERNATENAME:_BCryptVerifySignature@28=_BCryptVerifySignature@28")
#pragma comment(linker, "/ALTERNATENAME:_BCryptCreateHash@28=_BCryptCreateHash@28")
#pragma comment(linker, "/ALTERNATENAME:_BCryptHashData@16=_BCryptHashData@16")
#pragma comment(linker, "/ALTERNATENAME:_BCryptFinishHash@16=_BCryptFinishHash@16")
#pragma comment(linker, "/ALTERNATENAME:_BCryptDestroyHash@4=_BCryptDestroyHash@4")

// Assembly exports - fix mismatches
#pragma comment(linker, "/ALTERNATENAME:___dell@4=___dell@4@0")
#pragma comment(linker, "/ALTERNATENAME:__crdr=__crdr@0")

// Entry point fix
#pragma comment(linker, "/ALTERNATENAME:_DriverEntry@8=_DriverEntry")

// _fltused fix
#pragma comment(linker, "/INCLUDE:__fltused")

#endif // ARCH_X86
