#pragma once
#include <string>
#include <winnt.h>
#include <libloaderapi.h>
#include <time.h>
#include <memoryapi.h>
#include "../encryption/string/xor.hpp"

// Generates a random string of specified length using alphanumeric characters
std::string random_string(size_t length = 32) {
    std::string result;
    static const std::string chars = _("ABCDEFGHIJKLMNOPQRSTUVXYZabcdefghijklmopqrstuvxyz123456890");
    
    // Initialize random seed only once
    static bool seeded = false;
    if (!seeded) {
        srand(static_cast<unsigned>(time(nullptr)));
        seeded = true;
    }

    result.reserve(length); // Pre-allocate memory
    for (size_t i = 0; i < length; i++) {
        result += chars[rand() % chars.length()];
    }

    return result;
}

// Obfuscates PE headers and section information to make reverse engineering harder
void obfuscator() {
    // Get base address of current module
    const auto base = reinterpret_cast<uint64_t>(GetModuleHandleA(nullptr));
    if (!base) return;

    // Access PE headers
    const auto dos = reinterpret_cast<PIMAGE_DOS_HEADER>(base);
    const auto nt = reinterpret_cast<PIMAGE_NT_HEADERS64>(base + dos->e_lfanew);
    auto nt_copy = reinterpret_cast<PIMAGE_NT_HEADERS64>(malloc(sizeof(IMAGE_NT_HEADERS64)));

    // Process each section
    auto section = IMAGE_FIRST_SECTION(nt);
    for (uint32_t i = 0; i < nt->FileHeader.NumberOfSections; i++, section++) {
        DWORD old_protect;
        
        // Change section protection to allow writing
        if (VirtualProtect(section, sizeof(*section), PAGE_EXECUTE_READWRITE, &old_protect)) {
            // Randomize section name and zero out key fields
            section->Name[0] = static_cast<BYTE>(rand() & 0xff);
            section->Name[1] = '\0';
            section->VirtualAddress = 0;
            section->Misc.PhysicalAddress = 0;
            section->Misc.VirtualSize = 0;

            // Restore original protection
            VirtualProtect(section, sizeof(*section), old_protect, &old_protect);
        }
    }

    // Wipe NT headers
    DWORD old_protect;
    if (VirtualProtect(nt, sizeof(IMAGE_NT_HEADERS64), PAGE_EXECUTE_READWRITE, &old_protect)) {
        memset(nt_copy, 0, sizeof(IMAGE_NT_HEADERS64));
        memcpy(nt, nt_copy, sizeof(IMAGE_NT_HEADERS64));
        VirtualProtect(nt, sizeof(IMAGE_NT_HEADERS64), old_protect, &old_protect);
    }

    // Clean up
    free(nt_copy);
}
