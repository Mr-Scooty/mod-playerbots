/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#ifndef _PLAYERBOT_HUNTERTRIGGERS_H
#define _PLAYERBOT_HUNTERTRIGGERS_H

#include "CureTriggers.h"
#include "GenericTriggers.h"
#include "Trigger.h"
#include "PlayerbotAI.h"
#include <set>

class PlayerbotAI;

// Buff and Out of Combat Triggers

class HunterAspectOfTheDragonhawkTrigger : public BuffTrigger
{
public:
    HunterAspectOfTheDragonhawkTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "aspect of the dragonhawk") {}
    bool IsActive() override;
};

class HunterAspectOfTheWildTrigger : public BuffTrigger
{
public:
    HunterAspectOfTheWildTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "aspect of the wild") {}
};

class HunterAspectOfTheViperTrigger : public BuffTrigger
{
public:
    HunterAspectOfTheViperTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "aspect of the viper") {}
    bool IsActive() override;
};

class HunterAspectOfThePackTrigger : public BuffTrigger
{
public:
    HunterAspectOfThePackTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "aspect of the pack") {}
    bool IsActive() override;
};

class TrueshotAuraTrigger : public BuffTrigger
{
public:
    TrueshotAuraTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "trueshot aura") {}
};

class NoTrackTrigger : public BuffTrigger
{
public:
    NoTrackTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "no track") {}
    bool IsActive() override;
};

class HunterLowAmmoTrigger : public AmmoCountTrigger
{
public:
    HunterLowAmmoTrigger(PlayerbotAI* botAI) : AmmoCountTrigger(botAI, "ammo", 1, 30) {}

    bool IsActive() override;
};

class HunterNoAmmoTrigger : public AmmoCountTrigger
{
public:
    HunterNoAmmoTrigger(PlayerbotAI* botAI) : AmmoCountTrigger(botAI, "ammo", 1, 10) {}
};

class HunterHasAmmoTrigger : public AmmoCountTrigger
{
public:
    HunterHasAmmoTrigger(PlayerbotAI* botAI) : AmmoCountTrigger(botAI, "ammo", 1, 10) {}

    bool IsActive() override;
};

// ===== Cataclysm 4.3.4 Focus resource triggers =====
// Hunters use FOCUS (POWER_FOCUS), not mana. FocusValue ("focus") is registered in the shared ValueContext,
// so these read AI_VALUE2(uint8, "focus", "self target"). Mirrors Paladin's HolyPowerAvailableTrigger pattern.
class FocusAvailableTrigger : public StatAvailable
{
public:
    FocusAvailableTrigger(PlayerbotAI* botAI, int32 amount, std::string const name = "focus available")
        : StatAvailable(botAI, amount, name) {}
    bool IsActive() override;
};

// "bank" focus for the spec's main spender (Kill Command 40 / Explosive Shot ~50). Used to gate the
// Arcane Shot focus-dump so the bot keeps enough focus for the priority spender.
class FocusForArcaneShotTrigger : public FocusAvailableTrigger
{
public:
    FocusForArcaneShotTrigger(PlayerbotAI* botAI) : FocusAvailableTrigger(botAI, 60, "focus for arcane shot") {}
};

// Cobra/Steady Shot builder: cast when focus is getting low so we have focus to spend again.
class FocusLowTrigger : public StatAvailable
{
public:
    FocusLowTrigger(PlayerbotAI* botAI) : StatAvailable(botAI, 80, "focus low") {}
    bool IsActive() override;
};

// BM: pet has 5 Frenzy stacks -> Focus Fire to convert into a ranged-haste buff.
class FocusFireTrigger : public HasAuraTrigger
{
public:
    FocusFireTrigger(PlayerbotAI* botAI) : HasAuraTrigger(botAI, "frenzy effect") {}
    bool IsActive() override;
};

// MM: Master Marksman "Fire!" proc -> free instant Aimed Shot.
class FireProcTrigger : public HasAuraTrigger
{
public:
    FireProcTrigger(PlayerbotAI* botAI) : HasAuraTrigger(botAI, "fire!") {}
};

// MM: Careful Aim window -- target above 80% HP, hardcast Aimed Shot for guaranteed crits.
class CarefulAimTrigger : public Trigger
{
public:
    CarefulAimTrigger(PlayerbotAI* botAI) : Trigger(botAI, "careful aim") {}
    bool IsActive() override;
};

// Cooldown Triggers

class RapidFireTrigger : public BoostTrigger
{
public:
    RapidFireTrigger(PlayerbotAI* botAI) : BoostTrigger(botAI, "rapid fire") {}
};

class FervorTrigger : public StatAvailable
{
public:
    FervorTrigger(PlayerbotAI* botAI) : StatAvailable(botAI, 50, "fervor") {}
    bool IsActive() override;
};

class BestialWrathTrigger : public BuffTrigger
{
public:
    BestialWrathTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "bestial wrath") {}
};

// MM: Chimera Shot is ready (off cooldown). It also refreshes Serpent Sting.
class ChimeraShotTrigger : public SpellNoCooldownTrigger
{
public:
    ChimeraShotTrigger(PlayerbotAI* botAI) : SpellNoCooldownTrigger(botAI, "chimera shot") {}
};

class IntimidationTrigger : public BuffTrigger
{
public:
    IntimidationTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "intimidation") {}
};

class KillCommandTrigger : public BuffTrigger
{
public:
    KillCommandTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "kill command") {}
    bool IsActive() override;
};

class LockAndLoadTrigger : public BuffTrigger
{
public:
    LockAndLoadTrigger(PlayerbotAI* botAI) : BuffTrigger(botAI, "lock and load") {}

    bool IsActive() override
    {
        return botAI->HasAura("lock and load", botAI->GetBot());
    }
};

// CC Triggers

class FreezingTrapTrigger : public HasCcTargetTrigger
{
public:
    FreezingTrapTrigger(PlayerbotAI* botAI) : HasCcTargetTrigger(botAI, "freezing trap") {}
};

class ConcussiveShotOnSnareTargetTrigger : public SnareTargetTrigger
{
public:
    ConcussiveShotOnSnareTargetTrigger(PlayerbotAI* botAI) : SnareTargetTrigger(botAI, "concussive shot") {}
};

class ScareBeastTrigger : public HasCcTargetTrigger
{
public:
    ScareBeastTrigger(PlayerbotAI* botAI) : HasCcTargetTrigger(botAI, "scare beast") {}
};

class SilencingShotTrigger : public InterruptSpellTrigger
{
public:
    SilencingShotTrigger(PlayerbotAI* botAI) : InterruptSpellTrigger(botAI, "silencing shot") {}
};

// DoT/Debuff Triggers

class HuntersMarkTrigger : public DebuffTrigger
{
public:
    HuntersMarkTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "hunter's mark", 1, true, 0.5f) {}
    bool IsActive() override { return BuffTrigger::IsActive(); }
};

class ExplosiveShotTrigger : public DebuffTrigger
{
public:
    ExplosiveShotTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "explosive shot", 1, true) {}
    bool IsActive() override { return BuffTrigger::IsActive(); }
};

class BlackArrowTrigger : public DebuffTrigger
{
public:
    BlackArrowTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "black arrow", 1, true) {}
    bool IsActive() override;
};

class HunterNoStingsActiveTrigger : public DebuffTrigger
{
public:
    HunterNoStingsActiveTrigger(PlayerbotAI* botAI) : DebuffTrigger(botAI, "no stings") {}
    bool IsActive() override;
};

class SerpentStingOnAttackerTrigger : public DebuffOnAttackerTrigger
{
public:
    SerpentStingOnAttackerTrigger(PlayerbotAI* botAI) : DebuffOnAttackerTrigger(botAI, "serpent sting", true) {}
    bool IsActive() override;
};

// Damage/Combat Triggers

class AutoShotTrigger : public Trigger
{
public:
    AutoShotTrigger(PlayerbotAI* botAI) : Trigger(botAI, "auto shot") {}
};

class SwitchToRangedTrigger : public Trigger
{
public:
    SwitchToRangedTrigger(PlayerbotAI* botAI) : Trigger(botAI, "switch to ranged") {}

    bool IsActive() override;
};

class SwitchToMeleeTrigger : public Trigger
{
public:
    SwitchToMeleeTrigger(PlayerbotAI* botAI) : Trigger(botAI, "switch to melee") {}

    bool IsActive() override;
};

class MisdirectionOnMainTankTrigger : public BuffOnMainTankTrigger
{
public:
    MisdirectionOnMainTankTrigger(PlayerbotAI* botAI) : BuffOnMainTankTrigger(botAI, "misdirection", true) {}
};

class TargetRemoveEnrageTrigger : public TargetAuraDispelTrigger
{
public:
    TargetRemoveEnrageTrigger(PlayerbotAI* botAI) : TargetAuraDispelTrigger(botAI, "tranquilizing shot", DISPEL_ENRAGE) {}
};

class TargetRemoveMagicTrigger : public TargetAuraDispelTrigger
{
public:
    TargetRemoveMagicTrigger(PlayerbotAI* botAI) : TargetAuraDispelTrigger(botAI, "tranquilizing shot", DISPEL_MAGIC) {}
};

class ImmolationTrapNoCdTrigger : public SpellNoCooldownTrigger
{
public:
    ImmolationTrapNoCdTrigger(PlayerbotAI* botAI) : SpellNoCooldownTrigger(botAI, "immolation trap") {}
};

BEGIN_TRIGGER(HuntersPetDeadTrigger, Trigger)
END_TRIGGER()

BEGIN_TRIGGER(HuntersPetLowHealthTrigger, Trigger)
END_TRIGGER()

BEGIN_TRIGGER(HuntersPetMediumHealthTrigger, Trigger)
END_TRIGGER()

BEGIN_TRIGGER(HunterPetNotHappy, Trigger)
END_TRIGGER()

class VolleyChannelCheckTrigger : public Trigger
{
public:
    VolleyChannelCheckTrigger(PlayerbotAI* botAI, uint32 minEnemies = 2)
        : Trigger(botAI, "volley channel check"), minEnemies(minEnemies)
    {
    }

    bool IsActive() override;

protected:
    uint32 minEnemies;
    static const std::set<uint32> VOLLEY_SPELL_IDS;
};

#endif
