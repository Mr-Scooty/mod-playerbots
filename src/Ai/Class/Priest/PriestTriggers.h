/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#ifndef _PLAYERBOT_PRIESTTRIGGERS_H
#define _PLAYERBOT_PRIESTTRIGGERS_H

#include "CureTriggers.h"
#include "GenericTriggers.h"
#include "NamedObjectContext.h"  // ShatterCore: for Qualified (base of the in-header ShadowOrbsValue)
#include "SharedDefines.h"
#include "Trigger.h"
#include "Value.h"
#include <set>

class PlayerbotAI;

DEBUFF_CHECKISOWNER_TRIGGER(HolyFireTrigger, "holy fire");
DEBUFF_CHECKISOWNER_TRIGGER(PowerWordPainTrigger, "shadow word: pain");
DEBUFF_ENEMY_TRIGGER(PowerWordPainOnAttackerTrigger, "shadow word: pain");
DEBUFF_CHECKISOWNER_TRIGGER(VampiricTouchTrigger, "vampiric touch");
DEBUFF_ENEMY_TRIGGER(VampiricTouchOnAttackerTrigger, "vampiric touch on attacker");
BUFF_TRIGGER(VampiricEmbraceTrigger, "vampiric embrace");
CURE_TRIGGER(DispelMagicTrigger, "dispel magic", DISPEL_MAGIC);
CURE_PARTY_TRIGGER(DispelMagicPartyMemberTrigger, "dispel magic", DISPEL_MAGIC);
CURE_TRIGGER(CureDiseaseTrigger, "cure disease", DISPEL_DISEASE);
CURE_PARTY_TRIGGER(PartyMemberCureDiseaseTrigger, "cure disease", DISPEL_DISEASE);
BUFF_TRIGGER_A(InnerFireTrigger, "inner fire");
BUFF_TRIGGER_A(ShadowformTrigger, "shadowform");
BOOST_TRIGGER(PowerInfusionTrigger, "power infusion");
BUFF_TRIGGER(InnerFocusTrigger, "inner focus");
CC_TRIGGER(ShackleUndeadTrigger, "shackle undead");
INTERRUPT_TRIGGER(SilenceTrigger, "silence");
INTERRUPT_HEALER_TRIGGER(SilenceEnemyHealerTrigger, "silence");

// racials
DEBUFF_CHECKISOWNER_TRIGGER(DevouringPlagueTrigger, "devouring plague");
BUFF_TRIGGER(TouchOfWeaknessTrigger, "touch of weakness");
DEBUFF_TRIGGER(HexOfWeaknessTrigger, "hex of weakness");
BUFF_TRIGGER(ShadowguardTrigger, "shadowguard");
BUFF_TRIGGER(FearWardTrigger, "fear ward");
DEFLECT_TRIGGER(FeedbackTrigger, "feedback");
SNARE_TRIGGER(ChastiseTrigger, "chastise");

BOOST_TRIGGER_A(ShadowfiendTrigger, "shadowfiend");

class ShadowProtectionTrigger : public BuffTrigger
{
public:
    ShadowProtectionTrigger(PlayerbotAI* botAI)
    : BuffTrigger(botAI, "shadow protection", 4 * 2000) {}

    bool IsActive() override;
};

class ShadowProtectionOnPartyTrigger : public BuffOnPartyTrigger
{
public:
    ShadowProtectionOnPartyTrigger(PlayerbotAI* botAI)
        : BuffOnPartyTrigger(botAI, "shadow protection", 4 * 2000) {}
};

class PowerWordFortitudeOnPartyTrigger : public BuffOnPartyTrigger
{
public:
    PowerWordFortitudeOnPartyTrigger(PlayerbotAI* botAI)
        : BuffOnPartyTrigger(botAI, "power word: fortitude", 4 * 2000) {}
};

class PowerWordFortitudeTrigger : public BuffTrigger
{
public:
    PowerWordFortitudeTrigger(PlayerbotAI* botAI)
        : BuffTrigger(botAI, "power word: fortitude", 4 * 2000) {}

    bool IsActive() override;
};

class DivineSpiritOnPartyTrigger : public BuffOnPartyTrigger
{
public:
    DivineSpiritOnPartyTrigger(PlayerbotAI* botAI)
        : BuffOnPartyTrigger(botAI, "divine spirit", 4 * 2000) {}
};

class DivineSpiritTrigger : public BuffTrigger
{
public:
    DivineSpiritTrigger(PlayerbotAI* botAI)
        : BuffTrigger(botAI, "divine spirit", 4 * 2000) {}

    bool IsActive() override;
};

class BindingHealTrigger : public PartyMemberLowHealthTrigger
{
public:
    BindingHealTrigger(PlayerbotAI* botAI);

    bool IsActive() override;
};

class MindSearChannelCheckTrigger : public Trigger
{
public:
    MindSearChannelCheckTrigger(PlayerbotAI* botAI, uint32 minEnemies = 2)
        : Trigger(botAI, "mind sear channel check"), minEnemies(minEnemies) {}

    bool IsActive() override;

protected:
    uint32 minEnemies;
    static const std::set<uint32> MIND_SEAR_SPELL_IDS;
};

// ShatterCore 4.3.4 Shadow: Shadow Orbs are NOT a POWER type in 4.3.4 -- they are tracked as stacks of the
// "shadow orb" buff (0..3). This value reads the current stack count so triggers/actions can spend them.
class ShadowOrbsValue : public Uint8CalculatedValue, public Qualified
{
public:
    ShadowOrbsValue(PlayerbotAI* botAI, std::string const name = "shadow orbs")
        : Uint8CalculatedValue(botAI, name) {}

    uint8 Calculate() override;
};

// Shadow Orbs >= amount (default 3 = empower Mind Blast for max benefit; bots may also dump at any count).
class ShadowOrbsAvailableTrigger : public StatAvailable
{
public:
    ShadowOrbsAvailableTrigger(PlayerbotAI* botAI, int32 amount = 3)
        : StatAvailable(botAI, amount, "shadow orbs available") {}

    bool IsActive() override;
};

// Shadow Word: Death execute window (current target below 25% health).
class ShadowWordDeathExecuteTrigger : public Trigger
{
public:
    ShadowWordDeathExecuteTrigger(PlayerbotAI* botAI) : Trigger(botAI, "shadow word: death execute") {}

    bool IsActive() override;
};

// ShatterCore 4.3.4 Holy: Chakra-state maintenance. Serenity is the single-target stance, Sanctuary the AoE stance.
class ChakraSerenityTrigger : public HasAuraTrigger
{
public:
    ChakraSerenityTrigger(PlayerbotAI* botAI) : HasAuraTrigger(botAI, "chakra: serenity") {}

    bool IsActive() override;
};

class ChakraSanctuaryTrigger : public HasAuraTrigger
{
public:
    ChakraSanctuaryTrigger(PlayerbotAI* botAI) : HasAuraTrigger(botAI, "chakra: sanctuary") {}

    bool IsActive() override;
};

#endif
