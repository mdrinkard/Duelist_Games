

#include <iostream>
#include <string>
#include "Player.h"
#include "Enemy.h"
#include "Log.h"

void ProcessOutcome(Player& MainPlayer, Enemy& MainEnemy);

int main()
{
    Player MainPlayer(10, 2, 2, "Hero", 0);
    Enemy MainEnemy(10, 1.5, 0, "Goblin", 0);
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

    std::string PlayerActionString = (PlayerAction == ATTACK) ? "Attack" : (PlayerAction == DEFEND) ? "Defend" : "Parry";
    std::string EnemyActionString = (EnemyAction == ATTACK) ? "Attack" : (EnemyAction == DEFEND) ? "Defend" : "Parry";

    Log::LogMessage(LOG_INFO, "Player " + PlayerActionString + "s");
    Log::NewLine();
    Log::LogMessage(LOG_WARNING, "Enemy " + EnemyActionString + "s");
    Log::NewLine();

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
        default:
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
        default:
            Log::LogMessage(LOG_DEFAULT, "Nothing Happened!");
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
        default:
            Log::LogMessage(LOG_DEFAULT, "Nothing Happened!");
            break;
        }
        break;

    case NONE:
    default:
        break;
    }


    Log::NewLine();
   
}
