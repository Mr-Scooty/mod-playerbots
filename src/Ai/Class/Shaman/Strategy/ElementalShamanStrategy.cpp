/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "ElementalShamanStrategy.h"
#include "Playerbots.h"

ElementalShamanStrategy::ElementalShamanStrategy(PlayerbotAI* botAI) : GenericShamanStrategy(botAI)
{
    // No custom ActionNodeFactory needed
}

// ===== Default Actions =====
// ShatterCore 4.3.4 Elemental single-target filler order (class reference 9.1): when no higher-priority trigger
// node fires, weave Lava Burst (it is the priority nuke -- always crits on a Flame-Shocked target and resets on
// Lava Surge) then fall back to Lightning Bolt as the spammable filler.
std::vector<NextAction> ElementalShamanStrategy::getDefaultActions()
{
    return {
        NextAction("lava burst", 5.2f),     // priority nuke -- isUseful() requires the target be Flame Shocked
        NextAction("lightning bolt", 5.0f)  // spammable filler
    };
}

// ===== Trigger Initialization ===
void ElementalShamanStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericShamanStrategy::InitTriggers(triggers);

    // --- Shield / mana upkeep ---
    // Lightning Shield is the Elemental shield (Fulmination feeds off its charges); Water Shield only as a
    // fallback when out of combat / low mana handling.
    triggers.push_back(new TriggerNode("no lightning shield", { NextAction("lightning shield", 19.5f) }));

    // --- Totems ---
    triggers.push_back(new TriggerNode("call of the elements", { NextAction("call of the elements", 60.0f) }));
    triggers.push_back(new TriggerNode("low health", { NextAction("stoneclaw totem", 40.0f) }));

    // --- Cooldowns ---
    // Elemental Mastery: instant-cast + haste burst (the trigger gates it to a Lava Burst window).
    triggers.push_back(new TriggerNode("elemental mastery", { NextAction("elemental mastery", 29.0f) }));

    // --- Single-target rotation (high priority maintenance/spenders) ---
    // 1. Keep Flame Shock up (required for Lava Burst crits + Fire Nova spread). Highest combat priority.
    triggers.push_back(new TriggerNode("flame shock", { NextAction("flame shock", 25.0f) }));

    // 2. Lava Burst whenever off cooldown / Lava Surge proc (huge crit on the Flame-Shocked target).
    triggers.push_back(new TriggerNode("lava burst no cd", { NextAction("lava burst", 24.0f) }));

    // 3. Fulmination: dump Lightning Shield surplus with Earth Shock at 9 charges (don't clip Flame Shock --
    //    so this sits just below Flame Shock/Lava Burst).
    triggers.push_back(new TriggerNode("fulmination", { NextAction("earth shock", 23.0f) }));

    // 4. Unleash Elements on cooldown (Unleashed Flame buffs the next Lava Burst/Fire spell). Off-CD weave kept
    //    below the core maintenance/spenders; the SpellNoCooldownTrigger no-ops if the spell isn't learned.
    triggers.push_back(new TriggerNode("unleash elements no cd", { NextAction("unleash elements", 22.0f) }));

    // --- Mana / utility ---
    triggers.push_back(new TriggerNode("high mana", { NextAction("thunderstorm", 19.0f) }));

    // Knockback when something is in our face.
    triggers.push_back(new TriggerNode("enemy is close", { NextAction("thunderstorm", 19.0f) }));
}
