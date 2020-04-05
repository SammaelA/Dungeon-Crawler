#pragma once

#include "GameStructures.h"
#include "EcsComponents.h"

namespace game {

class DamageEffect : public GameEffect {
public:
    void OnCast(ecs::IEntity *entity) override {
        auto vis = entity->GetComp<CompVisibility>();
        if(vis){
            ticks_count = 5;
            state = vis;
        }
    }

    bool OnTick() override {
        if(!state) return true;
        ticks_count--;
        if(ticks_count <= 0) {
            state->visible = true;
            state->hideAfterOneTick = false;
            return true;
        }
        state->visible = !state->visible;
        state->hideAfterOneTick = false;
        return false;
    }

    ~DamageEffect() override {
    }
private:
    int ticks_count = 0;
    CompVisibility *state = nullptr;
};

class PoisonEffect : public GameEffect {
public:
    PoisonEffect(float dmg,float len)
    {
        damage=dmg;
        length=len;
    }

    void OnCast(ecs::IEntity *entity) override 
    {
        owner=entity;
        ticks_count=(int)length;
    }

    void Init(float dmg,float len)
    {
        damage=dmg;
        length=len;
    }
    bool OnTick() override {
        ticks_count--;
        if(ticks_count <= 0) 
        {
            return true;
        }
        CompDamage dam;
        dam.physDamage=damage;
        dam.magicDamage=0.0;
                CompPos *pos = owner->GetComp<CompPos>();
    CompArmor *resist = owner->GetComp<CompArmor>();
    float physResist = 0.0f;
    float magicResist = 0.0f;
    if (resist)
    {
        physResist = resist->physResist;
        magicResist = resist->magicResist;
    }
    CompHealth *h = owner->GetComp<CompHealth>();
    float damageVal = h->value;
    h->value -= stl::max(dam.physDamage * (1.0f - physResist), 0.0f);
    h->value -= stl::max(dam.magicDamage * (1.0f - magicResist), 0.0f);
    damageVal = h->value - damageVal;           
    ecs::gEntityManager.CreateEntity<StatusMessage>(
        std::to_string((int)damageVal), 2, pos->v, sf::Color(0X00FF00FFU));
        return false;
    }

    ~PoisonEffect() override 
    {
    }
private:
    int ticks_count = 1000;
    CompVisibility *state = nullptr;
    float damage=0.0,length=1000.0;
};

class FieryEffect : public GameEffect {
public:
    FieryEffect(float dmg,float len)
    {
        damage=dmg;
        length=len;
    }

    void OnCast(ecs::IEntity *entity) override 
    {
        owner=entity;
        ticks_count=(int)length;
    }

    void Init(float dmg,float len)
    {
        damage=dmg;
        length=len;
    }
    bool OnTick() override {
        ticks_count--;
        if(ticks_count <= 0) 
        {
            return true;
        }
        CompDamage dam;
        dam.physDamage=damage;
        dam.magicDamage=0.0;
                CompPos *pos = owner->GetComp<CompPos>();
    CompArmor *resist = owner->GetComp<CompArmor>();
    float physResist = 0.0f;
    float magicResist = 0.0f;
    if (resist)
    {
        physResist = resist->physResist;
        magicResist = resist->magicResist;
    }
    CompHealth *h = owner->GetComp<CompHealth>();
    float damageVal = h->value;
    h->value -= stl::max(dam.physDamage * (1.0f - physResist), 0.0f);
    h->value -= stl::max(dam.magicDamage * (1.0f - magicResist), 0.0f);
    damageVal = h->value - damageVal;           
    ecs::gEntityManager.CreateEntity<StatusMessage>(
        std::to_string((int)damageVal), 2, pos->v, sf::Color(0XFF7777FFU));
        return false;
    }

    ~FieryEffect() override 
    {
    }
private:
    int ticks_count = 1000;
    CompVisibility *state = nullptr;
    float damage=0.0,length=1000.0;
};

class CurseEffect : public GameEffect {
public:
    CurseEffect(float dmg,float len)
    {
        damage=dmg;
        length=len;
        
    }
    void OnCast(ecs::IEntity *entity) override 
    {
        owner=entity;
        ticks_count=(int)length;
    }
    bool OnTick() override {
        ticks_count--;
        if(ticks_count <= 0) 
        {
            
            CompDamage dam;
            dam.physDamage=damage;
            dam.magicDamage=0.0;
            CompPos *pos = owner->GetComp<CompPos>();
            CompArmor *resist = owner->GetComp<CompArmor>();
            float physResist = 0.0f;
            float magicResist = 0.0f;
            if (resist)
            {
                physResist = resist->physResist;
                magicResist = resist->magicResist;
            }
            CompHealth *h = owner->GetComp<CompHealth>();
            float damageVal = h->value;
            h->value -= stl::max(dam.physDamage * (1.0f - physResist), 0.0f);
            h->value -= stl::max(dam.magicDamage * (1.0f - magicResist), 0.0f);
            damageVal = h->value - damageVal;      
            ecs::gEntityManager.CreateEntity<StatusMessage>(
            std::to_string((int)damageVal), 2, pos->v, sf::Color(0XFF77FFFFU));  
            return true;   
        }
        CompPos *pos = owner->GetComp<CompPos>();
        ecs::gEntityManager.CreateEntity<StatusMessage>(
        "   cursed", 2, pos->v, sf::Color(0XFF77FFFFU));
        return false;
    }

    ~CurseEffect() override 
    {
    }
private:
    int ticks_count = 1000;
    CompVisibility *state = nullptr;
    float damage=0.0,length=1000.0;
};
}