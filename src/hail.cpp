
#include "functions.h"
#include "global.h"
#include "lookupForms.h"
#include <chrono>
#include <random>
#include <thread>

namespace logger = SKSE::log;

const auto weatherCheckInterval = std::chrono::seconds(45);

void Hail() {

    static int hailLineSpokenCount = 0; 

    logger::info("Hail starting");

    auto player = RE::PlayerCharacter::GetSingleton();
    if (!player) return;

    auto activatorBaseObject =
        HailData::activatorObject->As<RE::TESBoundObject>();  // this invisible activator casts hail spells

    if (!activatorBaseObject) {
        logger::error("Failed to cast ActivatorObject to TESBoundObject");
        return;
    }

    RE::ObjectRefHandle placedActivator = CreateActivator(player, activatorBaseObject)->GetHandle();

    int intensity = RandomFromThree(5, 7, 9);  // hail spells cast /ms

    if (globals::g_PerformanceMode == 1) {
        intensity = RandomFromThree(8, 10, 12);
    }

    logger::info("storm intensity = {}", intensity);

    auto lastPackageTime = std::make_shared<std::chrono::steady_clock::time_point>(std::chrono::steady_clock::now());

    auto var = std::make_shared<int>(3);  // start a delayed timer to make npcs react to hail.
                                          // then every 12 seconds after that, reset quest and fill aliases

    auto stormStartTime = std::chrono::steady_clock::now();

    while (globals::isHailing.load()) {

        auto ui = RE::UI::GetSingleton();

        if (ui && ui->GameIsPaused()) {
           // logger::info("game is paused, waiting");
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            globals::hailActivatorNeedsReset = true; 
            continue;
        }

        //player changed world spaces (ex. whiterun to tamriel) 
        if (globals::hailActivatorNeedsReset) {
        
                                            // copy the old handle by value
          SKSE::GetTaskInterface()->AddTask([placedActivator]() {
          
               logger::info("placed activator is null or doesent exist");

                if (!placedActivator) {
                    logger::warn("placed activator is null or doesent exist");
                    return;
                }

                if (placedActivator.get()) {
                    placedActivator.get()->Disable();
                    placedActivator.get()->SetDelete(true);
                }
            });

              // need to place new activator or continues to hail in old worldspace
              placedActivator = CreateActivator(player, activatorBaseObject)->GetHandle();

              globals::hailActivatorNeedsReset = false; 
        }

        auto now = std::chrono::steady_clock::now();

        if (now - *lastPackageTime >= std::chrono::seconds(*var)) {
            if (hailLineSpokenCount <= 3) {
                SayAOE();
                hailLineSpokenCount++; 
            }

            QuestMaintnence();

            *lastPackageTime = now;
            *var = 10;
        }

        if (now - stormStartTime >= std::chrono::seconds(150)) {
            logger::info("timeout reached");
            break;
        }

        auto spellToCast = (RandomFloat() < 33.0f) ? HailData::hailSpell : HailData::hailSpellSM;

       SKSE::GetTaskInterface()->AddTask([placedActivator, spellToCast, lastPackageTime, var]() {
            if (!placedActivator.get() || placedActivator.get()->IsDisabled() || placedActivator.get()->IsDeleted() ||
                !spellToCast) {
               // logger::warn("add task variable null");
                return;
            }

            auto playerInner = RE::PlayerCharacter::GetSingleton();
            if (!playerInner) {
                logger::info("no player inner");
                return;
            }

            MoveActivatorRandomly(playerInner, placedActivator.get().get());

            auto caster = placedActivator.get()->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);

            if (!caster) {
                logger::error("CastSpell: activator has no magic caster");
                return;
            }

            caster->CastSpellImmediate(spellToCast, false, nullptr, 0, false, 0, nullptr);
        });

        std::this_thread::sleep_for(std::chrono::milliseconds(intensity));
    }

        hailLineSpokenCount = 0;

    // delete the old activator used to spawn hail, shut off hail global
     SKSE::GetTaskInterface()->AddTask([placedActivator]() {
        if (HailData::hailGlobal) {
            HailData::hailGlobal->value = 0.0;
        }

        if (!placedActivator) {
            logger::warn("placed activator is null or doesent exist");
            return;
        }

        if (placedActivator.get()) {
            placedActivator.get()->Disable();
            placedActivator.get()->SetDelete(true);
        }
    });

     // while loop to play indor hail sfx 
    ShouldIndoorHail(player);

  
    return;
}


   




