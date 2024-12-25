#pragma once
#include <XInput.h>
#include <Windows.h>
#pragma comment(lib, "XInput.lib")

namespace controller {

// Class to handle Xbox controller input
class XboxController {
private:
    XINPUT_STATE _controllerState;
    int _controllerNum;

public:
    XboxController(int playerNumber);
    XINPUT_STATE GetControllerState();
    bool CheckConnection();
};

XboxController::XboxController(int playerNumber) {
    _controllerNum = playerNumber - 1;
}

XINPUT_STATE XboxController::GetControllerState() {
    ZeroMemory(&_controllerState, sizeof(XINPUT_STATE));
    XInputGetState(_controllerNum, &_controllerState);
    return _controllerState;
}

bool XboxController::CheckConnection() {
    ZeroMemory(&_controllerState, sizeof(XINPUT_STATE));
    return XInputGetState(_controllerNum, &_controllerState) == ERROR_SUCCESS;
}

// Constants
constexpr BYTE XINPUT_GAMEPAD_TRIGGER_THRESHOLD = 30;

// Global controller instance
static XboxController* Player1 = new XboxController(1);

// Main controller polling function
void PollController() {
    while (true) {
        if (Player1->CheckConnection()) {
            const auto& state = Player1->GetControllerState();
            
            // Handle trigger inputs
            if (state.Gamepad.bRightTrigger) {
                // Right trigger pressed
            }
            if (state.Gamepad.bLeftTrigger) {
                // Left trigger pressed
            }
        }
    }
}

// Helper functions to check trigger states
bool IsPressingRightTrigger() {
    return Player1->GetControllerState().Gamepad.bRightTrigger > 0;
}

bool IsPressingLeftTrigger() {
    return Player1->GetControllerState().Gamepad.bLeftTrigger > 0;
}

} // namespace controller
