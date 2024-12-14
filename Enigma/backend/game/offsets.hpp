#pragma once
#ifndef OFFSETS_HPP
#define OFFSETS_HPP

// Main offsets structure containing game memory offsets
struct Offsets {
    // World and game instance
    uintptr_t gworld = 0x126cf528;
    uintptr_t gameinstance = 0x1d8;      // UGameInstance* OwningGameInstance
    uintptr_t localplayers = 0x38;       // TArray<ULocalPlayer*> LocalPlayers
    uintptr_t persistentlevel = 0x30;    // ULevel* PersistentLevel
    uintptr_t gamestate = 0x160;         // AGameStateBase* GameState
    
    // Player related
    uintptr_t playerstate = 0x2b0;       // APlayerState* PlayerState
    uintptr_t playercontroller = 0x30;   // APlayerController* PlayerController
    uintptr_t acknowledgedpawn = 0x338;  // APawn* AcknowledgedPawn
    uintptr_t pawnpriv = 0x308;         // APawn* PawnPrivate
    
    // Components and location
    uintptr_t rootcomp = 0x198;          // USceneComponent* RootComponent
    uintptr_t relativelocation = 0x120;  // Vector3 RelativeLocation
    uintptr_t velocity = 0x168;          // FVector ComponentVelocity
    uintptr_t mesh = 0x318;              // USkeletalMeshComponent* Mesh
    uintptr_t comptowrld = 0x1c0;        // Component to world transform matrix
    
    // Entity management 
    uintptr_t entitycount = 0xA8;        // TArray<AActor*> OutActors
    uintptr_t entityarray = 0x2A8;       // TArray<APlayerState*> PlayerArray
    
    // Combat related
    uintptr_t currentweapon = 0xa20;     // AFortWeapon* CurrentWeapon
    uintptr_t weapondata = 0x4f0;        // UFortWeaponItemDefinition* WeaponData
    uintptr_t targetedpawn = 0x18A0;     // Currently targeted pawn
    
    // Vehicle
    uintptr_t vehicle = 0x2960;          // AActor* CurrentVehicle
    
    // Player state
    uintptr_t teamid = 0x1211;           // Team index (AFortPlayerStateAthena)
    uintptr_t dying = 0x758;             // Player dying state
    uintptr_t dbno = 0x93a;              // Down but not out state
    
    // Bone system
    uintptr_t bonearray = 0x5B0;         // Bone array offset
    uintptr_t bonecache = 0x5F8;         // Bone cache offset
    uintptr_t bonec = 0x60;              // Bone count
    
    // UI and settings
    uintptr_t hud = 0x4c0;               // HUD offset
    uintptr_t settingsrecord = 0x390;    // Game settings record
    
    // Rendering
    uintptr_t lastsubmit = 0x2E8;        // Last frame submit time
    uintptr_t lastrender = 0x2F0;        // Last frame render time
    
    // Misc
    uintptr_t displayname = 0x98;        // Display name offset
    uintptr_t length = 0x40;             // Length offset
    uintptr_t namepoolchunk = 0x10EFDA00; // Name pool chunk offset

    // Bone IDs structure for skeletal mesh
    struct BoneIDs {
        // Base bones
        int humanbase = 0;
        int humanpelvis = 2;
        
        // Leg bones
        int humanlthigh1 = 71;
        int humanlthigh2 = 77;
        int humanlthigh3 = 72;
        int humanlcalf = 74;
        int humanlfoot = 86;
        int humanlfoot2 = 86;
        int humanltoe = 76;
        int humanrthigh1 = 78;
        int humanrthigh2 = 84;
        int humanrthigh3 = 79;
        int humanrfoot2 = 82;
        int humanrcalf = 81;
        int humanrfoot = 87;
        int humanrtoe = 83;
        
        // Spine and torso
        int humanspine1 = 7;
        int humanspine2 = 5;
        int humanspine3 = 2;
        int humanchest = 66;
        int humanchestright = 37;
        int humanchestleft = 8;
        
        // Arm bones
        int humanlcollarbone = 9;
        int humanlupperarm = 35;
        int humanlforearm1 = 36;
        int humanlforearm23 = 10;
        int humanlforearm2 = 34;
        int humanlforearm3 = 33;
        int humanlpalm = 32;
        int humanlhand = 11;
        int humanrcollarbone = 98;
        int humanrupperarm = 64;
        int humanrforearm1 = 65;
        int humanrforearm23 = 39;
        int humanrhand = 40;
        int humanrforearm2 = 63;
        int humanrforearm3 = 62;
        int humanrpalm = 58;
        
        // Head
        int humanneck = 67;
        int humanhead = 68;
    };
};

// Create singleton instances
std::unique_ptr<Offsets> offsets = std::make_unique<Offsets>();
std::unique_ptr<Offsets::BoneIDs> bones = std::make_unique<Offsets::BoneIDs>();

#endif
