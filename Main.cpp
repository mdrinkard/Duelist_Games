

#include <iostream>
#include <string>
#include "Player.h"
#include "Enemy.h"
#include "Log.h"

void ProcessOutcome(Player& MainPlayer, Enemy& MainEnemy);

int main()
{
    Player MainPlayer(10, 2, 2, "Hero", 0, 2);
    Enemy MainEnemy(10, 1.5, 0, "Goblin", 0, 1);
    int RoundNumber = 1;

    while (MainPlayer.isAlive())
    {

        Log::Seperator();
        Log::LogMessage(LOG_DEFAULT, "Round " + std::to_string(RoundNumber));
        Log::Seperator();
        Log::NewLine();


        ProcessOutcome(MainPlayer, MainEnemy);

        if (!MainEnemy.isAlive())
        {
            Log::LogMessage(LOG_DEFAULT, MainEnemy.GetName() + " has been defeated by our Hero! A new stronger enemy appears!");

            Item droppedLoot = MainEnemy.RollLoot();
            if (droppedLoot.Name != "Nothing")
            {
                Log::NewLine();
                Log::LogMessage(LOG_DEFAULT, "You found: " + droppedLoot.Name + "!");
                Log::NewLine();
                Log::LogMessage(LOG_DEFAULT, "Do you want to equip this item? (y/n)");
                char choice;
                std::cin >> choice;
                Log::NewLine();

                if (choice == 'y' || choice == 'Y') {
                    MainPlayer.EquipItem(droppedLoot);
                    MainPlayer.DisplayStatus();
                }
                else {
                    Log::LogMessage(LOG_DEFAULT, "You decided not to equip " + droppedLoot.Name + ".");
                }
            }

            RoundNumber += 1;

            if (RoundNumber > 5)
            {
                Log::LogMessage(LOG_DEFAULT, "The Hero has emerged victorious! Thanks for playing!");
                break;
            }

            MainEnemy.IncreaseDifficulty(RoundNumber);
            MainEnemy.SetHealth(10 + RoundNumber * 2);
            MainPlayer.GainExperience(10 + RoundNumber);
        }

        if (!MainPlayer.isAlive())
        {
            Log::LogMessage(LOG_ERROR, "Our Hero was defeated! Git gud");
            break;
        }

        MainEnemy.DecrementCooldown();
        MainPlayer.DecrementCooldown();

        Log::LogMessage(LOG_DEFAULT, "Hero has " + std::to_string(MainPlayer.GetHealth()) + " health.");
        Log::LogMessage(LOG_DEFAULT, MainEnemy.GetName() + " has " + std::to_string(MainEnemy.GetHealth()) + " health.");
        Log::NewLine();
    }

    Log::LogMessage(LOG_DEFAULT, "Thanks for Playing!");
    return 0;
}

void ProcessOutcome(Player& MainPlayer, Enemy& MainEnemy)
{
    Action PlayerAction = MainPlayer.ChooseAction();
    Action EnemyAction = MainEnemy.ChooseAction();

    std::string PlayerActionString = (PlayerAction == ATTACK) ? "Attack" : (PlayerAction == DEFEND) ? "Defend" : (PlayerAction == PARRY) ? "Parry" : (PlayerAction == HEAVY_ATTACK) ? "Heavy Attack" : "Dodge";
    std::string EnemyActionString = (EnemyAction == ATTACK) ? "Attack" : (EnemyAction == DEFEND) ? "Defend" : (EnemyAction == PARRY) ? "Parry" : (EnemyAction == HEAVY_ATTACK) ? "Heavy Attack" : "Dodge";
    Log::LogMessage(LOG_INFO, "Player " + PlayerActionString + "s");
    Log::NewLine();
    Log::LogMessage(LOG_WARNING, "Enemy " + EnemyActionString + "s");
    Log::NewLine();

    if (PlayerAction == HEAVY_ATTACK)
    {
        MainPlayer.SetCoolDown(3);  // 3-turn cooldown
    }

    if (EnemyAction == HEAVY_ATTACK)
    {
        MainEnemy.SetCoolDown(6);
    }

    switch (PlayerAction)
    {
    case ATTACK:
        switch (EnemyAction)
        {
        case ATTACK:
            Log::LogMessage(LOG_DEFAULT, "It's a clash! The weapons ring as they hit each other!");
            break;
        case PARRY:
            Log::LogMessage(LOG_WARNING, MainEnemy.GetName() + " parried the Hero! You take counter damage");
            MainPlayer.HandleHealth(-std::max(0.0f, MainEnemy.GetAtkPower() * 2 - MainPlayer.GetArmor()));
            MainEnemy.SetCoolDown(4);
            break;
        case DEFEND:
            Log::LogMessage(LOG_WARNING, MainEnemy.GetName() + " defends! It takes half damage!");
            MainEnemy.HandleHealth(-std::max(0.0f, (MainPlayer.GetAtkPower() / 2) - MainEnemy.GetArmor()));
            break;
        case HEAVY_ATTACK:
            Log::LogMessage(LOG_DEFAULT, "Both go offensive — it's a brutal exchange!");
            MainPlayer.HandleHealth(-std::max(0.0f, MainEnemy.GetAtkPower() * 1.5f - MainPlayer.GetArmor()));
            MainEnemy.HandleHealth(-std::max(0.0f, MainPlayer.GetAtkPower() - MainEnemy.GetArmor()));
            break;
        case DODGE:
            Log::LogMessage(LOG_DEFAULT, MainEnemy.GetName() + " dodges! No damage dealt.");
            break;
        }
        break;

    case PARRY:
        switch (EnemyAction)
        {
        case ATTACK:
            Log::LogMessage(LOG_INFO, MainPlayer.GetName() + " parried the enemy! " + MainEnemy.GetName() + " takes counter damage");
            MainEnemy.HandleHealth(-std::max(0.0f, MainPlayer.GetAtkPower() * 2 - MainEnemy.GetArmor()));
            MainPlayer.SetCoolDown(2);
            break;
        case PARRY:
            Log::LogMessage(LOG_DEFAULT, "Both Combatants drain their energy!");
            break;
        case DEFEND:
            Log::LogMessage(LOG_DEFAULT, "The Hero misreads the enemy and allows the enemy to recover!");
            break;
        case HEAVY_ATTACK:
            Log::LogMessage(LOG_WARNING, MainEnemy.GetName() + " performs a heavy attack that breaks through the parry!");
            MainPlayer.HandleHealth(-std::max(0.0f, MainEnemy.GetAtkPower() * 1.5f - MainPlayer.GetArmor()));
            break;
        case DODGE:
            Log::LogMessage(LOG_DEFAULT, MainEnemy.GetName() + " dodges defensively. Nothing happens.");
            break;
        }
        break;

    case DEFEND:
        switch (EnemyAction)
        {
        case ATTACK:
            Log::LogMessage(LOG_INFO, MainPlayer.GetName() + " defends! You take half damage!");
            MainPlayer.HandleHealth(-std::max(0.0f, (MainEnemy.GetAtkPower() / 2) - MainPlayer.GetArmor()));
            break;
        case PARRY:
            Log::LogMessage(LOG_DEFAULT, "The Enemy misreads the hero and allows him to recover.");
            break;
        case DEFEND:
            Log::LogMessage(LOG_INFO, "Both Combatants are recovering energy!");
            break;
        case HEAVY_ATTACK:
            Log::LogMessage(LOG_WARNING, MainEnemy.GetName() + " performs a heavy attack! It breaks through the defense.");
            MainPlayer.HandleHealth(-std::max(0.0f, MainEnemy.GetAtkPower() * 1.5f - (MainPlayer.GetArmor() / 2)));
            break;
        case DODGE:
            Log::LogMessage(LOG_DEFAULT, MainEnemy.GetName() + " dodges while you're defending. No effect.");
            break;
        case NONE:
            break;
        }
        break;
    case HEAVY_ATTACK:
        switch (EnemyAction)
        {
        case ATTACK:
            Log::LogMessage(LOG_DEFAULT, "Hero's heavy attack strikes! Enemy attacks too. Both take damage.");
            MainEnemy.HandleHealth(-std::max(0.0f, MainPlayer.GetAtkPower() * 1.5f - MainEnemy.GetArmor()));
            MainPlayer.HandleHealth(-std::max(0.0f, MainEnemy.GetAtkPower() - MainPlayer.GetArmor()));
            break;
        case PARRY:
            Log::LogMessage(LOG_DEFAULT, MainEnemy.GetName() + " tries to parry but the heavy attack breaks through!");
            MainEnemy.HandleHealth(-std::max(0.0f, MainPlayer.GetAtkPower() * 1.5f - MainEnemy.GetArmor()));
            break;
        case DEFEND:
            Log::LogMessage(LOG_DEFAULT, MainEnemy.GetName() + " defends! Heavy attack breaks through partially.");
            MainEnemy.HandleHealth(-std::max(0.0f, MainPlayer.GetAtkPower() * 1.5f - (MainEnemy.GetArmor() / 2)));
            break;
        case HEAVY_ATTACK:
            Log::LogMessage(LOG_DEFAULT, "Double heavy! Brutal exchange!");
            MainEnemy.HandleHealth(-std::max(0.0f, MainPlayer.GetAtkPower() * 1.5f - MainEnemy.GetArmor()));
            MainPlayer.HandleHealth(-std::max(0.0f, MainEnemy.GetAtkPower() * 1.5f - MainPlayer.GetArmor()));
            break;
        case DODGE:
            Log::LogMessage(LOG_WARNING, MainEnemy.GetName() + " dodges your heavy attack and counters!");
            MainPlayer.HandleHealth(-std::max(0.0f, MainEnemy.GetAtkPower() - MainPlayer.GetArmor()));
            break;
        }
        break;
    case DODGE:
        switch (EnemyAction)
        {
        case ATTACK:
            Log::LogMessage(LOG_INFO, "Hero dodges the enemy attack! No damage taken.");
            break;
        case PARRY:
            Log::LogMessage(LOG_DEFAULT, "You dodge cautiously while the enemy parries. Nothing happens.");
            break;
        case DEFEND:
            Log::LogMessage(LOG_DEFAULT, "You dodge but the enemy is defending. No interaction.");
            break;
        case HEAVY_ATTACK:
            Log::LogMessage(LOG_INFO, "You dodge the heavy attack and land a counter blow!");
            MainEnemy.HandleHealth(-std::max(0.0f, MainPlayer.GetAtkPower() - MainEnemy.GetArmor()));
            break;
        case DODGE:
            Log::LogMessage(LOG_DEFAULT, "Both combatants dodge! A moment of calm.");
            break;
        }
        break;
    }


    Log::NewLine();
   
}
