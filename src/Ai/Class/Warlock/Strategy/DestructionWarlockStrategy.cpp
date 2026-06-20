/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "DestructionWarlockStrategy.h"
#include "Playerbots.h"

// ===== Action Node Factory =====
class DestructionWarlockStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    DestructionWarlockStrategyActionNodeFactory()
    {
        creators["immolate"] = &immolate;
        creators["immolate on attacker"] = &immolate_on_attacker;
        creators["conflagrate"] = &conflagrate;
        creators["chaos bolt"] = &chaos_bolt;
        creators["incinerate"] = &incinerate;
        creators["corruption"] = &corruption;
        creators["corruption on attacker"] = &corruption_on_attacker;
        creators["shadow bolt"] = &shadow_bolt;
        creators["shadowburn"] = &shadowburn;
        creators["soul fire"] = &soul_fire;
        creators["life tap"] = &life_tap;
        creators["shadowfury"] = &shadowfury;
        creators["shadowflame"] = &shadowflame;
        creators["seed of corruption"] = &seed_of_corruption;
        creators["seed of corruption on attacker"] = &seed_of_corruption;
        creators["rain of fire"] = &rain_of_fire;
        creators["bane of doom"] = &bane_of_doom;
        creators["demon soul"] = &demon_soul;
        creators["dark intent"] = &dark_intent;
    }

private:
    static ActionNode* immolate(PlayerbotAI*) { return new ActionNode("immolate", {}, {}, {}); }
    static ActionNode* immolate_on_attacker(PlayerbotAI*) { return new ActionNode("immolate on attacker", {}, {}, {}); }
    static ActionNode* conflagrate(PlayerbotAI*) { return new ActionNode("conflagrate", {}, {}, {}); }
    static ActionNode* chaos_bolt(PlayerbotAI*) { return new ActionNode("chaos bolt", {}, {}, {}); }
    static ActionNode* incinerate(PlayerbotAI*) { return new ActionNode("incinerate", {}, {}, {}); }
    static ActionNode* corruption(PlayerbotAI*) { return new ActionNode("corruption", {}, {}, {}); }
    static ActionNode* corruption_on_attacker(PlayerbotAI*) { return new ActionNode("corruption on attacker", {}, {}, {}); }
    static ActionNode* shadow_bolt(PlayerbotAI*) { return new ActionNode("shadow bolt", {}, {}, {}); }
    static ActionNode* shadowburn(PlayerbotAI*) { return new ActionNode("shadowburn", {}, {}, {}); }
    static ActionNode* soul_fire(PlayerbotAI*) { return new ActionNode("soul fire", {}, {}, {}); }
    static ActionNode* life_tap(PlayerbotAI*) { return new ActionNode("life tap", {}, {}, {}); }
    static ActionNode* shadowfury(PlayerbotAI*) { return new ActionNode("shadowfury", {}, {}, {}); }
    static ActionNode* shadowflame(PlayerbotAI*) { return new ActionNode("shadowflame", {}, {}, {}); }
    static ActionNode* seed_of_corruption(PlayerbotAI*) { return new ActionNode("seed of corruption", {}, {}, {}); }
    static ActionNode* rain_of_fire(PlayerbotAI*) { return new ActionNode("rain of fire", {}, {}, {}); }
    static ActionNode* bane_of_doom(PlayerbotAI*) { return new ActionNode("bane of doom", {}, {}, {}); }
    static ActionNode* demon_soul(PlayerbotAI*) { return new ActionNode("demon soul", {}, {}, {}); }
    static ActionNode* dark_intent(PlayerbotAI*) { return new ActionNode("dark intent", {}, {}, {}); }
};

// ===== Single Target Strategy =====
DestructionWarlockStrategy::DestructionWarlockStrategy(PlayerbotAI* botAI) : GenericWarlockStrategy(botAI)
{
    actionNodeFactories.Add(new DestructionWarlockStrategyActionNodeFactory());
}

// ===== Default Actions (fillers) =====
// 4.3.4 Destruction filler: Incinerate (Shadow Bolt only pre-Incinerate / level <64).
std::vector<NextAction> DestructionWarlockStrategy::getDefaultActions()
{
    return {
        NextAction("incinerate", ACTION_DEFAULT + 0.3f),  // primary filler once learned
        NextAction("shadow bolt", ACTION_DEFAULT + 0.2f), // low-level filler before Incinerate
        NextAction("shoot", ACTION_DEFAULT)
    };
}

// ===== Trigger Initialization ===
void DestructionWarlockStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericWarlockStrategy::InitTriggers(triggers);

    // 4.3.4 Destruction single-target priority (class reference doc section 11.3):
    // Maintain Immolate > Conflagrate on CD (requires Immolate up) > Bane of Doom + Corruption >
    // Chaos Bolt on CD > Shadowburn execute (<20%) > Incinerate filler. Soul Fire keeps the
    // Improved Soul Fire haste buff up.

    // Dark Intent self-buff.
    triggers.push_back(new TriggerNode("dark intent", { NextAction("dark intent", ACTION_HIGH + 8) }));

    // Maintain Immolate (also the Conflagrate enabler).
    triggers.push_back(new TriggerNode("immolate on attacker", { NextAction("immolate on attacker", ACTION_HIGH + 7) }));
    triggers.push_back(new TriggerNode("immolate", { NextAction("immolate", ACTION_HIGH + 7) }));

    // Conflagrate on cooldown.
    triggers.push_back(new TriggerNode("conflagrate", { NextAction("conflagrate", ACTION_HIGH + 6) }));

    // Maintain Bane of Doom + Corruption.
    triggers.push_back(new TriggerNode("bane of doom", { NextAction("bane of doom", ACTION_HIGH + 5) }));
    triggers.push_back(new TriggerNode("corruption on attacker", { NextAction("corruption on attacker", ACTION_HIGH + 4) }));
    triggers.push_back(new TriggerNode("corruption", { NextAction("corruption", ACTION_HIGH + 4) }));

    // Chaos Bolt on cooldown.
    triggers.push_back(new TriggerNode("chaos bolt", { NextAction("chaos bolt", ACTION_HIGH + 3) }));

    // Shadowburn execute: instant, target < 20% HP.
    triggers.push_back(new TriggerNode("target critical health", { NextAction("shadowburn", ACTION_HIGH + 2) }));

    // Backlash proc -> free instant Shadow Bolt.
    triggers.push_back(new TriggerNode("backlash", { NextAction("shadow bolt", ACTION_HIGH + 2) }));

    // Demon Soul (Imp) damage cooldown.
    triggers.push_back(new TriggerNode("demon soul", { NextAction("demon soul", ACTION_NORMAL + 4) }));

    // Life Tap glyph buff and mana filler.
    triggers.push_back(new TriggerNode("life tap glyph buff", { NextAction("life tap", ACTION_HIGH + 1) }));
    triggers.push_back(new TriggerNode("life tap", { NextAction("life tap", ACTION_DEFAULT + 0.1f) }));

    triggers.push_back(new TriggerNode("enemy too close for spell", { NextAction("flee", ACTION_EMERGENCY) }));
}
