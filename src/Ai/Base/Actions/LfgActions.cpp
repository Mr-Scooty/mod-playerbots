/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "LfgActions.h"

#include "AiFactory.h"
#include "ItemVisitors.h"
#include "LFGMgr.h"
#include "Opcodes.h"
#include "Playerbots.h"
#include "World.h"
#include "WorldPacket.h"
#include "RandomPlayerbotMgr.h"
#include "Map.h"
#include "WorldSession.h"
#include "Log.h"
#include "DBCStores.h"

using namespace lfg;

bool LfgJoinAction::Execute(Event event) { return JoinLFG(); }

uint32 LfgJoinAction::GetRoles()
{
    if (!RandomPlayerbotMgr::instance().IsRandomBot(bot))
    {
        if (botAI->IsTank(bot))
            return PLAYER_ROLE_TANK;
        if (botAI->IsHeal(bot))
            return PLAYER_ROLE_HEALER;
        else
            return PLAYER_ROLE_DAMAGE;
    }

    uint8 spec = AiFactory::GetPlayerSpecTab(bot);
    switch (bot->getClass())
    {
        case CLASS_DRUID:
            if (spec == 2)
                return PLAYER_ROLE_HEALER;
            else if (spec == 1 && bot->HasAura(16931) /* thick hide */)
                return PLAYER_ROLE_TANK;
            else
                return PLAYER_ROLE_DAMAGE;
            break;
        case CLASS_PALADIN:
            if (spec == 1)
                return PLAYER_ROLE_TANK;
            else if (!spec)
                return PLAYER_ROLE_HEALER;
            else
                return PLAYER_ROLE_DAMAGE;
            break;
        case CLASS_PRIEST:
            if (spec != 2)
                return PLAYER_ROLE_HEALER;
            else
                return PLAYER_ROLE_DAMAGE;
            break;
        case CLASS_SHAMAN:
            if (spec == 2)
                return PLAYER_ROLE_HEALER;
            else
                return PLAYER_ROLE_DAMAGE;
            break;
        case CLASS_WARRIOR:
            if (spec == 2)
                return PLAYER_ROLE_TANK;
            else
                return PLAYER_ROLE_DAMAGE;
            break;
        case CLASS_DEATH_KNIGHT:
            if (spec == 0)
                return PLAYER_ROLE_TANK;
            else
                return PLAYER_ROLE_DAMAGE;
            break;

        default:
            return PLAYER_ROLE_DAMAGE;
            break;
    }

    return PLAYER_ROLE_DAMAGE;
}

bool LfgJoinAction::JoinLFG()
{
    // check if already in lfg
    LfgState state = sLFGMgr->GetState(bot->GetGUID());
    if (state != LFG_STATE_NONE)
        return false;

    /*ItemCountByQuality visitor;
    IterateItems(&visitor, ITERATE_ITEMS_IN_EQUIP);
    bool random = urand(0, 100) < 20;
    bool heroic = urand(0, 100) < 50 &&
                  (visitor.count[ITEM_QUALITY_EPIC] >= 3 || visitor.count[ITEM_QUALITY_RARE] >= 10) &&
                  bot->getLevel() >= 70;
    bool rbotAId = !heroic && (urand(0, 100) < 50 && visitor.count[ITEM_QUALITY_EPIC] >= 5 &&
                               (bot->getLevel() == 60 || bot->getLevel() == 70 || bot->getLevel() == 80));*/

    LfgDungeonSet list;
    std::vector<uint32> selected;

    std::vector<uint32> dungeons = RandomPlayerbotMgr::instance().LfgDungeons[bot->GetTeamId()];
    if (!dungeons.size())
        return false;

    for (std::vector<uint32>::iterator i = dungeons.begin(); i != dungeons.end(); ++i)
    {
        LFGDungeonEntry const* dungeon = sLFGDungeonStore.LookupEntry(*i);
        if (!dungeon || (dungeon->TypeID != LFG_TYPE_RANDOM && dungeon->TypeID != LFG_TYPE_DUNGEON &&
                         dungeon->TypeID != LFG_TYPE_HEROIC && dungeon->TypeID != LFG_TYPE_RAID))
            continue;

        auto const& botLevel = bot->getLevel();

        /*LFG_TYPE_RANDOM on classic is 15-58 so bot over level 25 will never queue*/
        if (dungeon->MinLevel && (botLevel < dungeon->MinLevel || botLevel > dungeon->Maxlevel) ||
            (botLevel > dungeon->MinLevel + 10 && dungeon->TypeID == LFG_TYPE_DUNGEON))
            continue;

        selected.push_back(dungeon->ID);
        list.insert(dungeon->ID);
    }

    if (!selected.size())
        return false;

    if (list.empty())
        return false;

    bool many = list.size() > 1;
    LFGDungeonEntry const* dungeon = sLFGDungeonStore.LookupEntry(*list.begin());

    // check role for console msg
    std::string _roles = "multiple roles";
    uint32 roleMask = GetRoles();
    if (roleMask & PLAYER_ROLE_TANK)
        _roles = "TANK";

    if (roleMask & PLAYER_ROLE_HEALER)
        _roles = "HEAL";

    if (roleMask & PLAYER_ROLE_DAMAGE)
        _roles = "DPS";

    LOG_INFO("playerbots", "Bot {} {}:{} <{}>: queues LFG, Dungeon as {} ({})", bot->GetGUID().ToString().c_str(),
             bot->GetTeamId() == TEAM_ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName().c_str(), _roles,
             many ? "several dungeons" : dungeon->Name);

    // Set RbotAId Browser comment
    std::string const _gs = std::to_string(botAI->GetEquipGearScore(bot/*, false, false*/));

    // JoinLfg is not threadsafe, so make packet and queue into session
    // sLFGMgr->JoinLfg(bot, roleMask, list, _gs);

    // ShatterCore: Cataclysm 4.3.4 wire format — mirror WorldPackets::LFG::LFGJoin::Read
    WorldPacket* data = new WorldPacket(CMSG_LFG_JOIN);
    *data << (uint32)roleMask;
    for (uint8 i = 0; i < 3; ++i)
        *data << (uint32)0;             // Needs, count hardcoded to 3 in client
    data->WriteBits(_gs.length(), 9);   // comment length
    data->WriteBits(list.size(), 24);   // slot count
    data->FlushBits();
    data->WriteString(_gs);
    for (uint32 dungeonId : list)
        *data << (uint32)dungeonId;
    bot->GetSession()->QueuePacket(data);

    return true;
}

bool LfgRoleCheckAction::Execute(Event /*event*/)
{
    if (Group* group = bot->GetGroup())
    {
        uint32 currentRoles = sLFGMgr->GetRoles(bot->GetGUID());
        uint32 newRoles = GetRoles();
        // if (currentRoles == newRoles)
        //     return false;

        WorldPacket* packet = new WorldPacket(CMSG_LFG_SET_ROLES);
        *packet << (uint32)newRoles;  // ShatterCore: uint32 RolesDesired in Cataclysm (uint8 in WotLK)
        bot->GetSession()->QueuePacket(packet);
        // sLFGMgr->SetRoles(bot->GetGUID(), newRoles);
        // sLFGMgr->UpdateRoleCheck(group->GetGUID(), bot->GetGUID(), newRoles);

        LOG_INFO("playerbots", "Bot {} {}:{} <{}>: LFG roles checked", bot->GetGUID().ToString().c_str(),
                 bot->GetTeamId() == TEAM_ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName().c_str());

        return true;
    }

    return false;
}

// ShatterCore: Cataclysm 4.3.4 wire format — mirror WorldPackets::LFG::LFGProposalResponse::Read.
// The handler only uses ProposalID and Accepted, so the ride ticket and instance guid stay zeroed.
static WorldPacket* BuildLfgProposalResponsePacket(uint32 proposalId, bool accept)
{
    WorldPacket* packet = new WorldPacket(CMSG_LFG_PROPOSAL_RESULT);
    *packet << (uint32)proposalId;
    *packet << (uint32)0;         // Ticket.Time
    *packet << (uint32)0;         // Ticket.Type
    *packet << (uint32)0;         // Ticket.Id
    for (uint8 i = 0; i < 8; ++i)
        packet->WriteBit(0);      // Ticket.RequesterGuid bits (empty guid, no byte sequence)
    packet->WriteBit(0);          // instance guid [7]
    packet->WriteBit(accept);     // Accepted
    for (uint8 i = 0; i < 7; ++i)
        packet->WriteBit(0);      // instance guid [1][3][0][5][4][6][2]
    packet->FlushBits();
    return packet;
}

bool LfgAcceptAction::Execute(Event event)
{
    uint32 id = AI_VALUE(uint32, "lfg proposal");

    // Try accept if already stored
    if (id)
    {
        if (bot->IsInCombat() || bot->isDead())
        {
            bot->GetSession()->QueuePacket(BuildLfgProposalResponsePacket(id, false));
            return true;
        }

        botAI->GetAiObjectContext()->GetValue<uint32>("lfg proposal")->Set(0);
        bot->ClearUnitState(UNIT_STATE_ALL_STATE);

        bot->GetSession()->QueuePacket(BuildLfgProposalResponsePacket(id, true));

        if (RandomPlayerbotMgr::instance().IsRandomBot(bot) && !bot->GetGroup())
        {
            RandomPlayerbotMgr::instance().Refresh(bot);
            botAI->ResetStrategies();
        }

        botAI->Reset();
        return true;
    }

    // If we get the proposal packet, accept immediately
    if (!event.getPacket().empty())
    {
        WorldPacket p(event.getPacket());
        p.rpos(0);
        // ShatterCore: Cataclysm SMSG_LFG_PROPOSAL_UPDATE byte header — see
        // WorldPackets::LFG::LFGProposalUpdate::Write (ProposalID is the 6th uint32)
        uint32 ticketTime, completedMask, ticketId, ticketType, slot;
        p >> ticketTime >> completedMask >> ticketId >> ticketType >> slot >> id;

        if (id)
        {
            if (bot->IsInCombat() || bot->isDead())
            {
                bot->GetSession()->QueuePacket(BuildLfgProposalResponsePacket(id, false));
                return true;
            }

            botAI->GetAiObjectContext()->GetValue<uint32>("lfg proposal")->Set(0);
            bot->ClearUnitState(UNIT_STATE_ALL_STATE);

            bot->GetSession()->QueuePacket(BuildLfgProposalResponsePacket(id, true));

            if (RandomPlayerbotMgr::instance().IsRandomBot(bot) && !bot->GetGroup())
            {
                RandomPlayerbotMgr::instance().Refresh(bot);
                botAI->ResetStrategies();
            }

            botAI->Reset();
            return true;
        }
    }

    return false;
}

bool LfgLeaveAction::Execute(Event /*event*/)
{
    // Don't leave if lfg strategy enabled
    // if (botAI->HasStrategy("lfg", BOT_STATE_NON_COMBAT))
    //    return false;

    // Don't leave if already invited / in dungeon
    if (sLFGMgr->GetState(bot->GetGUID()) > LFG_STATE_QUEUED)
        return false;

    // ShatterCore: Cataclysm 4.3.4 wire format — mirror WorldPackets::LFG::LFGLeave::Read.
    // The handler leaves the queue for the guid packed in the ticket, so send the bot's own.
    ObjectGuid guid = bot->GetGUID();
    WorldPacket* packet = new WorldPacket(CMSG_LFG_LEAVE);
    *packet << (uint32)0;  // Roles (unused by the handler)
    *packet << (uint32)0;  // Ticket.Time
    *packet << (uint32)0;  // Ticket.Type
    *packet << (uint32)0;  // Ticket.Id
    packet->WriteBit(guid[4]);
    packet->WriteBit(guid[5]);
    packet->WriteBit(guid[0]);
    packet->WriteBit(guid[6]);
    packet->WriteBit(guid[2]);
    packet->WriteBit(guid[7]);
    packet->WriteBit(guid[1]);
    packet->WriteBit(guid[3]);
    packet->FlushBits();
    packet->WriteByteSeq(guid[7]);
    packet->WriteByteSeq(guid[4]);
    packet->WriteByteSeq(guid[3]);
    packet->WriteByteSeq(guid[2]);
    packet->WriteByteSeq(guid[6]);
    packet->WriteByteSeq(guid[0]);
    packet->WriteByteSeq(guid[1]);
    packet->WriteByteSeq(guid[5]);
    bot->GetSession()->QueuePacket(packet);
    // sLFGMgr->LeaveLfg(bot->GetGUID());
    return true;
}

bool LfgLeaveAction::isUseful() { return true; }

bool LfgTeleportAction::Execute(Event event)
{
    bool out = false;

    WorldPacket p(event.getPacket());
    if (!p.empty())
    {
        p.rpos(0);
        p >> out;
    }

    bot->ClearUnitState(UNIT_STATE_ALL_STATE);

    WorldPacket* packet = new WorldPacket(CMSG_LFG_TELEPORT);
    *packet << out;
    bot->GetSession()->QueuePacket(packet);
    // sLFGMgr->TeleportPlayer(bot, out);

    return true;
}

bool LfgJoinAction::isUseful()
{
    if (!sPlayerbotAIConfig.randomBotJoinLfg)
    {
        // botAI->ChangeStrategy("-lfg", BOT_STATE_NON_COMBAT);
        return false;
    }

    if (bot->getLevel() < 15)
        return false;

    // don't use if active player master
    if (GET_PLAYERBOT_AI(bot)->IsRealPlayer())
        return false;

    if (bot->GetGroup() && bot->GetGroup()->GetLeaderGUID() != bot->GetGUID())
    {
        // botAI->ChangeStrategy("-lfg", BOT_STATE_NON_COMBAT);
        return false;
    }

    if (bot->IsBeingTeleported())
        return false;

    if (bot->InBattleground())
        return false;

    if (bot->InBattlegroundQueue())
        return false;

    if (bot->isDead())
        return false;

    if (!RandomPlayerbotMgr::instance().IsRandomBot(bot))
        return false;

    Map* map = bot->GetMap();
    if (map && map->Instanceable())
        return false;

    LfgState state = sLFGMgr->GetState(bot->GetGUID());
    if (state != LFG_STATE_NONE)
        return false;

    return true;
}
