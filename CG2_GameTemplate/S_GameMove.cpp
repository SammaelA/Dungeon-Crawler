#include "EcsSystems.h"

static void MergeInventories(CompOwnedItems *dst, CompOwnedItems *src){
    if(src->weapons.size())
    {
        dst->weapons.reserve(dst->weapons.size() + src->weapons.size());
        dst->weapons.insert(dst->weapons.end(), src->weapons.begin(),
            src->weapons.end()); 
    }
    if(src->potions.size())
    {
        dst->potions.reserve(dst->potions.size() + src->potions.size());
        dst->potions.insert(dst->potions.end(), src->potions.begin(),
            src->potions.end()); 
    }
}

void MoveMovables()
{
    ecs::Apply<CompPos, CompMoveDir>([&](ecs::IEntity *movable)
    {
        CompMoveDir *dir = movable->GetComp<CompMoveDir>();
        CompPos *pos = movable->GetComp<CompPos>();

        if (movable->GetComp<TagCollidable>())
        {
            bool canMove = true;
            ecs::Apply<TagCollidable>([&](ecs::IEntity *collidable)
            {
                CompPos *colPos = collidable->GetComp<CompPos>();
                CompPosPattern *colPattern = collidable->GetComp<CompPosPattern>();
                if (colPos)
                {
                    if (pos->v + dir->v == colPos->v)
                        canMove = false;
                }
                else if (colPattern)
                {
                    if (colPattern->positions.IsPosExists(pos->v.x + dir->v.x, pos->v.y + dir->v.y))
                        canMove = false;
                }
            });
            if (canMove)
            {
                pos->v += dir->v;
            } else { /*Уничтожение снарядов, столкнувшихся со стеной*/
                if(movable->GetComp<TagDestroyOnCollision>()){
                    ecs::gEntityManager.DestroyEntity(movable->GetDesc());
                    return;
                }
            }
        }
        else
        {
            pos->v += dir->v;
        }
        if(movable->GetComp<TagPlayer>()){
            ecs::Apply<TagCollectable, CompPos, CompOwnedItems>([&](ecs::IEntity *ent)
            {
                auto collpos = ent->GetComp<CompPos>();
                if(pos->v == collpos->v)
                {
                    auto playerInv = movable->GetComp<CompOwnedItems>();
                    auto items = ent->GetComp<CompOwnedItems>();
                    MergeInventories(playerInv, items);
                    ecs::EntityDesc desc = ent->GetDesc(); 
                    ecs::gEntityManager.DestroyEntity(desc);
                }
            });
        }
    });
}

void MoveAttackArea()
{
    ecs::Apply<CompPos, CompCurrentAttackArea>([&](ecs::IEntity *ent)
    {
        CompCurrentAttackArea *areaComp = ent->GetComp<CompCurrentAttackArea>();
        ecs::IEntity *area = ecs::gEntityManager.GetEntityByDesc(areaComp->ent);
        area->GetComp<CompPosPattern>()->positions.pos = ent->GetComp<CompPos>()->v + areaComp->shift;
    });
}

void GameMoveSys::OnUpdate()
{
    MoveMovables();
    MoveAttackArea();
}