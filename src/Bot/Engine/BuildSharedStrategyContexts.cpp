#include "AiObjectContext.h"
#include "StrategyContext.h"

void AiObjectContext::BuildSharedStrategyContexts(SharedNamedObjectContextList<Strategy>& strategyContexts)
{
    strategyContexts.Add(new StrategyContext());
    strategyContexts.Add(new MovementStrategyContext());
    strategyContexts.Add(new AssistStrategyContext());
    strategyContexts.Add(new QuestStrategyContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - strategyContexts.Add(new DungeonStrategyContext());
    // 4.3.4 stage 1: WotLK raid/dungeon packs excluded - strategyContexts.Add(new RaidStrategyContext());
}
