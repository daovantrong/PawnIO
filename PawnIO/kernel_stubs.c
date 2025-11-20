// Kernel mode CRT stubs
// Provides C runtime functions for kernel mode without linking to user-mode CRT

// Disable intrinsics to avoid conflicts
#pragma function(memcpy, memmove, memset, memcmp, ceil, floor, strlen, strcmp, strcpy, strncpy)

#include <ntddk.h>
#include <stddef.h>

// Pool tag for memory allocations
#define PAWNIO_POOL_TAG 'nwaP'

// Floating point used flag - required by linker
#ifdef ARCH_X86
__declspec(selectany) int _fltused = 0x9875;
#pragma comment(linker, "/INCLUDE:__fltused")
#endif

// malloc - Allocate memory from non-paged pool
void* __cdecl malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }
    return ExAllocatePoolWithTag(NonPagedPool, size, PAWNIO_POOL_TAG);
}

// free - Free memory allocated with malloc
void __cdecl free(void* ptr) {
    if (ptr != NULL) {
        ExFreePoolWithTag(ptr, PAWNIO_POOL_TAG);
    }
}

// memcpy - Copy memory
void* __cdecl memcpy(void* dest, const void* src, size_t count) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    
    while (count--) {
        *d++ = *s++;
    }
    
    return dest;
}

// memmove - Copy memory with overlap handling
void* __cdecl memmove(void* dest, const void* src, size_t count) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    
    if (d == s || count == 0) {
        return dest;
    }
    
    if (d < s) {
        // Copy forward
        while (count--) {
            *d++ = *s++;
        }
    } else {
        // Copy backward to handle overlap
        d += count;
        s += count;
        while (count--) {
            *--d = *--s;
        }
    }
    
    return dest;
}

// memset - Fill memory with a byte value
void* __cdecl memset(void* dest, int value, size_t count) {
    unsigned char* d = (unsigned char*)dest;
    unsigned char val = (unsigned char)value;
    
    while (count--) {
        *d++ = val;
    }
    
    return dest;
}

// memcmp - Compare memory
int __cdecl memcmp(const void* buf1, const void* buf2, size_t count) {
    const unsigned char* b1 = (const unsigned char*)buf1;
    const unsigned char* b2 = (const unsigned char*)buf2;
    
    while (count--) {
        if (*b1 != *b2) {
            return (*b1 < *b2) ? -1 : 1;
        }
        b1++;
        b2++;
    }
    
    return 0;
}

// ceil - Ceiling function (integer-based for kernel mode)
double __cdecl ceil(double x) {
    long long i = (long long)x;
    if (x > (double)i) {
        return (double)(i + 1);
    }
    return (double)i;
}

// floor - Floor function
double __cdecl floor(double x) {
    long long i = (long long)x;
    if (x < (double)i) {
        return (double)(i - 1);
    }
    return (double)i;
}

// _callnewh - New handler callback (called when allocation fails)
int __cdecl _callnewh(size_t size) {
    UNREFERENCED_PARAMETER(size);
    return 0; // Return 0 to indicate failure
}

// __std_exception_copy - Exception copy handler
void __cdecl __std_exception_copy(const void* src, void* dest) {
    UNREFERENCED_PARAMETER(src);
    UNREFERENCED_PARAMETER(dest);
    // Do nothing in kernel mode
}

// __std_exception_destroy - Exception destroy handler
void __cdecl __std_exception_destroy(void* exception) {
    UNREFERENCED_PARAMETER(exception);
    // Do nothing in kernel mode
}

// _CxxThrowException - C++ exception throw (causes BSOD in kernel mode)
void __cdecl _CxxThrowException(void* exception, void* throwInfo) {
    UNREFERENCED_PARAMETER(exception);
    UNREFERENCED_PARAMETER(throwInfo);
    
    // In kernel mode, exceptions are fatal
    KeBugCheckEx(
        KERNEL_MODE_EXCEPTION_NOT_HANDLED,
        0xC0000005, // STATUS_ACCESS_VIOLATION
        0,
        0,
        0
    );
}

// __invoke_watson - Watson error reporting (causes BSOD in kernel mode)
void __cdecl __invoke_watson(
    const wchar_t* expression,
    const wchar_t* function,
    const wchar_t* file,
    unsigned int line,
    uintptr_t reserved
) {
    UNREFERENCED_PARAMETER(expression);
    UNREFERENCED_PARAMETER(function);
    UNREFERENCED_PARAMETER(file);
    UNREFERENCED_PARAMETER(line);
    UNREFERENCED_PARAMETER(reserved);
    
    // In kernel mode, this is fatal
    KeBugCheckEx(
        KERNEL_MODE_EXCEPTION_NOT_HANDLED,
        0xDEADBEEF,
        line,
        0,
        0
    );
}

// Note: __security_check_cookie and GS functions are not needed
// because we disabled buffer security checks (/GS-) for x86 builds

// _chkstk - Stack probe (x86 specific)
#ifdef ARCH_X86
void __cdecl _chkstk(void) {
    // Stack probing - implemented in assembly or no-op
}

void __cdecl _alloca_probe(void) {
    // Stack allocation probe
}
#endif

// atexit - Register exit function (no-op in kernel mode)
int __cdecl atexit(void (__cdecl* func)(void)) {
    UNREFERENCED_PARAMETER(func);
    return 0; // Success
}

// __dllonexit - DLL exit handler (no-op in kernel mode)
void* __cdecl __dllonexit(void* func, void** start, void** end) {
    UNREFERENCED_PARAMETER(func);
    UNREFERENCED_PARAMETER(start);
    UNREFERENCED_PARAMETER(end);
    return func;
}

// strlen - String length
size_t __cdecl strlen(const char* str) {
    const char* s = str;
    while (*s) {
        s++;
    }
    return (size_t)(s - str);
}

// strcmp - String compare
int __cdecl strcmp(const char* str1, const char* str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char*)str1 - *(unsigned char*)str2;
}

// strcpy - String copy
char* __cdecl strcpy(char* dest, const char* src) {
    char* d = dest;
    while ((*d++ = *src++) != '\0');
    return dest;
}

// strncpy - String copy with length limit
char* __cdecl strncpy(char* dest, const char* src, size_t count) {
    char* d = dest;
    while (count && (*d++ = *src++) != '\0') {
        count--;
    }
    while (count--) {
        *d++ = '\0';
    }
    return dest;
}
