#pragma once
#ifndef MOUSE
#define MOUSE

#include <iostream>
#include "../../../includes.hpp"
#include "movement/mouse.hpp"

// Global timing variables for triggerbot
bool has_clicked = false;
std::chrono::steady_clock::time_point tb_begin;
std::chrono::steady_clock::time_point tb_end;
int tb_time_since;
float custom_delay = 10;

// Helper function to generate random float between a and b
__forceinline float RandomFloat(float a, float b) {
    float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    float diff = b - a;
    return a + (random * diff);
}

// Optimized absolute value using SSE
float custom_fabsf(float x) {
    __m128 x_vec = _mm_set_ss(x);
    x_vec = _mm_and_ps(x_vec, _mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFF)));
    return _mm_cvtss_f32(x_vec);
}

// Calculate predicted position based on current location, distance and velocity
Vector3 PredictPlayerPosition(const Vector3& CurrentLocation, float Distance, const Vector3& Velocity) {
    constexpr float ProjectileSpeed = 60000.f;
    constexpr float ProjectileGravity = 3.0f;
    constexpr float GravityFactor = -49000.f / 50.f;
    
    if (ProjectileSpeed <= 0) return CurrentLocation;

    Vector3 CalculatedPosition = CurrentLocation;
    float TimeToTarget = Distance / custom_fabsf(ProjectileSpeed);

    // Update position based on velocity and time
    CalculatedPosition.x += Velocity.x * TimeToTarget * 120;
    CalculatedPosition.y += Velocity.y * TimeToTarget * 120;
    CalculatedPosition.z += Velocity.z * TimeToTarget * 120;
    
    // Add bullet drop compensation
    CalculatedPosition.z += custom_fabsf(GravityFactor * ProjectileGravity) / 2.0f * (TimeToTarget * 25);

    return CalculatedPosition;
}

namespace a {
    class aim {
    public:
        // Main aimbot function to handle mouse movement
        void mouse_aim(double x, double y, int smooth, const Vector3& bone, const Vector3& velocity, const Vector3& loc, const std::string& weapon) {
            const float ScreenCenterX = ImGui::GetIO().DisplaySize.x / 2;
            const float ScreenCenterY = ImGui::GetIO().DisplaySize.y / 2;

            // Calculate predicted position if needed
            double x_pred = x;
            double y_pred = y;
            
            if (aimbot->prediction && weapon == _("Reaper Sniper Rifle")) {
                float dist = loc.Distance(bone) / 100;
                Vector3 target = PredictPlayerPosition(bone, dist, velocity);
                x_pred = uengine->w2s(target).x;
                y_pred = uengine->w2s(target).y;
            }

            // Calculate target offsets with smoothing
            float TargetX = CalculateTargetOffset(x_pred, ScreenCenterX, smooth);
            float TargetY = CalculateTargetOffset(y_pred, ScreenCenterY, smooth);

            // Add random offset for more natural movement
            float offset_x = RandomFloat(TargetX - 1, TargetX + 1);
            float offset_y = RandomFloat(TargetY - 1, TargetY + 1);

            // Apply mouse movement
            SonyDriverHelper::api::MouseMove(static_cast<int>(offset_x), static_cast<int>(offset_y / 1));
        }

        // Main aimbot logic handler
        __forceinline void aimbot_function(uintptr_t entity, uintptr_t gworld, uintptr_t playercontroller, 
                                         uintptr_t pawnprivate, const Vector3& velocity, const Vector3& loc, const std::string& weapon) {
            Vector2 bone{};
            uintptr_t currentactormesh = ioctl.read<uintptr_t>(entity + offsets->mesh);
            
            // Get bone position based on selected target
            bone = GetTargetBonePosition(currentactormesh);
            
            if (bone.x != 0 || bone.y != 0) {
                HandleAimbotLogic(entity, playercontroller, pawnprivate, currentactormesh, bone, velocity, loc, weapon);
            }
        }

    private:
        // Helper function to calculate target offset with smoothing
        float CalculateTargetOffset(double target_pos, float screen_center, int aim_speed) {
            if (target_pos == 0) return 0;

            float target_offset = 0;
            if (target_pos > screen_center) {
                target_offset = -(screen_center - target_pos);
                target_offset /= aim_speed;
                if (target_offset + screen_center > screen_center * 2) target_offset = 0;
            } else if (target_pos < screen_center) {
                target_offset = target_pos - screen_center;
                target_offset /= aim_speed;
                if (target_offset + screen_center < 0) target_offset = 0;
            }
            return target_offset;
        }

        // Get bone position based on selected target
        Vector2 GetTargetBonePosition(uintptr_t mesh) {
            switch (aimbot->bone) {
                case 0:
                case 1:
                    return uengine->w2s(uengine->bonewithrotation(mesh, bones->humanneck));
                case 2:
                    return uengine->w2s(uengine->bonewithrotation(mesh, bones->humanspine2));
                default:
                    return Vector2{};
            }
        }

        // Handle main aimbot logic including triggerbot and player freeze
        void HandleAimbotLogic(uintptr_t entity, uintptr_t playercontroller, uintptr_t pawnprivate,
                             uintptr_t mesh, const Vector2& bone, const Vector3& velocity, const Vector3& loc, const std::string& weapon) {
            Vector3 head = uengine->bonewithrotation(mesh, bones->humanneck);
            
            // Handle triggerbot logic
            if (aimbot->triggerbot && weapon.find(_("Shotgun")) != std::string::npos && GetAsyncKeyState(aimbot->triggerkey) < 0) {
                HandleTriggerbot(playercontroller);
            }

            // Apply aim
            mouse_aim(bone.x, bone.y, aimbot->smoothing, head, velocity, loc, weapon);

            // Handle player freeze
            if (aimbot->playerfreeze) {
                HandlePlayerFreeze(pawnprivate);
            }
        }

        // Handle triggerbot timing and clicks
        void HandleTriggerbot(uintptr_t playercontroller) {
            if (ioctl.read<uintptr_t>(playercontroller + offsets->targetedpawn)) {
                if (has_clicked) {
                    tb_begin = std::chrono::steady_clock::now();
                    has_clicked = false;
                }
                
                tb_end = std::chrono::steady_clock::now();
                tb_time_since = std::chrono::duration_cast<std::chrono::milliseconds>(tb_end - tb_begin).count();
                
                if (tb_time_since >= aimbot->triggerspeed * 10) {
                    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0x0002, 0);
                    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0x0004, 0);
                    has_clicked = true;
                }
            }
        }

        // Handle player freeze functionality
        void HandlePlayerFreeze(uintptr_t pawnprivate) {
            float dilation = (GetAsyncKeyState(aimbot->key) < 0) ? 0.0f : 1.0f;
            ioctl.write<float>(pawnprivate + 0x68, dilation);
        }
    };
}

static a::aim* aim = new a::aim();
#endif 
