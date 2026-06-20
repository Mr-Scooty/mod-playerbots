/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#include "AreaTriggerAction.h"

#include "Event.h"
#include "LastMovementValue.h"
#include "PlayerbotTextMgr.h"
#include "Playerbots.h"
#include "Transport.h"
#include "WorldSession.h"
#include "MotionMaster.h"
#include "DBCStores.h"

bool ReachAreaTriggerAction::Execute(Event event)
{
    if (botAI->IsRealPlayer())  // Do not trigger own area trigger.
        return false;

    uint32 triggerId;
    WorldPacket p(event.getPacket());
    p.rpos(0);
    p >> triggerId;

    AreaTriggerEntry const* at = sAreaTriggerStore.LookupEntry(triggerId);
    if (!at)
        return false;

    if (!sObjectMgr->GetAreaTrigger(triggerId))
    {
        WorldPacket p1(CMSG_AREATRIGGER);
        p1 << triggerId;
        p1.rpos(0);
        bot->GetSession()->HandleAreaTriggerOpcode(p1);

        return true;
    }

    if (bot->GetMapId() != at->ContinentID)
    {
        botAI->TellError(PlayerbotTextMgr::instance().GetBotTextOrDefault(
            "area_trigger_follow_too_far_error", "I won't follow: too far away", {}));
        return true;
    }

    bot->GetMotionMaster()->MovePoint(
        /*id*/ at->ContinentID,
        /*coords*/ at->Pos.X, at->Pos.Y, at->Pos.Z,
        /*generatePath*/ true,
        /*speed*/ 0.0f);

    float distance = bot->GetDistance(at->Pos.X, at->Pos.Y, at->Pos.Z);
    float delay = 1000.0f * distance / bot->GetSpeed(MOVE_RUN) + sPlayerbotAIConfig.reactDelay;
    botAI->TellError(PlayerbotTextMgr::instance().GetBotTextOrDefault(
        "area_trigger_wait_for_me", "Wait for me", {}));
    botAI->SetNextCheckDelay(delay);
    context->GetValue<LastMovement&>("last area trigger")->Get().lastAreaTrigger = triggerId;

    return true;
}

bool AreaTriggerAction::Execute(Event /*event*/)
{
    LastMovement& movement = context->GetValue<LastMovement&>("last area trigger")->Get();

    uint32 triggerId = movement.lastAreaTrigger;
    movement.lastAreaTrigger = 0;

    if (!sObjectMgr->GetAreaTrigger(triggerId))
        return false;

    if (!sObjectMgr->GetAreaTrigger(triggerId))
        return true;

    WorldPacket p(CMSG_AREATRIGGER);
    p << triggerId;
    p.rpos(0);
    bot->GetSession()->HandleAreaTriggerOpcode(p);

    botAI->TellMaster(PlayerbotTextMgr::instance().GetBotTextOrDefault("hello", "Hello", {}));
    return true;
}
