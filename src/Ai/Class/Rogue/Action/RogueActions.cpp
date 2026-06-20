/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "RogueActions.h"

#include "Event.h"
#include "ObjectGuid.h"
#include "Player.h"
#include "PlayerbotAIConfig.h"
#include "Playerbots.h"

bool CastStealthAction::isUseful()
{
    Unit* target = AI_VALUE(Unit*, "current target");
    if (target && bot->GetDistance(target) >= sPlayerbotAIConfig.spellDistance)
        return false;
    return true;
}

bool CastStealthAction::isPossible()
{
    // do not use with WSG flag or EYE flag
    return !botAI->HasAura(23333, bot) && !botAI->HasAura(23335, bot) && !botAI->HasAura(34976, bot);
}

bool UnstealthAction::Execute(Event /*event*/)
{
    botAI->RemoveAura("stealth");
    // botAI->ChangeStrategy("+dps,-stealthed", BOT_STATE_COMBAT);

    return true;
}

bool CheckStealthAction::Execute(Event /*event*/)
{
    if (botAI->HasAura("stealth", bot))
    {
        botAI->ChangeStrategy("-dps,+stealthed", BOT_STATE_COMBAT);
    }
    else
    {
        botAI->ChangeStrategy("+dps,-stealthed", BOT_STATE_COMBAT);
    }

    return true;
}

bool CastVanishAction::isUseful()
{
    // do not use with WSG flag or EYE flag
    return !botAI->HasAura(23333, bot) && !botAI->HasAura(23335, bot) && !botAI->HasAura(34976, bot);
}

bool CastEnvenomAction::isUseful()
{
    return AI_VALUE2(uint8, "energy", "self target") >= 35;
}

bool CastEnvenomAction::isPossible()
{
    // Envenom (32645) is the core Assassination finisher; it only requires the bot to know the spell, have combo
    // points, and (for full benefit) Deadly Poison on the target. Do NOT gate it on the Master Poisoner talent
    // (58410) -- that is an optional Assassination talent, and gating on it silently disables the finisher for any
    // bot that did not spend that point or is below the talent level. Defer to the base spell-cast validation.
    return CastMeleeSpellAction::isPossible();
}

bool CastTricksOfTheTradeOnMainTankAction::isUseful()
{
    return CastSpellAction::isUseful() && AI_VALUE2(float, "distance", GetTargetName()) < 20.0f;
}

bool UseDeadlyPoisonAction::Execute(Event /*event*/)
{
    std::vector<std::string> poison_suffixs = {" IX", " VIII", " VII", " VI", " V", " IV", " III", " II", ""};
    std::vector<Item*> items;
    std::string poison_name;
    for (std::string& suffix : poison_suffixs)
    {
        poison_name = "Deadly Poison" + suffix;
        items = AI_VALUE2(std::vector<Item*>, "inventory items", poison_name);
        if (!items.empty())
        {
            break;
        }
    }
    if (items.empty())
    {
        return false;
    }
    Item* const itemForSpell = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
    return UseItem(*items.begin(), ObjectGuid::Empty, itemForSpell);
    // return UseItemAuto(*items.begin());
}

bool UseDeadlyPoisonAction::isPossible()
{
    std::vector<std::string> poison_suffixs = {" IX", " VIII", " VII", " VI", " V", " IV", " III", " II", ""};
    std::vector<Item*> items;
    std::string poison_name;
    for (std::string& suffix : poison_suffixs)
    {
        poison_name = "Deadly Poison" + suffix;
        items = AI_VALUE2(std::vector<Item*>, "inventory items", poison_name);
        if (!items.empty())
        {
            break;
        }
    }
    return !items.empty();
}

bool UseInstantPoisonAction::Execute(Event /*event*/)
{
    std::vector<std::string> poison_suffixs = {" IX", " VIII", " VII", " VI", " V", " IV", " III", " II", ""};
    std::vector<Item*> items;
    std::string poison_name;
    for (std::string& suffix : poison_suffixs)
    {
        poison_name = "Instant Poison" + suffix;
        items = AI_VALUE2(std::vector<Item*>, "inventory items", poison_name);
        if (!items.empty())
        {
            break;
        }
    }
    if (items.empty())
    {
        return false;
    }
    Item* const itemForSpell = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
    return UseItem(*items.begin(), ObjectGuid::Empty, itemForSpell);
}

bool UseInstantPoisonAction::isPossible()
{
    std::vector<std::string> poison_suffixs = {" IX", " VIII", " VII", " VI", " V", " IV", " III", " II", ""};
    std::vector<Item*> items;
    std::string poison_name;
    for (std::string& suffix : poison_suffixs)
    {
        poison_name = "Instant Poison" + suffix;
        items = AI_VALUE2(std::vector<Item*>, "inventory items", poison_name);
        if (!items.empty())
        {
            break;
        }
    }
    return !items.empty();
}

bool UseInstantPoisonOffHandAction::Execute(Event /*event*/)
{
    std::vector<std::string> poison_suffixs = {" IX", " VIII", " VII", " VI", " V", " IV", " III", " II", ""};
    std::vector<Item*> items;
    std::string poison_name;
    for (std::string& suffix : poison_suffixs)
    {
        poison_name = "Instant Poison" + suffix;
        items = AI_VALUE2(std::vector<Item*>, "inventory items", poison_name);
        if (!items.empty())
        {
            break;
        }
    }
    if (items.empty())
    {
        return false;
    }
    Item* const itemForSpell = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
    return UseItem(*items.begin(), ObjectGuid::Empty, itemForSpell);
}

bool UseInstantPoisonOffHandAction::isPossible()
{
    std::vector<std::string> poison_suffixs = {" IX", " VIII", " VII", " VI", " V", " IV", " III", " II", ""};
    std::vector<Item*> items;
    std::string poison_name;
    for (std::string& suffix : poison_suffixs)
    {
        poison_name = "Instant Poison" + suffix;
        items = AI_VALUE2(std::vector<Item*>, "inventory items", poison_name);
        if (!items.empty())
        {
            break;
        }
    }
    return !items.empty();
}
