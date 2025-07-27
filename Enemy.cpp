#include "Enemy.h"
#include "Item.h"
#include <random>

Enemy::Enemy(float Health, float AtkPower, float Armor, std::string Name, int CoolDown)
	: Character(Health, AtkPower, Armor, Name, CoolDown),
	Generator(std::random_device{}()),
	Distribution(0, 2)
{
	InitializeLootTable(1);
}

Action Enemy::ChooseAction()
{
	int Input;

	if (!CanExecute())
	{

		// Parry on cooldown - choose Attack(0) or Defend(2)
		std::uniform_int_distribution<> limitedDist(0, 1);
		int choice = limitedDist(Generator);
		Input = (choice == 0) ? 0 : 2;
	}
	else
	{
		Input = Distribution(Generator); // 0..2 as before
	}

	return GetActionFromInput(Input);
}

void Enemy::AddItem(const Item& item)
{
	LootTable.push_back(item);
}


Item Enemy::RollLoot()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(1, 100);

	for (const auto& item : LootTable)
	{
		int roll = dist(gen);
		if (roll <= item.DropChance)
		{
			return item;
		}
	}
	return Item("Nothing", 0, 0, 0, 0); 
}

void Enemy::InitializeLootTable(int RoundNumber)
{
	LootTable.clear();

	switch (RoundNumber)
	{
	case 1:
		// Basic Goblin loot
		LootTable.push_back(Item("Rusty Dagger", 0, 2, 0, 40)); 
		LootTable.push_back(Item("Rusty Shield", 0, 0, 2, 40));
		break;

	case 2:
		// Goblin Warrior loot
		LootTable.push_back(Item("Steel Sword", 0, 4, 0, 35));
		LootTable.push_back(Item("Chainmail Fragment", 5, 0, 2, 25));
		break;

	case 3:
		// Goblin Chief loot
		LootTable.push_back(Item("Chief's Axe", 0, 6, 0, 30));
		LootTable.push_back(Item("Chief's Helmet", 10, 0, 4, 20));
		break;

	case 4:
		// Ogre loot
		LootTable.push_back(Item("Ogre Club", 0, 8, 0, 25));
		LootTable.push_back(Item("Ogre Hide", 15, 0, 6, 15));
		break;

	case 5:
		// Ogre Chief loot
		LootTable.push_back(Item("Chief's Greatsword", 0, 10, 0, 20));
		LootTable.push_back(Item("Chief's Armor", 20, 0, 10, 10));
		break;

	default:
		// Default loot or empty
		LootTable.push_back(Item("Nothing", 0, 0, 0, 100));
		break;
	}
}

void Enemy::IncreaseDifficulty(int RoundNumber)
{
	switch (RoundNumber)
	{
	case 1:
		return;
	case 2:
		Name = "Goblin Warrior";
		break;
	case 3:
		AtkPower += RoundNumber;
		Name = "Goblin Chief";
		break;
	case 4:

		AtkPower += RoundNumber;
		Name = "Ogre";
		break;
	case 5:
		AtkPower += RoundNumber;
		Armor += RoundNumber / 2;
		Name = "Ogre Chief";
		break;
	}
	InitializeLootTable(RoundNumber);
}


void Enemy::PrintLootTable() const {
	std::cout << "Loot Table for Enemy: " << Name << "\n";

	if (LootTable.empty()) {
		std::cout << "  [Loot table is empty]\n";
		return;
	}

	for (const auto& item : LootTable) {
		std::cout << "  - Name: " << item.Name<< "\n";
	}
}



//Enemy::Enemy(int Health, int AtkPower, int Armor, std::string Name) : Character(Health, AtkPower, Armor, Name), Generator(std::random_device{}()), Distribution(0,2)
//{
//
//}
//
//std::string Enemy::ChooseAction()
//{
//	int Action;
//
//	if (!CanParry())
//	{
//		// Parry on cooldown - choose Attack(0) or Defend(2)
//		std::uniform_int_distribution<> limitedDist(0, 1);
//		int choice = limitedDist(Generator);
//		Action = (choice == 0) ? 0 : 2;
//	}
//	else
//	{
//		Action = Distribution(Generator); // 0..2 as before
//	}
//
//	return GetActionFromInput(Action);
//}
//
//void Enemy::AddItem(const LootItem& item)
//{
//	LootTable.push_back(item);
//}
//
//std::string Enemy::RollLoot()
//{
//	std::random_device rd;
//	std::mt19937 gen(rd());
//	std::uniform_int_distribution<> dist(1, 100);
//
//	for (const auto& item : LootTable)
//	{
//		int roll = dist(gen);
//		if (roll <= item.DropChance)
//		{
//			return item.Name;
//		}
//	}
//	return "Nothing";
//}
//
//void Enemy::IncreaseDifficulty(int RoundNumber)
//{
//	switch (RoundNumber)
//	{
//	case 1:
//		return;
//	case 2:
//		Name = "Goblin Warrior";
//		break;
//	case 3:
//		AtkPower += RoundNumber;
//		Name = "Goblin Chief";
//		break;
//	case 4:
//		AtkPower += RoundNumber;
//		Name = "Ogre";
//		break;
//	case 5:
//		AtkPower += RoundNumber;
//		Armor += RoundNumber / 2;
//		Name = "Ogre Chief";
//		break;
//	}
//
//}
