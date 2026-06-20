/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "EnhancementShamanStrategy.h"
#include "Playerbots.h"

// ===== Action Node Factory =====
class EnhancementShamanStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    EnhancementShamanStrategyActionNodeFactory()
    {
        creators["lava lash"] = &lava_lash;
    }

private:
    static ActionNode* lava_lash([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "lava lash",
            /*P*/ {},
            /*A*/ { NextAction("melee") },
            /*C*/ {}
        );
    }
};

// ===== Single Target Strategy =====
EnhancementShamanStrategy::EnhancementShamanStrategy(PlayerbotAI* botAI) : GenericShamanStrategy(botAI)
{
    actionNodeFactories.Add(new EnhancementShamanStrategyActionNodeFactory());
}

// ===== Default Actions =====
// ShatterCore 4.3.4 Enhancement single-target filler order (class reference 9.2): the priority/cooldown abilities
// (Stormstrike, Lava Lash) are the rotation spine; Earth Shock is the spare-GCD shock; auto-attack underneath
// drives Maelstrom Weapon / Windfury / Searing Flames procs.
std::vector<NextAction> EnhancementShamanStrategy::getDefaultActions()
{
    return {
       NextAction("stormstrike", 5.5f),   // top melee strike (also +nature crit debuff)
       NextAction("lava lash", 5.4f),     // consumes/amplifies Searing Flames stacks
       NextAction("earth shock", 5.3f),   // spare-GCD shock filler
       NextAction("feral spirit", 5.2f),  // wolves CD when otherwise idle
       NextAction("melee", 5.0f)
    };
}

// ===== Trigger Initialization ===
void EnhancementShamanStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericShamanStrategy::InitTriggers(triggers);

    // --- Totems ---
    triggers.push_back(new TriggerNode("call of the elements and enemy within melee",
        { NextAction("call of the elements", 60.0f) }));

    // --- Movement / utility ---
    triggers.push_back(new TriggerNode("spirit walk ready", { NextAction("spirit walk", 50.0f) }));
    triggers.push_back(new TriggerNode("enemy out of melee", { NextAction("reach melee", 40.0f) }));

    // --- Maelstrom Weapon spender ---
    // At 5 stacks the next Lightning Bolt is instant -- this is the highest single-target priority. We also fire
    // a slightly lower-priority node at 4 stacks so the bot doesn't waste a near-capped proc.
    triggers.push_back(new TriggerNode("maelstrom weapon 5", { NextAction("lightning bolt", 26.0f) }));
    triggers.push_back(new TriggerNode("maelstrom weapon 4", { NextAction("lightning bolt", 20.0f) }));

    // Stormstrike / Lava Lash are driven from getDefaultActions() (they out-rank Earth Shock there); the engine
    // skips them automatically while on cooldown, so no dedicated off-cooldown trigger node is required.

    // --- Unleash Elements weave ---
    // On cooldown, Unleash Elements (Unleashed Fury) is a free instant nature-damage strike that also buffs the
    // next imbue effect. Below the Maelstrom spenders but above plain Flame Shock upkeep. The SpellNoCooldownTrigger
    // no-ops when the spell isn't in the bot's spellbook.
    triggers.push_back(new TriggerNode("unleash elements no cd", { NextAction("unleash elements", 19.5f) }));

    // --- Maintenance ---
    triggers.push_back(new TriggerNode("flame shock", { NextAction("flame shock", 19.0f) }));
    triggers.push_back(new TriggerNode("no lightning shield", { NextAction("lightning shield", 18.5f) }));

    // --- Cooldowns ---
    // Feral Spirit wolves; Shamanistic Rage doubles as a defensive/mana CD (also a bot defensive trigger).
    triggers.push_back(new TriggerNode("medium mana", { NextAction("shamanistic rage", 23.0f) }));
    triggers.push_back(new TriggerNode("low health", { NextAction("shamanistic rage", 23.0f) }));
}
