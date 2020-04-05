#include "EcsSystems.h"
#include "stl.h"
#include "GameEffects.h"

void GameAttackSys::Attack(ecs::IEntity *creature,CompDamage *damage,sf::Color color)
{
    CompPos *pos = creature->GetComp<CompPos>();
    CompArmor *resist = creature->GetComp<CompArmor>();
    float physResist = 0.0f;
    float magicResist = 0.0f;
    if (resist)
    {
        physResist = resist->physResist;
        magicResist = resist->magicResist;
    }
    CompHealth *h = creature->GetComp<CompHealth>();
    float damageVal = h->value;
    h->value -= damage->physDamage * (1.0f - physResist);
    h->value -= damage->magicDamage * (1.0f - magicResist);
    damageVal = h->value - damageVal;           
    ecs::gEntityManager.CreateEntity<StatusMessage>(
        std::to_string((int)damageVal), 2, pos->v, color);
        if(creature->GetComp<TagPlayer>()){
        auto &effects = creature->GetComp<CompActiveEffects>()->list;
        effects.push_front(std::make_unique<game::DamageEffect>());
        effects.front()->OnCast(creature);
    }
}

void GameAttackSys::OnUpdate()
{
    ecs::Apply<CompHealth>([&](ecs::IEntity *creature)
        {
            auto h=creature->GetComp<CompHealth>();
            if (h->value <= 0.0f) 
            {
                ecs::gEntityManager.DestroyEntity(creature->GetDesc());
            }
        });
    //костыль - если нужно создать AttackArea, которая помрет в следующий же ход, проставляем в графе 
    //владельца кого-нибудь, имеющего тег DestroyOnCollision Такая зона будет уничтожена после первого же применения
    ecs::Apply<CompPosPattern, CompActivity, CompDamage, CompOwner>([&](ecs::IEntity *area)
    {
        CompActivity *act = area->GetComp<CompActivity>();
        if (!act->active)
            return;
        if (act->disableAfterOneTick)
            act->active = false;
        CompPosPattern *positions = area->GetComp<CompPosPattern>();
        

        ecs::Apply<CompPos, CompHealth, CompArmor>([&](ecs::IEntity *creature)
        {
            CompParty *creatParty = creature->GetComp<CompParty>();
            CompParty *ownerParty = area->GetComp<CompParty>();
            if (creatParty && ownerParty && creatParty->party == ownerParty->party
                && ownerParty->party!=ownerParty->NEUTRAL)
                return;
            auto color = (creature->GetComp<TagEnemy>())? sf::Color::White : sf::Color::Red;
            CompPos *pos = creature->GetComp<CompPos>();
            CompDamage *damage = area->GetComp<CompDamage>();
            if (positions->positions.IsPosExists(pos->v.x, pos->v.y))
            {
                Attack(creature,damage,color);
                auto& area_effects = area->GetComp<CompPassiveEffects>()->list;
                auto& effects = creature->GetComp<CompActiveEffects>()->list;
                for(auto& cons : area_effects)
                {
                    effects.push_front(cons());
                    effects.front()->OnCast(creature);
                }
            }
        });
        ecs::IEntity *owner = ecs::gEntityManager.GetEntityByDesc(area->GetComp<CompOwner>()->owner);
        if (owner==NULL)
        {
            ecs::gEntityManager.DestroyEntity(area->GetDesc());
        }
    });
}