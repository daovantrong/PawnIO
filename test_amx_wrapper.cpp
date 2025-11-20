#include "PawnIO/stdafx.h"

#define LITTLE_ENDIAN
#include "PawnIO/amx_wrapper.h"

// Test if types are defined
static_assert(sizeof(amx64) > 0, "amx64 should be defined");
static_assert(sizeof(amx64_loader) > 0, "amx64_loader should be defined");

int main() {
    return 0;
}
