#include "Character.h"
#include <random>


Character::Character(float MaxHealth, float AtkPower, float Armor, std::string Name, int CoolDown, int MaxStamina) : 
	MaxHealth(MaxHealth), AtkPower(AtkPower), Armor(Armor), Name(Name), CoolDown(CoolDown), MaxStamina(MaxStamina)
{
	InitStats();
}

void Character::InitStats()
{
	Health = MaxHealth;
	Stamina = MaxStamina;
}

Action Character::GetActionFromInput(int Input)
{
	switch (Input)
	{
	case 0: return ATTACK;

	case 1:
		HandleStamina(false);
		return PARRY;

	case 2: 
		HandleStamina(true);
		return DEFEND;
	case 3:
		HandleStamina(false);
		return HEAVY_ATTACK;
	case 4:
		HandleStamina(false);
		return DODGE;

	}
	return NONE;
}

void Character::HandleStamina(bool Increase)
{
	Stamina = (Increase) ? Stamina+=1 : Stamina -= 1;
	if (Stamina > MaxStamina)
	{
		Stamina = MaxStamina;
	}
	else if (Stamina < 0)
	{
		Stamina = 0;
	}
}



