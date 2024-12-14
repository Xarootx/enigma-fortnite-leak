#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <psapi.h>
#include "process.h"
//#include "../Encryption/custom_winapi.hpp"

// Helper function to convert string to lowercase
void to_lower(unsigned char* input) {
    char* p = (char*)input;
    size_t length = strlen(p);
    for (size_t i = 0; i < length; i++) {
        p[i] = tolower(p[i]);
    }
}

// Check for VirtualBox registry entries
bool check_virtualbox_registry() {
    HKEY h_key = 0;
    if (LI_FN(RegOpenKeyExA).forwarded_safe_cached()(
        HKEY_LOCAL_MACHINE, 
        _("HARDWARE\\ACPI\\DSDT\\VBOX__"), 
        0, 
        KEY_READ, 
        &h_key) == ERROR_SUCCESS) {
        *(uintptr_t*)(0) = 1;
    }
    return false;
}

// Check for VirtualBox drivers and components
bool check_virtualbox_drivers() {
    // Check VBox mini driver
    if (LI_FN(CreateFileA).forwarded_safe_cached()(
        "\\\\.\\VBoxMiniRdrDN",
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        0,
        OPEN_EXISTING,
        0,
        0) != INVALID_HANDLE_VALUE) {
        *(uintptr_t*)(0) = 1;
    }

    // Check VBox hook DLL
    if (LI_FN(LoadLibraryA).forwarded_safe_cached()(_("VBoxHook.dll"))) {
        *(uintptr_t*)(0) = 1;
    }

    // Check Guest Additions
    HKEY h_key = 0;
    if (LI_FN(RegOpenKeyExA).forwarded_safe_cached()(
        HKEY_LOCAL_MACHINE,
        _("SOFTWARE\\Oracle\\VirtualBox Guest Additions"),
        0,
        KEY_READ,
        &h_key) == ERROR_SUCCESS && h_key) {
        LI_FN(RegCloseKey).forwarded_safe_cached()(h_key);
        *(uintptr_t*)(0) = 1;
    }

    // Check system BIOS info
    h_key = 0;
    if (LI_FN(RegOpenKeyExA).forwarded_safe_cached()(
        HKEY_LOCAL_MACHINE,
        _("HARDWARE\\DESCRIPTION\\System"),
        0,
        KEY_READ,
        &h_key) == ERROR_SUCCESS) {

        // Check System BIOS Version
        DWORD type = 0;
        DWORD size = 0x100;
        char* systembiosversion = (char*)LI_FN(LocalAlloc).forwarded_safe_cached()(LMEM_ZEROINIT, size + 10);
        
        if (LI_FN(RegQueryValueExA).forwarded_safe_cached()(
            h_key,
            _("SystemBiosVersion"),
            0,
            &type,
            (BYTE*)systembiosversion,
            &size) == ERROR_SUCCESS) {

            to_lower((unsigned char*)systembiosversion);
            if ((type == REG_SZ || type == REG_MULTI_SZ) && strstr(systembiosversion, _("vbox"))) {
                *(uintptr_t*)(0) = 1;
            }
        }
        LI_FN(LocalFree).forwarded_safe_cached()(systembiosversion);

        // Check Video BIOS Version
        type = 0;
        size = 0x200;
        char* videobiosversion = (char*)LI_FN(LocalAlloc).forwarded_safe_cached()(LMEM_ZEROINIT, size + 10);
        
        if (LI_FN(RegQueryValueExA).forwarded_safe_cached()(
            h_key,
            _("VideoBiosVersion"),
            0,
            &type,
            (BYTE*)videobiosversion,
            &size) == ERROR_SUCCESS) {

            if (type == REG_MULTI_SZ) {
                char* video = videobiosversion;
                while (*video) {
                    to_lower((unsigned char*)video);
                    if (strstr(video, _("oracle")) || strstr(video, _("virtualbox"))) {
                        *(uintptr_t*)(0) = 1;
                    }
                    video += strlen(video) + 1;
                }
            }
        }
        LI_FN(LocalFree).forwarded_safe_cached()(videobiosversion);
        LI_FN(RegCloseKey).forwarded_safe_cached()(h_key);
    }
    return false;
}

// Check for VMware processes
void check_vmware_processes() {
    const char* vmware_processes[] = {
        "vmtoolsd.exe",
        "vmwaretray.exe", 
        "vmwareuser.exe",
        "VGAuthService.exe",
        "vmacthlp.exe"
    };

    for (const char* process : vmware_processes) {
        if (find(_(process))) {
            *(uintptr_t*)(0) = 1;
        }
    }
}
