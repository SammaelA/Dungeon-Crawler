#include "EnemiesCatalogue.h"
#include "AttackFunctions.h"
#include "WeaponsCatalogue.h"
#include "math.h"
enum{
    ENEMY_ATTACK_COOLDOWN=3,
    RANGED_ENEMY_ATTACK_COOLDOWN=10
};
namespace game {
bool EnemiesCatalogue::loadEnemy(sf::Vector2f pos) {
    auto desc = ecs::gEntityManager.CreateEntity<Enemy>();
    auto enemy = ecs::gEntityManager.GetEntityByDesc(desc);
    enemy->GetComp<CompPos>()->v = sf::Vector2i(pos);
    enemy->GetComp<CompHealth>()->value = 30;
    enemy->GetComp<CompDamage>()->magicDamage = 10;
    enemy->GetComp<CompDamage>()->physDamage = 30;
    enemy->GetComp<CompVisionRadius>()->value = 8;
    CompAnimInfo anim;
    anim.frame = anim.ticks = 0;
    anim.layer = CompDrawTileInfo::LAYER_OBJECTS;
    anim.id = gNamedAnimations["enemy_anim"];
    anim.frame_count = gRenderer.GetAnimFramesCount(anim.id);
    anim.update_ticks = 4;
    *enemy->GetComp<CompAnimInfo>() = anim;

    enemy->GetComp<CompOwnedItems>()->weapons.push_back(wc.getWeapon(4));
    enemy->GetComp<CompOwnedItems>()->curWeaponId=4;
    return true;
}
bool EnemiesCatalogue::loadRangedEnemy(sf::Vector2f pos) {
    auto desc = ecs::gEntityManager.CreateEntity<Enemy>();
    auto enemy = ecs::gEntityManager.GetEntityByDesc(desc);
    enemy->GetComp<CompPos>()->v = sf::Vector2i(pos);
    enemy->GetComp<CompHealth>()->value = 30;
    enemy->GetComp<CompDamage>()->magicDamage = 10;
    enemy->GetComp<CompDamage>()->physDamage = 30;
    enemy->GetComp<CompVisionRadius>()->value = 8;
    CompAnimInfo anim;
    anim.frame = anim.ticks = 0;
    anim.layer = CompDrawTileInfo::LAYER_OBJECTS;
    anim.id = gNamedAnimations["witch_anim"];
    anim.frame_count = gRenderer.GetAnimFramesCount(anim.id);
    anim.update_ticks = 4;
    *enemy->GetComp<CompAnimInfo>() = anim;

    enemy->GetComp<CompOwnedItems>()->weapons.push_back(wc.getWeapon(9));
    enemy->GetComp<CompOwnedItems>()->curWeaponId=9;
    return true;
}
bool EnemiesCatalogue::loadYourEvilCopy(sf::Vector2f pos)
{
    ecs::Apply<TagPlayer>([&](ecs::IEntity *player)
    {
        auto desc = ecs::gEntityManager.CreateEntity<Enemy>();
        auto ent = ecs::gEntityManager.GetEntityByDesc(desc);
        ent->GetComp<CompPos>()->v = sf::Vector2i(pos);
        ent->GetComp<CompVisionRadius>()->value = 8;
        ent->GetComp<CompHealth>()->value = 100;
        CompAnimInfo anim;
        anim.frame = anim.ticks = 0;
        anim.layer = CompDrawTileInfo::LAYER_OBJECTS;
        anim.id = gNamedAnimations["player_anim"];
        anim.frame_count = gRenderer.GetAnimFramesCount(anim.id);
        anim.update_ticks = 4;
        *ent->GetComp<CompAnimInfo>() = anim;
        *ent->GetComp<CompOwnedItems>()=*player->GetComp<CompOwnedItems>();
        ent->GetComp<CompOwnedItems>()->curWeaponId=1;
        

    });
    return true;
}
bool EnemiesCatalogue::loadGuardian(sf::Vector2f pos)
{
    auto desc = ecs::gEntityManager.CreateEntity<Enemy>();
    auto enemy = ecs::gEntityManager.GetEntityByDesc(desc);
    enemy->GetComp<CompPos>()->v = sf::Vector2i(pos);
    enemy->GetComp<CompHealth>()->value = 3000;
    enemy->GetComp<CompDamage>()->magicDamage = 10;
    enemy->GetComp<CompDamage>()->physDamage = 30;
    enemy->GetComp<CompVisionRadius>()->value = 8;
    CompAnimInfo anim;
    anim.frame = anim.ticks = 0;
    anim.layer = CompDrawTileInfo::LAYER_OBJECTS;
    anim.id = gNamedAnimations["guardian_anim"];
    anim.frame_count = gRenderer.GetAnimFramesCount(anim.id);
    anim.update_ticks = 4;
    *enemy->GetComp<CompAnimInfo>() = anim;

    enemy->GetComp<CompOwnedItems>()->weapons.push_back(wc.getWeapon(3));
    enemy->GetComp<CompOwnedItems>()->curWeaponId=3;
    return true;
}
void EnemiesCatalogue::UpdateBasicEnemy(Enemy *ent)
{
    ent->curTick++;
    
    ecs::IEntity *area = ecs::gEntityManager.GetEntityByDesc(ent->GetComp<CompCurrentAttackArea>()->ent);
    if (!(ent->curTick % ENEMY_ATTACK_COOLDOWN))
    {
        area->GetComp<CompActivity>()->active = true;
        auto visibility = area->GetComp<CompVisibility>();
        Weapon *w=game::wc.getWeapon(4);
        game::AttackFuncts::Attack(game::SIMPLE,w,ent,area);
        
    }
    else
    {
        area->GetComp<CompActivity>()->active = false;
        ent->GetComp<CompMoveDir>()->v = sf::Vector2i(0, 0);
        return;
    }

    unsigned vr = ent->GetComp<CompVisionRadius>()->value;

    sf::Vector2i dir(0, 0);
    sf::Vector2i pos = ent->GetComp<CompPos>()->v;
    ecs::Apply<TagPlayer>([&](ecs::IEntity *player)
    {
        CompPos *playerPos = player->GetComp<CompPos>();
        
        int dx = abs(playerPos->v.x - pos.x);
        int dy = abs(playerPos->v.y - pos.y);
        if (std::max(dx, dy) > ent->GetComp<CompVisionRadius>()->value)
            return;

        if (playerPos->v.x < pos.x)
            dir.x = -1;
        else if (playerPos->v.x > pos.x)
            dir.x = 1;
        if (playerPos->v.y < pos.y)
            dir.y = -1;
        else if (playerPos->v.y > pos.y)
            dir.y = 1;
    });

    ent->GetComp<CompMoveDir>()->v = dir;
}

void EnemiesCatalogue::UpdateRangedEnemy(Enemy *ent)
{
    ent->curTick++;
    ecs::IEntity *area = ecs::gEntityManager.GetEntityByDesc(ent->GetComp<CompCurrentAttackArea>()->ent);
    unsigned vr = ent->GetComp<CompVisionRadius>()->value;
    sf::Vector2i dir(0, 0);
    sf::Vector2i pos = ent->GetComp<CompPos>()->v;

    ecs::Apply<TagPlayer>([&](ecs::IEntity *player)
    {
        CompPos *playerPos = player->GetComp<CompPos>();
        int px=playerPos->v.x;
        int py=playerPos->v.y;
        int dx = px - pos.x;
        int dy = py - pos.y;
        if (std::max(abs(dx), abs(dy)) > ent->GetComp<CompVisionRadius>()->value)
            return;
        if (!(ent->curTick % RANGED_ENEMY_ATTACK_COOLDOWN))
        {
            if ((dx==0)||(dy==0))
            {
                if (dx<0)
                {
                   ent->GetComp<CompDir>()->v=sf::Vector2i(-1,0); 
                }
                else if (dx>0)
                {
                   ent->GetComp<CompDir>()->v=sf::Vector2i(1,0); 
                }
                else if (dy<0)
                {
                   ent->GetComp<CompDir>()->v=sf::Vector2i(0,-1); 
                }
                else 
                {
                   ent->GetComp<CompDir>()->v=sf::Vector2i(0,1); 
                }
                area->GetComp<CompActivity>()->active = true;
                auto visibility = area->GetComp<CompVisibility>();
                const Weapon *w=ent->GetComp<CompOwnedItems>()->weapons[0];
                game::AttackFuncts::Attack(game::MAGIC_BALL,w,ent,area);
            }
            
        }
        else
        {
            int od= ent->GetComp<CompVisionRadius>()->value/2;
            sf::Vector2i optimal_position(0, 0);
            dy=-dy;
            dx=-dx;
            if ((dy>dx)&&(dy>-dx))
            {
                optimal_position=sf::Vector2i(px,py+od);
            }
            else if ((dy<=dx)&&(dy>=-dx))
            {
                optimal_position=sf::Vector2i(px+od,py);
            }
            else if ((dy<=dx)&&(dy<=-dx))
            {
                optimal_position=sf::Vector2i(px,py-od);
            }
            else 
            {
                optimal_position=sf::Vector2i(px-od,py);
            }
            sf::Vector2i td(optimal_position.x-pos.x,optimal_position.y-pos.y);
            float dist=sqrt(td.x*td.x+td.y*td.y);
            sf::Vector2f normdir(0,0);
            if (dist>0.5)  
            {
                normdir=sf::Vector2f(td.x/dist,td.y/dist);
            }
            dir=sf::Vector2i((int)round(normdir.x*1.1),(int)round(normdir.y*1.1));
        }
    });

    ent->GetComp<CompMoveDir>()->v = dir;
}
void EnemiesCatalogue::UpdateYourEvilCopy(Enemy *ent)
{

    CompHealth *h = ent->GetComp<CompHealth>();
    if (h->value < 100.0f)
        h->value = std::min(100.0f, h->value + 0.5f);

    ent->curTick++;
    ecs::IEntity *area = ecs::gEntityManager.GetEntityByDesc(ent->GetComp<CompCurrentAttackArea>()->ent);
    unsigned vr = ent->GetComp<CompVisionRadius>()->value;
    sf::Vector2i dir(0, 0);
    sf::Vector2i pos = ent->GetComp<CompPos>()->v;
    
    ecs::Apply<TagPlayer>([&](ecs::IEntity *player)
    {
        CompPos *playerPos = player->GetComp<CompPos>();
        int px=playerPos->v.x;
        int py=playerPos->v.y;
        int dx = px - pos.x;
        int dy = py - pos.y;

        const game::Weapon *rw=nullptr;
        const game::Weapon *mw=nullptr;
        bool has_ranged=false;
        bool has_melee=false;
        auto weapons =ent->GetComp<CompOwnedItems>()->weapons;
        for (int i=0;i<weapons.size();i++)
        {
            if (weapons[i]->attack_type==MAGIC_BALL)
            {
                has_ranged=true;
                rw=weapons[i];
            }
            if ((weapons[i]->attack_type==SIMPLE)||
               (weapons[i]->attack_type==POISONOUS)||
               (weapons[i]->attack_type==FIERY))
            {
                has_melee=true;
                mw=weapons[i];
            }
        }
        bool attack_made=false;
        if (has_melee)
        {
            if ((abs(dx)<=1)&&(abs(dy)<=1))
            {
                if (!(ent->curTick % ENEMY_ATTACK_COOLDOWN))
                {
                    area->GetComp<CompActivity>()->active = true;
                    auto visibility = area->GetComp<CompVisibility>();
                    game::AttackFuncts::Attack(game::SIMPLE,mw,ent,area);
                    attack_made=true;
                }
            }
        }
        if ((has_ranged)&&(!attack_made))
        {
            if ((dx==0)||(dy==0))
            {
                attack_made=true;
                if (dx<0)
                {
                   ent->GetComp<CompDir>()->v=sf::Vector2i(-1,0); 
                }
                else if (dx>0)
                {
                   ent->GetComp<CompDir>()->v=sf::Vector2i(1,0); 
                }
                else if (dy<0)
                {
                   ent->GetComp<CompDir>()->v=sf::Vector2i(0,-1); 
                }
                else 
                {
                   ent->GetComp<CompDir>()->v=sf::Vector2i(0,1); 
                }
                area->GetComp<CompActivity>()->active = true;
                auto visibility = area->GetComp<CompVisibility>();
                game::AttackFuncts::Attack(game::MAGIC_BALL,rw,ent,area);
            }
            
        }
        if (!attack_made)
        {
            if (playerPos->v.x < pos.x)
                dir.x = -1;
            else if (playerPos->v.x > pos.x)
                dir.x = 1;
            if (playerPos->v.y < pos.y)
                dir.y = -1;
            else if (playerPos->v.y > pos.y)
                dir.y = 1;
        }
    });
    ent->GetComp<CompMoveDir>()->v = dir;
}
void EnemiesCatalogue::UpdateGuardian(Enemy *ent)
{
    ent->curTick++;
    
    ecs::IEntity *area = ecs::gEntityManager.GetEntityByDesc(ent->GetComp<CompCurrentAttackArea>()->ent);
    if (!(ent->curTick % ENEMY_ATTACK_COOLDOWN))
    {
        area->GetComp<CompActivity>()->active = true;
        auto visibility = area->GetComp<CompVisibility>();
        const Weapon *w=ent->GetComp<CompOwnedItems>()->weapons[0];
        game::AttackFuncts::Attack(game::FIERY,w,ent,area);
        
    }
    else
    {
        area->GetComp<CompActivity>()->active = false;
        ent->GetComp<CompMoveDir>()->v = sf::Vector2i(0, 0);
        return;
    }

    unsigned vr = ent->GetComp<CompVisionRadius>()->value;

    sf::Vector2i dir(0, 0);
    sf::Vector2i pos = ent->GetComp<CompPos>()->v;
    ecs::Apply<TagPlayer>([&](ecs::IEntity *player)
    {
        CompPos *playerPos = player->GetComp<CompPos>();
        
        int dx = abs(playerPos->v.x - pos.x);
        int dy = abs(playerPos->v.y - pos.y);
        if (std::max(dx, dy) > ent->GetComp<CompVisionRadius>()->value)
            return;

        if (playerPos->v.x < pos.x)
            dir.x = -1;
        else if (playerPos->v.x > pos.x)
            dir.x = 1;
        if (playerPos->v.y < pos.y)
            dir.y = -1;
        else if (playerPos->v.y > pos.y)
            dir.y = 1;
    });

    //ent->GetComp<CompMoveDir>()->v = dir;
}
}