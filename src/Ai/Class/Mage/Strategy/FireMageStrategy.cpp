/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "FireMageStrategy.h"
#include "Playerbots.h"
#include "Strategy.h"

FireMageStrategy::FireMageStrategy(PlayerbotAI* botAI) : GenericMageStrategy(botAI)
{
    // No custom ActionNodeFactory needed
}

// ===== Default Actions =====
// ShatterCore 4.3.4 Fire single-target filler priority (class reference 10.2).
// Fireball is the core filler that fishes for crits (which feed Ignite and Hot Streak);
// Scorch is the movement filler / Critical Mass applicator; Fire Blast is the instant
// on-the-move filler. The high-priority maintenance/proc steps (Living Bomb, Pyroblast!
// on Hot Streak) come from InitTriggers below.
std::vector<NextAction> FireMageStrategy::getDefaultActions()
{
    return {
        NextAction("fireball", ACTION_DEFAULT + 0.4f),   // primary filler -- fishes crits for Ignite/Hot Streak
        NextAction("scorch", ACTION_DEFAULT + 0.3f),     // instant-ish filler while moving / Critical Mass debuff
        NextAction("frostbolt", ACTION_DEFAULT + 0.2f),  // for fire-immune targets
        NextAction("fire blast", ACTION_DEFAULT + 0.1f), // instant filler on the move
        NextAction("shoot", ACTION_DEFAULT)
    };
}

// ===== Trigger Initialization =====
void FireMageStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericMageStrategy::InitTriggers(triggers);

    // ShatterCore 4.3.4 Fire rotation (class reference 10.2). High-priority maintenance/proc
    // steps live here; single-target fillers come from getDefaultActions(). Combustion and
    // Mirror Image cooldowns are handled by the shared MageBoostStrategy.

    // Pyroblast! on a Hot Streak proc: two consecutive crits grant a free instant Pyroblast.
    // This is the highest-value button in the rotation -- consume it immediately before it expires.
    triggers.push_back(
        new TriggerNode(
            "hot streak",
            {
                NextAction("pyroblast", ACTION_HIGH + 3)
            }
        )
    );

    // Maintain Living Bomb (rolling fire DoT that explodes; the spec's signature DoT, also
    // a Combustion-snapshot component). Refresh whenever it falls off the target.
    triggers.push_back(
        new TriggerNode(
            "living bomb",
            {
                NextAction("living bomb", ACTION_HIGH + 2)
            }
        )
    );

    // Keep the +crit debuff up (Critical Mass / Improved Scorch) when no other source provides it,
    // by weaving a Scorch -- only fires if the target is missing the debuff.
    triggers.push_back(
        new TriggerNode(
            "improved scorch",
            {
                NextAction("scorch", ACTION_HIGH + 1)
            }
        )
    );

    // Flame Orb on cooldown -- instant fire-and-forget DPS (4.3.4). Above the single-target
    // fillers but below the Hot Streak / Living Bomb maintenance steps.
    triggers.push_back(
        new TriggerNode(
            "flame orb off cd",
            {
                NextAction("flame orb", ACTION_DEFAULT + 1.0f)
            }
        )
    );
}

// Combat strategy to run to melee for Dragon's Breath and Blast Wave
// Disabled by default for the Fire/Frostfire spec
// To enable, type "co +firestarter"
// To disable, type "co -firestarter"
FirestarterStrategy::FirestarterStrategy(PlayerbotAI* botAI) : CombatStrategy(botAI) {}

void FirestarterStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode(
            "blast wave off cd and medium aoe",
            {
                NextAction("reach melee", 25.5f)
            }
        )
    );
}
