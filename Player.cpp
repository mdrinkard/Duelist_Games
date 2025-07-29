#include "Player.h"
#include "Character.h"
//#include "Log.h"
#include <random>
#include <iostream>
#include <raylib.h>


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



Action Player::ChooseAction()
{
    return GetActionFromInput(5);
}

//Action Player::ChooseAction()
//{
//    int Input;
//    while (true)
//    {
//        DrawText("Choose an Action - (1: Attack, 2: Parry, 3: Defend, 4: Heavy Attack, 5: Dodge)", 190, 200, 20, LIGHTGRAY);
//
//        std::cin >> Input;
//
//        if (std::cin.fail() || Input < 1 || Input > 5)
//        {
//            std::cin.clear();
//            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
//            DrawText("Invalid Input: Choose between 1 and 5.", 190, 200, 20, RED);
//
//
//            continue;
//        }
//
//        if ((Input == 4) && !CanExecute())
//        {
//            DrawText("Heavy Attack is on cooldown. Choose another action.", 190, 200, 20, RED);
//
//            continue;
//        }
//
//        if ((Input == 2 || Input == 4 || Input == 5) && Stamina == 0)
//        {
//            DrawText("You are out of stamina for that action!", 190, 200, 20, RED);
//
//            continue;
//        }
//
//        break;
//    }
//
//    Input -= 1;  // Adjust for enum
//    return GetActionFromInput(Input);
//}
