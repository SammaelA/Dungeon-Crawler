#include "EcsEntities.h"
#include "WeaponsCatalogue.h"
#include "Renderer.h"
#include "Game.h"
#include "AttackFunctions.h"
#include <math.h>
void Charger::OnCreated() {
    GetComp<CompParty>()->party = CompParty::ENEMIES;

    auto eid = ecs::gEntityManager.CreateEntity<AttackArea>();
    ecs::IEntity *area = ecs::gEntityManager.GetEntityByDesc(eid);

    area->GetComp<CompVisibility>()->visible = false;
    area->GetComp<CompOwner>()->owner = this->GetDesc();
    area->GetComp<CompActivity>()->active = false;
    area->GetComp<CompActivity>()->disableAfterOneTick = true;
    area->GetComp<CompDrawTileInfo>()->layer = CompDrawTileInfo::LAYER_EFFECTS;
    area->GetComp<CompDrawTileInfo>()->tileId = game::gRenderer.AddTile(sf::Color(0xffa50020));
    // 3x3 attack area.
    area->GetComp<CompPosPattern>()->positions.Init(3, 3, 1);

    GetComp<CompCurrentAttackArea>()->ent = eid;
    GetComp<CompCurrentAttackArea>()->shift = sf::Vector2i(-1, -1);
    
    weapon.physDamage = weapon.magicDamage = 20;
}

void Charger::OnUpdate() {
    ++curTick;
    IEntity *area = ecs::gEntityManager.GetEntityByDesc(GetComp<CompCurrentAttackArea>()->ent); 
    switch (state)
    {
    case IDLE:
        if(curTick >= idleTime) 
        {
            curTick = 0;
            state = PREPARATION;
            ecs::gEntityManager.CreateEntity<StatusMessage>(
                "DANGER!", prepTime, GetComp<CompPos>()->v, sf::Color::Red
            );
        }
        break;
    case PREPARATION:
        if(curTick >= prepTime) 
        {
            curTick = 0;
            area->GetComp<CompVisibility>()->visible = true;
            state = CHARGING;
        }
        break;
    case CHARGING:
        if(curTick >= chargeTime) 
        {
            curTick = 0;
            state = IDLE;
            GetComp<CompMoveDir>()->v = sf::Vector2i(0, 0);
            area->GetComp<CompActivity>()->active = false;
            area->GetComp<CompVisibility>()->visible = false;
            return;
        } 
    }

    if(state == CHARGING){
        area->GetComp<CompActivity>()->active = true;
        game::AttackFuncts::Attack(game::SIMPLE, &weapon, this, area);
        unsigned vr = GetComp<CompVisionRadius>()->value;

        sf::Vector2i dir(0, 0);
        sf::Vector2i pos = GetComp<CompPos>()->v;
        ecs::Apply<TagPlayer>([&](IEntity *player)
        {
            CompPos *playerPos = player->GetComp<CompPos>();
        
            int dx = abs(playerPos->v.x - pos.x);
            int dy = abs(playerPos->v.y - pos.y);
            if (std::max(dx, dy) > GetComp<CompVisionRadius>()->value)
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
        if(curTick % 3)
            GetComp<CompMoveDir>()->v = dir;
    }
    
}

void Charger::OnDestroy() {
    ecs::gEntityManager.DestroyEntity(GetComp<CompCurrentAttackArea>()->ent);
}

void FireDemon::OnCreated(){
    GetComp<CompParty>()->party = CompParty::ENEMIES;
    GetComp<CompVisionRadius>()->value = 10;
    lastHealth = -1.f;

    fireballAnim.id = game::gNamedAnimations["fire_anim"];
    fireballAnim.frame_count = game::gRenderer.GetAnimFramesCount(fireballAnim.id);
    fireballAnim.frame = fireballAnim.ticks = 0;
    fireballAnim.update_ticks = 1;
    fireballAnim.layer = CompDrawTileInfo::LAYER_EFFECTS;

    teleportAnim.id = game::gNamedAnimations["teleport_anim"];
    teleportAnim.frame_count = game::gRenderer.GetAnimFramesCount(teleportAnim.id);
    teleportAnim.frame = teleportAnim.ticks = 0;
    teleportAnim.update_ticks = 1;
    teleportAnim.layer = CompDrawTileInfo::LAYER_EFFECTS;
    
}

void FireDemon::OnUpdate()
{
    curTick++;
    if(lastHealth < 0) lastHealth = GetComp<CompHealth>()->value;
    auto hp = GetComp<CompHealth>()->value;
    std::cout << hp << " " << lastHealth << "\n";
    if(lastHealth > hp)
    {
        auto ents = ecs::gEntityManager.GetEntitiesByType(ecs::GetComponentTid<TeleportPoint>());
        int n = std::rand() % ents->size();
        auto lastPos = GetComp<CompPos>()->v;
        GetComp<CompPos>()->v = (*ents)[n]->GetComp<CompPos>()->v;
        ecs::gEntityManager.CreateEntity<Decoration>(teleportAnim, 2, lastPos);
        state = PREPARATION;
        curTick = 0;
        tpCount--;
        if(tpCount <= 0){
            lastHealth = GetComp<CompHealth>()->value;
            tpCount = 5;
        }
    }

    switch (state)
    {
    case IDLE:
        if(curTick >= idleTime) 
        {
            curTick = 0;
            state = PREPARATION;
            ecs::gEntityManager.CreateEntity<StatusMessage>(
                "DANGER!", prepTime, GetComp<CompPos>()->v, sf::Color::Black);
        }
        break;
    case PREPARATION:
        if(curTick >= prepTime) 
        {
            curTick = 0;
            state = SHOOTING;
        }
        break;
    case SHOOTING:
        if(curTick >= shootTime) 
        {
            curTick = 0;
            state = IDLE;
            return;
        } 
    }

    if(state == SHOOTING && (curTick % 2))
    {
        auto vision = GetComp<CompVisionRadius>()->value;
        auto demonpos = GetComp<CompPos>()->v;
        ecs::Apply<TagPlayer, CompPos>([&](IEntity *player)
        {
            auto ppos = player->GetComp<CompPos>()->v;
            if(std::abs(ppos.x - demonpos.x) + std::abs(ppos.y - demonpos.y) <= vision){
                auto desc = ecs::gEntityManager.CreateEntity<FireBall>(
                    fireballAnim, demonpos, ppos, *GetComp<CompDamage>()
                );
                *ecs::gEntityManager.GetEntityByDesc(desc)->GetComp<CompParty>() = *GetComp<CompParty>();
            }
        });
    }
}

void FireDemon::OnDestroy()
{

}
void Necromancer::OnCreated()
{
    

    auto eid = ecs::gEntityManager.CreateEntity<AttackArea>();
    ecs::IEntity *area = ecs::gEntityManager.GetEntityByDesc(eid);
    GetComp<CompParty>()->party = CompParty::ENEMIES;
    area->GetComp<CompVisibility>()->visible = false;
    area->GetComp<CompOwner>()->owner = this->GetDesc();
    area->GetComp<CompActivity>()->active = false;
    area->GetComp<CompActivity>()->disableAfterOneTick = true;
    area->GetComp<CompDrawTileInfo>()->layer = CompDrawTileInfo::LAYER_EFFECTS;
    area->GetComp<CompDrawTileInfo>()->tileId = game::gRenderer.AddTile(sf::Color(0xffa50020));
    // 3x3 attack area.
    area->GetComp<CompPosPattern>()->positions.Init(3, 3, 1);

    GetComp<CompCurrentAttackArea>()->ent = eid;
}
void Necromancer::OnUpdate()
{
    ecs::IEntity *ent=this;
    auto weapons = GetComp<CompOwnedItems>()->weapons;
    curTick++;
    CompHealth *h = ent->GetComp<CompHealth>();
    if (h->value < 200.0f)
        h->value = std::min(200.0f, h->value + 0.1f);

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
        if (state==SLEEPING)
        {
            if ((px<=30)&&(py>=65))
            {
                game::AttackFuncts::Attack(game::NECROMANCERS_GUARDIANS,weapons[3],this,area);          
                state=WEAK;
                coolTick=WEAK_COOLDOWN-20;
                ecs::gEntityManager.CreateEntity<StatusMessage>(
            "The Necromancer is near", 25, playerPos->v, sf::Color(0XFFFFFFFFU));
            }
        }
        else if (state!=INSANE)
        {
            coolTick++;
            int attack_delay=WEAK_COOLDOWN;
            if (state==MEDIUM) attack_delay=MEDIUM_COOLDOWN;
            else if (state==STRONG) attack_delay=STRONG_COOLDOWN;
            float player_dist=sqrt(dx*dx+dy*dy);
            if ((player_dist<10)&&(coolTick>=attack_delay))
            {
                coolTick=0;
                if (state==WEAK)
                {
                    game::AttackFuncts::Attack(game::NECROMANCERS_WEAK,weapons[0],this,area);
                }
                else if (state==MEDIUM)
                {
                    game::AttackFuncts::Attack(game::NECROMANCERS_MEDIUM,weapons[1],this,area);
                }
                else if (state==STRONG)
                {
                    game::AttackFuncts::Attack(game::NECROMANCERS_STRONG,weapons[2],this,area);
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
                sf::Vector2f normdir(0,0);
                float dist=sqrt(td.x*td.x+td.y*td.y);
                if (dist>0.5)  
                {
                    normdir=sf::Vector2f(td.x/dist,td.y/dist);
                }
                dir=sf::Vector2i((int)round(normdir.x*1.1),(int)round(normdir.y*1.1));
                
            }
            if ((state==WEAK)&&(h->value<150))
            {
                h->value=150;
                coolTick=MEDIUM_COOLDOWN-5;
                state=MEDIUM;
                ecs::gEntityManager.CreateEntity<StatusMessage>(
                    "I'm stornger than you can imagine!", 25, pos, sf::Color(0XFFFFFFFFU));
            }
            if ((state==MEDIUM)&&(h->value<100))
            {
                h->value=100;
                coolTick=STRONG_COOLDOWN-5;
                state=STRONG;
                ecs::gEntityManager.CreateEntity<StatusMessage>(
                    "You will die from your own shadows!", 25, pos, sf::Color(0XFFFFFFFFU));
            }
            if ((state==STRONG)&&(h->value<50))
            {
                h->value=50;
                state=INSANE;
                ecs::gEntityManager.CreateEntity<StatusMessage>(
                    "It's time for me to fight myself!", 25, pos, sf::Color(0XFFFFFFFFU));
            }
            
        }
        else
        {
            area->GetComp<CompActivity>()->active = true;
            auto visibility = area->GetComp<CompVisibility>();
            const game::Weapon *w=weapons[4];
            game::AttackFuncts::Attack(game::CURSE,w,ent,area);
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
void Necromancer::OnDestroy()
{
    ecs::Apply<TagEnemy>([&](ecs::IEntity *ent)
    {
       ent->GetComp<CompHealth>()->value=-100.0; 
    });
    ecs::gEntityManager.CreateEntity<StatusMessage>(
                    "The necromancer is dead!", 25, GetComp<CompPos>()->v, sf::Color(0X00FF00FFU));
}