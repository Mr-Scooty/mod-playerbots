#include "ScriptMgr.h"
#include "CharacterPackets.h"
#include "Opcodes.h"
#include "Player.h"
#include "ObjectAccessor.h"

#include "Playerbots.h"

namespace
{
    static Player* FindOnlineAltbotByGuid(ObjectGuid guid)
    {
        if (!guid)
            return nullptr;

        Player* p = ObjectAccessor::FindPlayer(guid);
        if (!p)
            return nullptr;

        PlayerbotAI* ai = GET_PLAYERBOT_AI(p);
        if (!ai || ai->IsRealPlayer())
            return nullptr;

        return p;
    }

    static void ForceLogoutViaPlayerbotHolder(Player* target)
    {
        if (!target)
            return;

        PlayerbotAI* ai = GET_PLAYERBOT_AI(target);

        if (!ai)
            return;

        if (Player* master = ai->GetMaster())
        {
            if (PlayerbotMgr* mgr = GET_PLAYERBOT_MGR(master))
            {
                mgr->LogoutPlayerBot(target->GetGUID());
                return;
            }
        }

        sRandomPlayerbotMgr.LogoutPlayerBot(target->GetGUID());
    }
}

class PlayerbotsSecureLoginServerScript : public ServerScript
{
public:
    PlayerbotsSecureLoginServerScript()
        : ServerScript("PlayerbotsSecureLoginServerScript", { SERVERHOOK_CAN_PACKET_RECEIVE }) {}

    bool CanPacketReceive(WorldSession* /*session*/, WorldPacket const& packet) override
    {
        if (packet.GetOpcode() != CMSG_PLAYER_LOGIN)
            return true;

        // 4.3.4 CMSG_PLAYER_LOGIN carries a bit-packed guid - parse through the core packet class
        // ShatterCore: structured WorldPackets need brace-init (avoids most-vexing-parse).
        WorldPackets::Character::PlayerLogin loginPacket{WorldPacket(packet)};
        loginPacket.Read();
        ObjectGuid loginGuid = loginPacket.Guid;

        if (!loginGuid)
            return true;

        Player* existingAltbot = FindOnlineAltbotByGuid(loginGuid);
        if (existingAltbot)
            ForceLogoutViaPlayerbotHolder(existingAltbot);

        return true;
    }
};

void AddPlayerbotsSecureLoginScripts()
{
    new PlayerbotsSecureLoginServerScript();
}
