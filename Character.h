#pragma once
#include <iostream>
#include <string>
#include <random>

enum Action
{
	ATTACK,
	PARRY,
	DEFEND,
	HEAVY_ATTACK,
	DODGE,
	NONE
};


class Character
{
// New Code
public:
	Character(float MaxHealth, float Armor, float Atkpower, std::string Name, int CoolDown, int MaxStamina);

	void InitStats();

	std::string GetName() const { return Name; };
	Action GetActionFromInput(int Input);
	virtual float GetHealth() const { return Health; };
	virtual float GetArmor() const { return Armor; };
	virtual float GetAtkPower() const { return AtkPower; }
	virtual float GetStamina() const { return Stamina; }
	int GetCoolDown() const { return CoolDown; };


	void SetName(std::string Designation) {}
	void SetHealth(float Amount) { Health = Amount; }
	void SetArmor(float Amount) { Armor = Amount; }
	void SetAtkpower(float Amount) { AtkPower = Amount; }
	void SetCoolDown(int Amount) { CoolDown = Amount; }

	void HandleStamina(bool);
	void HandleHealth(float Amount) { Health += Amount; }
	void DecrementCooldown() { if (CoolDown > 0) CoolDown--; }

	bool isAlive() const { return Health > 0; };
	bool CanExecute() const { return CoolDown == 0; };

	virtual Action ChooseAction() = 0;

private:


protected:

	std::string Name;

	float MaxHealth;
	float Health;
	float Armor;
	float AtkPower;
	int CoolDown;
	int MaxStamina;
	int Stamina;



};
