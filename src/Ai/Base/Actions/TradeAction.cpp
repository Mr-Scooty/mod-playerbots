/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "TradeAction.h"
#include "ChatHelper.h"
#include "Event.h"
#include "ItemCountValue.h"
#include "ItemVisitors.h"
#include "PlayerbotAI.h"
#include "WorldSession.h"
#include "TradeData.h"
#include "ObjectAccessor.h"
#include "TradePackets.h"

bool TradeAction::Execute(Event event)
{
    std::string const text = event.getParam();

    // If text starts with any excluded prefix, don't process it further.
    for (auto const& prefix : sPlayerbotAIConfig.tradeActionExcludedPrefixes)
    {
        if (text.find(prefix) == 0)
            return false;
    }

    if (!bot->GetTrader())
    {
        GuidVector guids = chat->parseGameobjects(text);
        Player* player = nullptr;

        for (auto& guid : guids)
            if (guid.IsPlayer())
                player = ObjectAccessor::FindPlayer(guid);

        if (!player && botAI->GetMaster())
            player = botAI->GetMaster();

        if (!player)
            return false;

        if (!player->GetTrader())
        {
            WorldPackets::Trade::InitiateTrade initPacket{WorldPacket(CMSG_INITIATE_TRADE)};
            initPacket.Guid = player->GetGUID();
            bot->GetSession()->HandleInitiateTradeOpcode(initPacket);
            return true;
        }
        else if (player->GetTrader() != bot)
            return false;
    }

    uint32 copper = chat->parseMoney(text);
    if (copper > 0)
    {
        WorldPackets::Trade::SetTradeGold goldPacket{WorldPacket(CMSG_SET_TRADE_GOLD)};
        goldPacket.Coinage = copper;
        bot->GetSession()->HandleSetTradeGoldOpcode(goldPacket);
    }

    size_t pos = text.rfind(" ");
    int count = pos != std::string::npos ? atoi(text.substr(pos + 1).c_str()) : 1;

    std::vector<Item*> found = parseItems(text);
    if (found.empty())
        return false;

    uint32 traded = 0;
    for (Item* item : found)
    {
        if (!bot->GetTrader() || item->IsInTrade())
            continue;

        int8 slot = item->CanBeTraded() ? -1 : TRADE_SLOT_NONTRADED;
        if (TradeItem(item, slot) && slot != TRADE_SLOT_NONTRADED && ++traded >= count)
            break;
    }

    return true;
}

bool TradeAction::TradeItem(Item const* item, int8 slot)
{
    int8 tradeSlot = -1;
    Item* itemPtr = const_cast<Item*>(item);

    TradeData* pTrade = bot->GetTradeData();
    if ((slot >= 0 && slot < TRADE_SLOT_COUNT) && pTrade->GetItem(TradeSlots(slot)) == nullptr)
        tradeSlot = slot;

    if (slot == TRADE_SLOT_NONTRADED)
        pTrade->SetItem(TRADE_SLOT_NONTRADED, itemPtr);
    else
    {
        for (uint8 i = 0; i < TRADE_SLOT_TRADED_COUNT && tradeSlot == -1; i++)
        {
            if (pTrade->GetItem(TradeSlots(i)) == itemPtr)
            {
                tradeSlot = i;

                WorldPackets::Trade::ClearTradeItem clearPacket{WorldPacket(CMSG_CLEAR_TRADE_ITEM)};
                clearPacket.TradeSlot = uint8(tradeSlot);
                bot->GetSession()->HandleClearTradeItemOpcode(clearPacket);
                pTrade->SetItem(TradeSlots(i), nullptr);
                return true;
            }
        }

        for (uint8 i = 0; i < TRADE_SLOT_TRADED_COUNT && tradeSlot == -1; i++)
        {
            if (pTrade->GetItem(TradeSlots(i)) == nullptr)
                tradeSlot = i;
        }
    }

    if (tradeSlot == -1)
        return false;

    WorldPackets::Trade::SetTradeItem setPacket{WorldPacket(CMSG_SET_TRADE_ITEM)};
    setPacket.TradeSlot = uint8(tradeSlot);
    setPacket.PackSlot = uint8(item->GetBagSlot());
    setPacket.ItemSlotInPack = uint8(item->GetSlot());
    bot->GetSession()->HandleSetTradeItemOpcode(setPacket);
    return true;
}
