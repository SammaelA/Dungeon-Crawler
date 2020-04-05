#pragma once
#include "stl.h"
#include "EcsSystems.h"
#include "Utils.h"
#include "GameStructures.h"
#include "Game.h"
namespace game
{
    class WeaponsCatalogue
{
    public:
    Weapon *getWeapon(int ID);
    Weapon *getRandom();
    void Init(int type);
};
extern WeaponsCatalogue wc;
}