/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#include "NonCombatActions.h"

#include "Event.h"
#include "Playerbots.h"

bool DrinkAction::Execute(Event event)
{
    if (bot->IsInCombat())
        return false;

    bool hasMana = AI_VALUE2(bool, "has mana", "self target");
    if (!hasMana)
        return false;

    if (sPlayerbotAIConfig->freeFood)
    {
        bot->ClearUnitState(UNIT_STATE_CHASE);
        bot->ClearUnitState(UNIT_STATE_FOLLOW);

        if (bot->isMoving())
            bot->StopMoving();

        bot->SetStandState(UNIT_STAND_STATE_SIT);

        botAI->InterruptSpell();

        float mp = bot->GetPowerPct(POWER_MANA);
        float p = mp;
        float delay;
        float manaRate = sWorld->getRate(RATE_POWER_MANA);
        float baseDelay;
        if (!bot->InBattleground())
            baseDelay = 30000.0f * (100 - p) / 100.0f;
        else
            baseDelay = 23000.0f * (100 - p) / 100.0f;

        delay = baseDelay / manaRate;

        // This formula calculates a delay that adjusts based on the current mana percentage and the server's mana
        // regeneration rate. It aims to make the bot drink for a shorter duration when mana regeneration
        // is faster, and for a longer duration when mana regeneration is slower.

        float minDelay = 1000.0f;
        delay = std::max(delay, minDelay);

        botAI->SetNextCheckDelay(delay);

        bot->AddAura(24707, bot);
        return true;
    }

    return UseItemAction::Execute(event);
}

bool DrinkAction::isUseful() { return UseItemAction::isUseful() && AI_VALUE2(uint8, "mana", "self target") < 75; }

bool DrinkAction::isPossible()
{
    return !bot->IsInCombat() && (sPlayerbotAIConfig->freeFood || UseItemAction::isPossible());
}

bool EatAction::Execute(Event event)
{
    if (bot->IsInCombat())
        return false;

    if (sPlayerbotAIConfig->freeFood)
    {
        bot->ClearUnitState(UNIT_STATE_CHASE);
        bot->ClearUnitState(UNIT_STATE_FOLLOW);

        if (bot->isMoving())
            bot->StopMoving();

        bot->SetStandState(UNIT_STAND_STATE_SIT);
        botAI->InterruptSpell();

        float hp = bot->GetHealthPct();
        float p = hp;
        float delay;
        float healthRate = sWorld->getRate(RATE_HEALTH);
        float baseDelay;
        if (!bot->InBattleground())
            baseDelay = 30000.0f * (100 - p) / 100.0f;
        else
            baseDelay = 23000.0f * (100 - p) / 100.0f;

        delay = baseDelay / healthRate;

        // This formula calculates a delay that adjusts based on the current health percentage and the server's health
        // regeneration rate. It aims to make the bot eat for a shorter duration when health regeneration
        // is faster, and for a longer duration when health regeneration is slower.
       
        float minDelay = 1000.0f;
        delay = std::max(delay, minDelay);

        botAI->SetNextCheckDelay(delay);

        bot->AddAura(24707, bot);
        return true;
    }

    return UseItemAction::Execute(event);
}

bool EatAction::isUseful() { return UseItemAction::isUseful() && AI_VALUE2(uint8, "health", "self target") < 75; }

bool EatAction::isPossible()
{
    return !bot->IsInCombat() && (sPlayerbotAIConfig->freeFood || UseItemAction::isPossible());
}
