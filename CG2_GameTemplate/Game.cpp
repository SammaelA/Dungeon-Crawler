#include "Game.h"
#include "stl.h"
#include "GameStructures.h"
#include "GameDataLoad.h"
#include "ECS.h"
#include "Timer.h"
#include "EcsSystems.h"
#include "WeaponsCatalogue.h"
#include "PotionsCatalogue.h"
#include "Renderer.h"
#include "GameLevelData.h"
#include "GameResources.h"

namespace game
{

int gstate=STATE_START; 
LevelData gLevelData;

static bool swordPattern[3 * 2] = {
    0, 0,
    1, 1,
    0, 0
};

static bool clubPattern[3 * 2] = {
    1, 0,
    0, 1,
    1, 0
};

stl::vector<Weapon> gGameWeapons;
stl::vector<Potion> gGamePotions;
#define UPDATE_PERIOD 0.1f

void Game::Init(const stl::shared_ptr<sf::RenderWindow> &window)
{
    wc.Init(0);
    pc.Init(0);
    this->window = window;
    gRenderer.Init(window);
    SendEvent(EventRendererWasInited());
}

void Game::Release()
{
    //window.reset();
    gRenderer.Release();
    gGameWeapons.clear();
    ecs::gEntityManager.DestroyAllEntities();
}

GameResults Game::StartLevel(const char *levelFilename)
{
    if(!loadResources("resources.txt")){
        std::cout << "game load failed\n";
        return GameResults{GameResults::FAILED};
    }
    gLevelData = LoadLevel(levelFilename);
    if(!loadAnimatedObjects("level1/level1.png")){
        std::cout << "game load failed\n";
        return GameResults{GameResults::FAILED};
    }
    ecs::Apply<CompOwnedItems>([&](ecs::IEntity *ent) {
        CompOwnedItems *ammo = ent->GetComp<CompOwnedItems>();
        
            ammo->potions.push_back(pc.getRandom());
            ammo->weapons.push_back(wc.getRandom());
    });
    gTime.Reset();
    volatile float lastTime = gTime.GetTime();
    while (window->isOpen())
    {
        gRenderer.BeginDraw(sf::Color(0x000000FFU));
        ecs::gEntityManager.UpdateEntities();
        gRenderer.EndDraw();
        auto delta = gTime.GetTime() - lastTime;
        if(delta < UPDATE_PERIOD){
            sf::sleep(sf::milliseconds(1000.0 * (UPDATE_PERIOD - delta)));
        }
        lastTime = gTime.GetTime();
    }
    return GameResults();
}

}