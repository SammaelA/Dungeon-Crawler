#include "GameResources.h"
#include "ECS.h"
#include "EcsEntities.h"
#include "EcsComponents.h"
#include "EnemiesCatalogue.h"
#include "WeaponsCatalogue.h"
#include "PotionsCatalogue.h"


namespace game {
static bool loadPlayer(sf::Vector2f pos) {
    auto desc = ecs::gEntityManager.CreateEntity<Player>();
    auto player = ecs::gEntityManager.GetEntityByDesc(desc);
    player->GetComp<CompPos>()->v = sf::Vector2i(pos);
    player->GetComp<CompDir>()->v = sf::Vector2i(0, 1);
    player->GetComp<CompHealth>()->value = 100;
    player->GetComp<CompMana>()->value = 100;
    player->GetComp<CompStamina>()->value = 100;
    CompAnimInfo anim;
    anim.frame = anim.ticks = 0;
    anim.layer = CompDrawTileInfo::LAYER_OBJECTS;
    anim.id = gNamedAnimations["player_anim"];
    anim.frame_count = gRenderer.GetAnimFramesCount(anim.id);
    anim.update_ticks = 4;
    *player->GetComp<CompAnimInfo>() = anim;
    return true;
}


static bool loadEnemy(sf::Vector2f pos) {
    auto desc = ecs::gEntityManager.CreateEntity<Enemy>();
    auto enemy = ecs::gEntityManager.GetEntityByDesc(desc);
    enemy->GetComp<CompPos>()->v = sf::Vector2i(pos);
    enemy->GetComp<CompHealth>()->value = 30;
    enemy->GetComp<CompDamage>()->magicDamage = 10;
    enemy->GetComp<CompDamage>()->physDamage = 10;
    enemy->GetComp<CompVisionRadius>()->value = 8;
    CompAnimInfo anim;
    anim.frame = anim.ticks = 0;
    anim.layer = CompDrawTileInfo::LAYER_OBJECTS;
    anim.id = gNamedAnimations["enemy_anim"];
    anim.frame_count = gRenderer.GetAnimFramesCount(anim.id);
    anim.update_ticks = 4;
    *enemy->GetComp<CompAnimInfo>() = anim;
    return true;
}

bool loadStartItems(std::vector<sf::Vector2f> &verts){
    if(verts.size() != 3){
        return false;
    }
    for(int i = 0; i < 3; i++){
        auto desc = ecs::gEntityManager.CreateEntity<Collectable>();
        auto ent = ecs::gEntityManager.GetEntityByDesc(desc);
        ent->GetComp<CompPos>()->v = sf::Vector2i(verts[i]);
        ent->GetComp<CompDrawTileInfo>()->layer = CompDrawTileInfo::LAYER_OBJECTS;
        ent->GetComp<CompDrawTileInfo>()->tileId = gNamedTiles["start_chest"];
        auto items = ent->GetComp<CompOwnedItems>();
        switch (i)
        {
        case 0:
            items->weapons.push_back(wc.getWeapon(0));
            break;
        case 1:
        case 2:
            items->potions.push_back(pc.getRandom());
        }
    }
    return true;
}   

bool loadRandomChest(sf::Vector2f pos){
    auto desc = ecs::gEntityManager.CreateEntity<Collectable>();
    auto ent = ecs::gEntityManager.GetEntityByDesc(desc);
    ent->GetComp<CompPos>()->v = sf::Vector2i(pos);
    ent->GetComp<CompDrawTileInfo>()->layer = CompDrawTileInfo::LAYER_OBJECTS;
    ent->GetComp<CompDrawTileInfo>()->tileId = gNamedTiles["chest"];
    if(std::rand() % 2){
        ent->GetComp<CompOwnedItems>()->potions.push_back(pc.getRandom());
    } else {
        ent->GetComp<CompOwnedItems>()->weapons.push_back(wc.getRandom());
    }
    return true;
}
bool loadNecromancer(sf::Vector2f pos)
{
    auto desc = ecs::gEntityManager.CreateEntity<Necromancer>();
    auto enemy = ecs::gEntityManager.GetEntityByDesc(desc);
    enemy->GetComp<CompPos>()->v = sf::Vector2i(pos);
    enemy->GetComp<CompHealth>()->value = 200;
    enemy->GetComp<CompDamage>()->magicDamage = 10;
    enemy->GetComp<CompDamage>()->physDamage = 30;
    enemy->GetComp<CompVisionRadius>()->value = 8;
    CompAnimInfo anim;
    anim.frame = anim.ticks = 0;
    anim.layer = CompDrawTileInfo::LAYER_OBJECTS;
    anim.id = game::gNamedAnimations["necr_anim"];
    anim.frame_count = game::gRenderer.GetAnimFramesCount(anim.id);
    anim.update_ticks = 4;
    *enemy->GetComp<CompAnimInfo>() = anim;

    enemy->GetComp<CompOwnedItems>()->weapons.push_back(game::wc.getWeapon(11));
    enemy->GetComp<CompOwnedItems>()->weapons.push_back(game::wc.getWeapon(12));
    enemy->GetComp<CompOwnedItems>()->weapons.push_back(game::wc.getWeapon(13));
    enemy->GetComp<CompOwnedItems>()->weapons.push_back(game::wc.getWeapon(14));
    enemy->GetComp<CompOwnedItems>()->weapons.push_back(game::wc.getWeapon(15));
    enemy->GetComp<CompOwnedItems>()->curWeaponId=0;
    return(true);
}
bool loadCharger(sf::Vector2f pos){
    loadNecromancer(pos);
    auto desc = ecs::gEntityManager.CreateEntity<Charger>();
    auto enemy = ecs::gEntityManager.GetEntityByDesc(desc);
    enemy->GetComp<CompPos>()->v = sf::Vector2i(pos);
    enemy->GetComp<CompHealth>()->value = 200;
    enemy->GetComp<CompVisionRadius>()->value = 15;
    CompAnimInfo anim;
    anim.frame = anim.ticks = 0;
    anim.layer = CompDrawTileInfo::LAYER_OBJECTS;
    anim.id = gNamedAnimations["charger_anim"];
    anim.frame_count = gRenderer.GetAnimFramesCount(anim.id);
    anim.update_ticks = 4;
    *enemy->GetComp<CompAnimInfo>() = anim;
    enemy->GetComp<CompDamage>()->physDamage = 30;
    return true;
}

bool loadTeleportPoint(sf::Vector2f pos){
    auto desc = ecs::gEntityManager.CreateEntity<TeleportPoint>();
    auto ent = ecs::gEntityManager.GetEntityByDesc(desc);
    ent->GetComp<CompPos>()->v = sf::Vector2i(pos);
    return true;
}

bool loadFireDemon(sf::Vector2f pos){
    auto desc = ecs::gEntityManager.CreateEntity<FireDemon>();
    auto enemy = ecs::gEntityManager.GetEntityByDesc(desc);
    enemy->GetComp<CompPos>()->v = sf::Vector2i(pos);
    enemy->GetComp<CompHealth>()->value = 200;
    enemy->GetComp<CompVisionRadius>()->value = 10;
    CompAnimInfo anim;
    anim.frame = anim.ticks = 0;
    anim.layer = CompDrawTileInfo::LAYER_OBJECTS;
    anim.id = gNamedAnimations["firedemon_anim"];
    anim.frame_count = gRenderer.GetAnimFramesCount(anim.id);
    anim.update_ticks = 4;
    *enemy->GetComp<CompAnimInfo>() = anim;
    enemy->GetComp<CompDamage>()->physDamage = 10;
    enemy->GetComp<CompDamage>()->magicDamage = 20;
    return true;
}

bool loadAnimatedObjects(const std::string& s) {
    //setSpawnPattern(0xffff00, loadStartItems);
    setSpawnSingle(0x00ff00, loadRandomChest);
    setSpawnSingle(0xff0000, EnemiesCatalogue::loadEnemy);
    setSpawnSingle(0x6900ff, loadPlayer);
    setSpawnSingle(0x00ffff, loadNecromancer);
    //setSpawnSingle(0xff00b6, loadFireDemon);
    //setSpawnSingle(0x006d00, loadTeleportPoint);
    if(!loadLevelMap(s)){
        std::cout << "load failed\n";
        return false;
    }
    return true;
}

}