/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "PaladinTriggers.h"

#include "GenericBuffUtils.h"
#include "PaladinGreaterBlessingAction.h"
#include "PaladinActions.h"
#include "PaladinHelper.h"
#include "Playerbots.h"

bool SealTrigger::IsActive()
{
    // ShatterCore (4.3.4): Cataclysm seal set is Truth / Righteousness / Insight / Justice (the WotLK
    // Command/Vengeance/Corruption/Wisdom/Light seals were removed). Fire when no current seal is up.
    Unit* target = GetTarget();
    return !botAI->HasAura("seal of truth", target) && !botAI->HasAura("seal of righteousness", target) &&
           !botAI->HasAura("seal of insight", target) && !botAI->HasAura("seal of justice", target);
}

bool HolyPowerAvailableTrigger::IsActive()
{
    return AI_VALUE2(uint8, "holy power", "self target") >= amount;
}

bool InquisitionTrigger::IsActive()
{
    // Only (re)cast Inquisition when there is Holy Power to spend, so the bot doesn't waste a failed cast
    // attempt every tick at 0 Holy Power; BuffTrigger::IsActive() handles the "buff missing/expiring" check.
    return AI_VALUE2(uint8, "holy power", "self target") >= 1 && BuffTrigger::IsActive();
}

bool CrusaderAuraTrigger::IsActive()
{
    Unit* target = GetTarget();
    return AI_VALUE2(bool, "mounted", "self target") && !botAI->HasAura("crusader aura", target);
}

bool BlessingTrigger::IsActive()
{
    Unit* target = GetTarget();
    return SpellTrigger::IsActive() &&
           !botAI->HasAnyAuraOf(target, "blessing of might", "blessing of wisdom",
                                "blessing of kings", "blessing of sanctuary", nullptr);
}

bool DivineShieldLowHealthTrigger::IsActive()
{
    return botAI->HasAura("divine shield", bot) && AI_VALUE2(uint8, "health", "self target") < 80;
}

Unit* HandOfFreedomOnPartyTrigger::GetTarget()
{
    bool const selfImpaired = botAI->IsMovementImpaired(bot);
    bool const hasSelfHand = selfImpaired && ai::paladin::HasAnyPaladinHandFromCaster(bot, bot);

    if (!bot->GetGroup())
    {
        if (selfImpaired && !hasSelfHand)
            return bot;

        return nullptr;
    }

    if (selfImpaired && !hasSelfHand)
        return bot;

    return Trigger::GetTarget();
}

bool HandOfFreedomOnPartyTrigger::IsActive()
{
    Unit* target = GetTarget();
    if (!target)
        return false;

    if (target != bot &&
        bot->GetExactDist2dSq(target->GetPositionX(), target->GetPositionY()) > 30.0f * 30.0f)
        return false;

    if (!botAI->CanCastSpell("hand of freedom", target))
        return false;

    return !ai::paladin::HasAnyPaladinHandFromCaster(target, bot) && botAI->IsMovementImpaired(target);
}

bool NotSensingUndeadTrigger::IsActive()
{
    return !botAI->HasAura("sense undead", bot);
}

bool GreaterBlessingNeededTrigger::IsActive()
{
    if (!ai::gbless::IsEligibleGroupForAutoBlessings(bot->GetGroup()))
        return false;

    if (ai::buff::ShouldDeferGreaterBlessingAssignmentForRecentLogin(bot))
        return false;

    Group* group = bot->GetGroup();
    uint32 const groupKey = group ? group->GetLeaderGUID().GetCounter() : 0;

    Value<ai::gbless::CachedPendingBlessingAssignment>* pendingValue =
        context->GetValue<ai::gbless::CachedPendingBlessingAssignment>("greater blessing pending assignment");
    if (!pendingValue)
        return false;

    ai::gbless::CachedPendingBlessingAssignment pendingAssignment = pendingValue->Get();
    if (pendingAssignment.groupKey != groupKey)
    {
        pendingValue->Reset();
        pendingAssignment = pendingValue->Get();
    }

    return pendingAssignment.valid && pendingAssignment.groupKey == groupKey;
}
