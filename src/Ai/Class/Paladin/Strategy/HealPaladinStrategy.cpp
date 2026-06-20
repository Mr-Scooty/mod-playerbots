/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "HealPaladinStrategy.h"

#include "Playerbots.h"
#include "Strategy.h"

class HealPaladinStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
};

HealPaladinStrategy::HealPaladinStrategy(PlayerbotAI* botAI) : GenericPaladinStrategy(botAI)
{
    actionNodeFactories.Add(new HealPaladinStrategyActionNodeFactory());
}

std::vector<NextAction> HealPaladinStrategy::getDefaultActions()
{
    // ShatterCore 4.3.4 Holy: Judgement on cooldown keeps Judgements of the Pure haste up and (with Seal of
    // Insight) returns mana; it is the default "do something" filler when nobody needs healing.
    return { NextAction("judgement", ACTION_DEFAULT) };
}

void HealPaladinStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericPaladinStrategy::InitTriggers(triggers);

    // Seal of Insight: mana return + self-heal procs for a Holy paladin.
    triggers.push_back(new TriggerNode("seal", { NextAction("seal of insight", ACTION_HIGH) }));

    // Keep Beacon of Light on the main tank (transfers a share of all heals to it).
    triggers.push_back(new TriggerNode("beacon of light on main tank",
        { NextAction("beacon of light on main tank", ACTION_CRITICAL_HEAL + 7) }));

    // Emergency single-target: Holy Shock (instant + Holy Power), then the free Word of Glory at 3 HP, then a
    // big Divine Light.
    triggers.push_back(new TriggerNode("party member critical health", {
        NextAction("holy shock on party", ACTION_CRITICAL_HEAL + 6),
        NextAction("word of glory on party", ACTION_CRITICAL_HEAL + 5),
        NextAction("divine light on party", ACTION_CRITICAL_HEAL + 4)
    }));

    // Heavy single-target damage: Holy Shock (HP gen) -> Word of Glory (free spend) -> Divine Light.
    triggers.push_back(new TriggerNode("party member low health", {
        NextAction("holy shock on party", ACTION_MEDIUM_HEAL + 6),
        NextAction("word of glory on party", ACTION_MEDIUM_HEAL + 5),
        NextAction("divine light on party", ACTION_MEDIUM_HEAL + 4)
    }));

    // Moderate damage: efficient Holy Light, fast Flash of Light.
    triggers.push_back(new TriggerNode("party member medium health", {
        NextAction("holy light on party", ACTION_LIGHT_HEAL + 5),
        NextAction("flash of light on party", ACTION_LIGHT_HEAL + 4)
    }));

    // Topping off.
    triggers.push_back(new TriggerNode("party member almost full health",
        { NextAction("flash of light on party", ACTION_LIGHT_HEAL + 3) }));

    // Group-wide damage: Holy Radiance (targeted AoE HoT-burst + Holy Power), Light of Dawn (HP-spender cone),
    // plus Avenging Wrath as a throughput cooldown.
    triggers.push_back(new TriggerNode("medium group heal setting", {
        NextAction("holy radiance on party", ACTION_CRITICAL_HEAL + 3),
        NextAction("light of dawn", ACTION_CRITICAL_HEAL + 2),
        NextAction("avenging wrath", ACTION_HIGH + 4)
    }));

    // Mana management throughput cooldown.
    triggers.push_back(new TriggerNode("low mana", { NextAction("divine favor", ACTION_HIGH + 1) }));

    triggers.push_back(new TriggerNode("party member to heal out of spell range",
        { NextAction("reach party member to heal", ACTION_EMERGENCY + 3) }));
}
