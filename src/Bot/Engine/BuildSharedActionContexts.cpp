#include "AiObjectContext.h"
#include "ActionContext.h"
#include "ChatActionContext.h"
#include "WorldPacketActionContext.h"

void AiObjectContext::BuildSharedActionContexts(SharedNamedObjectContextList<Action>& actionContexts)
{
    actionContexts.Add(new ActionContext());
    actionContexts.Add(new ChatActionContext());
    actionContexts.Add(new WorldPacketActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new RaidAq20ActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new RaidMcActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new RaidBwlActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new RaidKarazhanActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new RaidGruulsLairActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new RaidMagtheridonActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new RaidSSCActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new RaidTempestKeepActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new RaidHyjalSummitActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new RaidBlackTempleActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new RaidZulAmanActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new RaidNaxxActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new RaidOsActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new RaidEoEActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new RaidVoAActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new RaidUlduarActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new RaidOnyxiaActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new RaidIccActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new TbcDungeonAuchenaiCryptsActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new WotlkDungeonUKActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new WotlkDungeonNexActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new WotlkDungeonANActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new WotlkDungeonOKActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new WotlkDungeonDTKActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new WotlkDungeonVHActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new WotlkDungeonGDActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new WotlkDungeonHoSActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new WotlkDungeonHoLActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new WotlkDungeonOccActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new WotlkDungeonUPActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new WotlkDungeonCoSActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new WotlkDungeonFoSActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new WotlkDungeonPoSActionContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - actionContexts.Add(new WotlkDungeonToCActionContext());
}
