/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "AfflictionWarlockStrategy.h"
#include "Playerbots.h"

// ===== Action Node Factory =====
class AfflictionWarlockStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    AfflictionWarlockStrategyActionNodeFactory()
    {
        creators["corruption"] = &corruption;
        creators["corruption on attacker"] = &corruption;
        creators["unstable affliction"] = &unstable_affliction;
        creators["unstable affliction on attacker"] = &unstable_affliction;
        creators["haunt"] = &haunt;
        creators["shadow bolt"] = &shadow_bolt;
        creators["drain soul"] = &drain_soul;
        creators["life tap"] = &life_tap;
        creators["shadowflame"] = &shadowflame;
        creators["seed of corruption on attacker"] = &seed_of_corruption;
        creators["seed of corruption"] = &seed_of_corruption;
        creators["rain of fire"] = &rain_of_fire;
        creators["bane of agony"] = &bane_of_agony;
        creators["bane of agony on attacker"] = &bane_of_agony;
        creators["demon soul"] = &demon_soul;
        creators["dark intent"] = &dark_intent;
        creators["soulburn"] = &soulburn;
        creators["soul fire"] = &soul_fire;
    }

private:
    static ActionNode* corruption(PlayerbotAI*) { return new ActionNode("corruption", {}, {}, {}); }
    static ActionNode* unstable_affliction(PlayerbotAI*) { return new ActionNode("unstable affliction", {}, {}, {}); }
    static ActionNode* haunt(PlayerbotAI*) { return new ActionNode("haunt", {}, {}, {}); }
    static ActionNode* shadow_bolt(PlayerbotAI*) { return new ActionNode("shadow bolt", {}, {}, {}); }
    static ActionNode* drain_soul(PlayerbotAI*) { return new ActionNode("drain soul", {}, {}, {}); }
    static ActionNode* life_tap(PlayerbotAI*) { return new ActionNode("life tap", {}, {}, {}); }
    static ActionNode* shadowflame(PlayerbotAI*) { return new ActionNode("shadowflame", {}, {}, {}); }
    static ActionNode* seed_of_corruption(PlayerbotAI*) { return new ActionNode("seed of corruption", {}, {}, {}); }
    static ActionNode* rain_of_fire(PlayerbotAI*) { return new ActionNode("rain of fire", {}, {}, {}); }
    static ActionNode* bane_of_agony(PlayerbotAI*) { return new ActionNode("bane of agony", {}, {}, {}); }
    static ActionNode* demon_soul(PlayerbotAI*) { return new ActionNode("demon soul", {}, {}, {}); }
    static ActionNode* dark_intent(PlayerbotAI*) { return new ActionNode("dark intent", {}, {}, {}); }
    static ActionNode* soulburn(PlayerbotAI*) { return new ActionNode("soulburn", {}, {}, {}); }
    static ActionNode* soul_fire(PlayerbotAI*) { return new ActionNode("soul fire", {}, {}, {}); }
};

// ===== Single Target Strategy =====
AfflictionWarlockStrategy::AfflictionWarlockStrategy(PlayerbotAI* botAI) : GenericWarlockStrategy(botAI)
{
    actionNodeFactories.Add(new AfflictionWarlockStrategyActionNodeFactory());
}

// ===== Default Actions (single-target fillers, used when no higher-priority trigger fires) =====
// 4.3.4 Affliction filler spine: Drain Soul is the execute (target < 25%), Shadow Bolt otherwise.
std::vector<NextAction> AfflictionWarlockStrategy::getDefaultActions()
{
    return {
        NextAction("shadow bolt", ACTION_DEFAULT + 0.3f),  // primary filler > 25% HP
        NextAction("shoot", ACTION_DEFAULT)
    };
}

// ===== Trigger Initialization ===
void AfflictionWarlockStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericWarlockStrategy::InitTriggers(triggers);

    // 4.3.4 Affliction single-target priority (class reference doc section 11.1):
    // Haunt on CD (Shadow Embrace) > maintain UA/Corruption/Bane of Agony > Drain Soul execute (<25%) >
    // Shadow Bolt filler. DoT maintenance out-ranks Haunt only when a DoT has actually dropped so we never
    // clip a rolling DoT, but Haunt is prioritized as a damage-amp debuff on cooldown.

    // Dark Intent self-buff (haste/crit link) -- keep it up.
    triggers.push_back(new TriggerNode("dark intent", { NextAction("dark intent", ACTION_HIGH + 7) }));

    // Maintain the DoT package (recast only when missing/expiring -- triggers gate on remaining duration).
    triggers.push_back(new TriggerNode("unstable affliction on attacker", { NextAction("unstable affliction on attacker", ACTION_HIGH + 6) }));
    triggers.push_back(new TriggerNode("unstable affliction", { NextAction("unstable affliction", ACTION_HIGH + 6) }));
    triggers.push_back(new TriggerNode("corruption on attacker", { NextAction("corruption on attacker", ACTION_HIGH + 5) }));
    triggers.push_back(new TriggerNode("corruption", { NextAction("corruption", ACTION_HIGH + 5) }));
    triggers.push_back(new TriggerNode("bane of agony on attacker", { NextAction("bane of agony on attacker", ACTION_HIGH + 4) }));
    triggers.push_back(new TriggerNode("bane of agony", { NextAction("bane of agony", ACTION_HIGH + 4) }));

    // Haunt on cooldown (+DoT damage taken, refreshes Shadow Embrace).
    triggers.push_back(new TriggerNode("haunt", { NextAction("haunt", ACTION_HIGH + 3) }));

    // Shadow Trance (Nightfall) instant Shadow Bolt proc.
    triggers.push_back(new TriggerNode("shadow trance", { NextAction("shadow bolt", ACTION_HIGH + 2) }));

    // Drain Soul execute: target < 25% HP -- replaces the filler entirely.
    triggers.push_back(new TriggerNode("target low health 25", { NextAction("drain soul", ACTION_HIGH + 2) }));

    // Demon Soul (Felhunter) damage cooldown.
    triggers.push_back(new TriggerNode("demon soul", { NextAction("demon soul", ACTION_NORMAL + 4) }));

    // Life Tap glyph buff (keep the +spellpower-from-spirit buff up), then Life Tap as a mana filler.
    triggers.push_back(new TriggerNode("life tap glyph buff", { NextAction("life tap", ACTION_HIGH + 1) }));
    triggers.push_back(new TriggerNode("life tap", { NextAction("life tap", ACTION_DEFAULT + 0.1f) }));

    triggers.push_back(new TriggerNode("enemy too close for spell", { NextAction("flee", ACTION_EMERGENCY) }));
}
