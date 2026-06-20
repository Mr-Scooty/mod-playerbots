#include "AiObjectContext.h"
#include "TriggerContext.h"
#include "ChatTriggerContext.h"
#include "WorldPacketTriggerContext.h"

void AiObjectContext::BuildSharedTriggerContexts(SharedNamedObjectContextList<Trigger>& triggerContexts)
{
    triggerContexts.Add(new TriggerContext());
    triggerContexts.Add(new ChatTriggerContext());
    triggerContexts.Add(new WorldPacketTriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new RaidAq20TriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new RaidMcTriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new RaidBwlTriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new RaidKarazhanTriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new RaidGruulsLairTriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new RaidMagtheridonTriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new RaidNaxxTriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new RaidSSCTriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new RaidTempestKeepTriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new RaidHyjalSummitTriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new RaidBlackTempleTriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new RaidZulAmanTriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new RaidOsTriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new RaidEoETriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new RaidVoATriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new RaidUlduarTriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new RaidOnyxiaTriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new RaidIccTriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new TbcDungeonAuchenaiCryptsTriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new WotlkDungeonUKTriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new WotlkDungeonNexTriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new WotlkDungeonANTriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new WotlkDungeonOKTriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new WotlkDungeonDTKTriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new WotlkDungeonVHTriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new WotlkDungeonGDTriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new WotlkDungeonHoSTriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new WotlkDungeonHoLTriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new WotlkDungeonOccTriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new WotlkDungeonUPTriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new WotlkDungeonCoSTriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new WotlkDungeonFoSTriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new WotlkDungeonPoSTriggerContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - triggerContexts.Add(new WotlkDungeonToCTriggerContext());
}
