#include "HailMenu.h"
#include "configuration.h"
#include "global.h"
#include "functions.h"

namespace logger = SKSE::log;

namespace UI {

     void Register() {
        if (!SKSEMenuFramework::IsInstalled()) return;


        SKSEMenuFramework::SetSection("Hail Menu");

        // Register a section (calls Render when user clicks it)
        SKSEMenuFramework::AddSectionItem("Settings", UI::Render);

        // Create the main window (calls RenderWindow every frame while open)
        hailMenuWindow = SKSEMenuFramework::AddWindow(UI::RenderWindow);
    }

    void __stdcall Render() {
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4(0.6f, 0.8f, 1.0f, 1.0f));
        FontAwesome::PushSolid();

        auto iconUtf8 = FontAwesome::UnicodeToUtf8(0xf2dc);
        ImGuiMCP::Text("%s Hail Menu", iconUtf8.c_str());

        FontAwesome::Pop();
        ImGuiMCP::PopStyleColor();
        ImGuiMCP::Separator();

        if (ImGuiMCP::Button("Save")) {
            SaveSettingsToIni();
        }
        // Check hover immediately after the item
        if (ImGuiMCP::IsItemHovered()) {
            ImGuiMCP::SetTooltip("Save Settings to Hail.ini file");
        }

        ImGuiMCP::SameLine();

        ImGuiMCP::Checkbox("Performance Mode", &globals::g_PerformanceMode);

        if (ImGuiMCP::SliderFloat("Hail Chance (%)", &globals::g_HailChance, 0.0f, 100.0f, "%.1f %%")) {
            logger::info("Hail Chance changed: {}", globals::g_HailChance);
        }

        if (ImGuiMCP::SliderFloat("Hail Damage Multiplier", &globals::g_LargeHailDamageMultiplier, 0.0f, 10.0f)) {
            if (HailData::hailSpell) {
                for (auto& effect : HailData::hailSpell->effects) {
                    if (effect && effect->baseEffect == HailData::hailMagicEffect)
                        effect->effectItem.magnitude = globals::g_LargeHailDamageMultiplier;
                }
            }
        }

        if (ImGuiMCP::CollapsingHeader("Advanced Settings")) {
            ImGuiMCP::SliderFloat("Hail Start Height", &globals::fHeight, 0.0f, 10000.0f, "%.1f");
            ImGuiMCP::SliderFloat("Storm Radius", &globals::fPOSRandom, 0.0f, 10000.0f, "%.1f");

            if (ImGuiMCP::SliderFloat("Small Hail Speed", &globals::g_SmallHailSpeed, 0.0f, 2000.0f)) {
                if (HailData::smallHailP) {
                    HailData::smallHailP->data.speed = globals::g_SmallHailSpeed;
                }
            }

            if (ImGuiMCP::SliderFloat("Small Hail Gravity", &globals::g_SmallHailGravity, 0.0f, 5.0f)) {
                if (HailData::smallHailP) {
                    HailData::smallHailP->data.gravity = globals::g_SmallHailGravity;
                }
            }

            if (ImGuiMCP::SliderFloat("Large Hail Speed", &globals::g_LargeHailSpeed, 0.0f, 2000.0f)) {
                if (HailData::largeHailP) {
                    HailData::largeHailP->data.speed = globals::g_LargeHailSpeed;
                }
            }

            if (ImGuiMCP::SliderFloat("Large Hail Gravity", &globals::g_LargeHailGravity, 0.0f, 5.0f)) {
                if (HailData::largeHailP) {
                    HailData::largeHailP->data.gravity = globals::g_LargeHailGravity;
                }
            }
        }
    }

    void __stdcall RenderWindow() {
        auto viewport = ImGuiMCP::GetMainViewport();
        ImGuiMCP::SetNextWindowSize(ImGuiMCP::ImVec2{viewport->Size.x * 0.4f, viewport->Size.y * 0.4f},
                                    ImGuiMCP::ImGuiCond_Appearing);

        ImGuiMCP::Begin("Hail Menu", nullptr, ImGuiMCP::ImGuiWindowFlags_NoTitleBar);

        Render();  
      
        ImGuiMCP::End();
    }

}
