/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "BloodDKStrategy.h"

#include "Playerbots.h"

// 4.3.4 Blood (TANK -- the only DK tank spec). Presence: Blood Presence (armor/stam/threat -- required).
// Priority: Outbreak/diseases up -> Death Strike whenever Frost+Unholy (or Death) pair available
//           (heal + Blood Shield mastery, the active-mitigation button) -> Rune Strike as RP dump
//           (threat) -> Heart Strike with Blood/Death runes -> Blood Boil / Death and Decay for AoE.
// Maintain Bone Shield. Cooldowns: Vampiric Blood, Dancing Rune Weapon, Icebound Fortitude, Rune Tap.
// Taunts: Dark Command + Death Grip (ranged pickup).
class BloodDKStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    BloodDKStrategyActionNodeFactory()
    {
        creators["outbreak"] = &outbreak;
        creators["rune strike"] = &rune_strike;
        creators["heart strike"] = &heart_strike;
        creators["death strike"] = &death_strike;
        creators["icy touch"] = &icy_touch;
        creators["dark command"] = &dark_command;
        creators["taunt spell"] = &dark_command;
    }

private:
    static ActionNode* outbreak([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "outbreak",
            { NextAction("blood presence") },
            /*A*/ { NextAction("icy touch") },
            /*C*/ {}
        );
    }
    static ActionNode* rune_strike([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "rune strike",
            { NextAction("blood presence") },
            /*A*/ {},
            /*C*/ {}
        );
    }
    static ActionNode* icy_touch([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "icy touch",
            { NextAction("blood presence") },
            /*A*/ {},
            /*C*/ {}
        );
    }
    static ActionNode* heart_strike([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "heart strike",
            { NextAction("blood presence") },
            /*A*/ {
                NextAction("blood strike")
            },
            /*C*/ {}
        );
    }
    static ActionNode* death_strike([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "death strike",
            { NextAction("blood presence") },
            /*A*/ {},
            /*C*/ {}
        );
    }
    static ActionNode* dark_command([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode(
            "dark command",
            { NextAction("blood presence") },
            /*A*/ {
                NextAction("death grip")
            },
            /*C*/ {}
        );
    }
};

BloodDKStrategy::BloodDKStrategy(PlayerbotAI* botAI) : GenericDKStrategy(botAI)
{
    actionNodeFactories.Add(new BloodDKStrategyActionNodeFactory());
}

std::vector<NextAction> BloodDKStrategy::getDefaultActions()
{
    return {
        NextAction("death strike", ACTION_DEFAULT + 0.7f),
        NextAction("rune strike", ACTION_DEFAULT + 0.6f),
        NextAction("heart strike", ACTION_DEFAULT + 0.5f),
        NextAction("blood boil", ACTION_DEFAULT + 0.3f),
        NextAction("horn of winter", ACTION_DEFAULT + 0.1f),
        NextAction("melee", ACTION_DEFAULT)
    };
}

void BloodDKStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericDKStrategy::InitTriggers(triggers);

    // Survival cooldowns first.
    triggers.push_back(
        new TriggerNode(
            "critical health",
            {
                NextAction("vampiric blood", ACTION_HIGH + 7),
                NextAction("icebound fortitude", ACTION_HIGH + 6),
                NextAction("death strike", ACTION_HIGH + 5)
            }
        )
    );
    triggers.push_back(
        new TriggerNode(
            "low health",
            {
                NextAction("death strike", ACTION_HIGH + 4),
                NextAction("rune tap", ACTION_HIGH + 3)
            }
        )
    );

    // Taunt / ranged pickup when losing aggro.
    triggers.push_back(
        new TriggerNode(
            "lose aggro",
            {
                NextAction("dark command", ACTION_HIGH + 4)
            }
        )
    );

    // Diseases: Outbreak applies both with no rune cost.
    triggers.push_back(
        new TriggerNode(
            "outbreak",
            {
                NextAction("outbreak", ACTION_HIGH + 3)
            }
        )
    );
    // Fallback disease application when Outbreak on cooldown.
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

    // Maintain Bone Shield.
    triggers.push_back(
        new TriggerNode(
            "bone shield",
            {
                NextAction("bone shield", ACTION_NORMAL + 6)
            }
        )
    );

    // Death Strike (active mitigation + Blood Shield) whenever a Frost/Unholy/Death pair is up.
    triggers.push_back(
        new TriggerNode(
            "high unholy rune",
            {
                NextAction("death strike", ACTION_NORMAL + 5)
            }
        )
    );
    triggers.push_back(
        new TriggerNode(
            "high frost rune",
            {
                NextAction("death strike", ACTION_NORMAL + 4)
            }
        )
    );

    // Heart Strike with Blood/Death runes (cleaves).
    triggers.push_back(
        new TriggerNode(
            "high blood rune",
            {
                NextAction("heart strike", ACTION_NORMAL + 3)
            }
        )
    );

    // Rune Strike: Runic Power dump for threat (free in 4.x).
    triggers.push_back(
        new TriggerNode(
            "rune strike",
            {
                NextAction("rune strike", ACTION_NORMAL + 2)
            }
        )
    );

    // Dancing Rune Weapon: parry + threat cooldown.
    triggers.push_back(
        new TriggerNode(
            "blood tap",
            {
                NextAction("blood tap", ACTION_NORMAL + 1)
            }
        )
    );
}
