#include "Character.h"
#include <random>


Character::Character(float Health, float AtkPower, float Armor, std::string Name) : Health(Health), AtkPower(AtkPower), Armor(Armor), Name(Name) 
{
}


std::string Character::GetActionFromInput(int Input)
{
	switch (Input)
	{
	case 0:
		return "Attack";

	case 1:
		return "Parry";

	case 2:
		return "Defend";
		return std::string();

	}
}



