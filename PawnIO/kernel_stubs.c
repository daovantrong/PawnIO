// CRT stubs for x86 kernel driver
// Minimal implementations of CRT functions needed by the driver

#include <ntddk.h>

#ifdef __cplusplus
extern "C" {
#endif

// Memory management
void* malloc(size_t size) {
    return ExAllocatePoolWithTag(NonPagedPool, size, 'llam');
}

void free(void* ptr) {
    if (ptr) ExFreePoolWithTag(ptr, 'llam');
}

int _callnewh(size_t size) {
    UNREFERENCED_PARAMETER(size);
    return 0;
}

// Math functions
double ceil(double x) {
    long long i = (long long)x;
    return (x > i) ? (double)(i + 1) : (double)i;
}

// String/memory functions - use manual implementation to avoid recursion
void* memcpy(void* dst, const void* src, size_t count) {
    char* d = (char*)dst;
    const char* s = (const char*)src;
    while (count--) *d++ = *s++;
    return dst;
}

void* memmove(void* dst, const void* src, size_t count) {
    char* d = (char*)dst;
    const char* s = (const char*)src;
    if (d < s) {
        while (count--) *d++ = *s++;
    } else {
        d += count;
        s += count;
        while (count--) *--d = *--s;
    }
    return dst;
}

void* memset(void* dst, int val, size_t count) {
    unsigned char* d = (unsigned char*)dst;
    while (count--) *d++ = (unsigned char)val;
    return dst;
}

// strncmp is provided by ntoskrnl.lib

// Exception handling
void* __std_exception_copy(const void* src, void* dst) {
    if (src && dst) {
        RtlCopyMemory(dst, src, sizeof(void*) * 2);
    }
    return dst;
}

void __std_exception_destroy(void* exception) {
    UNREFERENCED_PARAMETER(exception);
}

void __stdcall _CxxThrowException(void* exception, void* info) {
    UNREFERENCED_PARAMETER(exception);
    UNREFERENCED_PARAMETER(info);
    KeBugCheckEx(DRIVER_VIOLATION, 1, 0, 0, 0);
}

void __invoke_watson(
    const wchar_t* expression,
    const wchar_t* function_name,
    const wchar_t* file_name,
    unsigned int line_number,
    uintptr_t reserved
) {
    UNREFERENCED_PARAMETER(expression);
    UNREFERENCED_PARAMETER(function_name);
    UNREFERENCED_PARAMETER(file_name);
    UNREFERENCED_PARAMETER(line_number);
    UNREFERENCED_PARAMETER(reserved);
    KeBugCheckEx(DRIVER_VIOLATION, 0, 0, 0, 0);
}

// Exception handler for x86
EXCEPTION_DISPOSITION __cdecl __except_handler3(
    struct _EXCEPTION_RECORD* ExceptionRecord,
    void* EstablisherFrame,
    struct _CONTEXT* ContextRecord,
    void* DispatcherContext
) {
    UNREFERENCED_PARAMETER(ExceptionRecord);
    UNREFERENCED_PARAMETER(EstablisherFrame);
    UNREFERENCED_PARAMETER(ContextRecord);
    UNREFERENCED_PARAMETER(DispatcherContext);
    return ExceptionContinueSearch;
}

#ifdef __cplusplus
}
#endif
