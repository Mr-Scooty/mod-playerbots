
#include "AssassinationRogueStrategy.h"

#include "Playerbots.h"

class AssassinationRogueStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    AssassinationRogueStrategyActionNodeFactory()
    {
        creators["mutilate"] = &mutilate;
        creators["envenom"] = &envenom;
        creators["backstab"] = &backstab;
        creators["rupture"] = &rupture;
    }

private:
    static ActionNode* mutilate([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "mutilate",
            /*P*/ {},
            /*A*/ { NextAction("backstab") },
            /*C*/ {}
        );
    }
    static ActionNode* envenom([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "envenom",
            /*P*/ {},
            /*A*/ { NextAction("eviscerate") },
            /*C*/ {}
        );
    }
    static ActionNode* backstab([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "backstab",
            /*P*/ {},
            /*A*/ { NextAction("sinister strike") },
            /*C*/ {}
        );
    }
    static ActionNode* rupture([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "rupture",
            /*P*/ {},
            /*A*/ { NextAction("eviscerate") },
            /*C*/ {}
        );
    }
};

AssassinationRogueStrategy::AssassinationRogueStrategy(PlayerbotAI* ai) : MeleeCombatStrategy(ai)
{
    actionNodeFactories.Add(new AssassinationRogueStrategyActionNodeFactory());
}

std::vector<NextAction> AssassinationRogueStrategy::getDefaultActions()
{
    // 4.3.4 Assassination single-target filler: Mutilate is the dagger combo builder (Backstab takes over below 35%
    // via the action node fallback). Maintenance (Slice and Dice / Rupture) and Envenom come from InitTriggers.
    return {
        NextAction("mutilate", ACTION_DEFAULT + 0.1f),
        NextAction("melee", ACTION_DEFAULT)
    };
}

void AssassinationRogueStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    MeleeCombatStrategy::InitTriggers(triggers);

    // ShatterCore 4.3.4 Assassination rotation (class reference doc section 6.1). Hunger for Blood and Cold Blood
    // were removed in Cataclysm, so they are gone here. Priority order:
    //   1. Slice and Dice (apply once at low CP; Cut to the Chase keeps it refreshed via Envenom).
    //   2. Rupture at 4-5 CP -- the Venomous Wounds energy engine; keep it up always on a poisoned target.
    //   3. Envenom at 4-5 CP (refreshes Slice and Dice; main damage finisher).
    //   4. Builder: Mutilate (target > 35%) / Backstab (target < 35%, behind) from getDefaultActions/action node.
    // Cooldown: Vendetta (single-target damage amp).

    // Stealth opener.
    triggers.push_back(
        new TriggerNode(
            "high energy available",
            {
                NextAction("garrote", ACTION_HIGH + 7),
                NextAction("ambush", ACTION_HIGH + 6)
            }
        )
    );

    // Maintain Slice and Dice (applied early at low CP; kept up by Envenom's Cut to the Chase).
    triggers.push_back(
        new TriggerNode(
            "slice and dice",
            {
                NextAction("slice and dice", ACTION_HIGH + 5),
            }
        )
    );

    // Rupture -- the Venomous Wounds energy engine. Keep it rolling at 4-5 CP before spending on Envenom.
    triggers.push_back(
        new TriggerNode(
            "rupture",
            {
                NextAction("rupture", ACTION_HIGH + 4),
            }
        )
    );

    // Vendetta damage-amp cooldown on the current target.
    triggers.push_back(
        new TriggerNode(
            "vendetta",
            {
                NextAction("vendetta", ACTION_HIGH + 4),
            }
        )
    );

    // Spend combo points with Envenom at 4-5 CP (also refreshes Slice and Dice).
    triggers.push_back(
        new TriggerNode(
            "combo points 4 available",
            {
                NextAction("envenom", ACTION_HIGH + 3)
            }
        )
    );

    // Dump combo points on a target about to die.
    triggers.push_back(
        new TriggerNode(
            "target with combo points almost dead",
            {
                NextAction("envenom", ACTION_HIGH + 6)
            }
        )
    );

    triggers.push_back(
        new TriggerNode(
            "expose armor",
            {
                NextAction("expose armor", ACTION_HIGH + 2),
            }
        )
    );

    triggers.push_back(
        new TriggerNode(
            "medium threat",
            {
                NextAction("vanish", ACTION_HIGH),
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
                NextAction("kick", ACTION_INTERRUPT + 2),
            }
        )
    );

    triggers.push_back(
        new TriggerNode(
            "kick on enemy healer",
            {
                NextAction("kick on enemy healer", ACTION_INTERRUPT + 1),
            }
        )
    );

    triggers.push_back(
        new TriggerNode(
            "medium aoe",
            {
                NextAction("fan of knives", ACTION_NORMAL + 5),
            }
        )
    );

    triggers.push_back(
        new TriggerNode(
            "low tank threat",
            {
                NextAction("tricks of the trade on main tank", ACTION_HIGH + 7),
            }
        )
    );

    triggers.push_back(
        new TriggerNode(
            "enemy out of melee",
            {
                NextAction("stealth", ACTION_HIGH + 3),
                NextAction("sprint", ACTION_HIGH + 2),
                NextAction("reach melee", ACTION_HIGH + 1),
            }
        )
    );
}
