#include "Player.h"
#include "Character.h"
#include <random>
#include <iostream>


Player::Player(float Health, float AtkPower, float Armor, std::string Name) : Character(Health, AtkPower, Armor, Name)
{

}

float Player::GetHealth() const {
    return Character::GetHealth() + EquippedItem.BonusHealth;
}
float Player::GetAtkPower() const {
    return Character::GetAtkPower() + EquippedItem.BonusAtk;
}
float Player::GetArmor() const {
    return Character::GetArmor() + EquippedItem.BonusArmor;
}

void Player::GainExperience(int amount)
{
    Experience += amount;
    std::cout << "Gained " << amount << " XP! Total XP: " << Experience << "/" << ExperienceToLevel << "\n";

    while (Experience >= ExperienceToLevel)
    {
        Experience -= ExperienceToLevel;
        LevelUp();
    }
}

void Player::LevelUp()
{
    Level++;
    ExperienceToLevel = static_cast<int>(ExperienceToLevel * 1.5);  // Increase XP needed for next level

    // Increase stats on level up
    SetHealth(10.0 + (5.0 * Level));
    AtkPower += 2;
    Armor += 1;

    std::cout << "LEVEL UP! You are now level " << Level << "!\n";
    std::cout << "Stats increased! Health: " << Health << ", Attack: " << AtkPower << ", Armor: " << Armor << "\n";
}

void Player::EquipItem(const Item& item) {

    Health -= EquippedItem.BonusHealth;
    AtkPower -= EquippedItem.BonusAtk;
    Armor -= EquippedItem.BonusArmor;


    EquippedItem = item;


    Health += EquippedItem.BonusHealth;
    AtkPower += EquippedItem.BonusAtk;
    Armor += EquippedItem.BonusArmor;

    std::cout << "Equipped " << item.Name << "! Stats updated: "
        << "Health: " << Health << ", Attack: " << AtkPower << ", Armor: " << Armor << "\n";
}


void Player::DisplayStatus() const
{
    std::cout << "\n===== HERO STATUS =====\n";
    std::cout << "Name: " << GetName() << "\n";
    std::cout << "Level: " << GetLevel() << " (XP: " << GetExperience() << "/" << GetExperienceToLevel() << ")\n";
    std::cout << "Health: " << GetHealth() << "\n";
    std::cout << "Attack Power: " << GetAtkPower() << "\n";
    std::cout << "Armor: " << GetArmor() << "\n";

    std::cout << "Equipped Item: " << EquippedItem.Name << "\n";
    if (EquippedItem.Name != "None")
    {
        std::cout << "  +HP: " << EquippedItem.BonusHealth
            << ", +ATK: " << EquippedItem.BonusAtk
            << ", +ARM: " << EquippedItem.BonusArmor << "\n";
    }

    if (!CanExecute())
    {
        std::cout << "Parry Cooldown: " << GetCoolDown() << " turn(s) left\n";
    }
    else
    {
        std::cout << "Parry is ready\n";
    }

    std::cout << "========================\n\n";
}

std::string Player::ChooseAction()
{
	int Input;
	while (true)
    {
        std::cout << "Choose an Action - (1: Attack, 2: Parry, 3: Defend)" << "\n";
        if (!CanExecute())
            std::cout << "Parry is on cooldown for " << GetCoolDown() << " more turn(s)!\n";

        std::cin >> Input;

        if (std::cin.fail() || Input < 1 || Input > 3)
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid Input: You must enter 1, 2, or 3 (Attack, Parry, Defend)" << "\n";
            continue;
        }

        if (Input == 2 && !CanExecute())
        {
            std::cout << "You can't use Parry right now. Choose another action.\n";
            continue;
        }
        break;
    }
	Input -= 1;

	return GetActionFromInput(Input);

}
