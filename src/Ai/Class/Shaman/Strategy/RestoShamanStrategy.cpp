/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "RestoShamanStrategy.h"
#include "Playerbots.h"

RestoShamanStrategy::RestoShamanStrategy(PlayerbotAI* botAI) : GenericShamanStrategy(botAI)
{
    // No custom ActionNodeFactory needed
}

// ===== Trigger Initialization ===
void RestoShamanStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    GenericShamanStrategy::InitTriggers(triggers);

    // ShatterCore 4.3.4 Restoration healing (class reference 9.3). Cataclysm renamed the core heals:
    //   Healing Wave        -> efficient filler heal
    //   Greater Healing Wave -> big, expensive tank heal (big hits)
    //   Healing Surge       -> fast, expensive emergency triage heal (replaces 3.3.5 Lesser Healing Wave's role)
    //   Riptide             -> instant HoT + grants Tidal Waves (weave on cooldown)
    //   Chain Heal / Healing Rain -> group/raid healing
    // Tidal Waves (from Riptide/Chain Heal) speeds the next HW/GHW, so Riptide stays the top of each tier.

    // Totem Triggers
    triggers.push_back(new TriggerNode("call of the elements", { NextAction("call of the elements", 60.0f) }));
    triggers.push_back(new TriggerNode("low health", { NextAction("stoneclaw totem", 40.0f) }));
    triggers.push_back(new TriggerNode("medium mana", { NextAction("mana tide totem", ACTION_HIGH + 5) }));

    // Earth Shield permanently on the tank (refresh when missing/low charges) -- top maintenance priority.
    triggers.push_back(new TriggerNode("earth shield on main tank", { NextAction("earth shield on main tank", ACTION_HIGH + 7) }));

    // Group damage: Riptide weave, then Healing Rain (ground AoE HoT) / Chain Heal on clumps.
    triggers.push_back(new TriggerNode("group heal setting", { NextAction("riptide on party", 27.0f),
                                                               NextAction("healing rain", 26.5f),
                                                               NextAction("chain heal on party", 26.0f) }));

    // Critical: instant Riptide first, then a fast Healing Surge emergency save.
    triggers.push_back(new TriggerNode("party member critical health", { NextAction("riptide on party", 25.0f),
                                                                         NextAction("healing surge on party", 24.0f),
                                                                         NextAction("greater healing wave on party", 23.0f) }));

    // Low: Riptide -> Greater Healing Wave (big heal, sped up by Tidal Waves) -> Healing Surge backup.
    triggers.push_back(new TriggerNode("party member low health", { NextAction("riptide on party", 19.0f),
                                                                    NextAction("greater healing wave on party", 18.0f),
                                                                    NextAction("healing surge on party", 17.0f) }));

    // Medium: efficient Healing Wave filler (Riptide kept on cooldown for Tidal Waves).
    triggers.push_back(new TriggerNode("party member medium health", { NextAction("riptide on party", 16.0f),
                                                                       NextAction("healing wave on party", 15.0f) }));

    // Topping off: cheap Healing Wave / Riptide.
    triggers.push_back(new TriggerNode("party member almost full health", { NextAction("riptide on party", 12.0f),
                                                                            NextAction("healing wave on party", 11.0f) }));

    // Dispel Triggers
    triggers.push_back(new TriggerNode("party member cleanse spirit poison", { NextAction("cleanse spirit poison on party", ACTION_DISPEL + 2) }));
    triggers.push_back(new TriggerNode("party member cleanse spirit disease", { NextAction("cleanse spirit disease on party", ACTION_DISPEL + 2) }));
    triggers.push_back(new TriggerNode("party member cleanse spirit curse",{ NextAction("cleanse spirit curse on party", ACTION_DISPEL + 2) }));

    // Range/Mana Triggers
    triggers.push_back(new TriggerNode("enemy too close for spell", { NextAction("flee", ACTION_MOVE + 9) }));
    triggers.push_back(new TriggerNode("party member to heal out of spell range", { NextAction("reach party member to heal", ACTION_CRITICAL_HEAL + 1) }));
    triggers.push_back(new TriggerNode("water shield", { NextAction("water shield", 19.5f) }));
}

void ShamanHealerDpsStrategy::InitTriggers(std::vector<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("healer should attack", { NextAction("flame shock", ACTION_DEFAULT + 0.2f),
                                                                 NextAction("lava burst", ACTION_DEFAULT + 0.1f),
                                                                 NextAction("lightning bolt", ACTION_DEFAULT) }));

    triggers.push_back( new TriggerNode("medium aoe and healer should attack", { NextAction("chain lightning", ACTION_DEFAULT + 0.3f) }));
}
