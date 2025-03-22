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
        {
            bot->StopMoving();
        }
        bot->SetStandState(UNIT_STAND_STATE_SIT);
        botAI->InterruptSpell();

        float mp = bot->GetPowerPct(POWER_MANA);
        float p = mp;
        float delay;

        if (!bot->InBattleground())
            delay = 27000.0f * (100 - p) / 100.0f;
        else
            delay = 20000.0f * (100 - p) / 100.0f;

        botAI->SetNextCheckDelay(delay);

        // Apply the aura (food/drink)
        bot->AddAura(24707, bot);

        // **NEW CODE: Check mana during the action**
        while (bot->GetPowerPct(POWER_MANA) < 90)
        {
            // Wait a short time to avoid busy-waiting
            botAI->Sleep(500);  // Sleep for 500 milliseconds (0.5 seconds)

            // Re-check mana.  Important to re-evaluate inside the loop.
            if (bot->GetPowerPct(POWER_MANA) >= 90)
            {
                // Stop drinking if mana is high enough
                bot->RemoveAura(24707, bot);                 // Remove the aura
                bot->SetStandState(UNIT_STAND_STATE_STAND);  // Stand up
                botAI->ClearNextCheckDelay();                // Reset the delay
                return true;                                 // Action completed successfully
            }
        }

        // If the loop finishes without interruption, it means mana reached 100%
        bot->RemoveAura(24707, bot);
        bot->SetStandState(UNIT_STAND_STATE_STAND);
        botAI->ClearNextCheckDelay();
        return true;
    }

    return UseItemAction::Execute(event);
}

bool DrinkAction::isUseful() { return UseItemAction::isUseful() && AI_VALUE2(uint8, "mana", "self target") < 85; }

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
        {
            bot->StopMoving();
        }

        bot->SetStandState(UNIT_STAND_STATE_SIT);
        botAI->InterruptSpell();

        float hp = bot->GetHealthPct();
        float p = hp;
        float delay;

        if (!bot->InBattleground())
            delay = 27000.0f * (100 - p) / 100.0f;
        else
            delay = 20000.0f * (100 - p) / 100.0f;

        botAI->SetNextCheckDelay(delay);

        // Apply the aura (food/drink)
        bot->AddAura(24707, bot);

        // **NEW CODE: Check health during the action**
        while (bot->GetHealthPct() < 90)
        {
            // Wait a short time to avoid busy-waiting
            botAI->Sleep(500);  // Sleep for 500 milliseconds (0.5 seconds)

            // Re-check health.  Important to re-evaluate inside the loop.
            if (bot->GetHealthPct() >= 90)
            {
                // Stop eating if health is high enough
                bot->RemoveAura(24707, bot);                 // Remove the aura
                bot->SetStandState(UNIT_STAND_STATE_STAND);  // Stand up
                botAI->ClearNextCheckDelay();                // Reset the delay
                return true;                                 // Action completed successfully
            }
        }

        // If the loop finishes without interruption, it means health reached 100%
        bot->RemoveAura(24707, bot);
        bot->SetStandState(UNIT_STAND_STATE_STAND);
        botAI->ClearNextCheckDelay();
        return true;
    }

    return UseItemAction::Execute(event);
}

bool EatAction::isUseful() { return UseItemAction::isUseful() && AI_VALUE2(uint8, "health", "self target") < 85; }

bool EatAction::isPossible()
{
    return !bot->IsInCombat() && (sPlayerbotAIConfig->freeFood || UseItemAction::isPossible());
}
