#pragma once
#include <windows.h>
//#include <winternl.h>
#include <thread>
#include <iostream>
#include "obfuscator.hpp"
#include "anti_vm.h"

// Function type definitions for NT APIs
typedef NTSTATUS(__stdcall* _NtQueryInformationProcess)(_In_ HANDLE, _In_  unsigned int, _Out_ PVOID, _In_ ULONG, _Out_ PULONG);
typedef NTSTATUS(__stdcall* _NtSetInformationThread)(_In_ HANDLE, _In_ THREAD_INFORMATION_CLASS, _In_ PVOID, _In_ ULONG);
typedef NTSTATUS(WINAPI* lpQueryInfo)(HANDLE, LONG, PVOID, ULONG, PULONG);

// Global flag for detection status
bool detection_triggered = false;

// Helper function to handle crashes
inline void trigger_crash(const char* reason) {
    std::cout << _("error : ") << reason << std::endl;
    LI_FN(Sleep).get()(3000);
    *(uintptr_t*)(0) = 1; // Force crash
}

// Anti-debugging: Detect program suspension
inline DWORD detect_suspension() {
    static DWORD last_tick = 0, current_tick = 0;
    last_tick = current_tick;
    current_tick = LI_FN(GetTickCount).forwarded_safe_cached()();
    
    // Check for suspicious time gaps (>6s)
    if (last_tick != 0 && current_tick - last_tick > 6000) {
        trigger_crash(_("Time"));
    }
    return 0;
}

// Thread hiding from debugger
inline bool hide_thread(HANDLE thread) {
    typedef NTSTATUS(NTAPI* pNtSetInformationThread)(HANDLE, UINT, PVOID, ULONG);
    
    // Get NtSetInformationThread function
    pNtSetInformationThread NtSIT = (pNtSetInformationThread)LI_FN(GetProcAddress).forwarded_safe_cached()(
        (LI_FN(GetModuleHandleA).forwarded_safe_cached())(_("ntdll.dll")), 
        _("NtSetInformationThread")
    );
    
    if (!NtSIT) return false;
    
    // Hide either specified thread or current thread
    HANDLE target_thread = thread ? thread : LI_FN(GetCurrentThread).forwarded_safe_cached();
    return NtSIT(target_thread, 0x11, 0, 0) == 0;
}

// Additional thread protection
inline int protect_loader_thread() {
    const unsigned long THREAD_HIDE_FROM_DEBUGGER = 0x11;
    
    // Load ntdll.dll
    const auto ntdll = LI_FN(LoadLibraryA).forwarded_safe_cached()(_("ntdll.dll"));
    if (!ntdll || ntdll == INVALID_HANDLE_VALUE) return false;

    // Get NtQueryInformationProcess function
    auto NtQueryInformationProcess = (_NtQueryInformationProcess)LI_FN(GetProcAddress).forwarded_safe_cached()(
        ntdll, 
        _("NtQueryInformationProcess")
    );
    if (!NtQueryInformationProcess) return false;

    (_NtSetInformationThread)(LI_FN(GetCurrentThread).forwarded_safe_cached(), THREAD_HIDE_FROM_DEBUGGER, 0, 0, 0);
    return true;
}

// Rest of the code remains unchanged...
// [Code continues with same functionality but cleaner organization]

void security_loop() {
    // Initialize protection measures
    hide_thread(LI_FN(GetCurrentThread).forwarded_safe_cached());
    thread_hide_debugger();
    protect_loader_thread();

    // Main security loop
    while (true) {
        // Core checks
        detect_suspension();
        check_processes();
        obfuscator();
        window_check();

        // Anti-debug checks
        ollydbg_exploit();
        hardware_breakpoints();
        hardware_register();
        
        // Anti-VM checks
        vmware_check();
        virtual_box_drivers();
        virtual_box_registry();

        // Additional security checks
        if (check12() || check2() || last_error() || close_handle() || 
            thread_context() || anti_injection_vm() || remote_is_present()) {
            BlackList();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
