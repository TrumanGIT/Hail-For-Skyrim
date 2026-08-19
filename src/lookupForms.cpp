#include "lookupForms.h"
#include "spdlog/spdlog.h"
#include "functions.h"
#include "global.h"

namespace HailData {

    void LoadForms() {
        auto dataHandler = RE::TESDataHandler::GetSingleton();

        if (!dataHandler) {
            logger::error("no data handler cant lookup forms"); 
            return; 
        }

        hailSpell = dataHandler->LookupForm<RE::SpellItem>(0x80A, "Hail.esp");
        hailSpellSM = dataHandler->LookupForm<RE::SpellItem>(0xD74, "Hail.esp");

        hailMagicEffect = dataHandler->LookupForm<RE::EffectSetting>(0x804, "Hail.esp");
        hailMagicEffectSM = dataHandler->LookupForm<RE::EffectSetting>(0xD76, "Hail.esp");

        smallHailP = dataHandler->LookupForm<RE::BGSProjectile>(0x805, "Hail.esp");
        largeHailP = dataHandler->LookupForm<RE::BGSProjectile>(0x80B, "Hail.esp");

        hailGlobal = dataHandler->LookupForm<RE::TESGlobal>(0x8D4, "Hail.esp");
        hailQuest = dataHandler->LookupForm<RE::TESQuest>(0xD7D, "Hail.esp");

        hailTopic = RE::TESForm::LookupByID<RE::TESTopic>(0x00011111);
        activatorObject = RE::TESForm::LookupByEditorID("HailActivator");

        currentFollowerFaction = dataHandler->LookupForm<RE::TESFaction>(0x0005C84E, "Skyrim.esm");
        winNeverFillAliasesFaction = dataHandler->LookupForm<RE::TESFaction>(0x000ABDAE, "Skyrim.esm");
        dragonPriestFaction = dataHandler->LookupForm<RE::TESFaction>(0x00106643, "Skyrim.esm");

        creatureFaction = dataHandler->LookupForm<RE::TESFaction>(0x00000013, "Skyrim.esm");
        preyFaction = dataHandler->LookupForm<RE::TESFaction>(0x0002E894, "Skyrim.esm");
        farmAnimalsFaction = dataHandler->LookupForm<RE::TESFaction>(0x0004E849, "Skyrim.esm");

        hailInteriorSoundLP = dataHandler->LookupForm<RE::BGSSoundDescriptorForm>(0x42D, "Hail.esp");

        if (!hailInteriorSoundLP)
        {

            logger::error("no sound form loaded");
        }


        factions = {dragonPriestFaction, creatureFaction, preyFaction, farmAnimalsFaction, currentFollowerFaction};
    }

    bool ValidateForms() {
        return hailSpell && hailSpellSM && hailMagicEffect && hailMagicEffectSM && smallHailP && largeHailP &&
               hailGlobal && activatorObject && dragonPriestFaction && hailTopic && currentFollowerFaction && hailInteriorSoundLP;
    }

    void ApplySpellMagnitude(RE::SpellItem* spell, RE::EffectSetting* effectSetting) {
        if (!spell || !effectSetting) {
            logger::warn("Missing spell or effect, cannot modify magnitude.");
            return;
        }

        for (auto& effect : spell->effects) {
            if (effect && effect->baseEffect == effectSetting) {
                effect->effectItem.magnitude = globals::g_LargeHailDamageMultiplier;
            }
        }
    }

    void ApplyInitialBalance() {
        if (smallHailP) {
            smallHailP->data.gravity = globals::g_SmallHailGravity;
            smallHailP->data.speed = globals::g_SmallHailSpeed;
        } else {
            logger::warn("smallHailP is nullptr");
        }

        if (largeHailP) {
            largeHailP->data.gravity = globals::g_LargeHailGravity;
            largeHailP->data.speed = globals::g_LargeHailSpeed;
        } else {
            logger::warn("largeHailP is nullptr");
        }

        ApplySpellMagnitude(hailSpell, hailMagicEffect);
        ApplySpellMagnitude(hailSpellSM, hailMagicEffectSM);
    }

void Initialize() {
        LoadForms();

        if (!ValidateForms()) {
            spdlog::error("Failed to load essential Hail and related forms!");
            return;
        }

        spdlog::info("All essential forms loaded successfully");

        ApplyInitialBalance();
    }

}

