#pragma once

#include <vector>
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

namespace logger = SKSE::log;

void PapyrusSay(RE::TESObjectREFR* target, RE::TESTopic* toSay, RE::Character* toSpeakAs, bool playerHead);

bool isLightning();

float RandomFloat(float min = 0.0, float max = 100.0);

int RandomFromThree(int a, int b, int c);

RE::TESObjectREFR* CreateActivator(RE::TESObjectREFR* caster, RE::TESBoundObject* activatorBase);

void MoveActivatorRandomly(RE::TESObjectREFR* player, RE::TESObjectREFR* activatorRef);

void SayAOE();

void QuestMaintnence();

void SetupLog();

void OnMessage(SKSE::MessagingInterface::Message* message);

void StartIndoorHail(RE::PlayerCharacter* player); 

void StopIndoorHail(); 

void ShouldIndoorHail(RE::PlayerCharacter* player);

bool ShouldHail(RE::PlayerCharacter* player, RE::Sky* sky, bool isInteriorCell); 

void StartHailThread();

void IniParser(); 

void SaveSettingsToIni(); 
