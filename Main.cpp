
#include "raylib.h"
#include <iostream>
#include "Log.h"
#include "Player.h"
#include "Enemy.h"
#include <deque>
#include "UserInterface.h"



enum GameState { WAITING_FOR_INPUT, PROCESSING, GAME_OVER, INFO };


void ProcessOutcome(Player& MainPlayer, Enemy& MainEnemy, Action PlayerAction);


int main(void)
{
	// Raylib Init
	const int screenWidth = 1600;
	const int screenHeight = 1200;
	InitWindow(screenWidth, screenHeight, "Dueling Game");
	SetTargetFPS(60);

	// Game Init Health, Attack, Armor, Name, CoolDown, Stamina
	Player MainPlayer(10, 2, 2, "Hero", 0, 2);
	Enemy MainEnemy(10, 1.5, 0, "Goblin", 0, 1);
	int RoundNumber = 1;
	bool GameOver = false;
	int Wait = 3;
	GameState State = WAITING_FOR_INPUT;
	GameState StateBuffer = WAITING_FOR_INPUT;

	Action PlayerAction = NONE;

	while (!WindowShouldClose())
	{
		// Refresh screen every loop
		if (State == INFO)
		{
			WaitTime(Wait);
			State = StateBuffer;
			Wait = 3;

		}

		BeginDrawing(); // Begin frame rendering
		ClearBackground(BLACK); // Clear the background

		//GameOver Logic
		if (State == GAME_OVER)
		{

			DrawText("Game Over. Press ESC to exit.", 10, 200, 20, LIGHTGRAY);
			EndDrawing(); // Finalize frame rendering
			continue;

		}
        DrawHealthBars(MainPlayer, MainEnemy, screenWidth, screenHeight);
        DrawUIBackgrounds(screenWidth, screenHeight);
        DrawStatsUI(MainPlayer, MainEnemy, screenWidth, screenHeight);
        DrawCombatLog(screenWidth, screenHeight);
		DrawText(("Round " + std::to_string(RoundNumber)).c_str(), 10, 10, 20, LIGHTGRAY); //Round Number Title

		//Initial Message that Waits for input
		if (State == WAITING_FOR_INPUT)
		{
			DrawText("Choose an Action - (1: Attack, 2: Parry, 3: Defend, 4: Heavy Attack, 5: Dodge )", 10, 30, 20, GREEN);
			switch (GetKeyPressed())
			{

			case KEY_ONE:
				PlayerAction = ATTACK;
				State = PROCESSING;
				break;


			case KEY_TWO:
				if (MainPlayer.GetStamina() > 0)
				{
					MainPlayer.HandleStamina(false);
					PlayerAction = PARRY;

					State = PROCESSING;
					break;
				}
				else
				{
					State = INFO;
					StateBuffer = WAITING_FOR_INPUT;
					DrawText("You are exhausted: You cannot Parry until you Defend!", 10, 300, 20, RED);
					break;
				}

			case KEY_THREE:
				PlayerAction = DEFEND;
				State = PROCESSING;
				MainPlayer.HandleStamina(true);
				break;

			case KEY_FOUR:
				if (MainPlayer.GetStamina() > 0)
				{
					MainPlayer.HandleStamina(false);
					PlayerAction = HEAVY_ATTACK;

					State = PROCESSING;
					break;
				}
				else
				{
					State = INFO;
					StateBuffer = WAITING_FOR_INPUT;
					DrawText("You are exhausted: You cannot Heavy Attack until you Defend!", 10, 300, 20, RED);
					break;
				}
			case KEY_FIVE:
				if (MainPlayer.GetStamina() > 0)
				{
					MainPlayer.HandleStamina(false);
					PlayerAction = DODGE;

					State = PROCESSING;
					break;
				}
				else
				{
					State = INFO;
					StateBuffer = WAITING_FOR_INPUT;
					DrawText("You are exhausted: You cannot Dodge until you Defend!", 10, 300, 20, RED);
					break;
				}

			default:
				break;
			}

		}
		else if (State == PROCESSING)
		{
			// Outcome Function
			ProcessOutcome(MainPlayer, MainEnemy, PlayerAction);
			StateBuffer = WAITING_FOR_INPUT;

			// Win Logic Add Loot Logic.
			if (!MainEnemy.isAlive())
			{
                MainPlayer.GainExperience(10);
                Item loot = MainEnemy.RollLoot();
                if (loot.Name != "Nothing") {
                    MainPlayer.EquipItem(loot);
                    AddCombatLogEntry("You looted: " + loot.Name);
                }
                else {
                    AddCombatLogEntry("No loot dropped.");
                }
				RoundNumber++;

				if (RoundNumber > 5)
				{
					DrawText("Our Hero has defeated all his opponents and is now champion of the arena !!!", 10, 200, 20, GREEN);
					StateBuffer = GAME_OVER;
				}

				else
				{
					DrawText((MainEnemy.GetName() + " has been defeated by our Hero! A new stronger enemy appears").c_str(), 10, 220, 20, LIGHTGRAY);
					MainEnemy.IncreaseDifficulty(RoundNumber);
                    MainEnemy.SetHealth(MainEnemy.GetMaxHealth());
					DrawText((MainEnemy.GetName() + " enters the fray!").c_str(), 10, 300, 20, YELLOW);
					//MainPlayer.InitStats();
					DrawText("Our Hero Recovers his stamina and energy ready for a new round!", 10, 320, 20, LIGHTGRAY);
					StateBuffer = WAITING_FOR_INPUT;
					Wait = 5;
				}

			}
      

			if (!MainPlayer.isAlive())
			{
				DrawText("Our Hero has perished! You must try again!", 10, 340, 20, RED);
				StateBuffer = GAME_OVER;

			}
			// Display Information to the player then move to state buffer
			State = INFO;


		}

		EndDrawing();

	}


	CloseWindow();
	return 0;
}

void ProcessOutcome(Player& MainPlayer, Enemy& MainEnemy, Action PlayerAction)
{
	// Process the round logic based on actions
	Action EnemyAction = MainEnemy.ChooseAction();

    std::string PlayerActionString = (PlayerAction == ATTACK) ? "Attack" : (PlayerAction == DEFEND) ? "Defend" : (PlayerAction == PARRY) ? "Parry" : (PlayerAction == HEAVY_ATTACK) ? "Heavy Attack" : "Dodge";
    std::string EnemyActionString = (EnemyAction == ATTACK) ? "Attack" : (EnemyAction == DEFEND) ? "Defend" : (EnemyAction == PARRY) ? "Parry" : (EnemyAction == HEAVY_ATTACK) ? "Heavy Attack" : "Dodge";

	// Display player and enemy actions
	DrawText(("Player " + PlayerActionString + "s").c_str(), 10, 60, 20, LIGHTGRAY);
    DrawText(("Enemy " + EnemyActionString + "s").c_str(), 10, 80, 20, LIGHTGRAY);

    switch (PlayerAction)
    {
    case ATTACK:
        switch (EnemyAction)
        {
        case ATTACK:
            AddCombatLogEntry("Both clash with weapons — no damage taken.");
            break;
        case PARRY:
            MainPlayer.HandleHealth(-std::max(0.0f, MainEnemy.GetAtkPower() * 2 - MainPlayer.GetArmor()));
            MainEnemy.SetCoolDown(4);
            AddCombatLogEntry(MainEnemy.GetName() + " parries and counters!");
            AddCombatLogEntry(MainPlayer.GetName() + " takes " + std::to_string((int)std::max(0.0f, MainEnemy.GetAtkPower() * 2 - MainPlayer.GetArmor())) + " damage.");
            break;
        case DEFEND:
            MainEnemy.HandleHealth(-std::max(0.0f, (MainPlayer.GetAtkPower() / 2) - MainEnemy.GetArmor()));
            AddCombatLogEntry(MainEnemy.GetName() + " defends and takes reduced damage.");
            AddCombatLogEntry(MainEnemy.GetName() + " takes " + std::to_string((int)std::max(0.0f, (MainPlayer.GetAtkPower() / 2) - MainEnemy.GetArmor())) + " damage.");
            break;
        case HEAVY_ATTACK:
            MainPlayer.HandleHealth(-std::max(0.0f, MainEnemy.GetAtkPower() * 1.5f - MainPlayer.GetArmor()));
            MainEnemy.HandleHealth(-std::max(0.0f, MainPlayer.GetAtkPower() - MainEnemy.GetArmor()));
            AddCombatLogEntry("Brutal exchange! Both deal damage.");
            break;
        case DODGE:
            AddCombatLogEntry(MainEnemy.GetName() + " dodges. No damage dealt.");
            break;
        }
        break;

    case PARRY:
        switch (EnemyAction)
        {
        case ATTACK:
            MainEnemy.HandleHealth(-std::max(0.0f, MainPlayer.GetAtkPower() * 2 - MainEnemy.GetArmor()));
            MainPlayer.SetCoolDown(2);
            AddCombatLogEntry("You parry and counter!");
            AddCombatLogEntry(MainEnemy.GetName() + " takes " + std::to_string((int)std::max(0.0f, MainPlayer.GetAtkPower() * 2 - MainEnemy.GetArmor())) + " damage.");
            break;
        case PARRY:
            AddCombatLogEntry("Both parry — energy is drained.");
            break;
        case DEFEND:
            AddCombatLogEntry("Enemy defends while you parry — no effect.");
            break;
        case HEAVY_ATTACK:
            MainPlayer.HandleHealth(-std::max(0.0f, MainEnemy.GetAtkPower() * 1.5f - MainPlayer.GetArmor()));
            AddCombatLogEntry(MainEnemy.GetName() + "'s heavy attack breaks through your parry!");
            AddCombatLogEntry(MainPlayer.GetName() + " takes " + std::to_string((int)std::max(0.0f, MainEnemy.GetAtkPower() * 1.5f - MainPlayer.GetArmor())) + " damage.");
            break;
        case DODGE:
            AddCombatLogEntry(MainEnemy.GetName() + " dodges — nothing happens.");
            break;
        }
        break;

    case DEFEND:
        switch (EnemyAction)
        {
        case ATTACK:
            MainPlayer.HandleHealth(-std::max(0.0f, (MainEnemy.GetAtkPower() / 2) - MainPlayer.GetArmor()));
            AddCombatLogEntry("You defend and absorb the attack.");
            AddCombatLogEntry(MainPlayer.GetName() + " takes " + std::to_string((int)std::max(0.0f, (MainEnemy.GetAtkPower() / 2) - MainPlayer.GetArmor())) + " damage.");
            break;
        case PARRY:
            AddCombatLogEntry("Enemy misreads your move and gains nothing.");
            break;
        case DEFEND:
            AddCombatLogEntry("Both defend — energy restored.");
            break;
        case HEAVY_ATTACK:
            MainPlayer.HandleHealth(-std::max(0.0f, MainEnemy.GetAtkPower() * 1.5f - (MainPlayer.GetArmor() / 2)));
            AddCombatLogEntry(MainEnemy.GetName() + "'s heavy attack partially breaks through your defense!");
            AddCombatLogEntry(MainPlayer.GetName() + " takes " + std::to_string((int)std::max(0.0f, MainEnemy.GetAtkPower() * 1.5f - (MainPlayer.GetArmor() / 2))) + " damage.");
            break;
        case DODGE:
            AddCombatLogEntry(MainEnemy.GetName() + " dodges defensively. Nothing happens.");
            break;
        }
        break;

    case HEAVY_ATTACK:
        switch (EnemyAction)
        {
        case ATTACK:
            MainEnemy.HandleHealth(-std::max(0.0f, MainPlayer.GetAtkPower() * 1.5f - MainEnemy.GetArmor()));
            MainPlayer.HandleHealth(-std::max(0.0f, MainEnemy.GetAtkPower() - MainPlayer.GetArmor()));
            AddCombatLogEntry("Both strike hard!");
            break;
        case PARRY:
            MainEnemy.HandleHealth(-std::max(0.0f, MainPlayer.GetAtkPower() * 1.5f - MainEnemy.GetArmor()));
            AddCombatLogEntry(MainEnemy.GetName() + " fails to parry your heavy attack!");
            AddCombatLogEntry(MainEnemy.GetName() + " takes " + std::to_string((int)std::max(0.0f, MainPlayer.GetAtkPower() * 1.5f - MainEnemy.GetArmor())) + " damage.");
            break;
        case DEFEND:
            MainEnemy.HandleHealth(-std::max(0.0f, MainPlayer.GetAtkPower() * 1.5f - (MainEnemy.GetArmor() / 2)));
            AddCombatLogEntry(MainEnemy.GetName() + " defends. Heavy attack partially pierces through.");
            AddCombatLogEntry(MainEnemy.GetName() + " takes " + std::to_string((int)std::max(0.0f, MainPlayer.GetAtkPower() * 1.5f - (MainEnemy.GetArmor() / 2))) + " damage.");
            break;
        case HEAVY_ATTACK:
            MainEnemy.HandleHealth(-std::max(0.0f, MainPlayer.GetAtkPower() * 1.5f - MainEnemy.GetArmor()));
            MainPlayer.HandleHealth(-std::max(0.0f, MainEnemy.GetAtkPower() * 1.5f - MainPlayer.GetArmor()));
            AddCombatLogEntry("Double heavy attacks — a devastating exchange!");
            break;
        case DODGE:
            MainPlayer.HandleHealth(-std::max(0.0f, MainEnemy.GetAtkPower() - MainPlayer.GetArmor()));
            AddCombatLogEntry(MainEnemy.GetName() + " dodges and counters!");
            AddCombatLogEntry(MainPlayer.GetName() + " takes " + std::to_string((int)std::max(0.0f, MainEnemy.GetAtkPower() - MainPlayer.GetArmor())) + " damage.");
            break;
        }
        break;

    case DODGE:
        switch (EnemyAction)
        {
        case ATTACK:
            AddCombatLogEntry("You dodge the attack. No damage taken.");
            break;
        case PARRY:
            AddCombatLogEntry("You dodge cautiously while the enemy parries. Nothing happens.");
            break;
        case DEFEND:
            AddCombatLogEntry("You dodge. Enemy defends. Nothing happens.");
            break;
        case HEAVY_ATTACK:
            MainEnemy.HandleHealth(-std::max(0.0f, MainPlayer.GetAtkPower() - MainEnemy.GetArmor()));
            AddCombatLogEntry("You dodge the heavy attack and land a counter!");
            AddCombatLogEntry(MainEnemy.GetName() + " takes " + std::to_string((int)std::max(0.0f, MainPlayer.GetAtkPower() - MainEnemy.GetArmor())) + " damage.");
            break;
        case DODGE:
            AddCombatLogEntry("Both combatants dodge. No action.");
            break;
        }
        break;

    default:
        break;
    }
}

