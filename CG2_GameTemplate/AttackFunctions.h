#pragma once

#include "GameStructures.h"
#include "EcsEntities.h"
#include "Game.h"
#include "GameEffects.h"
namespace game
{
    enum
    {
        SIMPLE=1,
        MAGIC_BALL=2,
        POISONOUS=3,
        POISONOUS_WEAK=4,
        POISON_SPELL=5,
        FIERY=6,
        CURSE=7,
        NECROMANCERS_WEAK=8,
        NECROMANCERS_MEDIUM=9,
        NECROMANCERS_STRONG=10,
        NECROMANCERS_GUARDIANS=11,
        FIREBALL_SPELL=12,
        ARROW_SPELL=13,
        BOLT_SPELL=14
    };
     class AttackFuncts
    {
        public:
        static bool SimpleAttack(const game::Weapon *w,ecs::IEntity *attacker,ecs::IEntity *area);
        static bool MagicBalls(const game::Weapon *w,ecs::IEntity *attacker,ecs::IEntity *area);
        static bool PoisonousAttack(const game::Weapon *w,ecs::IEntity *attacker,ecs::IEntity *area);
        static bool Attack(int attack_type,const game::Weapon *w,ecs::IEntity *attacker,ecs::IEntity *area);
        static bool Poison(const game::Weapon *w,ecs::IEntity *attacker,ecs::IEntity *area);
        static bool FieryAttack(const game::Weapon *w,ecs::IEntity *attacker,ecs::IEntity *area);
        static bool SpellCurse(const game::Weapon *w,ecs::IEntity *attacker,ecs::IEntity *area);
        static bool NecromancersAttackWeak(const game::Weapon *w,ecs::IEntity *attacker,ecs::IEntity *area);
        static bool NecromancersAttackMedium(const game::Weapon *w,ecs::IEntity *attacker,ecs::IEntity *area);
        static bool NecromancersAttackStrong(const game::Weapon *w,ecs::IEntity *attacker,ecs::IEntity *area);
        static bool NecromancersAttackGuardians(const game::Weapon *w,ecs::IEntity *attacker,ecs::IEntity *area);
        static bool FireBall(const game::Weapon *w,ecs::IEntity *attacker,ecs::IEntity *area);
        static bool Arrow(const game::Weapon *w,ecs::IEntity *attacker,ecs::IEntity *area);
        static bool Bolt(const game::Weapon *w,ecs::IEntity *attacker,ecs::IEntity *area);
    };
}