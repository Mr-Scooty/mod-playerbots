/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "MarksmanshipHunterStrategy.h"
#include "Playerbots.h"

MarksmanshipHunterStrategy::MarksmanshipHunterStrategy(PlayerbotAI* botAI) : GenericHunterStrategy(botAI)
{
    // No custom ActionNodeFactory needed
}

// ===== Default Actions (single-target fillers) =====
// ShatterCore 4.3.4 Marksmanship (class reference 5.2). Steady Shot is the focus builder (pairs maintain the
// Improved Steady Shot haste buff), Arcane Shot is the focus dump, Chimera Shot refreshes Serpent Sting.
std::vector<NextAction> MarksmanshipHunterStrategy::getDefaultActions()
{
    return {
        NextAction("chimera shot", 5.6f),   // refreshes Serpent Sting + strong hit (on cooldown)
        NextAction("kill shot", 5.5f),       // execute (< 20%)
        NextAction("arcane shot", 5.3f),     // instant focus dump
        NextAction("steady shot", 5.2f),     // focus builder / Improved Steady Shot maintenance
        NextAction("auto shot", 5.0f)
    };
}

// ===== Trigger Initialization =====
void MarksmanshipHunterStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericHunterStrategy::InitTriggers(triggers);

    // Silencing Shot interrupt (off-GCD).
    triggers.push_back(new TriggerNode("silencing shot", { NextAction("silencing shot", 40.0f) }));

    // Execute window.
    triggers.push_back(new TriggerNode("target critical health", { NextAction("kill shot", 18.5f) }));

    // Chimera Shot on cooldown (also refreshes Serpent Sting).
    triggers.push_back(new TriggerNode("chimera shot", { NextAction("chimera shot", 18.0f) }));

    // Fire! / Master Marksman proc -> free instant Aimed Shot.
    triggers.push_back(new TriggerNode("fire proc", { NextAction("aimed shot", 17.8f) }));

    // Careful Aim opener window (target > 80% HP) -> hardcast Aimed Shot for guaranteed crits.
    triggers.push_back(new TriggerNode("careful aim", { NextAction("aimed shot", 17.5f) }));

    // Maintain Serpent Sting (applied once; Chimera Shot keeps it rolling thereafter).
    triggers.push_back(new TriggerNode("no stings", { NextAction("serpent sting", 17.0f) }));
    triggers.push_back(new TriggerNode("serpent sting on attacker", { NextAction("serpent sting on attacker", 16.5f) }));

    // Arcane Shot focus dump -- bank focus above ~60 so Steady Shot pairs keep ISS rolling.
    triggers.push_back(new TriggerNode("focus for arcane shot", { NextAction("arcane shot", 16.0f) }));

    // AoE: Multi-Shot.
    triggers.push_back(new TriggerNode("light aoe", { NextAction("multi-shot", 15.0f) }));
}
