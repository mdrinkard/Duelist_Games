#include "Enemy.h"
#include "Item.h"
#include <random>

Enemy::Enemy(float MaxHealth, float AtkPower, float Armor, std::string Name, int CoolDown, int MaxStamina)
	: Character(MaxHealth, AtkPower, Armor, Name, CoolDown, MaxStamina),
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
		std::uniform_int_distribution<> limitedDist(0, 2); 
		Input = limitedDist(Generator);
		if (Stamina == 0)
		{
			
			Input = (limitedDist(Generator) % 2 == 0) ? 0 : 2;
		}
	}
	else
	{
		std::uniform_int_distribution<> fullDist(0, 4); 
		Input = fullDist(Generator);
		if (Stamina == 0)
		{
			
			std::uniform_int_distribution<> limitedDist(0, 2);
			Input = (limitedDist(Generator) == 0) ? 0 : 2;
		}
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
		//std::cout << "Rolling for " << item.Name << " with DropChance " << item.DropChance << std::endl;
		int roll = dist(gen);
		//std::cout << "Roll result: " << roll << std::endl;
		if (roll <= item.DropChance)
		{
			return item;
		}
	}
	return Item("Nothing", 0, 0, 0, 0, 0); 
}

void Enemy::InitializeLootTable(int RoundNumber)
{
	LootTable.clear();
	// Health, AtkPower, Armor, Stamina, DropChance

	switch (RoundNumber)
	{
	case 1:
		// Basic Goblin loot Health, AtkPower, Armor, Stamina, DropChance
		LootTable.push_back(Item("Goblin Charm", 1, 1, 1, 1, 100));
		LootTable.push_back(Item("Rusty Dagger", 0, 2, 0, 0, 50)); 
		LootTable.push_back(Item("Rusty Buckler", 0, 0, 1, 1, 50));
		LootTable.push_back(Item("Rusty Helmet", 0, 0, 2, 0, 25));
		break;

	case 2:
		// Goblin Warrior loot
		LootTable.push_back(Item("Steel Sword", 0, 4, 0, 0, 35));
		LootTable.push_back(Item("Steel Shield", 0, 0, 4, 0, 25));
		LootTable.push_back(Item("Warrior's Leather Armor", 2, 1, 2, 2, 25));
		LootTable.push_back(Item("Goblin Totem", 2, 2, 2, 2, 25));
		break;

	case 3:
		// Goblin Chief loot
		LootTable.push_back(Item("Chief's Axe", 0, 6, 0, 0, 30));
		LootTable.push_back(Item("Chief's Helmet", 10, 0, 4, 0, 20));
		LootTable.push_back(Item("Tower Shield", 0, 2, 6, 0, 25));
		LootTable.push_back(Item("Chieftan's Banner", 3, 3, 3, 3, 25));
		break;

	case 4:
		// Ogre loot
		LootTable.push_back(Item("Ogre Club", 0, 8, 0, 0, 25));
		LootTable.push_back(Item("Ogre Hide", 15, 0, 6, 0, 15));
		break;

	case 5:
		// Ogre Chief loot
		LootTable.push_back(Item("Chief's Greatsword", 0, 10, 0, 0, 20));
		LootTable.push_back(Item("Chief's Armor", 20, 0, 10, 0, 10));
		break;

	default:
		// Default loot or empty
		LootTable.push_back(Item("Nothing", 0, 0, 0, 0, 100));
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
		std::cout << "  - Name: " << item.Name << "\n";
	}
}



