#pragma once
#ifndef OVERLAY
#define OVERLAY

#include "../../entity.hpp"
#include <dxgi.h>
#include <d3d11.h>
#include "hijack.h"
#include "../../../../framework/imgui/Font.h"

// DirectX device and rendering objects
ID3D11Device* g_pd3dDevice = nullptr;
ID3D11DeviceContext* g_pd3dDeviceContext = nullptr; 
IDXGISwapChain* g_pSwapChain = nullptr;
ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

// ImGui fonts
ImFont* Main_Font = nullptr;
ImFont* Font1 = nullptr;
ImFont* ico = nullptr;
ImFont* icons = nullptr;

// Window handle and state
HWND window = nullptr;
bool clickable = false;

// Helper function to toggle window click-through
void change_click(bool canclick) {
    long style = ::GetWindowLong(window, GWL_EXSTYLE);
    if (canclick) {
        style &= ~WS_EX_LAYERED;
        SetWindowLong(window, GWL_EXSTYLE, style);
        SetFocus(window);
        clickable = true;
    } else {
        style |= WS_EX_LAYERED;
        SetWindowLong(window, GWL_EXSTYLE, style);
        SetFocus(window);
        clickable = false;
    }
}

namespace DirectX {
    class DirectX11 {
    public:
        // Hijacks window for overlay
        __forceinline auto hijack() -> bool {
            while (!window) {
                window = SetUp::HiJackNotepadWindow();
                Sleep(10);
            }

            // Configure window properties
            MARGINS margin = { -1 };
            DwmExtendFrameIntoClientArea(window, &margin);
            SetMenu(window, NULL);
            SetWindowLongPtr(window, GWL_STYLE, WS_VISIBLE);
            SetWindowLongPtr(window, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT);

            ShowWindow(window, SW_SHOW);
            UpdateWindow(window);

            SetWindowLong(window, GWL_EXSTYLE, GetWindowLong(window, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);

            return true;
        }

        // Creates render target view
        __forceinline void createrender() {
            ID3D11Texture2D* pBackBuffer;
            g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
            g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
            pBackBuffer->Release();
        }

        // Cleanup render target
        __forceinline void cleanrender() {
            if (g_mainRenderTargetView) {
                g_mainRenderTargetView->Release();
                g_mainRenderTargetView = nullptr;
            }
        }

        // Cleanup DirectX resources
        void cleand3d() {
            cleanrender();
            if (g_pSwapChain) {
                g_pSwapChain->Release();
                g_pSwapChain = nullptr;
            }
            if (g_pd3dDeviceContext) {
                g_pd3dDeviceContext->Release();
                g_pd3dDeviceContext = nullptr;
            }
            if (g_pd3dDevice) {
                g_pd3dDevice->Release();
                g_pd3dDevice = nullptr;
            }
        }

        // Initialize DirectX and ImGui
        HRESULT DirectXInit() {
            // Setup swap chain
            DXGI_SWAP_CHAIN_DESC SwapChainDesc = {};
            SwapChainDesc.Windowed = TRUE;
            SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
            SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            SwapChainDesc.BufferDesc.RefreshRate = {60, 1};
            SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            SwapChainDesc.BufferCount = 1;
            SwapChainDesc.OutputWindow = window;
            SwapChainDesc.SampleDesc = {8, 0};

            // Create device and swap chain
            D3D_FEATURE_LEVEL featureLevel;
            const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
            if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, 
                featureLevelArray, 2, D3D11_SDK_VERSION, &SwapChainDesc, &g_pSwapChain, 
                &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext)))
                return false;

            // Setup ImGui
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            io.IniFilename = nullptr;

            // Load fonts
            Main_Font = io.Fonts->AddFontFromMemoryTTF(&MainFont, sizeof MainFont, 21.f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
            Font1 = io.Fonts->AddFontFromMemoryTTF(&Font, sizeof Font, 20.f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
            ico = io.Fonts->AddFontFromMemoryTTF(&icon, sizeof icon, 30.f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
            icons = io.Fonts->AddFontFromMemoryTTF(&Icon, sizeof Icon, 20.f, nullptr, io.Fonts->GetGlyphRangesCyrillic());

            ImGui::StyleColorsDark();
            io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

            // Create render target
            createrender();

            ImGui_ImplWin32_Init(window);
            ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

            SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
            return S_OK;
        }

        // Main render loop
        __forceinline auto init() -> void {
            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            drawer = new v::draw();
            visualiser = new v::vis();

            // Toggle menu on Insert key
            if (GetAsyncKeyState(VK_INSERT) & 1)
                globals->show_menu = !globals->show_menu;

            if (globals->show_menu) {
                if (!clickable)
                    change_click(true);
                drawer->menu();
            } else {
                if (clickable)
                    change_click(false);
            }

            entity->loop();

            // Render frame
            ImGui::Render();
            const ImVec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
            g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
            g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_color);

            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
            g_pSwapChain->Present(1, 0);
        }

        // Main message loop
        __forceinline auto draw() -> bool {
            MSG Message = {};
            while (Message.message != WM_QUIT) {
                if (PeekMessage(&Message, window, 0, 0, PM_REMOVE)) {
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }

                // Handle mouse input
                ImGuiIO& io = ImGui::GetIO();
                POINT cursorPos;
                GetCursorPos(&cursorPos);
                io.MousePos = ImVec2((float)cursorPos.x, (float)cursorPos.y);
                
                io.MouseDown[0] = GetAsyncKeyState(VK_LBUTTON) != 0;
                if (io.MouseDown[0]) {
                    io.MouseClicked[0] = true;
                    io.MouseClickedPos[0] = io.MousePos;
                }

                init();
            }

            // Cleanup
            ImGui_ImplDX11_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();
            cleand3d();
            DestroyWindow(window);
            
            return Message.wParam;
        }
    };
}

static DirectX::DirectX11* render = new DirectX::DirectX11();

#endif
