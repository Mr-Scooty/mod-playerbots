/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "ArcaneMageStrategy.h"
#include "Playerbots.h"
#include "Strategy.h"

// ===== Action Node Factory =====
class ArcaneMageStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    ArcaneMageStrategyActionNodeFactory()
    {
        creators["arcane blast"] = &arcane_blast;
    }

private:
    // Arcane Barrage is the alternate for Arcane Blast (cast while moving, or
    // when Arcane Blast is unavailable - e.g. not yet learned at low levels).
    static ActionNode* arcane_blast([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("arcane blast",
                              /*P*/ {},
                              /*A*/ { NextAction("arcane barrage") },
                              /*C*/ {});
    }
};

// ===== Single Target Strategy =====
ArcaneMageStrategy::ArcaneMageStrategy(PlayerbotAI* botAI) : GenericMageStrategy(botAI)
{
    actionNodeFactories.Add(new ArcaneMageStrategyActionNodeFactory());
}

// ===== Default Actions =====
// ShatterCore 4.3.4 Arcane single-target filler priority (class reference 10.1).
// Mastery (Mana Adept) scales damage with current mana %, so the "Conserve" phase is
// the default: build Arcane Blast stacks, consume Arcane Missiles! procs, and dump the
// stacking debuff cheaply with Arcane Barrage when mana gets low. Arcane Blast (the
// stack builder + main nuke) must out-rank everything so the +damage debuff keeps stacking;
// the high-priority Burn-phase steps (proc consumption, 4-stack barrage dump) come from
// InitTriggers below.
std::vector<NextAction> ArcaneMageStrategy::getDefaultActions()
{
    return {
        NextAction("arcane blast", ACTION_DEFAULT + 0.6f),    // stack builder + primary nuke (Arcane Charges)
        NextAction("arcane missiles", ACTION_DEFAULT + 0.5f), // consume Arcane Missiles! proc when up
        NextAction("arcane barrage", ACTION_DEFAULT + 0.4f),  // instant filler / stack dump / cast while moving
        NextAction("fire blast", ACTION_DEFAULT + 0.3f),      // instant filler on the move (pre-barrage levels)
        NextAction("frostbolt", ACTION_DEFAULT + 0.2f),       // for arcane-immune targets
        NextAction("shoot", ACTION_DEFAULT + 0.1f)
    };
}

// ===== Trigger Initialization ===
void ArcaneMageStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericMageStrategy::InitTriggers(triggers);

    // ShatterCore 4.3.4 Arcane rotation (class reference 10.1). High-priority maintenance/proc
    // steps live here; the conserve-phase fillers come from getDefaultActions().

    // Consume the Arcane Missiles! proc immediately while Arcane Blast sits at high stacks.
    // Arcane Missiles is proc-only in Cata and is free, so it takes precedence over spending
    // mana on more Arcane Blasts.
    triggers.push_back(
        new TriggerNode(
            "arcane blast 4 stacks and missile barrage",
            {
                NextAction("arcane missiles", ACTION_HIGH + 2)
            }
        )
    );

    // At 4 stacks with no proc available, dump the stacks cheaply with Arcane Barrage rather
    // than paying the escalating Arcane Blast mana cost (Conserve-phase mana management).
    triggers.push_back(
        new TriggerNode(
            "arcane blast stack",
            {
                NextAction("arcane barrage", ACTION_HIGH + 1)
            }
        )
    );

    // Flame Orb on cooldown -- instant, off-GCD-style fire-and-forget DPS for all specs (4.3.4).
    // Priority above the conserve-phase fillers but below the proc/stack maintenance steps.
    triggers.push_back(
        new TriggerNode(
            "flame orb off cd",
            {
                NextAction("flame orb", ACTION_DEFAULT + 1.0f)
            }
        )
    );
}
