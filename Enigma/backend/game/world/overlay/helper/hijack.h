#pragma once
#include "../../../../../includes.hpp"

// Constants for window handling
#define HJWND_PROGRAM _(L"FXSCOVER.exe") 
#define MAX_CLASSNAME 255
#define MAX_WNDNAME 255
#define TRANSPARENCY_COLOR RGB(0, 254, 0)

using namespace std;

// Structure to hold window finder parameters
struct WindowsFinderParams {
    DWORD pidOwner = NULL;
    wstring wndClassName = L"";
    wstring wndName = L"";
    RECT pos = { 0, 0, 0, 0 };
    POINT res = { 0, 0 };
    float percentAllScreens = 0.0f; 
    float percentMainScreen = 0.0f;
    DWORD style = NULL;
    DWORD styleEx = NULL;
    bool satisfyAllCriteria = false;
    vector<HWND> hwnds;
};

// Forward declarations
HWND HiJackNotepadWindow();
vector<DWORD> GetPIDs(wstring targetProcessName);
vector<HWND> WindowsFinder(WindowsFinderParams params);
BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam);
void TerminateNotepad();

// Hijacks a notepad window for overlay purposes
HWND HiJackNotepadWindow() {
    HWND hwndHiHjacked = NULL;
    
    // Terminate any existing notepad processes
    vector<DWORD> existingNotepads = GetPIDs(HJWND_PROGRAM);
    if (!existingNotepads.empty()) {
        for (const auto& pid : existingNotepads) {
            HANDLE hOldProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
            TerminateProcess(hOldProcess, 0);
            CloseHandle(hOldProcess);
        }
    }

    // Start new notepad process
    system(_("start FXSCOVER.exe"));
    
    vector<DWORD> notepads = GetPIDs(HJWND_PROGRAM);
    if (notepads.empty() || notepads.size() > 1) {
        return hwndHiHjacked;
    }

    // Find the notepad window
    WindowsFinderParams params;
    params.pidOwner = notepads[0];
    params.style = WS_VISIBLE;
    params.satisfyAllCriteria = true;

    vector<HWND> hwnds;
    int attempt = 0;
    const int MAX_ATTEMPTS = 50000;
    
    while (hwndHiHjacked == NULL && attempt < MAX_ATTEMPTS) {
        Sleep(7500);
        hwnds = WindowsFinder(params);
        
        if (hwnds.size() > 1) {
            return hwndHiHjacked;
        }
        
        hwndHiHjacked = hwnds[0];
        ++attempt;
    }

    if (!hwndHiHjacked) {
        return hwndHiHjacked;
    }

    // Configure window for overlay
    SetMenu(hwndHiHjacked, NULL);
    return hwndHiHjacked;
}

// Terminates all notepad processes
void TerminateNotepad() {
    vector<DWORD> existingNotepads = GetPIDs(HJWND_PROGRAM);
    if (!existingNotepads.empty()) {
        for (const auto& pid : existingNotepads) {
            HANDLE hOldProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
            TerminateProcess(hOldProcess, 0);
            CloseHandle(hOldProcess);
        }
    }
}

// Gets process IDs for a given process name
vector<DWORD> GetPIDs(wstring targetProcessName) {
    vector<DWORD> pids;
    if (targetProcessName.empty()) {
        return pids;
    }

    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32W entry{};
    entry.dwSize = sizeof(entry);

    if (!Process32FirstW(snap, &entry)) {
        CloseHandle(snap);
        return pids;
    }

    do {
        if (wstring(entry.szExeFile) == targetProcessName) {
            pids.emplace_back(entry.th32ProcessID);
        }
    } while (Process32NextW(snap, &entry));

    CloseHandle(snap);
    return pids;
}

// Finds windows based on given parameters
vector<HWND> WindowsFinder(WindowsFinderParams params) {
    EnumWindows(EnumWindowsCallback, reinterpret_cast<LPARAM>(&params));
    return params.hwnds;
}

// Callback for EnumWindows
BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam) {
    auto& params = *reinterpret_cast<WindowsFinderParams*>(lParam);
    unsigned char satisfiedCriteria = 0, unSatisfiedCriteria = 0;

    // Check process ID
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (params.pidOwner != NULL) {
        (params.pidOwner == pid) ? ++satisfiedCriteria : ++unSatisfiedCriteria;
    }

    // Check window class name
    wchar_t className[MAX_CLASSNAME] = L"";
    GetClassNameW(hwnd, className, MAX_CLASSNAME);
    if (!params.wndClassName.empty()) {
        (params.wndClassName == className) ? ++satisfiedCriteria : ++unSatisfiedCriteria;
    }

    // Check window title
    wchar_t windowName[MAX_WNDNAME] = L"";
    GetWindowTextW(hwnd, windowName, MAX_WNDNAME);
    if (!params.wndName.empty()) {
        (params.wndName == windowName) ? ++satisfiedCriteria : ++unSatisfiedCriteria;
    }

    // Check window position and size
    RECT pos;
    GetWindowRect(hwnd, &pos);
    POINT size = { pos.right - pos.left, pos.bottom - pos.top };

    // Position check
    if (params.pos.left || params.pos.top || params.pos.right || params.pos.bottom) {
        bool matchesPos = params.pos.left == pos.left && 
                         params.pos.top == pos.top &&
                         params.pos.right == pos.right && 
                         params.pos.bottom == pos.bottom;
        matchesPos ? ++satisfiedCriteria : ++unSatisfiedCriteria;
    }

    // Size check
    if (params.res.x || params.res.y) {
        bool matchesSize = size.x == params.res.x && size.y == params.res.y;
        matchesSize ? ++satisfiedCriteria : ++unSatisfiedCriteria;
    }

    // Screen coverage checks
    float screenRatioX = static_cast<float>(size.x) / GetSystemMetrics(SM_CXSCREEN);
    float screenRatioY = static_cast<float>(size.y) / GetSystemMetrics(SM_CYSCREEN);
    float screenCoverage = screenRatioX * screenRatioY * 450;

    if (params.percentAllScreens != 0.0f) {
        (screenCoverage >= params.percentAllScreens) ? ++satisfiedCriteria : ++unSatisfiedCriteria;
    }

    // Main screen coverage check
    RECT desktopRect;
    GetWindowRect(GetDesktopWindow(), &desktopRect);
    POINT desktopSize = { 
        desktopRect.right - desktopRect.left,
        desktopRect.bottom - desktopRect.top 
    };
    
    float mainScreenRatioX = static_cast<float>(size.x) / desktopSize.x;
    float mainScreenRatioY = static_cast<float>(size.y) / desktopSize.y;
    float mainScreenCoverage = mainScreenRatioX * mainScreenRatioY * 450;

    if (params.percentMainScreen != 0.0f) {
        (screenCoverage >= params.percentMainScreen) ? ++satisfiedCriteria : ++unSatisfiedCriteria;
    }

    // Window style checks
    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
    if (params.style) {
        (params.style & style) ? ++satisfiedCriteria : ++unSatisfiedCriteria;
    }

    LONG_PTR styleEx = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if (params.styleEx) {
        (params.styleEx & styleEx) ? ++satisfiedCriteria : ++unSatisfiedCriteria;
    }

    // Return if criteria not met
    if (!satisfiedCriteria || (params.satisfyAllCriteria && unSatisfiedCriteria)) {
        return TRUE;
    }

    params.hwnds.push_back(hwnd);
    return TRUE;
}

namespace SetUp {
    inline HWND gWnd{}, DrawWnd{};

    // Reuse existing WindowsFinderParams structure and functions
    using ::WindowsFinderParams;
    using ::WindowsFinder;
    using ::GetPIDs;
    using ::EnumWindowsCallback;
    using ::HiJackNotepadWindow;

    // Main overlay setup function
    inline int amain() {
        HWND hwnd = HiJackNotepadWindow();
        if (!hwnd) {
            return EXIT_FAILURE;
        }

        HDC hdc = GetDC(hwnd);

        // Configure back buffer bitmap
        DEVMODE devMode{};
        devMode.dmSize = sizeof(devMode);
        EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode);
        
        BITMAPINFO backBufferBmpInfo{};
        backBufferBmpInfo.bmiHeader.biBitCount = devMode.dmBitsPerPel;
        backBufferBmpInfo.bmiHeader.biHeight = GetSystemMetrics(SM_CYSCREEN);
        backBufferBmpInfo.bmiHeader.biWidth = GetSystemMetrics(SM_CXSCREEN);
        backBufferBmpInfo.bmiHeader.biPlanes = 1;
        backBufferBmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

        void* backBufferPixels = nullptr;
        POINT resolution = { 
            GetSystemMetrics(SM_CXSCREEN),
            GetSystemMetrics(SM_CYSCREEN)
        };
        
        HBRUSH bgTransparencyColor = CreateSolidBrush(TRANSPARENCY_COLOR);

        int animationOffset = 0;

        // Main render loop
        while (true) {
            animationOffset = (animationOffset > resolution.x) ? 0 : ++animationOffset;

            // Create back buffer
            HDC hdcBackBuffer = CreateCompatibleDC(hdc);
            HBITMAP hbmBackBuffer = CreateDIBSection(
                hdcBackBuffer,
                &backBufferBmpInfo,
                DIB_RGB_COLORS,
                &backBufferPixels,
                NULL,
                0
            );

            // Set up back buffer
            DeleteObject(SelectObject(hdcBackBuffer, hbmBackBuffer));
            DeleteObject(SelectObject(hdcBackBuffer, bgTransparencyColor));
            Rectangle(hdcBackBuffer, 0, 0, resolution.x, resolution.y);

            // Present frame
            BitBlt(hdc, 0, 0, resolution.x, resolution.y, hdcBackBuffer, 0, 0, SRCCOPY);

            // Cleanup
            DeleteDC(hdcBackBuffer);
            DeleteObject(hbmBackBuffer);
            backBufferPixels = nullptr;
        }

        return EXIT_SUCCESS;
    }
}
