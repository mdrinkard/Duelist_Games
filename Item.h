#pragma once
#include <string>


struct Item {
    std::string Name;
    int BonusHealth = 0;
    int BonusAtk = 0;
    int BonusArmor = 0;
    int DropChance;

    Item(const std::string& name, int hp = 0, int atk = 0, int armor = 0, int DropChance=0)
        : Name(name), BonusHealth(hp), BonusAtk(atk), BonusArmor(armor), DropChance(DropChance)
    {
    }


};
