/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license, you may redistribute it
 * and/or modify it under version 3 of the License, or (at your option), any later version.
 */

#ifndef _PLAYERBOT_DRUIDBEARACTIONS_H
#define _PLAYERBOT_DRUIDBEARACTIONS_H

#include "GenericSpellActions.h"
#include "ReachTargetActions.h"

class PlayerbotAI;

class CastFeralChargeBearAction : public CastReachTargetSpellAction
{
public:
    CastFeralChargeBearAction(PlayerbotAI* botAI) : CastReachTargetSpellAction(botAI, "feral charge - bear", 1.5f) {}
};

class CastGrowlAction : public CastSpellAction
{
public:
    CastGrowlAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "growl") {}
};

class CastChallengingRoarAction : public CastMeleeDebuffSpellAction
{
public:
    CastChallengingRoarAction(PlayerbotAI* botAI) : CastMeleeDebuffSpellAction(botAI, "challenging roar") {}
};

class CastMaulAction : public CastMeleeSpellAction
{
public:
    CastMaulAction(PlayerbotAI* botAI) : CastMeleeSpellAction(botAI, "maul") {}

    bool isUseful() override;
};

class CastBashAction : public CastMeleeSpellAction
{
public:
    CastBashAction(PlayerbotAI* botAI) : CastMeleeSpellAction(botAI, "bash") {}
};

class CastSwipeAction : public CastMeleeSpellAction
{
public:
    CastSwipeAction(PlayerbotAI* botAI) : CastMeleeSpellAction(botAI, "swipe") {}
};

class CastDemoralizingRoarAction : public CastMeleeDebuffSpellAction
{
public:
    CastDemoralizingRoarAction(PlayerbotAI* botAI) : CastMeleeDebuffSpellAction(botAI, "demoralizing roar") {}
};

class CastMangleBearAction : public CastMeleeSpellAction
{
public:
    CastMangleBearAction(PlayerbotAI* botAI) : CastMeleeSpellAction(botAI, "mangle") {}
};

class CastSwipeBearAction : public CastMeleeSpellAction
{
public:
    CastSwipeBearAction(PlayerbotAI* botAI) : CastMeleeSpellAction(botAI, "swipe") {}
};

class CastLacerateAction : public CastMeleeSpellAction
{
public:
    CastLacerateAction(PlayerbotAI* botAI) : CastMeleeSpellAction(botAI, "lacerate") {}
};

// ShatterCore (4.3.4 Bear): Thrash -- baseline AoE bleed on a short CD; apply on cooldown and for AoE threat.
class CastThrashAction : public CastMeleeSpellAction
{
public:
    CastThrashAction(PlayerbotAI* botAI) : CastMeleeSpellAction(botAI, "thrash") {}
    ActionThreatType getThreatType() override { return ActionThreatType::Aoe; }
};

// ShatterCore (4.3.4 Bear): Pulverize -- consumes Lacerate stacks for a crit buff (Pulverize). Cast when 3
// Lacerate stacks are up (the trigger gates this) to refresh the crit buff.
class CastPulverizeAction : public CastMeleeSpellAction
{
public:
    CastPulverizeAction(PlayerbotAI* botAI) : CastMeleeSpellAction(botAI, "pulverize") {}
};

// ShatterCore (4.3.4): Skull Bash (Bear) -- baseline interrupt + charge replacing the old reliance on Bash.
class CastSkullBashBearAction : public CastSpellAction
{
public:
    CastSkullBashBearAction(PlayerbotAI* botAI) : CastSpellAction(botAI, "skull bash") {}
};

class CastBashOnEnemyHealerAction : public CastSpellOnEnemyHealerAction
{
public:
    CastBashOnEnemyHealerAction(PlayerbotAI* botAI) : CastSpellOnEnemyHealerAction(botAI, "bash") {}
};

#endif
