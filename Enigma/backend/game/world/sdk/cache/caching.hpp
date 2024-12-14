#include "../../../../../includes.hpp"

namespace ca {
    class caching {
    public:
        // Updates camera viewpoint in a separate thread
        __forceinline void viewpoint() {
            std::thread([&]() {
                while (true) {
                    uengine->viewpoint(cached->gworld, cached->playercontroller);
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }).detach();
        }

        // Caches game entities/actors in a separate thread
        __forceinline void actors() {
            std::thread([&]() {
                while (true) {
                    entity->cache();
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                }
            }).detach();
        }

        // Updates aimbot data in a separate thread
        __forceinline void aimbot_cache() {
            std::thread([&]() {
                while (true) {
                    entity->aimbot_thread();
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }).detach();
        }

        // Renders menu UI
        __forceinline void menu() {
            std::thread([&]() {
                render->draw();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }).detach();
        }

        // Commented out loot caching functionality
        /*
        __forceinline void loot() {
            std::thread([&]() {
                loot_cache->CacheLevels(cached->acknowledgedpawn, cached->gworld);
            }).detach();
        }
        */

        // Initializes all caching threads
        __forceinline void start() {
            actors();
            viewpoint(); 
            aimbot_cache();
        }
    };
}

// Global caching instance
static ca::caching* caching = new ca::caching;
