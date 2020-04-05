#pragma once

/*
    In current realisation, entity is something that
    have components and can work and interact with
    other entities through 'Update()' (called on each frame update).

    If you want to initialize an entity, use OnCreated() instead of constructor.

    Here you can place all of the entity types you want to use.
*/

#include "ECS.h"
#include "EcsComponents.h"
#include <iostream>

class Player : public ecs::Entity<
    CompPos,
    CompDir,
    CompMoveDir,
    CompHealth,
    //CompDrawTileInfo,
    CompAnimInfo,
    CompOwnedItems,
    CompCurrentAttackArea,
    CompStamina,
    CompMana,
    CompArmor,
    CompParty,
    CompVisibility, // May be used for hide player (feature).
    //TagAlly,
    CompActiveEffects,
    TagCollidable,
    TagPlayer>
{
public:
    ECS_ENTITY_DEFAULTS(Player)
    void OnCreated() override;
    void OnDestroy() override;
    void OnUpdate() override;
};

class AttackArea : public ecs::Entity<
    CompCoolDown,
    CompPosPattern,
    CompActivity,
    CompDamage,
    CompOwner,
    CompVisibility,
    CompPassiveEffects,
    CompDrawTileInfo,
    CompParty>
{
public:
    ECS_ENTITY_DEFAULTS(AttackArea)
};

class Walls : public ecs::Entity<
    CompPosPattern,
    CompDrawTileInfo,
    TagCollidable,
    TagWall>
{
public:
    ECS_ENTITY_DEFAULTS(Walls)
};

class Floor : public ecs::Entity<
    CompPosPattern,
    CompDrawTileInfo>
{
public:
    ECS_ENTITY_DEFAULTS(Floor)
};

// Ususal enemies have no stamina and mana.
class Enemy : public ecs::Entity<
    CompPos,
    CompMoveDir,
    CompDir,
    CompHealth,
    CompAnimInfo,
    CompCurrentAttackArea,
    CompArmor,
    CompDamage,
    CompParty,
    CompVisionRadius,
    CompActiveEffects,
    TagEnemy,
    TagCollidable,
    CompOwnedItems>
{
public:
    ECS_ENTITY_DEFAULTS(Enemy)
    void OnCreated() override;
    void OnDestroy() override;
    void OnUpdate() override;
    unsigned curTick = 0;
private:
    
};

class Charger : public ecs::Entity<
    CompPos,
    CompMoveDir,
    CompHealth,
    CompAnimInfo,
    CompCurrentAttackArea,
    CompArmor,
    CompDamage,
    CompParty,
    CompVisionRadius,
    CompActiveEffects,
    TagEnemy,
    TagCollidable>
{
public:
    ECS_ENTITY_DEFAULTS(Charger)
    void OnCreated() override;
    void OnDestroy() override;
    void OnUpdate() override;
private:
    enum {CHARGING, IDLE, PREPARATION} state = IDLE;
    unsigned curTick = 0;
    const int chargeTime = 7;
    const int idleTime = 20;
    const int prepTime = 10;
    game::Weapon weapon;
};

class FireDemon : public ecs::Entity<
    CompPos,
    CompMoveDir,
    CompHealth,
    CompAnimInfo,
    CompArmor,
    CompDamage,
    CompParty,
    CompVisionRadius,
    CompActiveEffects,
    TagEnemy,
    TagCollidable>
{
public:
    ECS_ENTITY_DEFAULTS(FireDemon)
    void OnCreated() override;
    void OnDestroy() override;
    void OnUpdate() override;
private:
    enum {SHOOTING, IDLE, PREPARATION} state = IDLE;
    unsigned curTick = 0;
    const int shootTime = 10;
    const int idleTime = 20;
    const int prepTime = 10; 
    int tpCount = 5;
    float lastHealth;
    CompAnimInfo fireballAnim, teleportAnim;
}; 

class Necromancer : public ecs::Entity<
    CompPos,
    CompMoveDir,
    CompHealth,
    CompAnimInfo,
    CompArmor,
    CompDamage,
    CompOwnedItems,
    CompParty,
    CompVisionRadius,
    CompActiveEffects,
    CompCurrentAttackArea,
    TagEnemy,
    TagCollidable>
{
public:
    ECS_ENTITY_DEFAULTS(Necromancer)
    void OnCreated() override;
    void OnDestroy() override;
    void OnUpdate() override;
private:
    enum {SLEEPING,WEAK,MEDIUM,STRONG,INSANE} state=SLEEPING;
    enum {WEAK_COOLDOWN=100,MEDIUM_COOLDOWN=200,STRONG_COOLDOWN=300};
    unsigned curTick = 0;
    unsigned coolTick=0;
};

class Collectable : public ecs::Entity<
    CompPos,
    CompOwnedItems,
    CompDrawTileInfo,
    TagCollectable>
{
public:
    ECS_ENTITY_DEFAULTS(Collectable)
};

class StatusMessage : public ecs::Entity<
    CompTimeToLive,
    CompFloatingText,
    CompPos>
{
public:
    ECS_ENTITY_DEFAULTS(StatusMessage)
    StatusMessage(const std::string& smsg, int lifetime, sf::Vector2i &pos, sf::Color color = sf::Color::Red);

    void OnCreated() override;
    void OnDestroy() override;
private:
    std::string msg;
    sf::Color mcolor;
};

/*Снаряд (лук/магия) Пока только летит, и ломается об стены*/
class Missle : public ecs::Entity<
    CompDrawTileInfo,
    CompPos,
    CompMoveDir,
    CompDamage,
    CompParty,
    TagDestroyOnCollision,
    TagCollidable>
{
public:
    ECS_ENTITY_DEFAULTS(Missle)
    Missle(int tileid, const sf::Vector2i &pos, const sf::Vector2i &dir, 
    const CompDamage & dmg);
    void OnDestroy() override;
    /*Удобный конструктор*/
};

class FireBall : public ecs::Entity<
    CompAnimInfo,
    CompPos, 
    CompMoveDir,
    CompDamage,
    CompHealth,
    CompParty>
{
public:
    ECS_ENTITY_DEFAULTS(FireBall)
    FireBall(const CompAnimInfo &anim, const sf::Vector2i &pos, const sf::Vector2i &dst, const CompDamage &dmg);
    void OnUpdate() override;
    void OnDestroy() override;
    void OnCreated() override;
private:
    sf::Vector2i dstPos;
};

class Decoration : public ecs::Entity<
    CompTimeToLive,
    CompAnimInfo,
    CompPos>
{
public:
    ECS_ENTITY_DEFAULTS(Decoration)
    Decoration(int aId, int frameC, int fticks, int layer, int lifetime, sf::Vector2i pos){
        auto anim = GetComp<CompAnimInfo>();
        anim->id = aId;
        anim->frame_count = frameC;
        anim->update_ticks = fticks;
        anim->layer = layer;
        GetComp<CompPos>()->v = pos;
        GetComp<CompTimeToLive>()->ticks = lifetime;
    }
    Decoration(const CompAnimInfo &anim, int lifetime, sf::Vector2i pos){
        *GetComp<CompAnimInfo>() = anim;
        GetComp<CompPos>()->v = pos;
        GetComp<CompTimeToLive>()->ticks = lifetime;
    }
};

class TeleportPoint : public ecs::Entity<
    CompPos>
{
public:
    ECS_ENTITY_DEFAULTS(TeleportPoint)
};


#ifndef ECS_REGISTER_ENTITY_TYPE
#define ECS_REGISTER_ENTITY_TYPE(typeName)
#endif

/*
    Register here all of the entity types
    you want to create by string names (and therefore from files).
*/
ECS_REGISTER_ENTITY_TYPE(Collectable)
ECS_REGISTER_ENTITY_TYPE(Player)
ECS_REGISTER_ENTITY_TYPE(AttackArea)
ECS_REGISTER_ENTITY_TYPE(Walls)
ECS_REGISTER_ENTITY_TYPE(Floor)
ECS_REGISTER_ENTITY_TYPE(Enemy)
