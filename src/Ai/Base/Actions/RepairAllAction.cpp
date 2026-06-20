/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "RepairAllAction.h"

#include "ChatHelper.h"
#include "Event.h"
#include "Playerbots.h"

bool RepairAllAction::Execute(Event /*event*/)
{
    GuidVector npcs = AI_VALUE(GuidVector, "nearest npcs");
    for (ObjectGuid const guid : npcs)
    {
        Creature* unit = bot->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_REPAIR);
        if (!unit)
            continue;

        if (bot->HasUnitState(UNIT_STATE_DIED))
            bot->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

        bot->SetFacingToObject(unit);
        float discountMod = bot->GetReputationPriceDiscount(unit);

        uint64 botMoney = bot->GetMoney();
        if (botAI->HasCheat(BotCheatMask::gold))
        {
            bot->SetMoney(10000000);
        }

        // 4.3.4: DurabilityRepair no longer reports the cost - track spent money instead.
        uint64 moneyBefore = bot->GetMoney();

        // Repair weapons first.
        bot->DurabilityRepair(uint16(EQUIPMENT_SLOT_MAINHAND | (INVENTORY_SLOT_BAG_0 << 8)), true, discountMod);
        bot->DurabilityRepair(uint16(EQUIPMENT_SLOT_RANGED | (INVENTORY_SLOT_BAG_0 << 8)), true, discountMod);
        bot->DurabilityRepair(uint16(EQUIPMENT_SLOT_OFFHAND | (INVENTORY_SLOT_BAG_0 << 8)), true, discountMod);

        bot->DurabilityRepairAll(true, discountMod, false);

        uint64 totalCost = moneyBefore > bot->GetMoney() ? moneyBefore - bot->GetMoney() : 0;

        if (botAI->HasCheat(BotCheatMask::gold))
        {
            bot->SetMoney(botMoney);
        }

        if (totalCost > 0)
        {
            std::ostringstream out;
            out << "Repair: " << chat->formatMoney(uint32(totalCost)) << " (" << unit->GetName() << ")";
            botAI->TellMasterNoFacing(out.str());

            bot->PlayDistanceSound(1116);
        }

        context->GetValue<uint32>("death count")->Set(0);

        return true;
    }

    botAI->TellError("Cannot find any npc to repair at");
    return false;
}
