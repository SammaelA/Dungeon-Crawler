#include "EcsEntities.h"
#include "WeaponsCatalogue.h"
#include "Renderer.h"
#include "Game.h"
#include "AttackFunctions.h"
#include "EnemiesCatalogue.h"
constexpr unsigned enemyUpdatePeriod = 2;

void Player::OnCreated()
{
    GetComp<CompParty>()->party = CompParty::ALLIANCE;

    int attackAreaTileId = game::gRenderer.AddTile(sf::Color(255, 0, 0, 128));
    auto eid = ecs::gEntityManager.CreateEntity<AttackArea>();
    ecs::IEntity *ent = ecs::gEntityManager.GetEntityByDesc(eid);
    ent->GetComp<CompDrawTileInfo>()->tileId = attackAreaTileId;
    ent->GetComp<CompDrawTileInfo>()->layer = CompDrawTileInfo::LAYER_EFFECTS;
    ent->GetComp<CompVisibility>()->visible = false;
    ent->GetComp<CompOwner>()->owner = this->GetDesc();

    GetComp<CompCurrentAttackArea>()->ent = eid;
}

void Player::OnDestroy()
{
    ecs::gEntityManager.DestroyEntity(GetComp<CompCurrentAttackArea>()->ent);
}

void Player::OnUpdate()
{
    CompHealth *h = GetComp<CompHealth>();
    CompStamina *s = GetComp<CompStamina>();
    CompMana *m = GetComp<CompMana>();
    if (h->value < 100.0f)
        h->value = std::min(100.0f, h->value + 0.5f);
    if (s->value < 100.0f)
        s->value = std::min(100.0f, s->value + 0.5f);
    if (m->value < 100.0f)
        m->value = std::min(100.0f, m->value + 0.5f);
}

void Enemy::OnCreated()
{
    GetComp<CompParty>()->party = CompParty::ENEMIES;

    auto eid = ecs::gEntityManager.CreateEntity<AttackArea>();
    ecs::IEntity *area = ecs::gEntityManager.GetEntityByDesc(eid);

    area->GetComp<CompVisibility>()->visible = false;
    area->GetComp<CompOwner>()->owner = this->GetDesc();
    area->GetComp<CompActivity>()->active = false;
    area->GetComp<CompActivity>()->disableAfterOneTick = true;
    
    // 3x3 attack area.
    area->GetComp<CompPosPattern>()->positions.Init(3, 3, 1);

    GetComp<CompCurrentAttackArea>()->ent = eid;
    GetComp<CompCurrentAttackArea>()->shift = sf::Vector2i(-1, -1);
}

void Enemy::OnDestroy()
{
    ecs::gEntityManager.DestroyEntity(GetComp<CompCurrentAttackArea>()->ent);
}

void Enemy::OnUpdate()
{
    if (GetComp<CompOwnedItems>()->curWeaponId==4)
    {
        game::EnemiesCatalogue::UpdateBasicEnemy(this);
    }
    else if (GetComp<CompOwnedItems>()->curWeaponId==9)
    {
        game::EnemiesCatalogue::UpdateRangedEnemy(this);
    }
    else if (GetComp<CompOwnedItems>()->curWeaponId==1)
    {
        game::EnemiesCatalogue::UpdateYourEvilCopy(this);
    }
    else if (GetComp<CompOwnedItems>()->curWeaponId==3)
    {
        game::EnemiesCatalogue::UpdateGuardian(this);
    }
}


StatusMessage::StatusMessage(const std::string& smsg, int lifetime, sf::Vector2i &pos, sf::Color color) :
    mcolor(color)
{
    GetComp<CompTimeToLive>()->ticks = lifetime;
    GetComp<CompPos>()->v = pos;
    msg = smsg;
}

void StatusMessage::OnCreated()
{
    GetComp<CompFloatingText>()->tid = game::gRenderer.AddText(
        msg, sf::Vector2f(GetComp<CompPos>()->v), 15, mcolor
    );
}

void StatusMessage::OnDestroy()
{
    game::gRenderer.RemoveText(GetComp<CompFloatingText>()->tid);
}

Missle::Missle(int tileid, const sf::Vector2i &pos, const sf::Vector2i &dir,
 const CompDamage & dmg)
{
    GetComp<CompDrawTileInfo>()->layer = CompDrawTileInfo::LAYER_EFFECTS;
    GetComp<CompDrawTileInfo>()->tileId = tileid;
    GetComp<CompPos>()->v = pos;
    GetComp<CompMoveDir>()->v = dir;
    *GetComp<CompDamage>() = dmg;
}
void Missle::OnDestroy() 
{
    sf::Vector2i pos = this->GetComp<CompPos>()->v;
    pos += this->GetComp<CompMoveDir>()->v;
    auto desc = ecs::gEntityManager.CreateEntity<AttackArea>();
    auto ent = ecs::gEntityManager.GetEntityByDesc(desc);
    game::PosPattern pattern;
    int attack_type;
    if ((int)GetComp<CompDamage>()->physDamage==1)
    {
        pattern.Init(5, 5, true);
        attack_type=game::POISONOUS;
    }
    else if ((int)GetComp<CompDamage>()->physDamage==10)
    {
        pattern.Init(3, 3, true);
        attack_type=game::FIERY;
    }
    else
    {
        pattern.Init(1, 1, true);
        attack_type=game::SIMPLE;
    }
    
    pattern.pos = pos;
    pattern.tileId = this->GetComp<CompDrawTileInfo>()->tileId;
    ent->GetComp<CompPosPattern>()->positions = pattern;
    ent->GetComp<CompActivity>()->active = true;
    ent->GetComp<CompDamage>()->physDamage = this->GetComp<CompDamage>()->physDamage;
    ent->GetComp<CompDamage>()->magicDamage = this->GetComp<CompDamage>()->magicDamage;
    game::AttackFuncts::Attack(attack_type,NULL,this,ent);
    ent->GetComp<CompVisibility>()->visible = true;
    ent->GetComp<CompDrawTileInfo>()->layer = 2;
    ent->GetComp<CompDrawTileInfo>()->tileId = pattern.tileId; 
    ent->GetComp<CompParty>()->party=this->GetComp<CompParty>()->party;
    ent->GetComp<CompOwner>()->owner=this->GetDesc();
}

FireBall::FireBall(const CompAnimInfo &anim, const sf::Vector2i &pos, const sf::Vector2i &dst, const CompDamage &dmg)
{
    *GetComp<CompAnimInfo>() = anim;
    GetComp<CompPos>()->v = pos;
    dstPos = dst;
    *GetComp<CompDamage>() = dmg;
    GetComp<CompMoveDir>()->v = sf::Vector2i(0, 0);
    GetComp<CompHealth>()->value = 1;
}

void FireBall::OnCreated()
{
    
}
void FireBall::OnUpdate() 
{
    auto dir = sf::Vector2i(0, 0);
    auto &pos = GetComp<CompPos>()->v;
    //std::cout << "update\n";
    if(pos == dstPos){
        //std::cout << "what\n";
        GetComp<CompMoveDir>()->v = sf::Vector2i(0, 0);
        GetComp<CompHealth>()->value = -1;
        return;
    }

    if(dstPos.x < pos.x)
        dir.x = -1;
    else if(dstPos.x > pos.x)
        dir.x = 1;
    if(dstPos.y < pos.y)
        dir.y = -1;
    else if(dstPos.y > pos.y)
        dir.y = 1;
    GetComp<CompMoveDir>()->v = dir;
}

void FireBall::OnDestroy()
{
    std::cout << "destroy\n";
    sf::Vector2i pos = this->GetComp<CompPos>()->v;
    auto desc = ecs::gEntityManager.CreateEntity<AttackArea>();
    auto ent = ecs::gEntityManager.GetEntityByDesc(desc);
    game::PosPattern pattern;
    pattern.Init(3, 3, true);
    pattern.pos = pos;
    ent->GetComp<CompPosPattern>()->positions = pattern;
    ent->GetComp<CompActivity>()->active = true;
    *ent->GetComp<CompDamage>()= *GetComp<CompDamage>();
    ent->GetComp<CompVisibility>()->visible = false;
    ent->GetComp<CompParty>()->party = GetComp<CompParty>()->party;
    ent->GetComp<CompOwner>()->owner = GetDesc();
    game::AttackFuncts::Attack(game::SIMPLE, nullptr, this, ent);
    
    for(int i = -1; i <= 1; i++){
        for(int j = -1; j <= 1; j++){
            auto des = ecs::gEntityManager.CreateEntity<Decoration>(
                    -1, -1, -1, -1, 5, sf::Vector2i(pos.x + i, pos.y + j)
            );
            *ecs::gEntityManager.GetEntityByDesc(des)->GetComp<CompAnimInfo>() = 
                *GetComp<CompAnimInfo>();
        }
    }
}