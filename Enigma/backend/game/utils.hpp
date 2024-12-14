#pragma once
#include "../../includes.hpp"
#include "aimbot/function.hpp"
HWND hwnd;
namespace ut {
    class utils {
    public:
        // Converts wide char string to char string
        __forceinline char* wchar_to_char(const wchar_t* pwchar) {
            int len = 0;
            while (pwchar[len] != '\0') len++;
            
            const int charCount = len + 1;
            char* result = (char*)malloc(sizeof(char) * charCount);
            
            for (int i = 0; i < len; i++) {
                result[i] = (char)pwchar[i];
            }
            result[len] = '\0';
            
            return result;
        }

        // Gets process ID from process name
        __forceinline DWORD PID(LPCWSTR processName) {
            HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
            if (snapshot == INVALID_HANDLE_VALUE)
                return NULL;

            PROCESSENTRY32W entry = { sizeof(PROCESSENTRY32W) };
            DWORD procID = NULL;

            if (Process32FirstW(snapshot, &entry)) {
                do {
                    if (!_wcsicmp(processName, entry.szExeFile)) {
                        procID = entry.th32ProcessID;
                        break;
                    }
                } while (Process32NextW(snapshot, &entry));
            }

            CloseHandle(snapshot);
            return procID;
        }

        // Clamps coordinates within a range
        void Range(double* x, double* y, float range) {
            if (fabs((*x)) <= range && fabs((*y)) <= range)
                return;

            if ((*y) > (*x)) {
                if ((*y) > -(*x)) {
                    (*x) = range * (*x) / (*y);
                    (*y) = range;
                }
                else {
                    (*y) = -range * (*y) / (*x);
                    (*x) = -range;
                }
            }
            else {
                if ((*y) > -(*x)) {
                    (*y) = range * (*y) / (*x);
                    (*x) = range;
                }
                else {
                    (*x) = -range * (*x) / (*y);
                    (*y) = -range;
                }
            }
        }

        // Calculates rotated point position for radar
        Vector2 RotatePoint(Vector2 radar_pos, Vector2 radar_size, Vector3 LocalLocation, Vector2 TargetLocation) {
            double dx = TargetLocation.x - LocalLocation.x;
            double dy = TargetLocation.y - LocalLocation.y;

            double x = -dy;
            double y = -dx;

            const double radar_range = 34000.0;
            Range(&x, &y, radar_range);

            Vector2 result;
            result.x = radar_pos.x + (radar_size.x / 2 + (x / radar_range * radar_size.x));
            result.y = radar_pos.y + (radar_size.y / 2 + (y / radar_range * radar_size.y));

            // Clamp coordinates to radar bounds
            result.x = std::clamp(result.x, radar_pos.x, radar_pos.x + radar_size.x - 5);
            result.y = std::clamp(result.y, radar_pos.y, radar_pos.y + radar_size.y - 5);

            return result;
        }

        // Converts multibyte string to wide string
        __forceinline std::wstring MBytesToWString(const char* str) {
            int len = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
            std::wstring wstr(len - 1, 0);
            MultiByteToWideChar(CP_ACP, 0, str, -1, &wstr[0], len);
            return wstr;
        }

        // Converts wide string to UTF8 string  
        __forceinline std::string WStringToUTF8(const wchar_t* wstr) {
            int len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
            std::string str(len - 1, 0);
            WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &str[0], len, NULL, NULL);
            return str;
        }

        // Draws filled rectangle
        __forceinline void DrawFilledRect(int x, int y, int w, int h, ImU32 color) {
            ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, 0, 0);
        }

        // Draws outlined text
        void draw_outlined_text(ImVec2 pos, ImColor color, std::string text) {
            std::stringstream stream(text);
            std::string line;
            float y = 0.0f;

            for (int i = 0; std::getline(stream, line); i++) {
                ImVec2 textSize = ImGui::CalcTextSize(line.c_str());
                ImVec2 textPos(pos.x - textSize.x / 2.0f, pos.y + textSize.y * i);

                // Draw outline
                const ImVec2 offsets[4] = {
                    ImVec2(1, 1), ImVec2(-1, -1),
                    ImVec2(1, -1), ImVec2(-1, 1)
                };

                for (const auto& offset : offsets) {
                    ImGui::GetBackgroundDrawList()->AddText(
                        ImVec2(textPos.x + offset.x, textPos.y + offset.y),
                        ImGui::GetColorU32(ImVec4(0, 0, 0, 1)),
                        line.c_str()
                    );
                }

                // Draw text
                ImGui::GetBackgroundDrawList()->AddText(textPos, color, line.c_str());
            }
        }

        // Draws formatted string with optional centering and outline
        __forceinline void DrawString(float fontSize, int x, int y, ImColor color, bool bCenter, bool stroke, const char* pText, ...) {
            char buf[1024];
            va_list args;
            va_start(args, pText);
            _vsnprintf_s(buf, sizeof(buf), pText, args);
            va_end(args);

            std::string text = WStringToUTF8(MBytesToWString(buf).c_str());
            
            if (bCenter) {
                ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
                x -= textSize.x / 4;
                y -= textSize.y;
            }

            if (stroke) {
                const ImVec2 offsets[4] = {
                    ImVec2(1, 1), ImVec2(-1, -1),
                    ImVec2(1, -1), ImVec2(-1, 1)
                };

                for (const auto& offset : offsets) {
                    ImGui::GetBackgroundDrawList()->AddText(
                        ImGui::GetFont(),
                        fontSize,
                        ImVec2(x + offset.x, y + offset.y),
                        ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)),
                        text.c_str()
                    );
                }
            }

            ImGui::GetBackgroundDrawList()->AddText(
                ImGui::GetFont(),
                fontSize,
                ImVec2(x, y),
                ImColor(color),
                text.c_str()
            );
        }

        // Initializes driver and connects to Fortnite process
        __forceinline auto init_driver() -> bool {
            system("cls");
            if (!ioctl.start_service())
                return false;

            std::cout << "\n[i] Waiting For Fortnite...\n";

            // Wait for Fortnite window
            while (!(hwnd = FindWindowA(0, "Fortnite  "))) {
                Sleep(100);
            }

            // Get process ID
            globals->pid = PID(L"FortniteClient-Win64-Shipping.exe");
            if (!globals->pid) {
                system("cls");
                Sleep(3500);
                exit(0);
            }

            // Map and initialize driver
            if (ioctl.is_mapped(globals->pid)) {
                ioctl.m_base = ioctl.get_module_base(0);
                if (!ioctl.m_base || !ioctl.resolve_dtb()) {
                    return false;
                }
            }

            globals->imagebase = ioctl.m_base;
            if (!globals->imagebase) {
                system("cls");
                Sleep(3500);
                exit(0);
            }

            system("cls");
            Sleep(2000);
            system("cls");
            return true;
        }
    };
}

static ut::utils* utility = new ut::utils;
