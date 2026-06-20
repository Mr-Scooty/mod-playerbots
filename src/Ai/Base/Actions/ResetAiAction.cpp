/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "ResetAiAction.h"

#include "Event.h"
#include "Group.h"
#include "ObjectGuid.h"
#include "PlayerbotRepository.h"
#include "Playerbots.h"
#include "WorldPacket.h"

bool ResetAiAction::Execute(Event event)
{
    if (!event.getPacket().empty())
    {
        WorldPacket packet = event.getPacket();
        if (packet.GetOpcode() == SMSG_PARTY_UPDATE)
        {
            // 4.3.4: SMSG_PARTY_UPDATE has a different (bit-packed) layout - query
            // the group state directly instead of parsing the packet
            if (bot->GetGroup() && bot->GetGroup()->GetMembersCount() != 0)
                return false;
        }
    }
    if (Player* master = botAI->GetMaster())
    {
        Group* botGroup = bot->GetGroup();
        Group* masterGroup = master->GetGroup();
        if (botGroup && (!masterGroup || masterGroup != botGroup))
            botAI->SetMaster(nullptr);
    }
    if (sRandomPlayerbotMgr.IsRandomBot(bot) && !bot->InBattleground())
    {
        if (bot->GetGroup() && (!botAI->GetMaster() || GET_PLAYERBOT_AI(botAI->GetMaster())))
        {
            if (Player* newMaster = botAI->FindNewMaster())
                botAI->SetMaster(newMaster);
        }
    }
    PlayerbotRepository::instance().Reset(botAI);
    botAI->ResetStrategies(false);
    botAI->TellMaster("AI was reset to defaults");
    return true;
}
