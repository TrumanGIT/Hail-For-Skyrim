#pragma once

#include "functions.h"
#include "global.h"
#include "hail.h"
#include "lookupForms.h"
#include "HailMenu.h"
#include <spdlog/sinks/basic_file_sink.h>

namespace logger = SKSE::log;

void SetupLog() {
    auto logsFolder = SKSE::log::log_directory();
    if (!logsFolder) SKSE::stl::report_and_fail("SKSE log_directory not provided, logs disabled.");
    auto pluginName = SKSE::PluginDeclaration::GetSingleton()->GetName();
    auto logFilePath = *logsFolder / std::format("{}.log", pluginName);
    auto fileLoggerPtr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);
    auto loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));
    spdlog::set_default_logger(std::move(loggerPtr));
    spdlog::set_level(spdlog::level::trace);
    spdlog::flush_on(spdlog::level::trace);
}

void PapyrusSay(RE::TESObjectREFR* target, RE::TESTopic* toSay, RE::Character* toSpeakAs, bool playerHead) {
    if (target && toSay) {
        using func_t = void (*)(std::int64_t, std::int64_t, RE::TESObjectREFR*, RE::TESTopic*, RE::Character*, bool);
        static REL::Relocation<func_t> func{REL::VariantID(
            55689, 56220, 0x9D1470)};  // takes SE, AE, then VR offset b/c VR might no exist they are filled manually
        return func(NULL, NULL, target, toSay, toSpeakAs, playerHead);
    }
}

bool isLightning() {
    RE::Sky* singleton = RE::Sky::GetSingleton();
    if (!singleton) {
        logger::error("no singleton");
        return false;
    }

    auto weather = singleton->currentWeather;
    if (!weather) {
        logger::error("no weather");
        return false;
    }

    std::uint8_t lightningFreq = static_cast<std::uint8_t>(weather->data.thunderLightningFrequency);

    return lightningFreq < 255;
}

float RandomFloat(float min, float max) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}

int RandomFromThree(int a, int b, int c) {
    static std::mt19937 rng(std::random_device{}());
    int values[3] = {a, b, c};
    return values[std::uniform_int_distribution<int>(0, 2)(rng)];
}

RE::TESObjectREFR* CreateActivator(RE::TESObjectREFR* caster, RE::TESBoundObject* activatorBase) {
    if (!caster || !activatorBase) {
        logger::error("CreatePersistentActivator: Missing caster or base");
        return nullptr;
    }

    auto ref = caster->PlaceObjectAtMe(activatorBase, true);
    if (!ref) {
        logger::error("CreatePersistentActivator: Failed to place activator");
        return nullptr;
    }
    return ref.get(); // raw pointer
}

void MoveActivatorRandomly(RE::TESObjectREFR* player, RE::TESObjectREFR* activatorRef) {
    if (!player) {
        logger::error("MoveActivatorRandomly: Null pointer on input.");
        return;
    }

    const auto casterPos = player->GetPosition();

    float posX = casterPos.x + RandomFloat(-globals::fPOSRandom, globals::fPOSRandom);
    float posY = casterPos.y + RandomFloat(-globals::fPOSRandom, globals::fPOSRandom);
    float posZ = casterPos.z + globals::fHeight;

    if (!activatorRef) {
        logger::error("MoveActivatorRandomly: Null pointer on input.");
        return;
    }

    activatorRef->SetPosition(posX, posY, posZ);
}

//just resets the quest in turn filling the aliases and in turn gives npcs packages to run to inn or home

void QuestMaintnence() {
    if (HailData::hailQuest) {
        if (HailData::hailQuest->IsRunning()) {
            logger::info("quest was running");
            HailData::hailQuest->Stop();
         //   HailData::hailQuest->Reset();
        }
        HailData::hailQuest->Start();
    }
}

// This function iterates through high proceess list actors and makes them say a hail reaction line. 

void SayAOE() {
    logger::info("ApplyPackage started.");

    auto processLists = RE::ProcessLists::GetSingleton();

    auto playerProcess = RE::PlayerCharacter::GetSingleton();

    if (!playerProcess || !processLists || !HailData::hailTopic) {
        logger::error("One or more required HailData forms are missing or invalid or process or playerlists.");
        return;
    }

    for (auto& handle : processLists->highActorHandles) {
        auto actor = handle.get();
        if (!actor || !actor->Is3DLoaded() || actor->IsDead() || actor->IsDisabled() ||
            actor->IsDeleted() || HailData::factions.empty()) {
            continue;
        }

     bool isDQd = false;

        // Check factions
        for (auto faction : HailData::factions) {
            if (faction && actor->IsInFaction(faction)) {
                isDQd = true;
                break;
            }
        }

        if (isDQd) {
            continue;
        } 

        RE::NiPoint3 actorPos = actor->GetPosition();
        RE::NiPoint3 playerPos = playerProcess->GetPosition();

        float distance = actorPos.GetDistance(playerPos);

        if (distance >= 3000.0) {
         //   logger::info("player distance > then 3000 skipping.");
            continue;
        }

        auto actorAsObject = actor->As<RE::TESObjectREFR>();

        if (actorAsObject) {
      //      logger::info("check passed, trying to make npc say something");

          if (RandomFloat() >= 50.0) {
                PapyrusSay(actorAsObject, HailData::hailTopic, nullptr, false);
    }
  }
 }
}  

void IniParser()
{
    logger::info("In plugin load");

    std::ifstream iniFile("Data\\SKSE\\Plugins\\Hail.ini");
    if (!iniFile.is_open()) {
        logger::warn("INI file not found or failed to open, using defaults.");
    } else {
        std::string line;
        while (std::getline(iniFile, line)) {
            line.erase(0, line.find_first_not_of(" \t"));
            if (line.empty() || line[0] == ';') continue;

            try {
         
                if (line.starts_with("fHailChance=")) {
                    globals::g_HailChance = std::stof(line.substr(strlen("fHailChance=")));
                    logger::info("INI override: fHailChance = {}", globals::g_HailChance);
                } else if (line.starts_with("fHailDamageMultiplier=")) {
                    globals::g_LargeHailDamageMultiplier = std::stof(line.substr(strlen("fHailDamageMultiplier=")));
                    logger::info("INI override: fHailDamageMultiplier = {}", globals::g_LargeHailDamageMultiplier);
                } else if (line.starts_with("fLargeHailGravity=")) {
                    globals::g_LargeHailGravity = std::stof(line.substr(strlen("fLargeHailGravity=")));
                    logger::info("INI override: fLargeHailGravity = {}", globals::g_LargeHailGravity);
                } else if (line.starts_with("fLargeHailSpeed=")) {
                    globals::g_LargeHailSpeed = std::stof(line.substr(strlen("fLargeHailSpeed=")));
                    logger::info("INI override: fLargeHailSpeed = {}", globals::g_LargeHailSpeed);
                } else if (line.starts_with("fSmallHailSpeed=")) {
                    globals::g_SmallHailSpeed = std::stof(line.substr(strlen("fSmallHailSpeed=")));
                    logger::info("INI override: fSmallHailSpeed = {}", globals::g_SmallHailSpeed);
                } else if (line.starts_with("fSmallHailGravity=")) {
                    globals::g_SmallHailGravity = std::stof(line.substr(strlen("fSmallHailGravity=")));
                    logger::info("INI override: fSmallHailGravity = {}", globals::g_SmallHailGravity);
                } else if (line.starts_with("fHeightHailFallsAt=")) {
                    globals::fHeight = std::stof(line.substr(strlen("fHeightHailFallsAt=")));
                    logger::info("INI override: fHeight = {}", globals::fHeight);
                } else if (line.starts_with("bPerformanceMode=")) {
                    std::string value = line.substr(strlen("bPerformanceMode="));

                    if (value == "1" || value == "true" || value == "True") {
                        globals::g_PerformanceMode = true;
                    } else {
                        globals::g_PerformanceMode = false;
                    }
                    logger::info("INI override: bPerformanceMode raw='{}' parsed={}", value,
                                 globals::g_PerformanceMode);
                } else if (line.starts_with("fStormRadius=")) {
                    globals::fPOSRandom = std::stof(line.substr(strlen("fStormRadius=")));
                    logger::info("INI override: fStormRadius = {}", globals::fPOSRandom);
                }
            } catch (const std::exception& e) {
                logger::error("Failed to parse INI line [{}]: {}", line, e.what());
          }       
       }
    }
}

void SaveSettingsToIni(){

   logger::info("In plugin load");

    std::ifstream iniFile("Data\\SKSE\\Plugins\\Hail.ini");
    if (!iniFile.is_open()) {
        logger::warn("INI file not found or failed to open, using defaults.");
    } else {
        std::string line;
        std::string newIniFile; 
        while (std::getline(iniFile, line)) {
           
          
            line.erase(0, line.find_first_not_of(" \t"));
            try {                    
                                                 //string&  = referece ie the function sees original string, no copy is made
            auto replaceValue = [&](const std::string& key, auto value) { 
                if (line.starts_with(key)) {
               auto pos = line.find("=");
                line = line.substr(0, pos +1) + std::to_string(value); 
                  }
                }; 

            replaceValue("fHailChance=", globals::g_HailChance);
            replaceValue("fHailDamageMultiplier=", globals::g_LargeHailDamageMultiplier);
            replaceValue("fLargeHailGravity=", globals::g_LargeHailGravity);
            replaceValue("fLargeHailSpeed=", globals::g_LargeHailSpeed);
            replaceValue("fSmallHailSpeed=", globals::g_SmallHailSpeed);
            replaceValue("fSmallHailGravity=", globals::g_SmallHailGravity);
            replaceValue("fHeightHailFallsAt=", globals::fHeight);
            if (line.starts_with("bPerformanceMode=")) {
                line = "bPerformanceMode=" + std::string(globals::g_PerformanceMode ? "true" : "false");
            }
            replaceValue("fStormRadius=", globals::fPOSRandom);
                    
            } catch (const std::exception& e) {
                logger::error("Failed to parse INI line [{}]: {}", line, e.what());
            }     
             newIniFile += line + "\n"; // have to add the new line  
        }

        std::ofstream outFile("Data\\SKSE\\Plugins\\Hail.ini", std::ios::trunc); // trunc just cleares the old ini file. 
        outFile << newIniFile;
        outFile.close();  
        
    }  

} 

void StartIndoorHail(RE::PlayerCharacter* player)
{
    if (!player) return;

    auto* sound = RE::TESForm::LookupByEditorID<RE::TESSound>("HailSoundInteriorLP");
    if (!sound || !sound->descriptor) return;

    auto* audioManager = RE::BSAudioManager::GetSingleton();
    if (!audioManager) return;

    audioManager->GetSoundHandle(globals::indoorSound, sound->descriptor, 0x1A);

    globals::indoorSound.SetPosition(player->GetPosition());

    if (auto node = player->Get3D()) {
        globals::indoorSound.SetObjectToFollow(node);
    }

    globals::indoorSound.Play();

    globals::isHailSFXPlaying.store(true);
}

void StopIndoorHail() {
    if (globals::indoorSound.IsValid()) {
        globals::indoorSound.Stop();
    }

    globals::isHailSFXPlaying.store(false); 
}

void ShouldIndoorHail(RE::PlayerCharacter* player) {
    auto interiorStartTime = std::chrono::steady_clock::now();
    float entryZ = player->GetPositionZ();

    while (globals::isHailSFXPlaying.load()) {
        auto now = std::chrono::steady_clock::now();

        if (now - interiorStartTime >= std::chrono::seconds(300)) {
            logger::info("interior timeout reached");
            break;
        }

        float currentZ = player->GetPositionZ();

        if (std::abs(currentZ - entryZ) >= 2000.0f) {
            logger::info("player too far from entrance elevation");
            break;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    StopIndoorHail(); 

}

bool ShouldHail(RE::PlayerCharacter* player, RE::Sky* sky, bool isInteriorCell) {
    auto ui = RE::UI::GetSingleton();

    if (!sky || !player || isInteriorCell || !isLightning() || !sky->IsRaining()) {
        return false;
    }

    if (ui && ui->GameIsPaused()) {
        return false;
    }

    if (RandomFloat() > globals::g_HailChance) {
        return false;
    }

    return true;
}

void StartHailThread() {

    if (HailData::hailGlobal) {
        HailData::hailGlobal->value = 1.0;  // set the global
    }

     std::thread t(Hail);
     t.detach();
 }



