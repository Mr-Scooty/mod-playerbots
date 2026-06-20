/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "AcceptInvitationAction.h"

#include "Event.h"
#include "ObjectAccessor.h"
#include "PlayerbotAIConfig.h"
#include "PlayerbotSecurity.h"
#include "PlayerbotTextMgr.h"
#include "Playerbots.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "PartyPackets.h"

bool AcceptInvitationAction::Execute(Event event)
{
    Group* grp = bot->GetGroupInvite();
    if (!grp)
        return false;
    WorldPacket packet = event.getPacket();
    uint8 flag;
    std::string name;
    packet >> flag >> name;

    Player* inviter = ObjectAccessor::FindPlayer(grp->GetLeaderGUID());
    if (!inviter)
        return false;

    if (!botAI->GetSecurity()->CheckLevelFor(PLAYERBOT_SECURITY_INVITE, false, inviter))
    {
        WorldPacket data(SMSG_GROUP_DECLINE, 10);
        data << bot->GetName();
        inviter->SendDirectMessage(&data);
        bot->UninviteFromGroup();
        return false;
    }

    if (bot->isAFK())
        bot->ToggleAFK();

    // 4.3.4: group invites are answered through the typed PartyInviteResponse handler
    WorldPackets::Party::PartyInviteResponse response{WorldPacket(CMSG_PARTY_INVITE_RESPONSE)};
    response.Accept = true;
    bot->GetSession()->HandlePartyInviteResponseOpcode(response);

    if (!bot->GetGroup() || !bot->GetGroup()->IsMember(inviter->GetGUID()))
        return false;

    if (sRandomPlayerbotMgr.IsRandomBot(bot))
        botAI->SetMaster(inviter);
    // else
    // PlayerbotRepository::instance().Save(botAI);

    botAI->ResetStrategies();
    botAI->ChangeStrategy("+follow,-lfg,-bg", BOT_STATE_NON_COMBAT);
    botAI->Reset();

    botAI->TellMaster(PlayerbotTextMgr::instance().GetBotTextOrDefault("hello", "Hello", {}));

    if (sPlayerbotAIConfig.summonWhenGroup && bot->GetDistance(inviter) > sPlayerbotAIConfig.sightDistance)
    {
        Teleport(inviter, bot, true);
    }
    return true;
}
