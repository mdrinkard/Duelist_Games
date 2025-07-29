
#include "raylib.h"
#include <iostream>
#include "Log.h"
#include "Player.h"
#include "Enemy.h"
#include <deque>

enum GameState { WAITING_FOR_INPUT, PROCESSING, GAME_OVER, INFO };

std::deque<std::string> CombatLog; // Rolling combat log
const int MaxLogEntries = 10;      // Limit to last 10 events
void ProcessOutcome(Player& MainPlayer, Enemy& MainEnemy, Action PlayerAction);
void DrawStatsUI(const Player& MainPlayer, const Enemy& MainEnemy, int screenWidth);
void AddCombatLogEntry(const std::string& entry);
void DrawCombatLog(int screenWidth);

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
        DrawCombatLog(screenWidth);
        DrawStatsUI(MainPlayer, MainEnemy,screenWidth);
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
                MainPlayer.GainExperience(RoundNumber);
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
					DrawText((MainEnemy.GetName() + " enters the fray!").c_str(), 10, 300, 20, YELLOW);
					MainPlayer.InitStats();
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


void DrawStatsUI(const Player& player, const Enemy& enemy, int screenWidth)
{
    int topRightX = screenWidth - 350; // Adjust for right side positioning

    // Player Info
    DrawText("Player Info", topRightX, 10, 20, GREEN);
    DrawText(("Name: " + player.GetName()).c_str(), topRightX, 40, 20, LIGHTGRAY);
    DrawText(("HP: " + std::to_string(player.GetHealth())).c_str(), topRightX, 60, 20, LIGHTGRAY);
    DrawText(("Stamina: " + std::to_string(player.GetStamina())).c_str(), topRightX, 80, 20, LIGHTGRAY);
    DrawText(("Armor: " + std::to_string(player.GetArmor())).c_str(), topRightX, 100, 20, LIGHTGRAY);
    DrawText(("ATK: " + std::to_string(player.GetAtkPower())).c_str(), topRightX, 120, 20, LIGHTGRAY);
    DrawText(("Equipped: " + player.GetEquippedItem().Name).c_str(), topRightX, 140, 20, LIGHTGRAY);

    // Enemy Info
    DrawText("Enemy Info", topRightX, 160, 20, RED);
    DrawText(("Name: " + enemy.GetName()).c_str(), topRightX, 190, 20, LIGHTGRAY);
    DrawText(("HP: " + std::to_string(enemy.GetHealth())).c_str(), topRightX, 210, 20, LIGHTGRAY);
    DrawText(("Armor: " + std::to_string(enemy.GetArmor())).c_str(), topRightX, 230, 20, LIGHTGRAY);
    DrawText(("ATK: " + std::to_string(enemy.GetAtkPower())).c_str(), topRightX, 250, 20, LIGHTGRAY);
}

void DrawCombatLog(int screenWidth)
{
    int startX = screenWidth - 700;  // Between stats and center
    int startY = 10;
    int lineHeight = 20;

    DrawText("Combat Log", startX, startY, 20, ORANGE);

    for (int i = 0; i < CombatLog.size(); ++i)
    {
        DrawText(CombatLog[i].c_str(), startX, startY + (i + 1) * lineHeight, 18, LIGHTGRAY);
    }
}


void AddCombatLogEntry(const std::string& entry)
{
    if (CombatLog.size() >= MaxLogEntries)
        CombatLog.pop_front(); // Remove oldest

    CombatLog.push_back(entry);
}

// old code
//#include "raylib.h"
//#include <iostream>
//#include <string>
//#include "Player.h"
//#include "Enemy.h"
////#include "Log.h"
//
//enum GameState{WAITING_FOR_INPUT, PROCESSING, GAME_OVER, INFO};
//
//void ProcessOutcome(Player& MainPlayer, Enemy& MainEnemy);
//
//int main(void)
//{
//    // RayLib Iniit
//    const int screenWidth = 800;
//    const int screenHeight = 600;
//    InitWindow(screenWidth, screenHeight, "Dueling Simulator");
//    SetTargetFPS(60);   
//
//    //Game Init
//    Player MainPlayer(10, 2, 2, "Hero", 0, 2);
//    Enemy MainEnemy(10, 1.5, 0, "Goblin", 0, 1);
//    int RoundNumber = 1;
//    bool Gameover = false;
//    int Wait = 3;
//    GameState State= WAITING_FOR_INPUT;
//    GameState StateBuffer = WAITING_FOR_INPUT;
//
//    Action PlayerAction = NONE;
//  
//
//    // Main game loop
//    while (!WindowShouldClose())  
//    {
//        //refresh screen everyloop
//        if (State == INFO)
//        {
//            WaitTime(Wait);
//            State = StateBuffer;
//            Wait = 3;
//        }
//
//        BeginDrawing();
//        ClearBackground(BLACK);
//
//
//        if (State == GAME_OVER)
//        {
//            DrawText("Game Over. Git Gud and press escape to exit.", 10, 200, 20, LIGHTGRAY);
//            EndDrawing();
//            continue;
//        }
//
//        DrawText(("Round " + std::to_string(RoundNumber)).c_str(), 10, 10, 20, LIGHTGRAY);
//
//        if (State == WAITING_FOR_INPUT)
//        {
//            DrawText("Choose an Action - (1: Attack, 2: Parry, 3: Defend)", 10, 30, 20, GREEN);
//            switch (GetKeyPressed())
//            {
//
//            case KEY_ONE:
//                PlayerAction = ATTACK;
//                State = PROCESSING;
//                break;
//
//
//            case KEY_TWO:
//                if (MainPlayer.GetStamina() > 0)
//                {
//                    MainPlayer.HandleStamina(false);
//                    PlayerAction = PARRY;
//
//                    State = PROCESSING;
//                    break;
//                }
//                else
//                {
//                    State = INFO;
//                    StateBuffer = WAITING_FOR_INPUT;
//                    DrawText("You are exhausted: You cannot Parry until you Defend!", 10, 300, 20, RED);
//                    break;
//                }
//
//            case KEY_THREE:
//                PlayerAction = DEFEND;
//                State = PROCESSING;
//                MainPlayer.HandleStamina(true);
//                break;
//
//
//            default:
//                break;
//            }
//
//        }
//        else if (State == PROCESSING)
//        {
//            ProcessOutcome(MainPlayer, MainEnemy, PlayerAction);
//            StateBuffer = WAITING_FOR_INPUT;
//
//            if (!MainEnemy.isAlive())
//            {
//
//                RoundNumber++;
//
//                if (RoundNumber > 5)
//                {
//                    DrawText("Our Hero has defeated all his opponents and is now champion of the arena !!!", 10, 200, 20, GREEN);
//                    StateBuffer = GAME_OVER;
//                }
//
//                else
//                {
//                    DrawText((MainEnemy.GetName() + " has been defeated by our Hero! A new stronger enemy appears").c_str(), 10, 220, 20, LIGHTGRAY);
//                    MainEnemy.IncreaseDifficulty(RoundNumber);
//                    DrawText((MainEnemy.GetName() + " enters the fray!").c_str(), 10, 300, 20, YELLOW);
//                    MainPlayer.InitStats();
//                    DrawText("Our Hero Recovers his stamina and energy ready for a new round!", 10, 320, 20, LIGHTGRAY);
//                    StateBuffer = WAITING_FOR_INPUT;
//                    Wait = 5;
//                }
//
///*
////              Below Goes in the Above if statement
//        if (!MainEnemy.isAlive())
//        {
//           DrawText((MainEnemy.GetName() + " has been defeated by our Hero! A new stronger enemy appears!").c_str(), 190, 200, 20, LIGHTGRAY);
//
//            Item droppedLoot = MainEnemy.RollLoot();
//            if (droppedLoot.Name != "Nothing")
//            {
//                DrawText(("You found: " + droppedLoot.Name + "!").c_str(), 190, 200, 20, LIGHTGRAY);
//
//               
//                DrawText("Do you want to equip this item? (y/n)", 190, 200, 20, LIGHTGRAY);
//
//
//                char choice;
//                std::cin >> choice;
//
//
//                if (choice == 'y' || choice == 'Y') {
//                    MainPlayer.EquipItem(droppedLoot);
//                    MainPlayer.DisplayStatus();
//                }
//                else {
//                    DrawText(("You decided not to equip " + droppedLoot.Name + ".").c_str(), 190, 200, 20, LIGHTGRAY);
//
//                }
//            }
//
//*/
//            RoundNumber ++;
//
//            if (RoundNumber > 5)
//            {
//                DrawText("The Hero has emerged victorious! Thanks for playing!", 190, 200, 20, LIGHTGRAY);
//                Gameover = true;
//
//                break;
//            }
//
//            MainEnemy.IncreaseDifficulty(RoundNumber);
//            MainEnemy.SetHealth(10 + RoundNumber * 2);
//            MainPlayer.GainExperience(10 + RoundNumber);
//        }
//
//
//        MainEnemy.DecrementCooldown();
//        MainPlayer.DecrementCooldown();
//
//        //DrawText(("Hero has " + std::to_string(MainPlayer.GetHealth()) + " health.").c_str(), 190, 200, 20, LIGHTGRAY);
//
//        //DrawText((MainEnemy.GetName() + " has " + std::to_string(MainEnemy.GetHealth()) + " health.").c_str(), 190, 200, 20, LIGHTGRAY);
//
//        if (!MainPlayer.isAlive())
//        {
//            DrawText("Our Hero has perished! You must try again!", 10, 340, 20, RED);
//            StateBuffer = GAME_OVER;
//
//        }
//
//    }
//
//    CloseWindow();        // Close window and OpenGL context
//    return 0;
//}
//
//
//
//void ProcessOutcome(Player& MainPlayer, Enemy& MainEnemy)
//{
//    Action PlayerAction = MainPlayer.ChooseAction();
//    Action EnemyAction = MainEnemy.ChooseAction();
//
    //std::string PlayerActionString = (PlayerAction == ATTACK) ? "Attack" : (PlayerAction == DEFEND) ? "Defend" : (PlayerAction == PARRY) ? "Parry" : (PlayerAction == HEAVY_ATTACK) ? "Heavy Attack" : "Dodge";
    //std::string EnemyActionString = (EnemyAction == ATTACK) ? "Attack" : (EnemyAction == DEFEND) ? "Defend" : (EnemyAction == PARRY) ? "Parry" : (EnemyAction == HEAVY_ATTACK) ? "Heavy Attack" : "Dodge";
//
//
//    DrawText(("Player " + PlayerActionString + "s").c_str(), 190, 200, 20, LIGHTGRAY);
//    DrawText(("Enemy " + EnemyActionString + "s").c_str(), 190, 200, 20, LIGHTGRAY);
//
//
//    if (PlayerAction == HEAVY_ATTACK)
//    {
//        MainPlayer.SetCoolDown(3);  // 3-turn cooldown
//    }
//
//    if (EnemyAction == HEAVY_ATTACK)
//    {
//        MainEnemy.SetCoolDown(6);
//    }
//
    //switch (PlayerAction)
    //{
    //case ATTACK:
    //    switch (EnemyAction)
    //    {
    //    case ATTACK:
    //        DrawText("It's a clash! The weapons ring as they hit each other!", 190, 200, 20, LIGHTGRAY);
    //       
    //        break;
    //    case PARRY:
    //        DrawText((MainEnemy.GetName() + " parried the Hero! You take counter damage").c_str(), 190, 200, 20, LIGHTGRAY);
    //        MainPlayer.HandleHealth(-std::max(0.0f, MainEnemy.GetAtkPower() * 2 - MainPlayer.GetArmor()));
    //        MainEnemy.SetCoolDown(4);
    //        break;
    //    case DEFEND:
    //        DrawText((MainEnemy.GetName() + " defends! It takes half damage!").c_str(), 190, 200, 20, LIGHTGRAY);
    //        MainEnemy.HandleHealth(-std::max(0.0f, (MainPlayer.GetAtkPower() / 2) - MainEnemy.GetArmor()));
    //        break;
    //    case HEAVY_ATTACK:
    //        DrawText("Both go offensive — it's a brutal exchange!", 190, 200, 20, LIGHTGRAY);

    //        MainPlayer.HandleHealth(-std::max(0.0f, MainEnemy.GetAtkPower() * 1.5f - MainPlayer.GetArmor()));
    //        MainEnemy.HandleHealth(-std::max(0.0f, MainPlayer.GetAtkPower() - MainEnemy.GetArmor()));
    //        break;
    //    case DODGE:
    //        DrawText((MainEnemy.GetName() + " dodges! No damage dealt.").c_str(), 190, 200, 20, LIGHTGRAY);

    //      
    //        break;
    //    }
    //    break;

    //case PARRY:
    //    switch (EnemyAction)
    //    {
    //    case ATTACK:
    //        DrawText((MainPlayer.GetName() + " parried the enemy! " + MainEnemy.GetName() + " takes counter damage").c_str(), 190, 200, 20, LIGHTGRAY);

    //        MainEnemy.HandleHealth(-std::max(0.0f, MainPlayer.GetAtkPower() * 2 - MainEnemy.GetArmor()));
    //        MainPlayer.SetCoolDown(2);
    //        break;
    //    case PARRY:
    //        DrawText("Both Combatants drain their energy!", 190, 200, 20, LIGHTGRAY);

    //        break;
    //    case DEFEND:
    //        DrawText("The Hero misreads the enemy and allows the enemy to recover!", 190, 200, 20, LIGHTGRAY);

    //        break;
    //    case HEAVY_ATTACK:
    //        DrawText((MainEnemy.GetName() + " performs a heavy attack that breaks through the parry!").c_str(), 190, 200, 20, LIGHTGRAY);

    //        MainPlayer.HandleHealth(-std::max(0.0f, MainEnemy.GetAtkPower() * 1.5f - MainPlayer.GetArmor()));
    //        break;
    //    case DODGE:
    //        DrawText((MainEnemy.GetName() + " dodges defensively. Nothing happens.").c_str(), 190, 200, 20, LIGHTGRAY);

    //        break;
    //    }
    //    break;

    //case DEFEND:
    //    switch (EnemyAction)
    //    {
    //    case ATTACK:
    //        DrawText((MainPlayer.GetName() + " defends! You take half damage!").c_str(), 190, 200, 20, LIGHTGRAY);

    //        MainPlayer.HandleHealth(-std::max(0.0f, (MainEnemy.GetAtkPower() / 2) - MainPlayer.GetArmor()));
    //        break;
    //    case PARRY:
    //        DrawText("The Enemy misreads the hero and allows him to recover.", 190, 200, 20, LIGHTGRAY);

    //        break;
    //    case DEFEND:
    //        DrawText("Both Combatants are recovering energy!", 190, 200, 20, LIGHTGRAY);
    //        break;
    //    case HEAVY_ATTACK:
    //        DrawText((MainEnemy.GetName() + " performs a heavy attack! It breaks through the defense.").c_str(), 190, 200, 20, LIGHTGRAY);
    //        MainPlayer.HandleHealth(-std::max(0.0f, MainEnemy.GetAtkPower() * 1.5f - (MainPlayer.GetArmor() / 2)));
    //        break;
    //    case DODGE:
    //        DrawText((MainEnemy.GetName() + " dodges while you're defending. No effect.").c_str(), 190, 200, 20, LIGHTGRAY);
    //        break;
    //    case NONE:
    //        break;
    //    }
    //    break;
    //case HEAVY_ATTACK:
    //    switch (EnemyAction)
    //    {
    //    case ATTACK:
    //        DrawText("Hero's heavy attack strikes! Enemy attacks too. Both take damage.", 190, 200, 20, LIGHTGRAY);

    //        MainEnemy.HandleHealth(-std::max(0.0f, MainPlayer.GetAtkPower() * 1.5f - MainEnemy.GetArmor()));
    //        MainPlayer.HandleHealth(-std::max(0.0f, MainEnemy.GetAtkPower() - MainPlayer.GetArmor()));
    //        break;
    //    case PARRY:
    //        DrawText((MainEnemy.GetName() + " tries to parry but the heavy attack breaks through!").c_str(), 190, 200, 20, LIGHTGRAY);


    //        MainEnemy.HandleHealth(-std::max(0.0f, MainPlayer.GetAtkPower() * 1.5f - MainEnemy.GetArmor()));
    //        break;
    //    case DEFEND:
    //        DrawText((MainEnemy.GetName() + " defends! Heavy attack breaks through partially.").c_str(), 190, 200, 20, LIGHTGRAY);

    //        MainEnemy.HandleHealth(-std::max(0.0f, MainPlayer.GetAtkPower() * 1.5f - (MainEnemy.GetArmor() / 2)));
    //        break;
    //    case HEAVY_ATTACK:
    //        DrawText("Double heavy! Brutal exchange!", 190, 200, 20, LIGHTGRAY);

    //        MainEnemy.HandleHealth(-std::max(0.0f, MainPlayer.GetAtkPower() * 1.5f - MainEnemy.GetArmor()));
    //        MainPlayer.HandleHealth(-std::max(0.0f, MainEnemy.GetAtkPower() * 1.5f - MainPlayer.GetArmor()));
    //        break;
    //    case DODGE:
    //        DrawText((MainEnemy.GetName() + " dodges your heavy attack and counters!").c_str(), 190, 200, 20, LIGHTGRAY);

    //        MainPlayer.HandleHealth(-std::max(0.0f, MainEnemy.GetAtkPower() - MainPlayer.GetArmor()));
    //        break;
    //    }
    //    break;
    //case DODGE:
    //    switch (EnemyAction)
    //    {
    //    case ATTACK:
    //        DrawText("Hero dodges the enemy attack! No damage taken.", 190, 200, 20, LIGHTGRAY);

    //        break;
    //    case PARRY:
    //        DrawText("You dodge cautiously while the enemy parries. Nothing happens.", 190, 200, 20, LIGHTGRAY);

    //        break;
    //    case DEFEND:
    //        DrawText("You dodge but the enemy is defending. No interaction.", 190, 200, 20, LIGHTGRAY);


    //        break;
    //    case HEAVY_ATTACK:
    //        DrawText("You dodge the heavy attack and land a counter blow!", 190, 200, 20, LIGHTGRAY);

    //        MainEnemy.HandleHealth(-std::max(0.0f, MainPlayer.GetAtkPower() - MainEnemy.GetArmor()));
    //        break;
    //    case DODGE:
    //        DrawText("Both combatants dodge! A moment of calm.", 190, 200, 20, LIGHTGRAY);

    //        break;
    //    }
    //    break;
    //}
//
//
//   
//}

    //switch (PlayerAction)
    //{
    //case ATTACK:
    //    switch (EnemyAction)
    //    {
    //    case ATTACK:
    //        DrawText("It's a clash! The weapons ring as they hit each other!", 190, 200, 20, LIGHTGRAY);

    //        break;
    //    case PARRY:
    //        DrawText((MainEnemy.GetName() + " parried the Hero! You take counter damage").c_str(), 190, 200, 20, LIGHTGRAY);
    //        MainPlayer.HandleHealth(-std::max(0.0f, MainEnemy.GetAtkPower() * 2 - MainPlayer.GetArmor()));
    //        MainEnemy.SetCoolDown(4);
    //        break;
    //    case DEFEND:
    //        DrawText((MainEnemy.GetName() + " defends! It takes half damage!").c_str(), 190, 200, 20, LIGHTGRAY);
    //        MainEnemy.HandleHealth(-std::max(0.0f, (MainPlayer.GetAtkPower() / 2) - MainEnemy.GetArmor()));
    //        break;
    //    case HEAVY_ATTACK:
    //        DrawText("Both go offensive — it's a brutal exchange!", 190, 200, 20, LIGHTGRAY);

    //        MainPlayer.HandleHealth(-std::max(0.0f, MainEnemy.GetAtkPower() * 1.5f - MainPlayer.GetArmor()));
    //        MainEnemy.HandleHealth(-std::max(0.0f, MainPlayer.GetAtkPower() - MainEnemy.GetArmor()));
    //        break;
    //    case DODGE:
    //        DrawText((MainEnemy.GetName() + " dodges! No damage dealt.").c_str(), 190, 200, 20, LIGHTGRAY);


    //        break;
    //    }
    //    break;

    //case PARRY:
    //    switch (EnemyAction)
    //    {
    //    case ATTACK:
    //        DrawText((MainPlayer.GetName() + " parried the enemy! " + MainEnemy.GetName() + " takes counter damage").c_str(), 190, 200, 20, LIGHTGRAY);

    //        MainEnemy.HandleHealth(-std::max(0.0f, MainPlayer.GetAtkPower() * 2 - MainEnemy.GetArmor()));
    //        MainPlayer.SetCoolDown(2);
    //        break;
    //    case PARRY:
    //        DrawText("Both Combatants drain their energy!", 190, 200, 20, LIGHTGRAY);

    //        break;
    //    case DEFEND:
    //        DrawText("The Hero misreads the enemy and allows the enemy to recover!", 190, 200, 20, LIGHTGRAY);

    //        break;
    //    case HEAVY_ATTACK:
    //        DrawText((MainEnemy.GetName() + " performs a heavy attack that breaks through the parry!").c_str(), 190, 200, 20, LIGHTGRAY);

    //        MainPlayer.HandleHealth(-std::max(0.0f, MainEnemy.GetAtkPower() * 1.5f - MainPlayer.GetArmor()));
    //        break;
    //    case DODGE:
    //        DrawText((MainEnemy.GetName() + " dodges defensively. Nothing happens.").c_str(), 190, 200, 20, LIGHTGRAY);

    //        break;
    //    }
    //    break;

    //case DEFEND:
    //    switch (EnemyAction)
    //    {
    //    case ATTACK:
    //        DrawText((MainPlayer.GetName() + " defends! You take half damage!").c_str(), 190, 200, 20, LIGHTGRAY);

    //        MainPlayer.HandleHealth(-std::max(0.0f, (MainEnemy.GetAtkPower() / 2) - MainPlayer.GetArmor()));
    //        break;
    //    case PARRY:
    //        DrawText("The Enemy misreads the hero and allows him to recover.", 190, 200, 20, LIGHTGRAY);

    //        break;
    //    case DEFEND:
    //        DrawText("Both Combatants are recovering energy!", 190, 200, 20, LIGHTGRAY);
    //        break;
    //    case HEAVY_ATTACK:
    //        DrawText((MainEnemy.GetName() + " performs a heavy attack! It breaks through the defense.").c_str(), 190, 200, 20, LIGHTGRAY);
    //        MainPlayer.HandleHealth(-std::max(0.0f, MainEnemy.GetAtkPower() * 1.5f - (MainPlayer.GetArmor() / 2)));
    //        break;
    //    case DODGE:
    //        DrawText((MainEnemy.GetName() + " dodges while you're defending. No effect.").c_str(), 190, 200, 20, LIGHTGRAY);
    //        break;
    //    case NONE:
    //        break;
    //    }
    //    break;
    //case HEAVY_ATTACK:
    //    switch (EnemyAction)
    //    {
    //    case ATTACK:
    //        DrawText("Hero's heavy attack strikes! Enemy attacks too. Both take damage.", 190, 200, 20, LIGHTGRAY);

    //        MainEnemy.HandleHealth(-std::max(0.0f, MainPlayer.GetAtkPower() * 1.5f - MainEnemy.GetArmor()));
    //        MainPlayer.HandleHealth(-std::max(0.0f, MainEnemy.GetAtkPower() - MainPlayer.GetArmor()));
    //        break;
    //    case PARRY:
    //        DrawText((MainEnemy.GetName() + " tries to parry but the heavy attack breaks through!").c_str(), 190, 200, 20, LIGHTGRAY);


    //        MainEnemy.HandleHealth(-std::max(0.0f, MainPlayer.GetAtkPower() * 1.5f - MainEnemy.GetArmor()));
    //        break;
    //    case DEFEND:
    //        DrawText((MainEnemy.GetName() + " defends! Heavy attack breaks through partially.").c_str(), 190, 200, 20, LIGHTGRAY);

    //        MainEnemy.HandleHealth(-std::max(0.0f, MainPlayer.GetAtkPower() * 1.5f - (MainEnemy.GetArmor() / 2)));
    //        break;
    //    case HEAVY_ATTACK:
    //        DrawText("Double heavy! Brutal exchange!", 190, 200, 20, LIGHTGRAY);

    //        MainEnemy.HandleHealth(-std::max(0.0f, MainPlayer.GetAtkPower() * 1.5f - MainEnemy.GetArmor()));
    //        MainPlayer.HandleHealth(-std::max(0.0f, MainEnemy.GetAtkPower() * 1.5f - MainPlayer.GetArmor()));
    //        break;
    //    case DODGE:
    //        DrawText((MainEnemy.GetName() + " dodges your heavy attack and counters!").c_str(), 190, 200, 20, LIGHTGRAY);

    //        MainPlayer.HandleHealth(-std::max(0.0f, MainEnemy.GetAtkPower() - MainPlayer.GetArmor()));
    //        break;
    //    }
    //    break;
    //case DODGE:
    //    switch (EnemyAction)
    //    {
    //    case ATTACK:
    //        DrawText("Hero dodges the enemy attack! No damage taken.", 190, 200, 20, LIGHTGRAY);

    //        break;
    //    case PARRY:
    //        DrawText("You dodge cautiously while the enemy parries. Nothing happens.", 190, 200, 20, LIGHTGRAY);

    //        break;
    //    case DEFEND:
    //        DrawText("You dodge but the enemy is defending. No interaction.", 190, 200, 20, LIGHTGRAY);


    //        break;
    //    case HEAVY_ATTACK:
    //        DrawText("You dodge the heavy attack and land a counter blow!", 190, 200, 20, LIGHTGRAY);

    //        MainEnemy.HandleHealth(-std::max(0.0f, MainPlayer.GetAtkPower() - MainEnemy.GetArmor()));
    //        break;
    //    case DODGE:
    //        DrawText("Both combatants dodge! A moment of calm.", 190, 200, 20, LIGHTGRAY);

    //        break;
    //    }
    //    break;
    //}
//}