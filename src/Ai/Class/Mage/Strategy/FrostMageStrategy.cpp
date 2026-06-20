/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "FrostMageStrategy.h"
#include "Playerbots.h"
#include "Strategy.h"

FrostMageStrategy::FrostMageStrategy(PlayerbotAI* botAI) : GenericMageStrategy(botAI)
{
    // No custom ActionNodeFactory needed
}

// ===== Default Actions =====
// ShatterCore 4.3.4 Frost single-target filler priority (class reference 10.3).
// Frostbolt is the core filler that generates Fingers of Frost and Brain Freeze procs;
// Ice Lance / Fire Blast are the instant on-the-move fillers. The high-priority proc
// consumers (Deep Freeze / Ice Lance on Fingers of Frost, Frostfire Bolt on Brain Freeze)
// come from InitTriggers below. The permanent Water Elemental pet and Molten Armor are
// maintained by the pet/buff triggers.
std::vector<NextAction> FrostMageStrategy::getDefaultActions()
{
    return {
        NextAction("frostbolt", ACTION_DEFAULT + 0.4f),  // primary filler -- generates FoF / Brain Freeze procs
        NextAction("ice lance", ACTION_DEFAULT + 0.3f),  // instant filler while moving
        NextAction("fire blast", ACTION_DEFAULT + 0.2f), // instant filler if ice lance not learned
        NextAction("fireball", ACTION_DEFAULT + 0.1f),   // for frost-immune targets
        NextAction("shoot", ACTION_DEFAULT)
    };
}

// ===== Trigger Initialization ===
void FrostMageStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericMageStrategy::InitTriggers(triggers);

    // ShatterCore 4.3.4 Frost rotation (class reference 10.3). High-priority pet/proc steps live
    // here; single-target fillers come from getDefaultActions(). Icy Veins / Cold Snap burst
    // cooldowns are handled by the shared MageBoostStrategy.

    // Pet/Defensive triggers -- keep the permanent Water Elemental out.
    triggers.push_back(
        new TriggerNode(
            "no pet",
            {
                NextAction("summon water elemental", 30.0f)
            }
        )
    );
    triggers.push_back(
        new TriggerNode(
            "has pet",
            {
                NextAction("toggle pet spell", 60.0f)
            }
        )
    );
    triggers.push_back(
        new TriggerNode(
            "new pet",
            {
                NextAction("set pet stance", 60.0f)
            }
        )
    );
    triggers.push_back(
        new TriggerNode(
            "medium health",
            {
                NextAction("ice barrier", 29.0f)
            }
        )
    );
    triggers.push_back(
        new TriggerNode(
            "being attacked",
            {
                NextAction("ice barrier", 29.0f)
            }
        )
    );

    // Brain Freeze proc: free instant Frostfire Bolt -- consume immediately (highest priority proc).
    triggers.push_back(
        new TriggerNode(
            "brain freeze",
            {
                NextAction("frostfire bolt", ACTION_HIGH + 3)
            }
        )
    );

    // Fingers of Frost proc: target counts as frozen. Spend it on Deep Freeze (heavy direct damage,
    // also usable on stun-immune bosses in 4.x) first, then Ice Lance for the Frostburn-mastery bonus.
    // Do NOT spend FoF on Frostbolt -- that is the filler that generates the procs.
    triggers.push_back(
        new TriggerNode(
            "fingers of frost",
            {
                NextAction("deep freeze", ACTION_HIGH + 2),
                NextAction("ice lance", ACTION_HIGH + 1)
            }
        )
    );

    // Target frozen by Frostbite (talent root proc): same frozen-target burst -- Deep Freeze then Ice Lance.
    triggers.push_back(
        new TriggerNode(
            "frostbite on target",
            {
                NextAction("deep freeze", ACTION_HIGH + 2),
                NextAction("ice lance", ACTION_HIGH + 1)
            }
        )
    );

    // Target frozen by Frost Nova: Deep Freeze then Ice Lance into the frozen target.
    triggers.push_back(
        new TriggerNode(
            "frost nova on target",
            {
                NextAction("deep freeze", ACTION_HIGH + 2),
                NextAction("ice lance", ACTION_HIGH + 1)
            }
        )
    );

    // Frostfire Orb (Flame Orb's frost upgrade) on cooldown -- instant fire-and-forget DPS (4.3.4).
    // The "flame orb" action falls through to "frostfire orb" for frost mages via its alternatives.
    triggers.push_back(
        new TriggerNode(
            "flame orb off cd",
            {
                NextAction("flame orb", ACTION_DEFAULT + 1.0f)
            }
        )
    );
}
