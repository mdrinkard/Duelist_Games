#include "Player.h"
#include "Character.h"
#include "Log.h"
#include <random>
#include <iostream>


Player::Player(float MaxHealth, float AtkPower, float Armor, std::string Name, int CoolDown, int MaxStamina) : Character(MaxHealth, AtkPower, Armor, Name, CoolDown, MaxStamina)
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
        std::cout << "Heavy Attack CoolDown:  " << GetCoolDown() << " turn(s) left\n";
    }
    else
    {
        std::cout << "Heavy Attack is Ready!\n";
    }

    std::cout << "========================\n\n";
}

Action Player::ChooseAction()
{
    int Input;
    while (true)
    {
        Log::LogMessage(LOG_DEFAULT, "Choose an Action - (1: Attack, 2: Parry, 3: Defend, 4: Heavy Attack, 5: Dodge)");
        std::cin >> Input;

        if (std::cin.fail() || Input < 1 || Input > 5)
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            Log::LogMessage(LOG_ERROR, "Invalid Input: Choose between 1 and 5.");
            continue;
        }

        if ((Input == 4) && !CanExecute())
        {
            Log::LogMessage(LOG_ERROR, "Heavy Attack is on cooldown. Choose another action.");
            continue;
        }

        if ((Input == 2 || Input == 4 || Input == 5) && Stamina == 0)
        {
            Log::LogMessage(LOG_ERROR, "You are out of stamina for that action!");
            continue;
        }

        break;
    }

    Input -= 1;  // Adjust for enum
    return GetActionFromInput(Input);
}
