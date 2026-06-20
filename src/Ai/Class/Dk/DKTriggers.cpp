/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "DKTriggers.h"

#include <string>

#include "GenericTriggers.h"
#include "Playerbots.h"
#include "SharedDefines.h"

bool DKPresenceTrigger::IsActive()
{
    Unit* target = GetTarget();
    return !botAI->HasAura("blood presence", target) && !botAI->HasAura("unholy presence", target) &&
           !botAI->HasAura("frost presence", target);
}

bool PestilenceGlyphTrigger::IsActive()
{
    if (!SpellTrigger::IsActive())
    {
        return false;
    }
    if (!bot->HasAura(63334))
    {
        return false;
    }
    Aura* blood_plague = botAI->GetAura("blood plague", GetTarget(), true, true);
    Aura* frost_fever = botAI->GetAura("frost fever", GetTarget(), true, true);
    if ((blood_plague && blood_plague->GetDuration() <= 3000) || (frost_fever && frost_fever->GetDuration() <= 3000))
    {
        return true;
    }
    return false;
}

// Based on runeSlotTypes
bool HighBloodRuneTrigger::IsActive()
{
    return bot->GetRuneCooldown(0) <= 2000 && bot->GetRuneCooldown(1) <= 2000;
}

bool HighFrostRuneTrigger::IsActive()
{
    return bot->GetRuneCooldown(4) <= 2000 && bot->GetRuneCooldown(5) <= 2000;
}

bool HighUnholyRuneTrigger::IsActive()
{
    return bot->GetRuneCooldown(2) <= 2000 && bot->GetRuneCooldown(3) <= 2000;
}

bool NoRuneTrigger::IsActive()
{
    for (uint32 i = 0; i < MAX_RUNES; ++i)
    {
        if (!bot->GetRuneCooldown(i))
            return false;
    }
    return true;
}

bool DesolationTrigger::IsActive()
{
    return bot->HasAura(66817) && BuffTrigger::IsActive();
}

bool DeathAndDecayCooldownTrigger::IsActive()
{
    uint32 spellId = AI_VALUE2(uint32, "spell id", name);
    if (!spellId)
        return true;

    return bot->GetSpellCooldownDelay(spellId) >= 2000;
}

bool OutbreakTrigger::IsActive()
{
    // Outbreak must be off cooldown.
    uint32 spellId = AI_VALUE2(uint32, "spell id", "outbreak");
    if (!spellId)
        return false;
    if (bot->GetSpellCooldownDelay(spellId))
        return false;

    Unit* target = GetTarget();
    if (!target)
        return false;

    Aura* bloodPlague = botAI->GetAura("blood plague", target, true, true);
    Aura* frostFever = botAI->GetAura("frost fever", target, true, true);
    // Apply when a disease is missing or about to fall off (<= 3s).
    bool bloodMissing = !bloodPlague || bloodPlague->GetDuration() <= 3000;
    bool frostMissing = !frostFever || frostFever->GetDuration() <= 3000;
    return bloodMissing || frostMissing;
}

bool DarkTransformationTrigger::IsActive()
{
    Pet* pet = bot->GetPet();
    if (!pet)
        return false;
    // Dark Transformation requires 5 stacks of Shadow Infusion on the ghoul.
    Aura* infusion = pet->GetAura(91342);  // Shadow Infusion
    if (!infusion || infusion->GetStackAmount() < 5)
        return false;
    // Must be off cooldown / castable.
    uint32 spellId = AI_VALUE2(uint32, "spell id", "dark transformation");
    if (!spellId)
        return false;
    return !bot->GetSpellCooldownDelay(spellId);
}
