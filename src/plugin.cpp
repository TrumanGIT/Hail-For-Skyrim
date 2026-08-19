#include "functions.h"
#include "HailMenu.h"
#include "PlayerCellChangeEvent.h"

static void MessageHandler(SKSE::MessagingInterface::Message* msg) {
    switch (msg->type) {
    case SKSE::MessagingInterface::kPostLoad:
    {
        break;
    }
    case SKSE::MessagingInterface::kSaveGame: 
    {
		break;
    }
    case SKSE::MessagingInterface::kPreLoadGame:
    {
        break;
    }
    case SKSE::MessagingInterface::kPostLoadGame:
    {
        break;
    }
    case SKSE::MessagingInterface::kNewGame:
    {
        break;
    } case SKSE::MessagingInterface::kDataLoaded:
    {
        IniParser();
        HailData::Initialize();
        EventSinks::PlayerCellEvent::RegisterEventSink();
        break;
    }
    default:

        break;
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* skse) {
    SKSE::Init(skse);
    logger::info("Hail Plugin is Loaded");
    SKSE::GetMessagingInterface()->RegisterListener(MessageHandler);
    UI::Register();
    return true;
}
