/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "TankPaladinStrategy.h"

#include "Playerbots.h"

class TankPaladinStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    TankPaladinStrategyActionNodeFactory()
    {
        // ShatterCore (4.3.4): the Prot seal is Seal of Truth (Insight as a survival fallback). The old WotLK
        // Seal of Corruption/Vengeance/Command chain is gone.
        creators["seal of truth"] = &seal_of_truth;
        creators["hand of reckoning"] = &hand_of_reckoning;
        creators["taunt spell"] = &hand_of_reckoning;
    }

private:
    static ActionNode* seal_of_truth([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "seal of truth",
            /*P*/ {},
            /*A*/ { NextAction("seal of insight") },
            /*C*/ {}
        );
    }

    static ActionNode* hand_of_reckoning([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "hand of reckoning",
            /*P*/ {},
            /*A*/ { NextAction("righteous defense") },
            /*C*/ {}
        );
    }
};

TankPaladinStrategy::TankPaladinStrategy(PlayerbotAI* botAI) : GenericPaladinStrategy(botAI)
{
    actionNodeFactories.Add(new TankPaladinStrategyActionNodeFactory());
}

std::vector<NextAction> TankPaladinStrategy::getDefaultActions()
{
    // ShatterCore 4.3.4 Protection filler priority (the "939" rotation spine): Crusader Strike is the 3s-CD
    // Holy Power generator, then single-target fillers in damage/threat order. AoE generators + the Holy Power
    // spender (Shield of the Righteous) come from the trigger nodes below.
    return {
        NextAction("crusader strike", ACTION_DEFAULT + 0.6f),  // Holy Power generator (single target)
        NextAction("judgement", ACTION_DEFAULT + 0.4f),        // filler + Judgements of the Wise mana
        NextAction("avenger's shield", ACTION_DEFAULT + 0.3f), // ranged filler (also a pull/threat tool)
        NextAction("consecration", ACTION_DEFAULT + 0.1f),     // ground threat (mana-expensive)
        NextAction("melee", ACTION_DEFAULT)
    };
}

void TankPaladinStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericPaladinStrategy::InitTriggers(triggers);

    // Taunt back a lost target first (Hand of Reckoning -> Righteous Defense fallback).
    triggers.push_back(new TriggerNode("lose aggro", { NextAction("hand of reckoning", ACTION_HIGH + 7) }));

    // Maintain Seal of Truth (Censure stacks); Seal of Insight is the action-node survival fallback.
    triggers.push_back(new TriggerNode("seal", { NextAction("seal of truth", ACTION_HIGH + 6) }));

    // Grand Crusader proc: free, instant Avenger's Shield (also resets its CD) -- always take it.
    triggers.push_back(new TriggerNode("grand crusader", { NextAction("avenger's shield", ACTION_HIGH + 5) }));

    // Emergency self-heal: spend Holy Power on Word of Glory when health drops (out-ranks Shield of the
    // Righteous so survival beats mitigation-damage; cast no-ops via CanCastSpell if no Holy Power).
    triggers.push_back(new TriggerNode("medium health", { NextAction("word of glory", ACTION_HIGH + 4) }));

    // Spend Holy Power on Shield of the Righteous at 3 charges (single-target strike + mitigation).
    triggers.push_back(new TriggerNode("holy power three", { NextAction("shield of the righteous", ACTION_HIGH + 3) }));

    // Execute filler.
    triggers.push_back(new TriggerNode("target critical health", { NextAction("hammer of wrath", ACTION_HIGH + 2) }));

    // AoE tanking: Hammer of the Righteous (HP generator, 3+ targets) + Consecration + Holy Wrath.
    triggers.push_back(new TriggerNode("medium aoe", {
        NextAction("hammer of the righteous", ACTION_HIGH + 2),
        NextAction("consecration", ACTION_HIGH + 1),
        NextAction("holy wrath", ACTION_HIGH)
    }));

    // Burst/threat cooldown.
    triggers.push_back(new TriggerNode("avenging wrath", { NextAction("avenging wrath", ACTION_HIGH + 1) }));

    triggers.push_back(new TriggerNode("not facing target", { NextAction("set facing", ACTION_NORMAL + 7) }));
    triggers.push_back(new TriggerNode("enemy out of melee", { NextAction("reach melee", ACTION_HIGH + 1) }));
}
