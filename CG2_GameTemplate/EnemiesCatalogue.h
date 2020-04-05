#pragma once
#include "GameResources.h"
#include "ECS.h"
#include "EcsEntities.h"
#include "EcsComponents.h"
namespace game{
    class EnemiesCatalogue
    {
        public:
        static bool loadEnemy(sf::Vector2f pos);
        static bool loadRangedEnemy(sf::Vector2f pos);
        static bool loadYourEvilCopy(sf::Vector2f pos);
        static bool loadGuardian(sf::Vector2f pos);
        static void UpdateBasicEnemy(Enemy *ent);
        static void UpdateRangedEnemy(Enemy *ent);
        static void UpdateYourEvilCopy(Enemy *ent);
        static void UpdateGuardian(Enemy *ent);
    };
}