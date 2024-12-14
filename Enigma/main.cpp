#pragma once

#ifndef ENTRY
#define ENTRY

// Core includes
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "includes.hpp"

// Security and authentication
#include "backend/auth/auth.h"
#include "frontend/protection/process/antiDbg.h"

// Game functionality
#include "backend/game/world/sdk/cache/caching.hpp"
#include "backend/driver/mouse/mouse.cpp"

// Main entry point
auto main() -> void
{
    // Initialize security
    std::thread(security_loop).detach();
    SonyDriverHelper::api::Init();
    
    // System initialization
    ClearTempFolder();
    system(_("color D")); 
    
    // Disabled authentication and driver loading
    //auth();
    //load_driver();
    
    // Graphics initialization
    vsync();
    utility->init_driver();

    // Get screen dimensions
    globals->width = GetSystemMetrics(SM_CXSCREEN);
    globals->height = GetSystemMetrics(SM_CYSCREEN);
    
    // Initialize render system
    if (!render->hijack())
    {
        MessageBoxA(0, _("{!} Cheat failed"), _(""), MB_ICONINFORMATION);
        exit(0);
    }

    // Show console and initialize game systems
    ShowWindow(GetConsoleWindow(), SW_SHOW);
    caching->start();
    render->DirectXInit();
    caching->menu();

    system(_("pause"));
}

#endif
