/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "BeastMasteryHunterStrategy.h"
#include "Playerbots.h"

BeastMasteryHunterStrategy::BeastMasteryHunterStrategy(PlayerbotAI* botAI) : GenericHunterStrategy(botAI)
{
    // No custom ActionNodeFactory needed
}

// ===== Default Actions (single-target fillers) =====
// ShatterCore 4.3.4 Beast Mastery (class reference 5.1). Focus model: Kill Command is the primary spender,
// Cobra Shot is the focus builder/filler, Arcane Shot is the focus dump. High-priority maintenance and the
// banked-focus gating live in InitTriggers below; these are the fallbacks when no trigger node fires.
std::vector<NextAction> BeastMasteryHunterStrategy::getDefaultActions()
{
    return {
        NextAction("kill command", 5.7f),   // primary focus spender (needs pet in range + ~40 focus)
        NextAction("kill shot", 5.6f),       // execute (< 20%)
        NextAction("arcane shot", 5.4f),     // instant focus dump
        NextAction("cobra shot", 5.2f),      // focus builder / filler (also extends Serpent Sting)
        NextAction("steady shot", 5.1f),     // pre-Cobra fallback for low-level bots
        NextAction("auto shot", 5.0f)
    };
}

// ===== Trigger Initialization =====
void BeastMasteryHunterStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericHunterStrategy::InitTriggers(triggers);

    // Bestial Wrath (+ Beast Within) burst cooldown.
    triggers.push_back(new TriggerNode("bestial wrath", { NextAction("bestial wrath", 19.0f) }));

    // Intimidation (pet stun cooldown).
    triggers.push_back(new TriggerNode("intimidation", { NextAction("intimidation", 40.0f) }));

    // Kill Command on cooldown -- the primary BM spender.
    triggers.push_back(new TriggerNode("kill command", { NextAction("kill command", 18.5f) }));

    // Focus Fire: pet has 5 Frenzy stacks -> convert into a ranged-haste buff.
    triggers.push_back(new TriggerNode("focus fire", { NextAction("focus fire", 18.2f) }));

    // Execute window.
    triggers.push_back(new TriggerNode("target critical health", { NextAction("kill shot", 18.0f) }));

    // Fervor (instant 50 focus) when focus-starved.
    triggers.push_back(new TriggerNode("fervor", { NextAction("fervor", 17.5f) }));

    // Maintain Serpent Sting (apply once; Cobra Shot refreshes it afterwards).
    triggers.push_back(new TriggerNode("no stings", { NextAction("serpent sting", 17.0f) }));
    triggers.push_back(new TriggerNode("serpent sting on attacker", { NextAction("serpent sting on attacker", 16.5f) }));

    // Arcane Shot focus dump -- only when we have focus banked above ~60 so Kill Command stays castable.
    triggers.push_back(new TriggerNode("focus for arcane shot", { NextAction("arcane shot", 16.0f) }));

    // AoE: Multi-Shot for pet cleave.
    triggers.push_back(new TriggerNode("light aoe", { NextAction("multi-shot", 15.0f) }));
}
