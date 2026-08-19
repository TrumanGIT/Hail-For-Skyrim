#pragma once

namespace HailData {
    inline RE::SpellItem* hailSpell = nullptr;
    inline RE::SpellItem* hailSpellSM = nullptr;

    inline RE::EffectSetting* hailMagicEffect = nullptr;
    inline RE::EffectSetting* hailMagicEffectSM = nullptr;

    inline RE::BGSProjectile* smallHailP = nullptr;
    inline RE::BGSProjectile* largeHailP = nullptr;

    inline RE::TESGlobal* hailGlobal = nullptr;

    inline RE::TESForm* activatorObject = nullptr;

    inline std::vector<RE::FormID> appropriateWorldSpaces;

    inline RE::TESQuest* hailQuest = nullptr;

    inline RE::TESTopic* hailTopic = nullptr;

    inline RE::BGSKeyword* executionerKeyword = nullptr;

    inline RE::TESObjectARMO* prisonerTunic = nullptr;

    inline RE::TESFaction* currentFollowerFaction = nullptr;

    inline RE::TESFaction* dragonPriestFaction = nullptr;

    inline RE::TESFaction* creatureFaction = nullptr;
    inline RE::TESFaction* preyFaction = nullptr;
    inline RE::TESFaction* farmAnimalsFaction = nullptr;

    inline RE::BGSSoundDescriptorForm* hailInteriorSoundLP = nullptr;

    inline RE::TESFaction* winNeverFillAliasesFaction = nullptr;

    inline std::vector<RE::TESFaction*> factions;

    void Initialize();
}


