

#include <iostream>
#include "Player.h"
#include "Enemy.h"

void ProcessOutcome(Player& MainPlayer, Enemy& MainEnemy);

int main()
{
	Player MainPlayer(10, 2, 2, "Hero");
	Enemy MainEnemy(10, 1.5, 0, "Goblin");
	int RoundNumber = 1;
	/*MainPlayer.DisplayStatus();*/

	while (MainPlayer.isAlive())
	{
		ProcessOutcome(MainPlayer, MainEnemy);

		if (!MainEnemy.isAlive())
		{
			std::cout << MainEnemy.GetName() << " has been defeated by our Hero! A new stronger enemy appears!" << '\n';

			Item droppedLoot = MainEnemy.RollLoot();
			if (droppedLoot.Name != "Nothing")
			{
				std::cout << "You found: " << droppedLoot.Name << "!\n";

				// Ask player if they want to equip the item
				std::cout << "Do you want to equip this item? (y/n): ";
				char choice;
				std::cin >> choice;
				if (choice == 'y' || choice == 'Y') {
					MainPlayer.EquipItem(droppedLoot);
					MainPlayer.DisplayStatus();
				}
				else {
					std::cout << "You decided not to equip " << droppedLoot.Name << ".\n";
				}
			}

			RoundNumber += 1;
			std::cout << RoundNumber <<" is the RoundNumber!" << "\n";
			if (RoundNumber > 5)
			{
				std::cout << "The Hero has emerged victorious! Thanks for playing!" << '\n';
				break;
			}

			MainEnemy.IncreaseDifficulty(RoundNumber);
			MainEnemy.SetHealth(10 + RoundNumber * 2);
			MainPlayer.GainExperience(10 + RoundNumber);
		}
		if (!MainPlayer.isAlive())
		{
			std::cout <<"Our Hero was defeated! Git gud" << "\n";
			break;
		}
	
		MainEnemy.DecrementCooldown();
		MainPlayer.DecrementCooldown();
		//MainPlayer.DisplayStatus();
		std::cout << "Hero has " << MainPlayer.GetHealth() << '\n';
		std::cout << MainEnemy.GetName() << " has " << MainEnemy.GetHealth() << '\n';
	}
	std::cout << "Thanks for Playing!" << "\n";
	return 0;
}

void ProcessOutcome(Player& MainPlayer, Enemy& MainEnemy)
{
	std::string PlayerAction = MainPlayer.ChooseAction();
	std::string EnemyAction = MainEnemy.ChooseAction();

	std::cout << "Player " << PlayerAction << "s" << "\n";
	std::cout << "Enemy " << EnemyAction << "s" << "\n";

	if (PlayerAction == "Attack" && EnemyAction == "Attack")
	{
		std::cout << "It's a clash! The weapons ring as they hit each other!" << '\n';
	}
	else if (PlayerAction == "Attack" && EnemyAction == "Parry")
	{
		std::cout << MainEnemy.GetName() << " parried the Hero! You take counter damage\n";
		std::cout << "Counter Damage Calculation:\n"
			<< "  Raw Damage: " << MainEnemy.GetAtkPower() * 2 << "\n"
			<< "  Hero Armor: " << MainPlayer.GetArmor() << "\n"
			<< "  Net Damage: " << std::max(0.0f, MainEnemy.GetAtkPower() * 2 - MainPlayer.GetArmor()) << "\n";

		MainPlayer.HandleHealth(-std::max(0.0f, MainEnemy.GetAtkPower() * 2 - MainPlayer.GetArmor()));
		std::cout << "Hero Health after damage: " << MainPlayer.GetHealth() << "\n";

			MainEnemy.SetCoolDown(4);
	}
	else if (PlayerAction == "Attack" && EnemyAction == "Defend")
	{
			std::cout << MainEnemy.GetName() << " defends! It takes half damage!\n";
			std::cout << "Damage Calculation:\n"
				<< "  Raw Damage (Half Attack): " << MainPlayer.GetAtkPower() / 2 << "\n"
				<< "  Enemy Armor: " << MainEnemy.GetArmor() << "\n"
				<< "  Net Damage: " << std::max(0.0f, (MainPlayer.GetAtkPower() / 2) - MainEnemy.GetArmor()) << "\n";

			MainEnemy.HandleHealth(-std::max(0.0f, (MainPlayer.GetAtkPower() / 2) - MainEnemy.GetArmor()));
			std::cout << MainEnemy.GetName() << " Health after damage: " << MainEnemy.GetHealth() << "\n";
	}
	else if (PlayerAction == "Parry" && EnemyAction == "Attack")
	{
			std::cout << MainPlayer.GetName() << " parried the enemy! " << MainEnemy.GetName() << " takes counter damage\n";
			std::cout << "Counter Damage Calculation:\n"
				<< "  Raw Damage: " << MainPlayer.GetAtkPower() * 2 << "\n"
				<< "  Enemy Armor: " << MainEnemy.GetArmor() << "\n"
				<< "  Net Damage: " << std::max(0.0f, MainPlayer.GetAtkPower() * 2 - MainEnemy.GetArmor()) << "\n";

			MainEnemy.HandleHealth(-std::max(0.0f, MainPlayer.GetAtkPower() * 2 - MainEnemy.GetArmor()));
			std::cout << MainEnemy.GetName() << " Health after damage: " << MainEnemy.GetHealth() << "\n";

			MainPlayer.SetCoolDown(2);
	}
	else if (PlayerAction == "Defend" && EnemyAction == "Attack")
	{
			std::cout << MainPlayer.GetName() << " defends! You take half damage!\n";
			std::cout << "Damage Calculation:\n"
				<< "  Raw Damage (Half Attack): " << MainEnemy.GetAtkPower() / 2 << "\n"
				<< "  Hero Armor: " << MainPlayer.GetArmor() << "\n"
				<< "  Net Damage: " << std::max(0.0f, (MainEnemy.GetAtkPower() / 2) - MainPlayer.GetArmor()) << "\n";

			MainPlayer.HandleHealth(-std::max(0.0f, (MainEnemy.GetAtkPower() / 2) - MainPlayer.GetArmor()));
			std::cout << "Hero Health after damage: " << MainPlayer.GetHealth() << "\n";
	}
	else
	{
			std::cout << "Nothing Happened!" << '\n';
	}
}
