#pragma once

#include <atomic>

namespace globals {
    inline float fHeight = 3800.0f;
    inline float fPOSRandom = 2600.0f;

    inline float g_HailChance = 4.0f;
    inline float g_LargeHailDamageMultiplier = 4.0f;
    inline float g_SmallHailSpeed = 1000.0f;
    inline float g_SmallHailGravity = 1.6f;

    inline float g_LargeHailSpeed = 1200.0f;
    inline float g_LargeHailGravity = 1.9f;

    inline bool g_PerformanceMode = false;
    
    inline std::atomic_bool isHailing = false;

    inline std::atomic_bool isHailSFXPlaying = false;

    inline RE::BSSoundHandle indoorSound;

    inline bool currentCellIsInterior = false;
    inline bool lastCellWasInterior = false;

   inline RE::TESWorldSpace* lastWorldspace = nullptr; 

   inline bool hailActivatorNeedsReset = false;
}   