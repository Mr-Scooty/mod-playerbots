/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "DpsPaladinStrategy.h"

#include "Playerbots.h"
#include "Strategy.h"

class DpsPaladinStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    DpsPaladinStrategyActionNodeFactory()
    {
        creators["sanctity aura"] = &sanctity_aura;
        creators["retribution aura"] = &retribution_aura;
        creators["blessing of might"] = &blessing_of_might;
        creators["crusader strike"] = &crusader_strike;
        creators["repentance"] = &repentance;
        creators["repentance on enemy healer"] = &repentance_on_enemy_healer;
        creators["repentance on snare target"] = &repentance_on_snare_target;
        creators["repentance of shield"] = &repentance_or_shield;
    }

private:
    static ActionNode* blessing_of_might([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "blessing of might",
            /*P*/ {},
            /*A*/ { NextAction("blessing of kings") },
            /*C*/ {}
        );
    }

    static ActionNode* crusader_strike([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "crusader strike",
            /*P*/ {},
            /*A*/ {},
            /*C*/ {}
        );
    }

    static ActionNode* repentance([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "repentance",
            /*P*/ {},
            /*A*/ { NextAction("hammer of justice") },
            /*C*/ {}
        );
    }

    static ActionNode* repentance_on_enemy_healer([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "repentance on enemy healer",
            /*P*/ {},
            /*A*/ { NextAction("hammer of justice on enemy healer") },
            /*C*/ {}
        );
    }

    static ActionNode* repentance_on_snare_target([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "repentance on snare target",
            /*P*/ {},
            /*A*/ { NextAction("hammer of justice on snare target") },
            /*C*/ {}
        );
    }

    static ActionNode* sanctity_aura([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "sanctity aura",
            /*P*/ {},
            /*A*/ { NextAction("retribution aura") },
            /*C*/ {}
        );
    }

    static ActionNode* retribution_aura([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "retribution aura",
            /*P*/ {},
            /*A*/ { NextAction("devotion aura") },
            /*C*/ {}
        );
    }

    static ActionNode* repentance_or_shield([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "repentance",
            /*P*/ {},
            /*A*/ { NextAction("divine shield") },
            /*C*/ {}
        );
    }
};

DpsPaladinStrategy::DpsPaladinStrategy(PlayerbotAI* botAI) : GenericPaladinStrategy(botAI)
{
    actionNodeFactories.Add(new DpsPaladinStrategyActionNodeFactory());
}

std::vector<NextAction> DpsPaladinStrategy::getDefaultActions()
{
    // ShatterCore 4.3.4 Retribution filler priority (used when no higher-priority trigger node fires):
    // Holy Power generator (Crusader Strike -- the rotation spine) > execute > single Judgement > Exorcism filler
    // > Consecration > melee. CS must out-rank the others so Holy Power keeps flowing to Templar's Verdict/Inquisition.
    return {
        NextAction("crusader strike", ACTION_DEFAULT + 0.6f),    // Holy Power generator -- the rotation spine
        NextAction("hammer of wrath", ACTION_DEFAULT + 0.5f),    // execute filler (target < 20%)
        NextAction("judgement", ACTION_DEFAULT + 0.4f),          // single 4.3.4 Judgement (no more Judgement of Wisdom)
        NextAction("exorcism", ACTION_DEFAULT + 0.3f),           // filler when no Art of War proc
        NextAction("consecration", ACTION_DEFAULT + 0.1f),
        NextAction("melee", ACTION_DEFAULT)
    };
}

void DpsPaladinStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericPaladinStrategy::InitTriggers(triggers);

    // ShatterCore 4.3.4 Retribution rotation (class reference doc section 4.3). The trigger nodes below are the
    // high-priority maintenance/spender steps; the single-target fillers (Crusader Strike to build Holy Power,
    // Hammer of Wrath execute, Judgement, etc.) come from getDefaultActions().

    // Maintain Seal of Truth (Cataclysm seal; replaces WotLK Seal of Corruption + Seal of Wisdom swapping).
    triggers.push_back(new TriggerNode("seal", { NextAction("seal of truth", ACTION_HIGH + 5) }));

    // Maintain Inquisition (Holy Power spender, +30% Holy damage) -- recast when it drops; takes precedence
    // over Templar's Verdict so the damage buff stays up.
    triggers.push_back(new TriggerNode("inquisition", { NextAction("inquisition", ACTION_HIGH + 4) }));

    // Spend Holy Power at 3 charges with Templar's Verdict (the single-target finisher).
    triggers.push_back(new TriggerNode("holy power three", { NextAction("templar's verdict", ACTION_HIGH + 3) }));

    // Avenging Wrath burst cooldown.
    triggers.push_back(new TriggerNode("avenging wrath", { NextAction("avenging wrath", ACTION_HIGH + 2) }));

    // Free instant Exorcism on The Art of War proc.
    triggers.push_back(new TriggerNode("art of war", { NextAction("exorcism", ACTION_HIGH + 2) }));

    // AoE: Divine Storm (Holy-Power-free in 4.3) + Consecration.
    triggers.push_back(new TriggerNode("medium aoe", {
        NextAction("divine storm", ACTION_HIGH + 1),
        NextAction("consecration", ACTION_HIGH)
    }));

    // Close to melee range when the target steps out.
    triggers.push_back(new TriggerNode("enemy out of melee", { NextAction("reach melee", ACTION_HIGH + 1) }));
}
