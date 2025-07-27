#include "Character.h"
#include <random>


Character::Character(float Health, float AtkPower, float Armor, std::string Name, int CoolDown) : Health(Health), AtkPower(AtkPower), Armor(Armor), Name(Name), CoolDown(CoolDown)
{
}


Action Character::GetActionFromInput(int Input)
{
	switch (Input)
	{
	case 0: return ATTACK;

	case 1: return PARRY;

	case 2: return DEFEND;

	}
	return NONE;
}



