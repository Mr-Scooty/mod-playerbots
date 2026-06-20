/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#ifndef _PLAYERBOT_STATSVALUE_H
#define _PLAYERBOT_STATSVALUE_H

#include "NamedObjectContext.h"
#include "Value.h"

class PlayerbotAI;
class Unit;

class HealthValue : public Uint8CalculatedValue, public Qualified
{
public:
    HealthValue(PlayerbotAI* botAI, std::string const name = "health") : Uint8CalculatedValue(botAI, name) {}

    Unit* GetTarget();
    uint8 Calculate() override;
};

class IsDeadValue : public BoolCalculatedValue, public Qualified
{
public:
    IsDeadValue(PlayerbotAI* botAI, std::string const name = "dead") : BoolCalculatedValue(botAI, name) {}

    Unit* GetTarget();
    bool Calculate() override;
};

class PetIsDeadValue : public BoolCalculatedValue
{
public:
    PetIsDeadValue(PlayerbotAI* botAI, std::string const name = "pet dead") : BoolCalculatedValue(botAI, name) {}

    bool Calculate() override;
};

class PetIsHappyValue : public BoolCalculatedValue
{
public:
    PetIsHappyValue(PlayerbotAI* botAI, std::string const name = "pet happy") : BoolCalculatedValue(botAI, name) {}

    bool Calculate() override;
};

class RageValue : public Uint8CalculatedValue, public Qualified
{
public:
    RageValue(PlayerbotAI* botAI, std::string const name = "rage") : Uint8CalculatedValue(botAI, name) {}

    Unit* GetTarget();
    uint8 Calculate() override;
};

class EnergyValue : public Uint8CalculatedValue, public Qualified
{
public:
    EnergyValue(PlayerbotAI* botAI, std::string const name = "energy") : Uint8CalculatedValue(botAI, name) {}

    Unit* GetTarget();
    uint8 Calculate() override;
};

// ShatterCore: Holy Power (Paladin, 4.3.4) -- 0..3 charges read from POWER_HOLY_POWER.
class HolyPowerValue : public Uint8CalculatedValue, public Qualified
{
public:
    HolyPowerValue(PlayerbotAI* botAI, std::string const name = "holy power") : Uint8CalculatedValue(botAI, name) {}

    Unit* GetTarget();
    uint8 Calculate() override;
};

// ShatterCore (4.3.4): Cataclysm class resources read directly from their POWER_* type. (Shadow Orbs, Eclipse
// and DK Runes are special-cased per spec -- aura stacks / a signed power / per-rune counts, not a plain pool --
// so those are added in the owning class's own value files.)
class FocusValue : public Uint8CalculatedValue, public Qualified  // Hunter (replaces mana in Cata)
{
public:
    FocusValue(PlayerbotAI* botAI, std::string const name = "focus") : Uint8CalculatedValue(botAI, name) {}

    Unit* GetTarget();
    uint8 Calculate() override;
};

class SoulShardsValue : public Uint8CalculatedValue, public Qualified  // Warlock
{
public:
    SoulShardsValue(PlayerbotAI* botAI, std::string const name = "soul shards") : Uint8CalculatedValue(botAI, name) {}

    Unit* GetTarget();
    uint8 Calculate() override;
};

class RunicPowerValue : public Uint8CalculatedValue, public Qualified  // Death Knight (0..130)
{
public:
    RunicPowerValue(PlayerbotAI* botAI, std::string const name = "runic power") : Uint8CalculatedValue(botAI, name) {}

    Unit* GetTarget();
    uint8 Calculate() override;
};

class ManaValue : public Uint8CalculatedValue, public Qualified
{
public:
    ManaValue(PlayerbotAI* botAI, std::string const name = "mana") : Uint8CalculatedValue(botAI, name) {}

    Unit* GetTarget();
    uint8 Calculate() override;
};

class HasManaValue : public BoolCalculatedValue, public Qualified
{
public:
    HasManaValue(PlayerbotAI* botAI, std::string const name = "has mana") : BoolCalculatedValue(botAI, name, 2 * 1000)
    {
    }

    Unit* GetTarget();
    bool Calculate() override;
};

class ComboPointsValue : public Uint8CalculatedValue, public Qualified
{
public:
    ComboPointsValue(PlayerbotAI* botAI, std::string const name = "combo points") : Uint8CalculatedValue(botAI, name) {}

    Unit* GetTarget();
    uint8 Calculate() override;
};

class IsMountedValue : public BoolCalculatedValue, public Qualified
{
public:
    IsMountedValue(PlayerbotAI* botAI, std::string const name = "mounted") : BoolCalculatedValue(botAI, name) {}

    Unit* GetTarget();
    bool Calculate() override;
};

class IsInCombatValue : public MemoryCalculatedValue<bool>, public Qualified
{
public:
    IsInCombatValue(PlayerbotAI* botAI, std::string const name = "combat") : MemoryCalculatedValue(botAI, name) {}

    Unit* GetTarget();
    bool Calculate() override;
    bool EqualToLast(bool value) override;
};

class BagSpaceValue : public Uint8CalculatedValue
{
public:
    BagSpaceValue(PlayerbotAI* botAI, std::string const name = "bag space") : Uint8CalculatedValue(botAI, name) {}

    uint8 Calculate() override;
};

class DurabilityValue : public Uint8CalculatedValue
{
public:
    DurabilityValue(PlayerbotAI* botAI, std::string const name = "durability") : Uint8CalculatedValue(botAI, name) {}

    uint8 Calculate() override;
};

class SpeedValue : public Uint8CalculatedValue, public Qualified
{
public:
    SpeedValue(PlayerbotAI* botAI, std::string const name = "speed") : Uint8CalculatedValue(botAI, name) {}

    Unit* GetTarget();
    uint8 Calculate() override;
};

class IsInGroupValue : public BoolCalculatedValue
{
public:
    IsInGroupValue(PlayerbotAI* botAI, std::string const name = "in group") : BoolCalculatedValue(botAI, name) {}

    bool Calculate() override;
};

class DeathCountValue : public ManualSetValue<uint32>
{
public:
    DeathCountValue(PlayerbotAI* botAI, std::string const name = "death count") : ManualSetValue<uint32>(botAI, 0, name)
    {
    }
};

class ExperienceValue : public MemoryCalculatedValue<uint32>
{
public:
    ExperienceValue(PlayerbotAI* botAI, std::string const name = "experience", uint32 checkInterval = 60)
        : MemoryCalculatedValue<uint32>(botAI, name, checkInterval)
    {
    }

    bool EqualToLast(uint32 value) override;
    uint32 Calculate() override;
};

#endif
