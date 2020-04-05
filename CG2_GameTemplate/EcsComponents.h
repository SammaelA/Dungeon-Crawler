#pragma once

#include "stl.h"

#include "ECS.h"
#include "GameStructures.h"
#include <SFML/Graphics.hpp>

/*
    Component is any non-standard (not int, double, etc.) type
    of data, which identified by its type. So you can't use several components
    of the same type in one entity.
    If you, for example, want to use int as amount of health, create
    a structure HealthComponent with an int member inside it
    (and then other entities will be able to work with your health
    without knowing what the entity you are).

    Here you can place all of the component types you want to use.
*/

#define FROM_STRING(fmt, ...) \
    void FromString(const char *str) \
    { sscanf(str, fmt, __VA_ARGS__); }

struct CompName : ecs::Component
{ 
    stl::string value;
    void FromString(const char *str)
    { value = stl::string(str); }
};

struct CompPos : ecs::Component
{ 
    sf::Vector2i v = sf::Vector2i(0, 0);
    FROM_STRING("%d/%d", &v.x, &v.y)
};

struct CompDir : ecs::Component
{
    sf::Vector2i v = sf::Vector2i(0, 0);
    FROM_STRING("%d/%d", &v.x, &v.y)
};

struct CompMoveDir : ecs::Component
{
    sf::Vector2i v = sf::Vector2i(0, 0);
    FROM_STRING("%d/%d", &v.x, &v.y)
};

struct CompHealth : ecs::Component
{ 
    float value = 100.0f;
    FROM_STRING("%f", &value)
};

struct CompStamina : ecs::Component
{ 
    float value = 100.0f;
    FROM_STRING("%f", &value)
};

struct CompMana : ecs::Component
{ 
    float value = 100.0f;
    FROM_STRING("%f", &value)
};

struct CompArmor : ecs::Component
{
    // finalDamage = physDamage * (1 - physResist) + magicDamage * (1 - magicResist)
    float physResist = 0.0f;
    float magicResist = 0.0f;
    FROM_STRING("%f/%f", &physResist, &magicResist)
};

struct CompDamage : ecs::Component
{
    float physDamage = 0.0f;
    float magicDamage = 0.0f;
    FROM_STRING("%f/%f", &physDamage, &magicDamage)
};

// How many stamina and mana entity takes.
struct CompConsumption : ecs::Component
{
    float stamina = 0.0f;
    float mana = 0.0f;
    FROM_STRING("%f/%f", &stamina, &mana)
};

struct CompTimeToLive : ecs::Component
{ 
    int ticks = 0;
    FROM_STRING("%d", &ticks)
};

struct CompDrawTileInfo
{ 
    enum
    {
        LAYER_BACKGROUND,
        LAYER_OBJECTS,
        LAYER_EFFECTS,
        LAYERS_COUNT
    };
    unsigned tileId = 0;
    unsigned layer = 0;
    FROM_STRING("%u%u", &tileId, &layer)
};

struct CompAnimInfo : ecs::Component
{
    int id = -1;
    int frame = 0; 
    int frame_count = 1;
    int ticks = 0;
    int update_ticks = 0;
    int layer = 0; 

    void update() {
        ticks++;
        if(ticks >= update_ticks){
            ticks = 0;
            frame++;
            if(frame >= frame_count) frame = 0;
        }
    }
};

struct CompOwnedItems : ecs::Component
{
    stl::vector<const game::Weapon *> weapons;
    stl::vector<const game::Potion *> potions;
    int curWeaponId = -1;
    int curPotionId = -1;
    int potionHotbarStartPosition=0;
    //
    int weaponHotbarStartPosition=0;
    //
    bool potionActive=0;
};

// 2D array of positions.
struct CompPosPattern : ecs::Component
{
    game::PosPattern positions;
};

struct CompActivity : ecs::Component
{
    bool active = false;
    bool disableAfterOneTick = true;
};

struct CompVisibility : ecs::Component
{
    bool visible = true;
    bool hideAfterOneTick = false;
};

struct CompCurrentAttackArea : ecs::Component
{
    ecs::EntityDesc ent = ecs::EntityDesc(ecs::GetEntityTid<void>(), 0);
    sf::Vector2i shift = sf::Vector2i(0, 0);
};

struct CompOwner : ecs::Component
{
    ecs::EntityDesc owner = ecs::EntityDesc(ecs::GetEntityTid<void>(), 0);
};

struct CompParty : ecs::Component
{
    enum
    {
        NEUTRAL,
        ALLIANCE,
        ENEMIES
    };
    uint8_t party;
};

struct CompVisionRadius : ecs::Component
{
    unsigned value = 0;
    FROM_STRING("%u", &value)
};

struct CompCoolDown : ecs::Component {
    unsigned wait_ticks = 1;
    unsigned ticks_count = 0;
    bool is_updating = false;
    
    bool isReady() { return ticks_count == wait_ticks; }
    void reset() { ticks_count = 0; is_updating = true; }
    void stop() { ticks_count = 0; is_updating = false; }

    FROM_STRING("%u", &wait_ticks)
};

struct CompActiveEffects : ecs::Component {
    std::list<std::unique_ptr<game::GameEffect>> list;
};
struct CompPassiveEffects : ecs::Component {
    std::list<std::function<std::unique_ptr<game::GameEffect>()>> list;
};
struct CompFloatingText : ecs::Component {
    int tid;
};

struct TagCollidable : ecs::Component {};

struct TagCollectable : ecs::Component {};

struct TagPlayer : ecs::Component {};

// Player is ally too.
// It is for enemies to know, whom to attack.
struct TagAlly : ecs::Component {};

struct TagEnemy : ecs::Component {};

struct TagDestroyOnCollision : ecs::Component {};

struct TagWall : ecs::Component {};

#ifndef ECS_REGISTER_COMPONENT
#define ECS_REGISTER_COMPONENT(compName)
#endif

/*
    Register here all of the entity types
    you want to create by string names (and therefore from files).
*/

ECS_REGISTER_COMPONENT(CompName)
ECS_REGISTER_COMPONENT(CompPos)
ECS_REGISTER_COMPONENT(CompDir)
ECS_REGISTER_COMPONENT(CompMoveDir)
ECS_REGISTER_COMPONENT(CompHealth)
ECS_REGISTER_COMPONENT(CompStamina)
ECS_REGISTER_COMPONENT(CompMana)
ECS_REGISTER_COMPONENT(CompArmor)
ECS_REGISTER_COMPONENT(CompDamage)
ECS_REGISTER_COMPONENT(CompConsumption)
ECS_REGISTER_COMPONENT(CompTimeToLive)
ECS_REGISTER_COMPONENT(CompDrawTileInfo)
ECS_REGISTER_COMPONENT(CompOwnedItems)
ECS_REGISTER_COMPONENT(CompPosPattern)
ECS_REGISTER_COMPONENT(CompActivity)
ECS_REGISTER_COMPONENT(CompVisibility)
ECS_REGISTER_COMPONENT(CompCurrentAttackArea)
ECS_REGISTER_COMPONENT(CompOwner)
ECS_REGISTER_COMPONENT(CompParty)
ECS_REGISTER_COMPONENT(CompVisionRadius)
ECS_REGISTER_COMPONENT(CompCoolDown)