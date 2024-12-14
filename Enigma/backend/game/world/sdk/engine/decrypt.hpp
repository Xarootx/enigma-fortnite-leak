#pragma once
#ifndef DECRYPT
#define DECRYPT
#include <iostream>
#include "../../../../../includes.hpp"
#pragma warning(disable : 4996)

// Cache structure for decrypted data
namespace cache {
    struct decrypt {
        uintptr_t pname;
        uintptr_t weapondata; 
        uintptr_t displayname;
        uintptr_t weaponlength;
        std::string weaponname;
    };
}

static cache::decrypt* decryptCache = new cache::decrypt();

// Main decryption namespace for Unreal Engine data
namespace unrealenginedecryption {
    class decrypt {
    public:
        // Gets platform string from player state
        __forceinline std::string platform(uintptr_t PlayerState) {
            uintptr_t platformPtr = ioctl.read<uintptr_t>(PlayerState + 0x438);
            wchar_t platform[64];
            ioctl.read_process(platformPtr, platform, sizeof(platform));
            
            std::wstring platformWStr(platform);
            std::string platformStr(platformWStr.begin(), platformWStr.end());

            // Map platform codes to readable names
            const std::unordered_map<std::string, std::string> platformMap = {
                {"XBL", "Xbox"},
                {"PSN", "PlayStation 4"},
                {"PS5", "PlayStation 5"}, 
                {"XSX", "Xbox Series X"},
                {"SWT", "Nintendo Switch"},
                {"WIN", "PC"}
            };

            auto it = platformMap.find(platformStr);
            return it != platformMap.end() ? it->second : "";
        }

        // Decrypts and returns player username
        __forceinline std::string username(uintptr_t playerstate) {
            uintptr_t nameStruct = ioctl.read<uintptr_t>(playerstate + 0xAF0);
            if (!ioctl.is_valid(nameStruct)) {
                return "";
            }

            int nameLength = ioctl.read<int>(nameStruct + 0x10);
            if (nameLength <= 0) {
                return "Bot";
            }

            auto nameBuffer = std::make_unique<char16_t[]>(nameLength);
            uintptr_t encryptedBuffer = ioctl.read<uintptr_t>(nameStruct + 0x8);
            
            if (!ioctl.is_valid(encryptedBuffer)) {
                return "";
            }

            ioctl.read_process(encryptedBuffer, nameBuffer.get(), nameLength);

            // Decrypt name buffer
            int remainingChars = nameLength - 1;
            uint16_t* bufferPtr = (uint16_t*)nameBuffer.get();
            
            for (int i = remainingChars & 3, pos = 0; pos < remainingChars; i += 3, pos++) {
                *bufferPtr++ += i & 7;
            }

            if (!ioctl.is_valid((uint64_t)nameBuffer.get())) {
                return "";
            }

            std::u16string tempWString(nameBuffer.get());
            return std::string(tempWString.begin(), tempWString.end());
        }

        // Gets weapon name from current weapon pointer
        __forceinline std::string weapon(uint64_t CurrentWeapon) {
            uint64_t weaponData = ioctl.read<uint64_t>(CurrentWeapon + offsets->weapondata);
            uint64_t itemName = ioctl.read<uint64_t>(weaponData + 0x38);
            
            if (!itemName) {
                return "";
            }

            uint64_t nameData = ioctl.read<uint64_t>(itemName + 0x28);
            int nameLength = ioctl.read<int>(itemName + 0x30);

            if (nameLength <= 0 || nameLength >= 50) {
                return "";
            }

            auto weaponBuffer = std::make_unique<wchar_t[]>(nameLength);
            ioctl.read_process(nameData, weaponBuffer.get(), nameLength * sizeof(wchar_t));
            
            std::wstring wstrBuf(weaponBuffer.get());
            return std::string(wstrBuf.begin(), wstrBuf.end());
        }

        // Checks if weapon is reloading
        __forceinline auto reloading() -> bool {
            
        }

        // Gets current ammo count
        __forceinline auto ammocount() -> int {
    
        }
    };
}

static unrealenginedecryption::decrypt* decrypt = new unrealenginedecryption::decrypt();

#endif
