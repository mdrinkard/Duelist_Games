#pragma once
#include "Character.h"
#include "Item.h"
#include <random>

class Player : public Character

{

public:
	Player(float MaxHealth, float AtkPower, float Armor, std::string Name, int CoolDown, int MaxStamina);

	
	Action ChooseAction() override;


	void GainExperience(int amount);
	int GetLevel() const { return Level; }
	int GetExperience() const { return Experience; }
	int GetExperienceToLevel() const { return ExperienceToLevel; }

	float GetHealth() const override;
	float GetAtkPower() const override;
	float GetArmor() const override;

	const Item& GetEquippedItem() const { return EquippedItem; }
	void EquipItem(const Item& item);
	/*void UnequipItem();*/

private:

	// Leveling system
	int Level = 1;
	int Experience = 0;
	int ExperienceToLevel = 20;  // Experience needed to level up

	Item EquippedItem = Item("None");
	void LevelUp();

protected:

};


/*


Public Members of Character remain public in player
protected members of character remain protected in player
private members of character remain inaccessable in player


*/