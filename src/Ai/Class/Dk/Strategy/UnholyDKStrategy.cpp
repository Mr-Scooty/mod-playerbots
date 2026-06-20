/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "UnholyDKStrategy.h"

#include "Playerbots.h"

// 4.3.4 Unholy (melee DPS, 2H, permanent ghoul). Presence: Unholy Presence (haste + rune regen).
// Priority: diseases via Outbreak (extended with Festering Strike, rarely recast) ->
//           Dark Transformation at 5 Shadow Infusion -> Scourge Strike (Unholy/Death runes) ->
//           Festering Strike (Blood+Frost) to extend diseases -> Death Coil (Sudden Doom / RP dump,
//           builds Shadow Infusion) -> Horn of Winter filler.
// Cooldowns: Summon Gargoyle (31-pt burst), Army of the Dead pre-pull.
class UnholyDKStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    UnholyDKStrategyActionNodeFactory()
    {
        creators["outbreak"] = &outbreak;
        creators["festering strike"] = &festering_strike;
        creators["scourge strike"] = &scourge_strike;
        creators["death coil"] = &death_coil;
        creators["dark transformation"] = &dark_transformation;
        creators["icy touch"] = &icy_touch;
    }

private:
    static ActionNode* outbreak([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "outbreak",
            /*P*/ { NextAction("unholy presence") },
            /*A*/ { NextAction("icy touch") },
            /*C*/ {}
        );
    }
    static ActionNode* festering_strike([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "festering strike",
            /*P*/ { NextAction("unholy presence") },
            /*A*/ { NextAction("blood strike") },
            /*C*/ {}
        );
    }
    static ActionNode* scourge_strike([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "scourge strike",
            /*P*/ { NextAction("unholy presence") },
            /*A*/ {},
            /*C*/ {}
        );
    }
    static ActionNode* death_coil([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "death coil",
            /*P*/ { NextAction("unholy presence") },
            /*A*/ {},
            /*C*/ {}
        );
    }
    static ActionNode* dark_transformation([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "dark transformation",
            /*P*/ { NextAction("unholy presence") },
            /*A*/ {},
            /*C*/ {}
        );
    }
    static ActionNode* icy_touch([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "icy touch",
            /*P*/ { NextAction("unholy presence") },
            /*A*/ {},
            /*C*/ {}
        );
    }
};

UnholyDKStrategy::UnholyDKStrategy(PlayerbotAI* botAI) : GenericDKStrategy(botAI)
{
    actionNodeFactories.Add(new UnholyDKStrategyActionNodeFactory());
}

std::vector<NextAction> UnholyDKStrategy::getDefaultActions()
{
    return {
        NextAction("scourge strike", ACTION_DEFAULT + 0.6f),
        NextAction("festering strike", ACTION_DEFAULT + 0.5f),
        NextAction("summon gargoyle", ACTION_DEFAULT + 0.4f),
        NextAction("death coil", ACTION_DEFAULT + 0.3f),
        NextAction("horn of winter", ACTION_DEFAULT + 0.1f),
        NextAction("melee", ACTION_DEFAULT)
    };
}

void UnholyDKStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericDKStrategy::InitTriggers(triggers);

    // Permanent ghoul (Master of Ghouls): keep it up and managed.
    triggers.push_back(
        new TriggerNode("no pet", { NextAction("raise dead", ACTION_NORMAL + 5) }));
    triggers.push_back(
        new TriggerNode("has pet", { NextAction("toggle pet spell", 60.0f) }));
    triggers.push_back(
        new TriggerNode("new pet", { NextAction("set pet stance", 60.0f) }));

    // Diseases via Outbreak (no rune cost) -- applied once, then extended by Festering Strike.
    triggers.push_back(
        new TriggerNode(
            "outbreak",
            {
                NextAction("outbreak", ACTION_HIGH + 4)
            }
        )
    );
    // Fallback disease application if Outbreak on cooldown.
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

    // Dark Transformation: empower ghoul once 5 Shadow Infusion stacks reached.
    triggers.push_back(
        new TriggerNode(
            "dark transformation",
            {
                NextAction("dark transformation", ACTION_HIGH + 5)
            }
        )
    );

    // Sudden Doom proc: free Death Coil (also builds Shadow Infusion).
    triggers.push_back(
        new TriggerNode(
            "sudden doom",
            {
                NextAction("death coil", ACTION_HIGH + 3)
            }
        )
    );

    // Festering Strike (Blood+Frost) to extend diseases before they fall off.
    triggers.push_back(
        new TriggerNode(
            "festering strike",
            {
                NextAction("festering strike", ACTION_DEFAULT + 0.9f)
            }
        )
    );

    // Spend Unholy/Death runes on Scourge Strike.
    triggers.push_back(
        new TriggerNode(
            "high unholy rune",
            {
                NextAction("scourge strike", ACTION_NORMAL + 3)
            }
        )
    );
    // Spend Blood+Frost runes on Festering Strike (Reaping returns them as Death runes).
    triggers.push_back(
        new TriggerNode(
            "high blood rune",
            {
                NextAction("festering strike", ACTION_NORMAL + 2)
            }
        )
    );

    // Army of the Dead pre-pull / burst cooldown.
    triggers.push_back(
        new TriggerNode(
            "army of the dead",
            {
                NextAction("army of the dead", ACTION_HIGH + 6)
            }
        )
    );
}

void UnholyDKAoeStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    // AoE: Death and Decay + Pestilence disease spread + Blood Boil with Death runes.
    triggers.push_back(
        new TriggerNode(
            "medium aoe",
            {
                NextAction("death and decay", ACTION_HIGH + 4),
                NextAction("pestilence", ACTION_HIGH + 3),
                NextAction("blood boil", ACTION_HIGH + 2)
            }
        )
    );
}
