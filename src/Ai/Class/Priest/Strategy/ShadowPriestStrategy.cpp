/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "ShadowPriestStrategy.h"

#include "Playerbots.h"
#include "ShadowPriestStrategyActionNodeFactory.h"

ShadowPriestStrategy::ShadowPriestStrategy(PlayerbotAI* botAI) : GenericPriestStrategy(botAI)
{
    actionNodeFactories.Add(new ShadowPriestStrategyActionNodeFactory());
}

std::vector<NextAction> ShadowPriestStrategy::getDefaultActions()
{
    // ShatterCore 4.3.4 Shadow single-target FILLER priority (used when no higher-priority trigger node fires).
    // Mind Blast on cooldown (converts Shadow Orbs -> Empowered Shadow) > Mind Flay channel filler (also refreshes
    // SW:P and procs orbs) > Mind Spike (instant filler / on the move) > shoot. DoTs, the SW:D execute and the
    // 3-orb Mind Blast are handled as trigger nodes below so they always out-rank the plain fillers.
    return {
        NextAction("mind blast", ACTION_DEFAULT + 0.4f),
        NextAction("mind flay", ACTION_DEFAULT + 0.3f),
        NextAction("mind spike", ACTION_DEFAULT + 0.2f),
        NextAction("shadow word: death execute", ACTION_DEFAULT + 0.1f),  // execute-gated movement filler
        NextAction("shoot", ACTION_DEFAULT)
    };
}

void ShadowPriestStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericPriestStrategy::InitTriggers(triggers);

    // Shadowform must always be up (Shadow's core stance; also gates Mind Flay/Shadow Orb generation).
    triggers.push_back(new TriggerNode("shadowform", { NextAction("shadowform", ACTION_HIGH + 6) }));

    // Vampiric Embrace (raid self-/group-heal from Shadow damage) -- keep the buff up.
    triggers.push_back(new TriggerNode("vampiric embrace", { NextAction("vampiric embrace", ACTION_HIGH + 5) }));

    // Spend 3 Shadow Orbs with Mind Blast to roll Empowered Shadow (Mastery: Shadow Orb Power). This out-ranks the
    // filler Mind Blast so the empowered cast lands as soon as the orbs are capped.
    triggers.push_back(new TriggerNode("shadow orbs available", { NextAction("mind blast", ACTION_HIGH + 4) }));

    // Shadow Word: Death execute (target < 25%). Self-damage otherwise, so it is gated to the execute window.
    triggers.push_back(
        new TriggerNode("shadow word: death execute", { NextAction("shadow word: death execute", ACTION_HIGH + 3) }));

    // Shadowfiend on cooldown (sustained DPS + mana). Trigger fires off cooldown.
    triggers.push_back(new TriggerNode("shadowfiend", { NextAction("shadowfiend", ACTION_HIGH + 2) }));

    // Survival / mana: Dispersion as an emergency button.
    triggers.push_back(new TriggerNode("low mana", { NextAction("dispersion", ACTION_HIGH + 1) }));
    triggers.push_back(new TriggerNode("critical health", { NextAction("dispersion", ACTION_HIGH + 1) }));

    // Interrupts.
    triggers.push_back(new TriggerNode("silence", { NextAction("silence", ACTION_INTERRUPT + 1) }));
    triggers.push_back(
        new TriggerNode("silence on enemy healer", { NextAction("silence on enemy healer", ACTION_INTERRUPT) }));
}

void ShadowPriestAoeStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode(
            "shadow word: pain on attacker",
            {
                NextAction("shadow word: pain on attacker", ACTION_NORMAL + 5)
            }
        )
    );
    triggers.push_back(
        new TriggerNode(
            "vampiric touch on attacker",
            {
                NextAction("vampiric touch on attacker", ACTION_NORMAL + 4)
            }
        )
    );
    triggers.push_back(
        new TriggerNode(
            "mind sear channel check",
            {
                NextAction("cancel channel", ACTION_HIGH + 5)
            }
        )
    );
    triggers.push_back(
        new TriggerNode(
            "medium aoe",
            {
                NextAction("mind sear", ACTION_HIGH + 4)
            }
        )
    );
}

void ShadowPriestDebuffStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode(
            "vampiric touch",
            {
                NextAction("vampiric touch", ACTION_HIGH + 3)
            }
        )
    );
    triggers.push_back(
        new TriggerNode(
            "devouring plague",
            {
                NextAction("devouring plague", ACTION_HIGH + 2)
            }
        )
    );
    triggers.push_back(
        new TriggerNode(
            "shadow word: pain",
            {
                NextAction("shadow word: pain", ACTION_HIGH + 1)
            }
        )
    );
}
