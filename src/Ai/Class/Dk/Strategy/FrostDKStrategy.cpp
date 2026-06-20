/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "FrostDKStrategy.h"

#include "Playerbots.h"

// 4.3.4 Frost (melee DPS). Presence: Frost Presence (+damage/RP).
// Priority: Outbreak/diseases up -> Obliterate (esp. Killing Machine) -> Frost Strike RP dump
//           (esp. Killing Machine) -> Howling Blast on Rime/Freezing Fog -> Horn of Winter filler.
// Cooldowns: Pillar of Frost (Str burst), Empower Rune Weapon, Raise Dead.
class FrostDKStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    FrostDKStrategyActionNodeFactory()
    {
        creators["outbreak"] = &outbreak;
        creators["icy touch"] = &icy_touch;
        creators["obliterate"] = &obliterate;
        creators["howling blast"] = &howling_blast;
        creators["frost strike"] = &frost_strike;
        creators["rune strike"] = &rune_strike;
        creators["pillar of frost"] = &pillar_of_frost;
    }

private:
    static ActionNode* outbreak([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "outbreak",
            /*P*/ { NextAction("frost presence") },
            /*A*/ { NextAction("icy touch") },
            /*C*/ {}
        );
    }

    static ActionNode* icy_touch([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "icy touch",
            /*P*/ { NextAction("frost presence") },
            /*A*/ {},
            /*C*/ {}
        );
    }

    static ActionNode* obliterate([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "obliterate",
            /*P*/ { NextAction("frost presence") },
            /*A*/ {},
            /*C*/ {}
        );
    }

    static ActionNode* rune_strike([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "rune strike",
            /*P*/ { NextAction("frost presence") },
            /*A*/ { NextAction("melee") },
            /*C*/ {}
        );
    }

    static ActionNode* frost_strike([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "frost strike",
            /*P*/ { NextAction("frost presence") },
            /*A*/ {},
            /*C*/ {}
        );
    }

    static ActionNode* howling_blast([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "howling blast",
            /*P*/ { NextAction("frost presence") },
            /*A*/ {},
            /*C*/ {}
        );
    }

    static ActionNode* pillar_of_frost([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "pillar of frost",
            /*P*/ { NextAction("frost presence") },
            /*A*/ {},
            /*C*/ {}
        );
    }
};

FrostDKStrategy::FrostDKStrategy(PlayerbotAI* botAI) : GenericDKStrategy(botAI)
{
    actionNodeFactories.Add(new FrostDKStrategyActionNodeFactory());
}

std::vector<NextAction> FrostDKStrategy::getDefaultActions()
{
    return {
        NextAction("obliterate", ACTION_DEFAULT + 0.7f),
        NextAction("frost strike", ACTION_DEFAULT + 0.5f),
        NextAction("howling blast", ACTION_DEFAULT + 0.3f),
        NextAction("horn of winter", ACTION_DEFAULT + 0.1f),
        NextAction("melee", ACTION_DEFAULT)
    };
}

void FrostDKStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericDKStrategy::InitTriggers(triggers);

    // Diseases: Outbreak applies both with no rune cost; highest maintenance priority.
    triggers.push_back(
        new TriggerNode(
            "outbreak",
            {
                NextAction("outbreak", ACTION_HIGH + 3)
            }
        )
    );

    // Fallback disease application when Outbreak is on cooldown.
    triggers.push_back(
        new TriggerNode(
            "icy touch",
            {
                NextAction("icy touch", ACTION_HIGH + 2)
            }
        )
    );
    triggers.push_back(
        new TriggerNode(
            "plague strike",
            {
                NextAction("plague strike", ACTION_HIGH + 1)
            }
        )
    );

    // Killing Machine: guaranteed crit on next Obliterate/Frost Strike.
    triggers.push_back(
        new TriggerNode(
            "killing machine",
            {
                NextAction("obliterate", ACTION_HIGH + 5),
                NextAction("frost strike", ACTION_HIGH + 4)
            }
        )
    );

    // Rime/Freezing Fog: free instant Howling Blast.
    triggers.push_back(
        new TriggerNode(
            "freezing fog",
            {
                NextAction("howling blast", ACTION_HIGH + 4)
            }
        )
    );

    // Pillar of Frost: Strength burst cooldown.
    triggers.push_back(
        new TriggerNode(
            "pillar of frost",
            {
                NextAction("pillar of frost", ACTION_NORMAL + 5)
            }
        )
    );

    // Blood runes: convert to Death runes via Blood Strike to feed Obliterate.
    triggers.push_back(
        new TriggerNode(
            "high blood rune",
            {
                NextAction("blood strike", ACTION_DEFAULT + 0.2f)
            }
        )
    );

    // Army of the Dead as a pull/burst cooldown.
    triggers.push_back(
        new TriggerNode(
            "army of the dead",
            {
                NextAction("army of the dead", ACTION_HIGH + 6)
            }
        )
    );
}

void FrostDKAoeStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    // AoE: Howling Blast spam (hits all + applies Frost Fever) and spread diseases.
    triggers.push_back(
        new TriggerNode(
            "medium aoe",
            {
                NextAction("howling blast", ACTION_HIGH + 4),
                NextAction("pestilence", ACTION_HIGH + 3),
                NextAction("death and decay", ACTION_HIGH + 2)
            }
        )
    );
}
