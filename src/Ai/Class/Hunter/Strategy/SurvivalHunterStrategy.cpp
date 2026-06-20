/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "SurvivalHunterStrategy.h"
#include "Playerbots.h"

// ===== Action Node Factory =====
// Explosive Shot is rank-gated; this factory lets the bot fall back to the highest rank it actually knows
// (rank 4 -> 3 -> 2 -> 1) when the requested rank isn't in the spellbook.
class SurvivalHunterStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    SurvivalHunterStrategyActionNodeFactory()
    {
        creators["explosive shot rank 4"] = &explosive_shot_rank_4;
        creators["explosive shot rank 3"] = &explosive_shot_rank_3;
        creators["explosive shot rank 2"] = &explosive_shot_rank_2;
    }

private:
    static ActionNode* explosive_shot_rank_4([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("explosive shot rank 4",
                              /*P*/ {},
                              /*A*/ { NextAction("explosive shot rank 3") },
                              /*C*/ {});
    }
    static ActionNode* explosive_shot_rank_3([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("explosive shot rank 3",
                              /*P*/ {},
                              /*A*/ { NextAction("explosive shot rank 2") },
                              /*C*/ {});
    }
    static ActionNode* explosive_shot_rank_2([[maybe_unused]] PlayerbotAI* botAI)
    {
        return new ActionNode("explosive shot rank 2",
                              /*P*/ {},
                              /*A*/ { NextAction("explosive shot rank 1") },
                              /*C*/ {});
    }
};

SurvivalHunterStrategy::SurvivalHunterStrategy(PlayerbotAI* botAI) : GenericHunterStrategy(botAI)
{
    actionNodeFactories.Add(new SurvivalHunterStrategyActionNodeFactory());
}

// ===== Default Actions (single-target fillers) =====
// ShatterCore 4.3.4 Survival (class reference 5.3). Cobra Shot is the focus builder (extends Serpent Sting),
// Arcane Shot is the focus dump, Explosive Shot is the top spender (handled in InitTriggers).
std::vector<NextAction> SurvivalHunterStrategy::getDefaultActions()
{
    return {
        NextAction("kill shot", 5.7f),       // execute (< 20%)
        NextAction("explosive shot", 5.6f),  // top spender (fallback if the trigger node didn't catch it)
        NextAction("black arrow", 5.5f),     // maintain (shares trap CD)
        NextAction("arcane shot", 5.3f),     // instant focus dump
        NextAction("cobra shot", 5.2f),      // focus builder / extends Serpent Sting
        NextAction("steady shot", 5.1f),     // pre-Cobra fallback for low-level bots
        NextAction("auto shot", 5.0f)
    };
}

// ===== Trigger Initialization =====
void SurvivalHunterStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericHunterStrategy::InitTriggers(triggers);

    // Lock and Load proc -> 2 free Explosive Shots (consume the charges).
    triggers.push_back(new TriggerNode("lock and load", { NextAction("explosive shot rank 4", 28.0f) }));

    // Execute window.
    triggers.push_back(new TriggerNode("target critical health", { NextAction("kill shot", 18.5f) }));

    // Explosive Shot on cooldown -- the SV priority spender.
    triggers.push_back(new TriggerNode("explosive shot", { NextAction("explosive shot", 18.0f) }));

    // Black Arrow maintenance (also feeds Lock and Load).
    triggers.push_back(new TriggerNode("black arrow", { NextAction("black arrow", 17.5f) }));

    // Maintain Serpent Sting (Improved Serpent Sting; Cobra Shot refreshes it).
    triggers.push_back(new TriggerNode("no stings", { NextAction("serpent sting", 17.0f) }));
    triggers.push_back(new TriggerNode("serpent sting on attacker", { NextAction("serpent sting on attacker", 16.5f) }));

    // Arcane Shot focus dump -- bank focus above ~60 so Explosive Shot stays castable.
    triggers.push_back(new TriggerNode("focus for arcane shot", { NextAction("arcane shot", 16.0f) }));

    // AoE: Multi-Shot (procs Serpent Spread).
    triggers.push_back(new TriggerNode("light aoe", { NextAction("multi-shot", 15.0f) }));
}
