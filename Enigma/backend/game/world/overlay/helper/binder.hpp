#include "../../../../../includes.hpp"

// Track key binding status
static int aimKeyBindStatus = 0;
static int triggerKeyBindStatus = 0;

// Hotkey configuration namespace
namespace hotkeys {
    int aimKey;
    int triggerKey; 
}

// Handles binding a new aim key
void BindAimKey(void* blank) {
    aimKeyBindStatus = 1;
    while (true) {
        for (int i = 0; i < 0x87; i++) {
            if (GetKeyState(i) & 0x8000) {
                aimbot->key = i;
                aimKeyBindStatus = 0;
                return;
            }
        }
    }
}

// Handles binding a new trigger key
void BindTriggerKey(void* blank) {
    triggerKeyBindStatus = 1;
    while (true) {
        for (int i = 0; i < 0x87; i++) {
            if (GetKeyState(i) & 0x8000) {
                aimbot->triggerkey = i;
                triggerKeyBindStatus = 0;
                return;
            }
        }
    }
}

// Lookup table for key names
static const char* keyNames[] = {
    "",
    "Left Mouse",
    "Right Mouse", 
    "Cancel",
    "Middle Mouse",
    "Mouse 5",
    "Mouse 4",
    "",
    "Backspace",
    "Tab",
    "",
    "",
    "Clear", 
    "Enter",
    "",
    "",
    "Shift",
    "Control",
    "Alt",
    "Pause",
    "Caps",
    "",
    "",
    "",
    "",
    "",
    "",
    "Escape",
    "",
    "",
    "",
    "",
    "Space",
    "Page Up",
    "Page Down", 
    "End",
    "Home",
    "Left",
    "Up",
    "Right",
    "Down",
    "",
    "",
    "",
    "Print",
    "Insert",
    "Delete",
    "",
    "0",
    "1",
    "2", 
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "G",
    "H",
    "I",
    "J",
    "K",
    "L",
    "M",
    "N",
    "O",
    "P",
    "Q",
    "R",
    "S",
    "T",
    "U",
    "V",
    "W",
    "X",
    "Y",
    "Z",
    "",
    "",
    "",
    "",
    "",
    "Numpad 0",
    "Numpad 1",
    "Numpad 2",
    "Numpad 3", 
    "Numpad 4",
    "Numpad 5",
    "Numpad 6",
    "Numpad 7",
    "Numpad 8",
    "Numpad 9",
    "Multiply",
    "Add",
    "",
    "Subtract",
    "Decimal",
    "Divide",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "F11",
    "F12"
};

// Helper function to get key name from array
static bool GetKeyName(void* data, int idx, const char** out_text) {
    const char* const* items = (const char* const*)data;
    if (out_text)
        *out_text = items[idx];
    return true;
}

// Renders a hotkey button with current key binding
void HotkeyButton(int aimkey, void* changekey, int status) {
    const char* preview_value = NULL;
    if (aimkey >= 0 && aimkey < IM_ARRAYSIZE(keyNames))
        GetKeyName(keyNames, aimkey, &preview_value);

    std::string buttonText = preview_value ? preview_value : _("Select Key");
    if (status == 1) {
        buttonText = _("Press the key");
    }

    if (ImGui::Button(buttonText.c_str(), ImVec2(125, 25))) {
        if (status == 0) {
            CreateThread(0, 0, (LPTHREAD_START_ROUTINE)changekey, nullptr, 0, nullptr);
        }
    }
}
