/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "DpsRogueStrategy.h"

#include "Playerbots.h"

class DpsRogueStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    DpsRogueStrategyActionNodeFactory()
    {
        creators["sinister strike"] = &sinister_strike;
        creators["kick"] = &kick;
        creators["kidney shot"] = &kidney_shot;
        creators["backstab"] = &backstab;
        creators["rupture"] = &rupture;
    }

private:
    static ActionNode* sinister_strike([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "sinister strike",
            /*P*/ {},
            /*A*/ {
                NextAction("melee") },
            /*C*/ {}
        );
    }
    static ActionNode* kick([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "kick",
            /*P*/ {},
            /*A*/ {
                NextAction("kidney shot") },
            /*C*/ {}
        );
    }
    static ActionNode* kidney_shot([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "kidney shot",
            /*P*/ {},
            /*A*/ {},
            /*C*/ {}
        );
    }
    static ActionNode* backstab([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "backstab",
            /*P*/ {},
            /*A*/ {
                NextAction("sinister strike") },
            /*C*/ {}
        );
    }
    static ActionNode* rupture([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "rupture",
            /*P*/ {},
            /*A*/ {
                NextAction("eviscerate") },
            /*C*/ {}
        );
    }
};

DpsRogueStrategy::DpsRogueStrategy(PlayerbotAI* botAI) : MeleeCombatStrategy(botAI)
{
    actionNodeFactories.Add(new DpsRogueStrategyActionNodeFactory());
}

std::vector<NextAction> DpsRogueStrategy::getDefaultActions()
{
    // 4.3.4 Combat single-target filler: Sinister Strike is the energy-spending combo builder. High-priority
    // maintenance (Slice and Dice, Revealing Strike) and finishers (Rupture/Eviscerate) come from InitTriggers.
    return {
        NextAction("sinister strike", ACTION_DEFAULT + 0.1f),
        NextAction("melee", ACTION_DEFAULT)
    };
}

void DpsRogueStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    MeleeCombatStrategy::InitTriggers(triggers);

    // ShatterCore 4.3.4 Combat rotation (class reference doc section 6.2). High-priority maintenance and spenders
    // are trigger nodes; the Sinister Strike filler comes from getDefaultActions(). Priority order:
    //   1. Slice and Dice (maintain always -- top melee-haste finisher).
    //   2. Revealing Strike (maintain debuff -- +finisher damage; Bandit's Guile builder).
    //   3. Rupture / Eviscerate at 5 CP (Rupture as the bleed, else Eviscerate as the damage finisher).
    //   4. Eviscerate on a nearly-dead combo-pointed target (dump CP before it dies).
    //   5. Sinister Strike builder (getDefaultActions).
    // Cooldowns: Adrenaline Rush via RogueBoostStrategy; Killing Spree as an execute-ish burst when at 0 CP.

    // Stealth opener (Garrote/Ambush) when we have the energy and are still hidden.
    triggers.push_back(
        new TriggerNode(
            "high energy available",
            {
                NextAction("garrote", ACTION_HIGH + 7),
                NextAction("ambush", ACTION_HIGH + 6)
            }
        )
    );

    // Maintain Slice and Dice -- top priority melee-haste buff for Combat.
    triggers.push_back(
        new TriggerNode(
            "slice and dice",
            {
                NextAction("slice and dice", ACTION_HIGH + 5)
            }
        )
    );

    // Maintain Revealing Strike debuff before dumping finishers (Combat +35% finisher damage).
    triggers.push_back(
        new TriggerNode(
            "revealing strike",
            {
                NextAction("revealing strike", ACTION_HIGH + 4)
            }
        )
    );

    // Spend combo points at 5: keep Rupture rolling, otherwise Eviscerate as the main damage finisher.
    triggers.push_back(
        new TriggerNode(
            "combo points 5 available",
            {
                NextAction("rupture", ACTION_HIGH + 3),
                NextAction("eviscerate", ACTION_HIGH + 2)
            }
        )
    );

    // Dump combo points on a target about to die.
    triggers.push_back(
        new TriggerNode(
            "target with combo points almost dead",
            {
                NextAction("eviscerate", ACTION_HIGH + 6)
            }
        )
    );

    // Adrenaline Rush burst (energy-regen cooldown).
    triggers.push_back(
        new TriggerNode(
            "adrenaline rush",
            {
                NextAction("adrenaline rush", ACTION_HIGH + 1)
            }
        )
    );

    // Killing Spree burst when we have no combo points to spend (avoid clipping a finisher).
    triggers.push_back(
        new TriggerNode(
            "combo points not full",
            {
                NextAction("killing spree", ACTION_NORMAL + 2)
            }
        )
    );

    triggers.push_back(
        new TriggerNode(
            "medium threat",
            {
                NextAction("vanish", ACTION_HIGH)
            }
        )
    );

    triggers.push_back(
        new TriggerNode(
            "low health",
            {
                NextAction("evasion", ACTION_HIGH + 9),
                NextAction("feint", ACTION_HIGH + 8)
            }
        )
    );

    triggers.push_back(
        new TriggerNode(
            "critical health",
            {
                NextAction("cloak of shadows", ACTION_HIGH + 7)
            }
        )
    );

    triggers.push_back(
        new TriggerNode(
            "kick",
            {
                NextAction("kick", ACTION_INTERRUPT + 2)
            }
        )
    );

    triggers.push_back(
        new TriggerNode(
            "kick on enemy healer",
            {
                NextAction("kick on enemy healer", ACTION_INTERRUPT + 1)
            }
        )
    );

    triggers.push_back(
        new TriggerNode(
            "light aoe",
            {
                NextAction("blade flurry", ACTION_HIGH + 3)
            }
        )
    );

    triggers.push_back(
        new TriggerNode(
            "blade flurry",
                {
                NextAction("blade flurry", ACTION_HIGH + 2)
            }
        )
    );

    triggers.push_back(
        new TriggerNode(
            "enemy out of melee",
            {
                NextAction("stealth", ACTION_HIGH + 3),
                NextAction("sprint", ACTION_HIGH + 2),
                NextAction("reach melee", ACTION_HIGH + 1)
            }
        )
    );

    triggers.push_back(
        new TriggerNode(
            "expose armor",
            {
                NextAction("expose armor", ACTION_HIGH + 3)
            }
        )
    );

    triggers.push_back(
        new TriggerNode(
            "low tank threat",
            {
                NextAction("tricks of the trade on main tank", ACTION_HIGH + 7)
            }
        )
    );
}

class StealthedRogueStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    StealthedRogueStrategyActionNodeFactory()
    {
        creators["ambush"] = &ambush;
        creators["cheap shot"] = &cheap_shot;
        creators["garrote"] = &garrote;
        creators["sap"] = &sap;
        creators["sinister strike"] = &sinister_strike;
    }

private:
    static ActionNode* ambush([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "ambush",
            /*P*/ {},
            /*A*/ { NextAction("garrote") },
            /*C*/ {}
        );
    }

    static ActionNode* cheap_shot([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "cheap shot",
            /*P*/ {},
            /*A*/ {},
            /*C*/ {}
        );
    }

    static ActionNode* garrote([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "garrote",
            /*P*/ {},
            /*A*/ {},
            /*C*/ {}
        );
    }

    static ActionNode* sap([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "sap",
            /*P*/ {},
            /*A*/ {},
            /*C*/ {}
        );
    }

    static ActionNode* sinister_strike([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "sinister strike",
            /*P*/ {},
            /*A*/ { NextAction("cheap shot") },
            /*C*/ {}
        );
    }
};

StealthedRogueStrategy::StealthedRogueStrategy(PlayerbotAI* botAI) : Strategy(botAI)
{
    actionNodeFactories.Add(new StealthedRogueStrategyActionNodeFactory());
}

std::vector<NextAction> StealthedRogueStrategy::getDefaultActions()
{
    return {
        NextAction("ambush", ACTION_NORMAL + 4),
        NextAction("backstab", ACTION_NORMAL + 3),
        NextAction("cheap shot", ACTION_NORMAL + 2),
        NextAction("sinister strike", ACTION_NORMAL + 1),
        NextAction("melee", ACTION_NORMAL)
    };
}

void StealthedRogueStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode(
            "combo points 5 available",
            {
                NextAction("eviscerate", ACTION_HIGH)
            }
        )
    );
    triggers.push_back(
        new TriggerNode(
            "kick",
            {
                NextAction("cheap shot", ACTION_INTERRUPT)
            }
        )
    );
    triggers.push_back(
        new TriggerNode(
            "kick on enemy healer",
            {
                NextAction("cheap shot", ACTION_INTERRUPT)
            }
        )
    );
    triggers.push_back(
        new TriggerNode(
            "behind target",
            {
                NextAction("ambush", ACTION_HIGH)
            }
        )
    );
    triggers.push_back(
        new TriggerNode(
            "not behind target",
            {
                NextAction("cheap shot", ACTION_HIGH)
            }
        )
    );
    triggers.push_back(
        new TriggerNode(
            "enemy flagcarrier near",
            {
                NextAction("sprint", ACTION_EMERGENCY + 1)
            }
        )
    );
    triggers.push_back(
        new TriggerNode(
            "unstealth",
            {
                NextAction("unstealth", ACTION_NORMAL)
            }
        )
    );

    triggers.push_back(
        new TriggerNode(
            "no stealth",
            {
                NextAction("check stealth", ACTION_EMERGENCY)
            }
        )
    );
    triggers.push_back(
        new TriggerNode(
            "sprint",
            {
                NextAction("sprint", ACTION_INTERRUPT)
            }
        )
    );
}

void StealthStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode(
            "stealth",
            {
                NextAction("stealth", ACTION_INTERRUPT)
            }
        )
    );
}

void RogueAoeStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode(
            "light aoe",
            {
                NextAction("blade flurry", ACTION_HIGH)
            }
        )
    );
    triggers.push_back(
        new TriggerNode(
            "medium aoe",
            {
                NextAction("fan of knives", ACTION_NORMAL + 5)
            }
        )
    );
}

void RogueBoostStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode(
            "adrenaline rush",
            {
                NextAction("adrenaline rush", ACTION_HIGH + 2)
            }
        )
    );
}

void RogueCcStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode(
            "sap",
            {
                NextAction("stealth", ACTION_INTERRUPT),
                NextAction("sap", ACTION_INTERRUPT)
            }
        )
    );
}
