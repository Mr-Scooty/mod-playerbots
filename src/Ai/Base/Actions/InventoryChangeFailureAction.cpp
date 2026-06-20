/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "InventoryChangeFailureAction.h"

#include "Event.h"
#include "Playerbots.h"

std::map<InventoryResult, std::string> InventoryChangeFailureAction::messages;

bool InventoryChangeFailureAction::Execute(Event event)
{
    if (!botAI->GetMaster())
        return false;

    if (messages.empty())
    {
        messages[EQUIP_ERR_CANT_EQUIP_LEVEL_I] = "My level is too low";
        messages[EQUIP_ERR_CANT_EQUIP_SKILL] = "My skill level is too low";
        messages[EQUIP_ERR_WRONG_SLOT] = "Invalid slot";
        messages[EQUIP_ERR_BAG_FULL] = "My bags are full";
        messages[EQUIP_ERR_BAG_IN_BAG] = "This bag is not empty";
        messages[EQUIP_ERR_TRADE_EQUIPPED_BAG] = "Cannot trade equipped bags";
        messages[EQUIP_ERR_AMMO_ONLY] = "Invalid slot (only ammo is required)";
        messages[EQUIP_ERR_PROFICIENCY_NEEDED] = "I don't have necessary skill";
        messages[EQUIP_ERR_NO_SLOT_AVAILABLE] = "No equipment slot available";
        messages[EQUIP_ERR_CANT_EQUIP_EVER] = "I will never be able to use this";
        messages[EQUIP_ERR_CANT_EQUIP_EVER_2] = "I will never be able to use this";
        messages[EQUIP_ERR_NO_SLOT_AVAILABLE_2] = messages[EQUIP_ERR_NO_SLOT_AVAILABLE];
        messages[EQUIP_ERR_2HANDED_EQUIPPED] = "Cannot equip with two-handed weapon equipped";
        messages[EQUIP_ERR_2HSKILLNOTFOUND] = "I cannot dual-wield";
        messages[EQUIP_ERR_WRONG_BAG_TYPE] = "This item cannot go in this bag";
        messages[EQUIP_ERR_WRONG_BAG_TYPE_2] = "This item cannot go in this bag";
        messages[EQUIP_ERR_ITEM_MAX_COUNT] = "I can't carry anymore of those";
        messages[EQUIP_ERR_NO_SLOT_AVAILABLE_3] = messages[EQUIP_ERR_NO_SLOT_AVAILABLE];
        messages[EQUIP_ERR_CANT_STACK] = "Item cannot stack";
        messages[EQUIP_ERR_NOT_EQUIPPABLE] = "Item cannot be equipped";
        messages[EQUIP_ERR_CANT_SWAP] = "Cannot swap these items";
        messages[EQUIP_ERR_SLOT_EMPTY] = "Nothing to equip";
        messages[EQUIP_ERR_ITEM_NOT_FOUND] = "Cannot find the item";
        messages[EQUIP_ERR_DROP_BOUND_ITEM] = "Cannot drop soulbound items";
        messages[EQUIP_ERR_OUT_OF_RANGE] = "I am out of range";
        messages[EQUIP_ERR_TOO_FEW_TO_SPLIT] = "Invalid split number";
        messages[EQUIP_ERR_SPLIT_FAILED] = "Cannot split this";
        messages[EQUIP_ERR_SPELL_FAILED_REAGENTS_GENERIC] = "Missing reagents";
        messages[EQUIP_ERR_NOT_ENOUGH_MONEY] = "Not enough money";
        messages[EQUIP_ERR_NOT_A_BAG] = "This is not a bag";
        messages[EQUIP_ERR_DESTROY_NONEMPTY_BAG] = "The bag is not empty";
        messages[EQUIP_ERR_NOT_OWNER] = "This is not my item";
        messages[EQUIP_ERR_ONLY_ONE_QUIVER] = "Only quiver can be equipped";
        messages[EQUIP_ERR_NO_BANK_SLOT] = "I must purchase the slot before";
        messages[EQUIP_ERR_NO_BANK_HERE] = "I am too far away from bank";
        messages[EQUIP_ERR_ITEM_LOCKED] = "This item is locked";
        messages[EQUIP_ERR_GENERIC_STUNNED] = "I am stunned";
        messages[EQUIP_ERR_PLAYER_DEAD] = "I am dead";
        messages[EQUIP_ERR_CLIENT_LOCKED_OUT] = "I can't do this right now";
        messages[EQUIP_ERR_INTERNAL_BAG_ERROR] = "Internal error";
        messages[EQUIP_ERR_ONLY_ONE_BOLT] = "Only bolts are allowed";
        messages[EQUIP_ERR_ONLY_ONE_AMMO] = "Ammo poach is allowed";
        messages[EQUIP_ERR_CANT_WRAP_STACKABLE] = "Item can't be wrapped";
        messages[EQUIP_ERR_CANT_WRAP_EQUIPPED] = messages[EQUIP_ERR_CANT_WRAP_STACKABLE];
        messages[EQUIP_ERR_CANT_WRAP_WRAPPED] = messages[EQUIP_ERR_CANT_WRAP_STACKABLE];
        messages[EQUIP_ERR_CANT_WRAP_BOUND] = messages[EQUIP_ERR_CANT_WRAP_STACKABLE];
        messages[EQUIP_ERR_CANT_WRAP_UNIQUE] = messages[EQUIP_ERR_CANT_WRAP_STACKABLE];
        messages[EQUIP_ERR_CANT_WRAP_BAGS] = messages[EQUIP_ERR_CANT_WRAP_STACKABLE];
        messages[EQUIP_ERR_LOOT_GONE] = "Already looted";
        messages[EQUIP_ERR_INV_FULL] = "My inventory is full";
        messages[EQUIP_ERR_BANK_FULL] = "My bank is full";
        messages[EQUIP_ERR_VENDOR_SOLD_OUT] = "Item item is sold out";
        messages[EQUIP_ERR_BAG_FULL_3] = messages[EQUIP_ERR_BANK_FULL];
        messages[EQUIP_ERR_ITEM_NOT_FOUND_2] = messages[EQUIP_ERR_ITEM_NOT_FOUND];
        messages[EQUIP_ERR_CANT_STACK_2] = messages[EQUIP_ERR_CANT_STACK];
        messages[EQUIP_ERR_BAG_FULL_4] = messages[EQUIP_ERR_BAG_FULL];
        messages[EQUIP_ERR_VENDOR_SOLD_OUT_2] = messages[EQUIP_ERR_VENDOR_SOLD_OUT];
        messages[EQUIP_ERR_OBJECT_IS_BUSY] = "This object is busy";
        messages[EQUIP_ERR_NOT_IN_COMBAT] = "I am in combat";
        messages[EQUIP_ERR_NOT_WHILE_DISARMED] = "Cannot do while disarmed";
        messages[EQUIP_ERR_BAG_FULL_5] = messages[EQUIP_ERR_BAG_FULL];
        messages[EQUIP_ERR_CANT_EQUIP_RANK] = "Not enough rank";
        messages[EQUIP_ERR_CANT_EQUIP_REPUTATION] = "Not enough reputation";
        messages[EQUIP_ERR_TOO_MANY_SPECIAL_BAGS] = "Too many special bags";
        messages[EQUIP_ERR_LOOT_CANT_LOOT_THAT_NOW] = "Cannot loot this right now";
    }

    WorldPacket p(event.getPacket());
    p.rpos(0);
    uint8 err;
    p >> err;
    if (err == EQUIP_ERR_OK)
        return false;

    std::string const msg = messages[(InventoryResult)err];
    if (!msg.empty())
    {
        botAI->TellError(msg);
        return true;
    }

    return false;
}
