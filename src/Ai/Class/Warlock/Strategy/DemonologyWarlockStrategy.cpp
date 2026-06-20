/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "DemonologyWarlockStrategy.h"
#include "Playerbots.h"

// ===== Action Node Factory =====
class DemonologyWarlockStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    DemonologyWarlockStrategyActionNodeFactory()
    {
        creators["metamorphosis"] = &metamorphosis;
        creators["demonic empowerment"] = &demonic_empowerment;
        creators["corruption"] = &corruption;
        creators["corruption on attacker"] = &corruption_on_attacker;
        creators["immolate"] = &immolate;
        creators["immolate on attacker"] = &immolate_on_attacker;
        creators["incinerate"] = &incinerate;
        creators["soul fire"] = &soul_fire;
        creators["shadow bolt"] = &shadow_bolt;
        creators["life tap"] = &life_tap;
        creators["immolation aura"] = &immolation_aura;
        creators["shadowflame"] = &shadowflame;
        creators["seed of corruption on attacker"] = &seed_of_corruption_on_attacker;
        creators["seed of corruption"] = &seed_of_corruption;
        creators["rain of fire"] = &rain_of_fire;
        creators["demon charge"] = &demon_charge;
        creators["hand of gul'dan"] = &hand_of_guldan;
        creators["bane of doom"] = &bane_of_doom;
        creators["demon soul"] = &demon_soul;
        creators["dark intent"] = &dark_intent;
    }

private:
    static ActionNode* metamorphosis(PlayerbotAI*) { return new ActionNode("metamorphosis", {}, {}, {}); }
    static ActionNode* demonic_empowerment(PlayerbotAI*) { return new ActionNode("demonic empowerment", {}, {}, {}); }
    static ActionNode* corruption(PlayerbotAI*) { return new ActionNode("corruption", {}, {}, {}); }
    static ActionNode* corruption_on_attacker(PlayerbotAI*) { return new ActionNode("corruption on attacker", {}, {}, {}); }
    static ActionNode* immolate(PlayerbotAI*) { return new ActionNode("immolate", {}, {}, {}); }
    static ActionNode* immolate_on_attacker(PlayerbotAI*) { return new ActionNode("immolate on attacker", {}, {}, {}); }
    static ActionNode* incinerate(PlayerbotAI*) { return new ActionNode("incinerate", {}, {}, {}); }
    static ActionNode* soul_fire(PlayerbotAI*) { return new ActionNode("soul fire", {}, {}, {}); }
    static ActionNode* shadow_bolt(PlayerbotAI*) { return new ActionNode("shadow bolt", {}, {}, {}); }
    static ActionNode* life_tap(PlayerbotAI*) { return new ActionNode("life tap", {}, {}, {}); }
    static ActionNode* immolation_aura(PlayerbotAI*) { return new ActionNode("immolation aura", {}, {}, {}); }
    static ActionNode* shadowflame(PlayerbotAI*) { return new ActionNode("shadowflame", {}, {}, {}); }
    static ActionNode* seed_of_corruption_on_attacker(PlayerbotAI*) { return new ActionNode("seed of corruption on attacker", {}, {}, {}); }
    static ActionNode* seed_of_corruption(PlayerbotAI*) { return new ActionNode("seed of corruption", {}, {}, {}); }
    static ActionNode* rain_of_fire(PlayerbotAI*) { return new ActionNode("rain of fire", {}, {}, {}); }
    static ActionNode* demon_charge(PlayerbotAI*) { return new ActionNode("demon charge", {}, {}, {}); }
    static ActionNode* hand_of_guldan(PlayerbotAI*) { return new ActionNode("hand of gul'dan", {}, {}, {}); }
    static ActionNode* bane_of_doom(PlayerbotAI*) { return new ActionNode("bane of doom", {}, {}, {}); }
    static ActionNode* demon_soul(PlayerbotAI*) { return new ActionNode("demon soul", {}, {}, {}); }
    static ActionNode* dark_intent(PlayerbotAI*) { return new ActionNode("dark intent", {}, {}, {}); }
};

// ===== Single Target Strategy =====
DemonologyWarlockStrategy::DemonologyWarlockStrategy(PlayerbotAI* botAI) : GenericWarlockStrategy(botAI)
{
    actionNodeFactories.Add(new DemonologyWarlockStrategyActionNodeFactory());
}

// ===== Default Actions (fillers) =====
// 4.3.4 Demonology filler: Shadow Bolt when no proc/maintenance step is due.
std::vector<NextAction> DemonologyWarlockStrategy::getDefaultActions()
{
    return {
        NextAction("shadow bolt", ACTION_DEFAULT + 0.2f),
        NextAction("shoot", ACTION_DEFAULT)
    };
}

// ===== Trigger Initialization ===
void DemonologyWarlockStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericWarlockStrategy::InitTriggers(triggers);

    // 4.3.4 Demonology single-target priority (class reference doc section 11.2):
    // Hand of Gul'dan on CD > maintain Immolate/Corruption/Bane of Doom > Metamorphosis CD >
    // Soul Fire during Decimation (<25%) > Incinerate during Molten Core procs > Shadow Bolt filler.

    // Dark Intent self-buff.
    triggers.push_back(new TriggerNode("dark intent", { NextAction("dark intent", ACTION_HIGH + 8) }));

    // Metamorphosis burst cooldown (paired with Demon Soul).
    triggers.push_back(new TriggerNode("metamorphosis", { NextAction("metamorphosis", ACTION_HIGH + 7) }));
    triggers.push_back(new TriggerNode("demon soul", { NextAction("demon soul", ACTION_NORMAL + 4) }));

    // Keep the Felguard empowered (Demonic Empowerment off cooldown).
    triggers.push_back(new TriggerNode("demonic empowerment", { NextAction("demonic empowerment", ACTION_HIGH + 6) }));

    // Hand of Gul'dan on cooldown -- core nuke.
    triggers.push_back(new TriggerNode("hand of gul'dan", { NextAction("hand of gul'dan", ACTION_HIGH + 5) }));

    // Maintain Immolate / Corruption / Bane of Doom (recast only when missing/expiring).
    triggers.push_back(new TriggerNode("immolate on attacker", { NextAction("immolate on attacker", ACTION_HIGH + 4) }));
    triggers.push_back(new TriggerNode("immolate", { NextAction("immolate", ACTION_HIGH + 4) }));
    triggers.push_back(new TriggerNode("corruption on attacker", { NextAction("corruption on attacker", ACTION_HIGH + 3) }));
    triggers.push_back(new TriggerNode("corruption", { NextAction("corruption", ACTION_HIGH + 3) }));
    triggers.push_back(new TriggerNode("bane of doom", { NextAction("bane of doom", ACTION_HIGH + 2) }));

    // Procs: Soul Fire during Decimation (execute, target < 25%); Incinerate during Molten Core (3 charges).
    triggers.push_back(new TriggerNode("decimation", { NextAction("soul fire", ACTION_HIGH + 2) }));
    triggers.push_back(new TriggerNode("molten core", { NextAction("incinerate", ACTION_HIGH + 1) }));

    // Life Tap glyph buff and mana filler.
    triggers.push_back(new TriggerNode("life tap glyph buff", { NextAction("life tap", ACTION_HIGH + 1) }));
    triggers.push_back(new TriggerNode("life tap", { NextAction("life tap", ACTION_DEFAULT + 0.1f) }));

    triggers.push_back(new TriggerNode("meta melee flee check", { NextAction("flee", ACTION_EMERGENCY) }));
}

// Combat strategy to run to melee for Immolation Aura
// Enabled by default for the Demonology spec
// To enable, type "co +meta melee"
// To disable, type "co -meta melee"
MetaMeleeAoeStrategy::MetaMeleeAoeStrategy(PlayerbotAI* botAI) : CombatStrategy(botAI) {}

void MetaMeleeAoeStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(
        new TriggerNode(
            "immolation aura active",
            {
                NextAction("reach melee", 25.5f),
                NextAction("demon charge", 25.0f)
            }
        )
    );
}
