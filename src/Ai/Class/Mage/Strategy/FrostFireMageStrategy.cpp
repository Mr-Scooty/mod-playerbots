/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "FrostFireMageStrategy.h"
#include "Playerbots.h"
#include "Strategy.h"

FrostFireMageStrategy::FrostFireMageStrategy(PlayerbotAI* botAI) : GenericMageStrategy(botAI)
{
    // No custom ActionNodeFactory needed
}

// ===== Default Actions =====
// ShatterCore 4.3.4 Frostfire (deep-Fire build that uses Frostfire Bolt as its filler instead of
// Fireball -- class reference 10.2). Same crit-fishing Hot Streak/Living Bomb logic as Fire; only
// the filler nuke differs. High-priority maintenance/proc steps come from InitTriggers below.
std::vector<NextAction> FrostFireMageStrategy::getDefaultActions()
{
    return {
        NextAction("frostfire bolt", ACTION_DEFAULT + 0.3f), // primary filler (Frostfire build)
        NextAction("scorch", ACTION_DEFAULT + 0.2f),         // movement filler / Critical Mass debuff
        NextAction("fire blast", ACTION_DEFAULT + 0.1f),     // instant filler on the move
        NextAction("shoot", ACTION_DEFAULT)
    };
}

// ===== Trigger Initialization =====
void FrostFireMageStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericMageStrategy::InitTriggers(triggers);

    // Pyroblast! on a Hot Streak proc -- consume the free instant Pyroblast immediately.
    triggers.push_back(
        new TriggerNode(
            "hot streak",
            {
                NextAction("pyroblast", ACTION_HIGH + 3)
            }
        )
    );

    // Maintain Living Bomb (rolling fire DoT).
    triggers.push_back(
        new TriggerNode(
            "living bomb",
            {
                NextAction("living bomb", ACTION_HIGH + 2)
            }
        )
    );

    // Keep the +crit debuff up via Scorch when nothing else provides it.
    triggers.push_back(
        new TriggerNode(
            "improved scorch",
            {
                NextAction("scorch", ACTION_HIGH + 1)
            }
        )
    );

    // Flame Orb / Frostfire Orb on cooldown -- instant fire-and-forget DPS (4.3.4).
    triggers.push_back(
        new TriggerNode(
            "flame orb off cd",
            {
                NextAction("flame orb", ACTION_DEFAULT + 1.0f)
            }
        )
    );
}
