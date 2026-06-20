/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "FuryWarriorStrategy.h"

class FuryWarriorStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    FuryWarriorStrategyActionNodeFactory()
    {
        creators["charge"] = &charge;
        creators["intercept"] = &intercept;
        creators["piercing howl"] = &piercing_howl;
        creators["pummel"] = &pummel;
        creators["enraged regeneration"] = &enraged_regeneration;
    }

private:
    static ActionNode* charge(PlayerbotAI* /*botAI*/)
    {
        return new ActionNode(
            "charge",
            /*P*/ {},
            /*A*/ { NextAction("intercept" )},
            /*C*/ {}
        );
    }

    static ActionNode* intercept(PlayerbotAI* /*botAI*/)
    {
        return new ActionNode(
            "intercept",
            /*P*/ {},
            /*A*/ { NextAction("reach melee" )},
            /*C*/ {}
        );
    }

    static ActionNode* piercing_howl(PlayerbotAI* /*botAI*/)
    {
        return new ActionNode(
            "piercing howl",
            /*P*/ {},
            /*A*/ { NextAction("hamstring" )},
            /*C*/ {}
        );
    }

    static ActionNode* pummel(PlayerbotAI* /*botAI*/)
    {
        return new ActionNode(
            "pummel",
            /*P*/ {},
            /*A*/ { NextAction("intercept" )},
            /*C*/ {}
        );
    }

    static ActionNode* enraged_regeneration(PlayerbotAI* /*botAI*/)
    {
        return new ActionNode(
            "enraged regeneration",
            /*P*/ {},
            /*A*/ {},
            /*C*/ {}
        );
    }
};

FuryWarriorStrategy::FuryWarriorStrategy(PlayerbotAI* botAI) : GenericWarriorStrategy(botAI)
{
    actionNodeFactories.Add(new FuryWarriorStrategyActionNodeFactory());
}

std::vector<NextAction> FuryWarriorStrategy::getDefaultActions()
{
    // ShatterCore 4.3.4 Fury single-target filler priority (used when no higher-priority trigger fires):
    // Bloodthirst is the on-CD generator/Enrage source; Raging Blow when Enraged; Slam as low filler.
    return {
        NextAction("bloodthirst", ACTION_DEFAULT + 0.4f),  // on-CD strike, Enrage source
        NextAction("raging blow", ACTION_DEFAULT + 0.3f),  // usable only while Enraged (engine CanCast gates it)
        NextAction("slam", ACTION_DEFAULT + 0.1f),
        NextAction("melee", ACTION_DEFAULT)
    };
}

void FuryWarriorStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericWarriorStrategy::InitTriggers(triggers);

    triggers.push_back(
        new TriggerNode(
            "enemy out of melee",
            {
                NextAction("charge", ACTION_MOVE + 9)
            }
        )
    );
    triggers.push_back(
        new TriggerNode(
            "berserker stance", {
                NextAction("berserker stance", ACTION_HIGH + 9)
            }
        )
    );
    triggers.push_back(
        new TriggerNode(
            "battle shout",
            {
                NextAction("battle shout", ACTION_HIGH + 8)
            }
        )
    );
    triggers.push_back(
        new TriggerNode(
            "pummel on enemy healer",
            {
                NextAction("pummel on enemy healer", ACTION_INTERRUPT)
            }
        )
    );
    triggers.push_back(
        new TriggerNode(
            "pummel",
            {
                NextAction("pummel", ACTION_INTERRUPT)
            }
        )
    );
    triggers.push_back(
        new TriggerNode(
            "victory rush",
            {
                NextAction("victory rush", ACTION_INTERRUPT)
            }
        )
    );

    // ShatterCore 4.3.4 Fury single-target priority (class reference doc 3.2):
    // 1) Colossus Smash on CD 2) Execute (<20%) 3) Bloodthirst on CD 4) Raging Blow while Enraged
    // 5) Slam on Bloodsurge proc 6) Heroic Strike rage dump.

    // Colossus Smash on cooldown -- armor-ignore window.
    triggers.push_back(
        new TriggerNode(
            "colossus smash",
            {
                NextAction("colossus smash", ACTION_HIGH + 7)
            }
        )
    );

    // Execute when target below 20%.
    triggers.push_back(
        new TriggerNode(
            "target critical health",
            {
                NextAction("execute", ACTION_HIGH + 7)
            }
        )
    );

    // Bloodthirst on cooldown -- primary strike + Enrage source.
    triggers.push_back(
        new TriggerNode(
            "bloodthirst",
            {
                NextAction("bloodthirst", ACTION_HIGH + 6)
            }
        )
    );

    // Raging Blow -- only usable while Enraged. The Enrage trigger gates it; CanCast also enforces the requirement.
    triggers.push_back(
        new TriggerNode(
            "enrage",
            {
                NextAction("raging blow", ACTION_HIGH + 5)
            }
        )
    );

    // Slam becomes instant + free on a Bloodsurge proc. In 4.3.4 the Bloodsurge talent is a passive that
    // procs the "Slam!" buff (there is no player aura literally named "bloodsurge"), so gate on the "instant
    // slam" trigger (HasAura "slam!") -- the real proc aura -- instead of the never-firing "bloodsurge" name.
    triggers.push_back(
        new TriggerNode(
            "instant slam",
            {
                NextAction("slam", ACTION_HIGH + 4)
            }
        )
    );

    // AoE: Whirlwind + Cleave.
    triggers.push_back(
        new TriggerNode(
            "light aoe",
            {
                NextAction("whirlwind", ACTION_HIGH + 3),
                NextAction("cleave", ACTION_HIGH + 2)
            }
        )
    );

    triggers.push_back(
        new TriggerNode(
            "bloodrage",
            {
                NextAction("bloodrage", ACTION_HIGH + 1)
            }
        )
    );

    // Heroic Strike rage dump at high rage; Inner Rage windows lower the effective threshold.
    triggers.push_back(
        new TriggerNode(
            "high rage available",
            {
                NextAction("heroic strike", ACTION_HIGH)
            }
        )
    );

    // Berserker Rage to force Enrage (keeps Raging Blow usable) + fear break.
    triggers.push_back(
        new TriggerNode(
            "berserker rage",
            {
                NextAction("berserker rage", ACTION_HIGH)
            }
        )
    );

    triggers.push_back(
        new TriggerNode(
            "death wish",
            {
                NextAction("death wish", ACTION_HIGH)
            }
        )
    );
    triggers.push_back(
        new TriggerNode(
            "recklessness",
            {
                NextAction("recklessness", ACTION_HIGH)
            }
        )
    );
    triggers.push_back(
        new TriggerNode(
            "critical health",
            {
                NextAction("enraged regeneration", ACTION_EMERGENCY)
            }
        )
    );
}
