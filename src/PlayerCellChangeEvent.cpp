#include "PlayerCellChangeEvent.h"
#include "functions.h"
#include "global.h"

namespace EventSinks {

    // normally bgsActorCelLEvent runs for every NPC but since we made the player the event source,
    // this only runs for player character, ideal for changing collision base on interiors / or location type keywords
    RE::BSEventNotifyControl PlayerCellEvent::ProcessEvent(const RE::BGSActorCellEvent* event,
                                                           RE::BSTEventSource<RE::BGSActorCellEvent>*) {
        if (!event || event->flags == RE::BGSActorCellEvent::CellFlag::kLeave) {
            return RE::BSEventNotifyControl::kContinue;
        }

        static bool s_firstCellEvent = true;

        auto player = RE::PlayerCharacter::GetSingleton();

        if (!player) return RE::BSEventNotifyControl::kContinue;

        auto cell = RE::TESForm::LookupByID<RE::TESObjectCELL>(event->cellID);
        if (!cell) {
            return RE::BSEventNotifyControl::kContinue;
        }

        auto currentWorldspace = player->GetWorldspace(); 

        // set lastcellwasinterior or not and last worldspace on intial startup
        if (s_firstCellEvent) {
            s_firstCellEvent = false;
            logger::info("player is in interior on startup: {}", globals::currentCellIsInterior);
            globals::lastCellWasInterior = cell->IsInteriorCell();
            globals::lastWorldspace = currentWorldspace; 
            return RE::BSEventNotifyControl::kContinue;
        }

         globals::currentCellIsInterior = cell->IsInteriorCell();

         //player transitioned from outside to inside, stop hail, play sfx
        if (!globals::lastCellWasInterior && globals::currentCellIsInterior) {
             logger::info("player transitioned from outside to inside, stop hail, play sfx");
       
            if (globals::isHailing.load()) {
                 // play sfx
                 StartIndoorHail(player);

                 globals::isHailing.store(false); 
            }
        }

        //player transitioned to exterior, stop sfx and continue playing hail
         if (!globals::currentCellIsInterior) {

            //meaning player went inside while it was hailing, lets start hail again 
            if (globals::isHailSFXPlaying.load()){
                StopIndoorHail(); 
                // start hail again? 
            }
        }

         // player went from interior -> interior 
          if (globals::lastCellWasInterior && globals::currentCellIsInterior) {
              StopIndoorHail();
         }
           
        // if not hailing, check to see if it should
        if (!globals::isHailing.load()) {

             auto sky = RE::Sky::GetSingleton();

            if (!ShouldHail(player, sky, globals::currentCellIsInterior)) {
                return RE::BSEventNotifyControl::kContinue;
            }

            globals::isHailing.store(true);
            StartHailThread();
        }

         globals::lastCellWasInterior = globals::currentCellIsInterior;

         globals::lastWorldspace = currentWorldspace;

        return RE::BSEventNotifyControl::kContinue;
    }

    void PlayerCellEvent::RegisterEventSink() {
        if (auto* player = RE::PlayerCharacter::GetSingleton()) {
            player->AsBGSActorCellEventSource()->AddEventSink(PlayerCellEvent::GetSingleton());
            logger::info("BGSActorCellEvent sink registered");
        }
    }
}