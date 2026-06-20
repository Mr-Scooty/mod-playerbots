#include "QuestConfirmAcceptAction.h"

#include "WorldPacket.h"
#include "WorldSession.h"
#include "QuestPackets.h"

bool QuestConfirmAcceptAction::Execute(Event event)
{
    WorldPacket packet(event.getPacket());
    uint32 questId;
    packet >> questId;

    WorldPacket sendPacket(CMSG_QUEST_CONFIRM_ACCEPT);
    sendPacket << questId;
    Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
    if (!quest || !bot->CanAddQuest(quest, true))
    {
        return false;
    }
    std::ostringstream out;
    out << "Quest: " << chat->FormatQuest(quest) << " confirm accept";
    botAI->TellMaster(out);
    WorldPackets::Quest::QuestConfirmAccept confirmPacket{WorldPacket(CMSG_QUEST_CONFIRM_ACCEPT)};
    confirmPacket.QuestID = questId;
    bot->GetSession()->HandleQuestConfirmAccept(confirmPacket);
    return true;
}
