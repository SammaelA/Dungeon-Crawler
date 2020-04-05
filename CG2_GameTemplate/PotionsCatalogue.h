#pragma once
#include "stl.h"
#include "EcsSystems.h"
#include "Utils.h"
#include "GameStructures.h"
#include "Game.h"
namespace game
{
    class PotionsCatalogue
{
    public:
    Potion *getPotion(int ID);
    Potion *getRandom();
    void Init(int type);
};
extern PotionsCatalogue pc;
}