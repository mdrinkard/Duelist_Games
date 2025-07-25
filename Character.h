#pragma once
#include <iostream>
#include <string>
#include <random>



class Character
{
// New Code
public:
	Character(float Health, float Armor, float Atkpower, std::string Name);


	std::string GetName() const { return Name; };
	std::string GetActionFromInput(int Input);
	virtual float GetHealth() const { return Health; };
	virtual float GetArmor() const { return Armor; };
	virtual float GetAtkPower() const { return AtkPower; }
	int GetCoolDown() const { return CoolDown; };


	void SetName(std::string Designation) {}
	void SetHealth(float Amount) { Health = Amount; }
	void SetArmor(float Amount) { Armor = Amount; }
	void SetAtkpower(float Amount) { AtkPower = Amount; }
	void SetCoolDown(int Amount) { CoolDown = Amount; }

	void HandleHealth(float Amount) { Health += Amount; }
	void DecrementCooldown() { if (CoolDown > 0) CoolDown--; }

	bool isAlive() const { return Health > 0; };
	bool CanExecute() const { return CoolDown == 0; };

	virtual std::string ChooseAction() = 0;

private:


protected:

	std::string Name;

	float Health;
	float Armor;
	float AtkPower;

	int CoolDown;



};
