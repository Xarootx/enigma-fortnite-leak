#include "../../../../../includes.hpp"

namespace cfg {
    // Helper class for reading/writing config values
    class helper {
    public:
        // Write integer value to config file
        __forceinline BOOL WritePrivateProfileInt(LPCSTR lpAppName, LPCSTR lpKeyName, int nInteger, LPCSTR lpFileName) {
            char lpString[1024];
            sprintf_s(lpString, sizeof(lpString), TEXT("%d"), nInteger);
            return WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, lpFileName);
        }

        // Write float value to config file 
        __forceinline BOOL WritePrivateProfileFloat(LPCSTR lpAppName, LPCSTR lpKeyName, float nInteger, LPCSTR lpFileName) {
            char lpString[1024];
            sprintf_s(lpString, sizeof(lpString), TEXT("%f"), nInteger);
            return WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, lpFileName);
        }

        // Read float value from config file
        __forceinline float GetPrivateProfileFloat(LPCSTR lpAppName, LPCSTR lpKeyName, FLOAT flDefault, LPCSTR lpFileName) {
            char szData[32];
            GetPrivateProfileStringA(lpAppName, lpKeyName, std::to_string(flDefault).c_str(), szData, 32, lpFileName);
            return (float)atof(szData);
        }
    };

    static cfg::helper* help = new cfg::helper();

    // Main config class for saving/loading settings
    class config {
    public:
        // Save all settings to config file
        void savecfg(LPCSTR path) {
            // Aimbot settings
            help->WritePrivateProfileInt("Aimbot", "Enable", aimbot->enable, path);
            help->WritePrivateProfileInt("Aimbot", "Show FOV", aimbot->fov, path);
            help->WritePrivateProfileFloat("Aimbot", "FOV Size", aimbot->fovsize, path);
            help->WritePrivateProfileFloat("Aimbot", "Smoothing", aimbot->smoothing, path);
            help->WritePrivateProfileInt("Aimbot", "Key", aimbot->key, path);
            help->WritePrivateProfileInt("Aimbot", "Bone", aimbot->bone, path);

            // Visual settings
            help->WritePrivateProfileInt("Visuals", "Enable", visuals->enable, path);
            help->WritePrivateProfileInt("Visuals", "Boxes", visuals->box, path);
            help->WritePrivateProfileInt("Visuals", "2D Box", visuals->regular, path);
            help->WritePrivateProfileInt("Visuals", "Corner Box", visuals->cornered, path);
            help->WritePrivateProfileInt("Visuals", "Rounded Box", visuals->rounded, path);
            help->WritePrivateProfileInt("Visuals", "Skeleton", visuals->skeleton, path);
            help->WritePrivateProfileInt("Visuals", "Head ESP", visuals->head, path);
            help->WritePrivateProfileInt("Visuals", "Outlined Skeleton", visuals->skeleton_outlined, path);
            help->WritePrivateProfileInt("Visuals", "Outlined Box", visuals->outlinedbox, path);
            help->WritePrivateProfileInt("Visuals", "Distance", visuals->distance, path);
            help->WritePrivateProfileInt("Visuals", "Weapon", visuals->weapon, path);
            help->WritePrivateProfileInt("Visuals", "Username", visuals->username, path);
            help->WritePrivateProfileInt("Visuals", "Platform", visuals->platform, path);
            help->WritePrivateProfileFloat("Visuals", "Box Thickness", visuals->box_thickness, path);
            help->WritePrivateProfileFloat("Visuals", "Skeleton Thickness", visuals->skeleton_thickness, path);

            // Color settings
            help->WritePrivateProfileFloat("Colors", "Visible_R", colours->visible_color_float[0], path);
            help->WritePrivateProfileFloat("Colors", "Visible_G", colours->visible_color_float[1], path);
            help->WritePrivateProfileFloat("Colors", "Visible_B", colours->visible_color_float[2], path);
            help->WritePrivateProfileFloat("Colors", "Invisible_R", colours->invisible_color_float[0], path);
            help->WritePrivateProfileFloat("Colors", "Invisible_G", colours->invisible_color_float[1], path);
            help->WritePrivateProfileFloat("Colors", "Invisible_B", colours->invisible_color_float[2], path);

            // Misc settings
            help->WritePrivateProfileInt("Misc", "Show FPS", misc->fps, path);
        }

        // Load all settings from config file
        void loadcfg(LPCSTR path) {
            // Load aimbot settings
            aimbot->enable = GetPrivateProfileIntA("Aimbot", "Enable", aimbot->enable, path);
            aimbot->fov = GetPrivateProfileIntA("Aimbot", "Show FOV", aimbot->fov, path);
            aimbot->fovsize = help->GetPrivateProfileFloat("Aimbot", "FOV Size", aimbot->fovsize, path);
            aimbot->smoothing = help->GetPrivateProfileFloat("Aimbot", "Smoothing", aimbot->smoothing, path);
            aimbot->key = GetPrivateProfileIntA("Aimbot", "Key", aimbot->key, path);
            aimbot->bone = GetPrivateProfileIntA("Aimbot", "Bone", aimbot->bone, path);

            // Load visual settings
            visuals->enable = GetPrivateProfileIntA("Visuals", "Enable", visuals->enable, path);
            visuals->box = GetPrivateProfileIntA("Visuals", "Boxes", visuals->box, path);
            visuals->box_type = GetPrivateProfileIntA("Visuals", "Box Type", visuals->box_type, path);
            visuals->head = GetPrivateProfileIntA("Visuals", "Head ESP", visuals->head, path);
            visuals->skeleton = GetPrivateProfileIntA("Visuals", "Skeleton", visuals->skeleton, path);
            visuals->skeleton_outlined = GetPrivateProfileIntA("Visuals", "Outlined Skeleton", visuals->skeleton_outlined, path);
            visuals->outlinedbox = GetPrivateProfileIntA("Visuals", "Outlined Box", visuals->outlinedbox, path);
            visuals->rounded = GetPrivateProfileIntA("Visuals", "Rounded Box", visuals->rounded, path);
            visuals->cornered = GetPrivateProfileIntA("Visuals", "Corner Box", visuals->cornered, path);
            visuals->regular = GetPrivateProfileIntA("Visuals", "2D Box", visuals->regular, path);
            visuals->distance = GetPrivateProfileIntA("Visuals", "Distance", visuals->distance, path);
            visuals->weapon = GetPrivateProfileIntA("Visuals", "Weapon", visuals->weapon, path);
            visuals->username = GetPrivateProfileIntA("Visuals", "Username", visuals->username, path);
            visuals->platform = GetPrivateProfileIntA("Visuals", "Platform", visuals->platform, path);
            visuals->box_thickness = help->GetPrivateProfileFloat("Visuals", "Box Thickness", visuals->box_thickness, path);
            visuals->skeleton_thickness = help->GetPrivateProfileFloat("Visuals", "Skeleton Thickness", visuals->skeleton_thickness, path);

            // Load color settings
            colours->visible_color_float[0] = help->GetPrivateProfileFloat("Colors", "Visible_R", colours->visible_color_float[0], path);
            colours->visible_color_float[1] = help->GetPrivateProfileFloat("Colors", "Visible_G", colours->visible_color_float[1], path);
            colours->visible_color_float[2] = help->GetPrivateProfileFloat("Colors", "Visible_B", colours->visible_color_float[2], path);
            colours->invisible_color_float[0] = help->GetPrivateProfileFloat("Colors", "Invisible_R", colours->invisible_color_float[0], path);
            colours->invisible_color_float[1] = help->GetPrivateProfileFloat("Colors", "Invisible_G", colours->invisible_color_float[1], path);
            colours->invisible_color_float[2] = help->GetPrivateProfileFloat("Colors", "Invisible_B", colours->invisible_color_float[2], path);

            // Load misc settings
            misc->fps = GetPrivateProfileIntA("Misc", "Show FPS", misc->fps, path);
        }
    };
}

static cfg::config* configs = new cfg::config();
