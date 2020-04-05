#include "EcsSystems.h"

template <typename T>
class UznatTip;

void GameTickSys::OnUpdate() {
    ecs::Apply<CompCoolDown>([&](ecs::IEntity *entity)
    {
        auto cd = entity->GetComp<CompCoolDown>();
        if(!cd->is_updating) return;
        if(cd->ticks_count < cd->wait_ticks)
        {
            cd->ticks_count++;
        }
    });
    ecs::Apply<CompTimeToLive>([&](ecs::IEntity *entity)
    {
        auto life = entity->GetComp<CompTimeToLive>();
        life->ticks--;
        if(life->ticks <= 0){
            ecs::gEntityManager.DestroyEntity(entity->GetDesc());
        }
    });

    ecs::Apply<CompActiveEffects>([&](ecs::IEntity *entity)
    {
        if(!entity) return;
        auto effects = entity->GetComp<CompActiveEffects>();
        if(!effects) return;
        effects->list.remove_if([](auto& elem)->bool
        {
            return elem->OnTick();
        });
    });
}