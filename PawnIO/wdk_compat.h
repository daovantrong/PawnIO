// WDK Compatibility Header
// Provides missing definitions from newer WDK versions for x86 builds using WDK 10.0.19041

#pragma once

// PCUCHAR type (added in later WDK)
#ifndef PCUCHAR
typedef const UCHAR *PCUCHAR;
#endif

// FAST_FAIL constants (added in later WDK)
#ifndef FAST_FAIL_INVALID_THREAD_STATE
#define FAST_FAIL_INVALID_THREAD_STATE 0x4A
#endif

#ifndef FAST_FAIL_ASAN_ERROR
#define FAST_FAIL_ASAN_ERROR 0x4B
#endif

// SDDL constants are defined in driver.cpp as UNICODE_STRING, not needed here

// DECLARE_CONST_UNICODE_STRING macro (added in later WDK)
#ifndef DECLARE_CONST_UNICODE_STRING
#define DECLARE_CONST_UNICODE_STRING(name, str) \
    const UNICODE_STRING name = RTL_CONSTANT_STRING(str)
#endif

// RTL_CONSTANT_STRING macro (if not defined)
#ifndef RTL_CONSTANT_STRING
#define RTL_CONSTANT_STRING(s) { sizeof(s) - sizeof((s)[0]), sizeof(s), (PWSTR)(s) }
#endif
