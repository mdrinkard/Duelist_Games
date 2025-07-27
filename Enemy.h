#pragma once

#include "Character.h"
#include <random>
#include <vector>
#include "Item.h"

class Enemy : public Character


{
public:
	//Constructor
	Enemy(float Health, float AtkPower, float Armor, std::string Name, int CoolDown);

	void PrintLootTable() const;
	Action ChooseAction() override;
	void IncreaseDifficulty(int RoundNumber);

	void AddItem(const Item& item);
	Item RollLoot();

	void InitializeLootTable(int RoundNumber);
private:

	std::vector<Item> LootTable;
	float BaseHealth;
	std::mt19937 Generator;
	std::uniform_int_distribution<> Distribution;


};