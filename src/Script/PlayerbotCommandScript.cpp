/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "BattleGroundTactics.h"
#include "Chat.h"
#include "GuildTaskMgr.h"
#include "PerfMonitor.h"
#include "PlayerbotMgr.h"
#include "RandomPlayerbotMgr.h"
#include "ScriptMgr.h"
#include "WorldSession.h"
#include "Log.h"
#include "RBAC.h"

// ShatterCore (TrinityCore 4.3.4) command tables use rbac permissions instead of
// AzerothCore security levels. Until dedicated playerbots rbac permissions exist,
// player commands ride on COMMAND_HELP (granted to every player role) and GM
// commands on COMMAND_GM.
class playerbots_commandscript : public CommandScript
{
public:
    playerbots_commandscript() : CommandScript("playerbots_commandscript") {}

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> playerbotsDebugCommandTable = {
            { "bg", rbac::RBAC_PERM_COMMAND_GM, true, &HandleDebugBGCommand, "" },
        };

        static std::vector<ChatCommand> playerbotsAccountCommandTable = {
            { "setKey", rbac::RBAC_PERM_COMMAND_HELP, false, &HandleSetSecurityKeyCommand, "" },
            { "link", rbac::RBAC_PERM_COMMAND_HELP, false, &HandleLinkAccountCommand, "" },
            { "linkedAccounts", rbac::RBAC_PERM_COMMAND_HELP, false, &HandleViewLinkedAccountsCommand, "" },
            { "unlink", rbac::RBAC_PERM_COMMAND_HELP, false, &HandleUnlinkAccountCommand, "" },
        };

        static std::vector<ChatCommand> playerbotsCommandTable = {
            { "bot", rbac::RBAC_PERM_COMMAND_HELP, false, &HandlePlayerbotCommand, "" },
            { "gtask", rbac::RBAC_PERM_COMMAND_GM, true, &HandleGuildTaskCommand, "" },
            { "pmon", rbac::RBAC_PERM_COMMAND_GM, true, &HandlePerfMonCommand, "" },
            { "rndbot", rbac::RBAC_PERM_COMMAND_GM, true, &HandleRandomPlayerbotCommand, "" },
            { "debug", rbac::RBAC_PERM_COMMAND_GM, true, nullptr, "", playerbotsDebugCommandTable },
            { "account", rbac::RBAC_PERM_COMMAND_HELP, false, nullptr, "", playerbotsAccountCommandTable },
        };

        static std::vector<ChatCommand> commandTable = {
            { "playerbots", rbac::RBAC_PERM_COMMAND_HELP, true, nullptr, "", playerbotsCommandTable },
        };

        return commandTable;
    }

    static bool HandlePlayerbotCommand(ChatHandler* handler, char const* args)
    {
        return PlayerbotMgr::HandlePlayerbotMgrCommand(handler, args);
    }

    static bool HandleRandomPlayerbotCommand(ChatHandler* handler, char const* args)
    {
        return RandomPlayerbotMgr::HandlePlayerbotConsoleCommand(handler, args);
    }

    static bool HandleGuildTaskCommand(ChatHandler* handler, char const* args)
    {
        return GuildTaskMgr::HandleConsoleCommand(handler, args);
    }

    static bool HandlePerfMonCommand(ChatHandler* handler, char const* args)
    {
        if (!strcmp(args, "reset"))
        {
            sPerfMonitor.Reset();
            return true;
        }

        if (!strcmp(args, "tick"))
        {
            sPerfMonitor.PrintStats(true, false);
            return true;
        }

        if (!strcmp(args, "stack"))
        {
            sPerfMonitor.PrintStats(false, true);
            return true;
        }

        if (!strcmp(args, "toggle"))
        {
            sPlayerbotAIConfig.perfMonEnabled = !sPlayerbotAIConfig.perfMonEnabled;
            if (sPlayerbotAIConfig.perfMonEnabled)
                LOG_INFO("playerbots", "Performance monitor enabled");
            else
                LOG_INFO("playerbots", "Performance monitor disabled");
            return true;
        }

        sPerfMonitor.PrintStats();
        return true;
    }

    static bool HandleDebugBGCommand(ChatHandler* handler, char const* args)
    {
        return BGTactics::HandleConsoleCommand(handler, args);
    }

    static bool HandleSetSecurityKeyCommand(ChatHandler* handler, char const* args)
    {
        if (!args || !*args)
        {
            handler->PSendSysMessage("Usage: .playerbots account setKey <securityKey>");
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();
        std::string key = args;

        PlayerbotMgr* mgr = PlayerbotsMgr::instance().GetPlayerbotMgr(player);
        if (mgr)
        {
            mgr->HandleSetSecurityKeyCommand(player, key);
            return true;
        }
        else
        {
            handler->PSendSysMessage("PlayerbotMgr instance not found.");
            return false;
        }
    }

    static bool HandleLinkAccountCommand(ChatHandler* handler, char const* args)
    {
        if (!args || !*args)
            return false;

        char* accountName = strtok((char*)args, " ");
        char* key = strtok(nullptr, " ");

        if (!accountName || !key)
        {
            handler->PSendSysMessage("Usage: .playerbots account link <accountName> <securityKey>");
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();

        PlayerbotMgr* mgr = PlayerbotsMgr::instance().GetPlayerbotMgr(player);
        if (mgr)
        {
            mgr->HandleLinkAccountCommand(player, accountName, key);
            return true;
        }
        else
        {
            handler->PSendSysMessage("PlayerbotMgr instance not found.");
            return false;
        }
    }

    static bool HandleViewLinkedAccountsCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();

        PlayerbotMgr* mgr = PlayerbotsMgr::instance().GetPlayerbotMgr(player);
        if (mgr)
        {
            mgr->HandleViewLinkedAccountsCommand(player);
            return true;
        }
        else
        {
            handler->PSendSysMessage("PlayerbotMgr instance not found.");
            return false;
        }
    }

    static bool HandleUnlinkAccountCommand(ChatHandler* handler, char const* args)
    {
        if (!args || !*args)
            return false;

        char* accountName = strtok((char*)args, " ");
        if (!accountName)
        {
            handler->PSendSysMessage("Usage: .playerbots account unlink <accountName>");
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();

        PlayerbotMgr* mgr = PlayerbotsMgr::instance().GetPlayerbotMgr(player);
        if (mgr)
        {
            mgr->HandleUnlinkAccountCommand(player, accountName);
            return true;
        }
        else
        {
            handler->PSendSysMessage("PlayerbotMgr instance not found.");
            return false;
        }
    }
};

void AddPlayerbotsCommandscripts() { new playerbots_commandscript(); }
