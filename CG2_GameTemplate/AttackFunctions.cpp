#include "AttackFunctions.h"
#include "EnemiesCatalogue.h"
static bool isFree(sf::Vector2f pos)
{
    bool wall=false;
    sf::Vector2i pi=sf::Vector2i(pos);
    ecs::Apply<TagWall>([&](ecs::IEntity *w)
    {
        if (pi==w->GetComp<CompPosPattern>()->positions.pos) wall=true;
    });
    return(!wall);
    
}
sf::Vector2i shiftPos(sf::Vector2i pos)
{
    float shx=(pos.x-18)>0?-3:3;
    float shy=(pos.y-76)>0?-3:3;
    return(sf::Vector2i(pos.x+shx,pos.y+shy));
}
static bool ConsumeManaStamina(const game::Weapon *w,ecs::IEntity *attacker)
{
    if (w==NULL) return(true);
    if (attacker==NULL) return(true);
    float stamCons = w->staminaConsumption;
    float manaCons = w->manaConsumption;
    CompStamina *s = attacker->GetComp<CompStamina>();
    CompMana *m = attacker->GetComp<CompMana>();
    if (s==NULL) return(true);
    if (m==NULL) return(true);
    if (s->value - stamCons >= 0.0f && m->value - manaCons >= 0.0f)
    {
        s->value -= stamCons;
        m->value -= manaCons;
        return(true);
    }
    return(false);
}
static bool ChangeAttackField(CompPos *myPos, CompDir *dir, CompOwnedItems *ammo,
    CompCurrentAttackArea *area)
{

    if (ammo->curWeaponId < 0 || ammo->curWeaponId >= int(ammo->weapons.size()))
        return false;

    sf::Vector2i normDir(0, 0);
    if (abs(dir->v.x) > abs(dir->v.y))
        normDir.x = dir->v.x > 0 ? 1 : -1;
    else
        normDir.y = dir->v.y > 0 ? 1 : -1;

    ecs::IEntity *areaEnt = ecs::gEntityManager.GetEntityByDesc(area->ent);
    if (!areaEnt)
        return false;
    CompPosPattern *pp = areaEnt->GetComp<CompPosPattern>();

    const game::Weapon *weap = ammo->weapons[ammo->curWeaponId];
     
    int w = weap->areaSize.x;
    int h = weap->areaSize.y;

    if (normDir.y == 1)
    {
        pp->positions.Init(w, h, 0);
        area->shift = sf::Vector2i(- w / 2, 1 + weap->shift);
        for (int i = 0; i < w; ++i)
            for (int j = 0; j < h; ++j)
                pp->positions.pattern[i][j] = weap->pattern[i * h + j];
    }
    else if (normDir.y == -1)
    {
        pp->positions.Init(w, h, 0);
        area->shift = sf::Vector2i(- w / 2, - h - weap->shift);
        for (int i = 0; i < w; ++i)
            for (int j = 0; j < h; ++j)
                pp->positions.pattern[w - 1 - i][h - 1 - j] = weap->pattern[i * h + j];
    }
    else if (normDir.x == 1)
    {
        pp->positions.Init(h, w, 0);
        area->shift = sf::Vector2i(1 + weap->shift, - w / 2);
        for (int i = 0; i < w; ++i)
            for (int j = 0; j < h; ++j)
                pp->positions.pattern[j][w - 1 - i] = weap->pattern[i * h + j];
    }
    else
    {
        pp->positions.Init(h, w, 0);
        area->shift = sf::Vector2i(- h - weap->shift, - w / 2);
        for (int i = 0; i < w; ++i)
            for (int j = 0; j < h; ++j)
                pp->positions.pattern[h - 1 - j][i] = weap->pattern[i * h + j];
    }
    
    CompDamage *damage = areaEnt->GetComp<CompDamage>();
    damage->physDamage = weap->physDamage;
    damage->magicDamage = weap->magicDamage;

    return true;
}

static void ShowAttackField(CompVisibility *vis)
{
    vis->visible = true;
    vis->hideAfterOneTick = true;
}

static bool Attack2(CompActivity *act, CompOwnedItems *items, ecs::IEntity *player)
{
    act->active = true;
    act->disableAfterOneTick = true; // So the damage will be only in one step.
    return true;
    game::AttackFuncts::Attack(items->weapons[items->curWeaponId]->attack_type,items->weapons[items->curWeaponId],player,NULL);
    return false;
}

namespace game
{
 bool AttackFuncts::SimpleAttack(const game::Weapon *w,ecs::IEntity *attacker,ecs::IEntity *area)
{
    if (attacker->GetComp<TagPlayer>())
    {
        bool sucess = false;
        ecs::IEntity *player=attacker;
        auto items = player->GetComp<CompOwnedItems>();
        
        if (area)
        {
            auto cd = area->GetComp<CompCoolDown>();
            if(cd->isReady())
            {
                    
                sucess = Attack2(area->GetComp<CompActivity>(), items, player);
                ShowAttackField(area->GetComp<CompVisibility>());
                cd->stop();
                if (!sucess) return(false);
                ChangeAttackField(player->GetComp<CompPos>(), player->GetComp<CompDir>(),
                items, player->GetComp<CompCurrentAttackArea>());
            }
        }
        return(sucess);
    }
    else
    {

        CompDamage *myDamage = attacker->GetComp<CompDamage>();
        if (w)
        {
            myDamage->physDamage = w->physDamage;
            myDamage->magicDamage = w->magicDamage;
        }
        area->GetComp<CompDamage>()->physDamage = myDamage->physDamage;
        area->GetComp<CompDamage>()->magicDamage = myDamage->magicDamage;
        
    }
    
    return(true);
}
bool AttackFuncts::PoisonousAttack(const game::Weapon *w,ecs::IEntity *attacker,ecs::IEntity *area)
{
    bool success=SimpleAttack(w,attacker,area);
    if (!success) return(false);
    auto &effects =area->GetComp<CompPassiveEffects>()->list;
    effects.clear();
    effects.push_front([&](void) -> auto /*Вот как-то так. Вроде удобно*/
    {
        return std::make_unique<game::PoisonEffect>(3.0,10.0);
    });
    return(true);
}
 bool AttackFuncts::MagicBalls(const game::Weapon *w,ecs::IEntity *attacker,ecs::IEntity *area)
 {
     CompDamage d;
        d.magicDamage = d.physDamage = 1.f;
     CompParty p;
     if (attacker==NULL) 
        p.party=p.NEUTRAL;
    else
        p.party=attacker->GetComp<CompParty>()->party;
    
       ecs::EntityDesc des= ecs::gEntityManager.CreateEntity<Missle>(
            game::gNamedTiles["magic1"],
            attacker->GetComp<CompPos>()->v,
            attacker->GetComp<CompDir>()->v,
            d
        );
    ecs::IEntity *mis=ecs::gEntityManager.GetEntityByDesc(des);
    mis->GetComp<CompParty>()->party=p.party;
    area->GetComp<CompVisibility>()->visible=false;
	return(true);
 }
 bool AttackFuncts::FireBall(const game::Weapon *w,ecs::IEntity *attacker,ecs::IEntity *area)
 {
     CompDamage d;
        d.magicDamage = 0.0f;
         d.physDamage = 10.f;
     CompParty p;
     if (attacker==NULL) 
        p.party=p.NEUTRAL;
    else
        p.party=attacker->GetComp<CompParty>()->party;
    
       ecs::EntityDesc des= ecs::gEntityManager.CreateEntity<Missle>(
            game::gNamedTiles["fireball"],
            attacker->GetComp<CompPos>()->v,
            attacker->GetComp<CompDir>()->v,
            d
        );
    ecs::IEntity *mis=ecs::gEntityManager.GetEntityByDesc(des);
    mis->GetComp<CompParty>()->party=p.party;
    area->GetComp<CompVisibility>()->visible=false;
	return(true);
 }
 bool AttackFuncts::Arrow(const game::Weapon *w,ecs::IEntity *attacker,ecs::IEntity *area)
 {
     CompDamage d;
        d.magicDamage =0.0f;
        d.physDamage = 20.f;
     CompParty p;
     if (attacker==NULL) 
        p.party=p.NEUTRAL;
    else
        p.party=attacker->GetComp<CompParty>()->party;
    
       ecs::EntityDesc des= ecs::gEntityManager.CreateEntity<Missle>(
            game::gNamedTiles["arrow"],
            attacker->GetComp<CompPos>()->v,
            attacker->GetComp<CompDir>()->v,
            d
        );
    ecs::IEntity *mis=ecs::gEntityManager.GetEntityByDesc(des);
    mis->GetComp<CompParty>()->party=p.party;
    area->GetComp<CompVisibility>()->visible=false;
	return(true);
 }
 bool AttackFuncts::Bolt(const game::Weapon *w,ecs::IEntity *attacker,ecs::IEntity *area)
 {
     CompDamage d;
        d.magicDamage = 0.0f;
        d.physDamage = 25.f;
     CompParty p;
     if (attacker==NULL) 
        p.party=p.NEUTRAL;
    else
        p.party=attacker->GetComp<CompParty>()->party;
    
       ecs::EntityDesc des= ecs::gEntityManager.CreateEntity<Missle>(
            game::gNamedTiles["bolt"],
            attacker->GetComp<CompPos>()->v,
            attacker->GetComp<CompDir>()->v,
            d
        );
    ecs::IEntity *mis=ecs::gEntityManager.GetEntityByDesc(des);
    mis->GetComp<CompParty>()->party=p.party;
    area->GetComp<CompVisibility>()->visible=false;
	return(true);
 }
 bool AttackFuncts::Poison(const game::Weapon *w,ecs::IEntity *attacker,ecs::IEntity *area)
 {
     auto &effects =area->GetComp<CompPassiveEffects>()->list;
    effects.clear();
    effects.push_front([&](void) -> auto /*Вот как-то так. Вроде удобно*/
    {
        return std::make_unique<game::PoisonEffect>(1.0,30.0);
    });
	return(true);
 }
 bool AttackFuncts::FieryAttack(const game::Weapon *w,ecs::IEntity *attacker,ecs::IEntity *area)
 {
    bool success=SimpleAttack(w,attacker,area);
    if (!success) return(false);
    auto &effects =area->GetComp<CompPassiveEffects>()->list;
    effects.clear();
    effects.push_front([&](void) -> auto /*Вот как-то так. Вроде удобно*/
    {
        return std::make_unique<game::FieryEffect>(3.0,10.0);
    });
    return(true);
 }
 bool AttackFuncts::SpellCurse(const game::Weapon *w,ecs::IEntity *attacker,ecs::IEntity *area)
 {
    bool success=SimpleAttack(w,attacker,area);
    if (!success) return(false);
    auto &effects =area->GetComp<CompPassiveEffects>()->list;
    effects.clear();
    effects.push_front([&](void) -> auto /*Вот как-то так. Вроде удобно*/
    {
        return std::make_unique<game::CurseEffect>(30.0,50.0);
    });
    return(true);
 }

 bool AttackFuncts::NecromancersAttackWeak(const game::Weapon *w,ecs::IEntity *attacker,ecs::IEntity *area)
 {
     area->GetComp<CompVisibility>()->visible=false;
     sf::Vector2i pos=attacker->GetComp<CompPos>()->v;
     pos=shiftPos(pos);
     sf::Vector2f enemy=sf::Vector2f(pos.x-2.0,pos.y-2.0);

     if (isFree(enemy)) game::EnemiesCatalogue::loadEnemy(enemy);
     enemy=sf::Vector2f(pos.x-1.0,pos.y);
     if (isFree(enemy)) game::EnemiesCatalogue::loadEnemy(enemy);
     enemy=sf::Vector2f(pos.x+2.0,pos.y-2.0);
     if (isFree(enemy)) game::EnemiesCatalogue::loadEnemy(enemy);
     enemy=sf::Vector2f(pos.x+1.0,pos.y);
     if (isFree(enemy)) game::EnemiesCatalogue::loadEnemy(enemy);
     return(true);
 }
 bool AttackFuncts::NecromancersAttackMedium(const game::Weapon *w,ecs::IEntity *attacker,ecs::IEntity *area)
 {
     area->GetComp<CompVisibility>()->visible=false;
     sf::Vector2i pos=attacker->GetComp<CompPos>()->v;
     pos=shiftPos(pos);
     sf::Vector2f enemy=sf::Vector2f(pos.x-1.0,pos.y-2.0);
     if (isFree(enemy)) game::EnemiesCatalogue::loadRangedEnemy(enemy);
     enemy=sf::Vector2f(pos.x,pos.y-2.0);
     if (isFree(enemy)) game::EnemiesCatalogue::loadRangedEnemy(enemy);
     enemy=sf::Vector2f(pos.x+1.0,pos.y-2.0);
     if (isFree(enemy)) game::EnemiesCatalogue::loadRangedEnemy(enemy);
     return(true);
 }
 bool AttackFuncts::NecromancersAttackStrong(const game::Weapon *w,ecs::IEntity *attacker,ecs::IEntity *area)
 {
     area->GetComp<CompVisibility>()->visible=false;
     sf::Vector2i pos=attacker->GetComp<CompPos>()->v;
     pos=shiftPos(pos);
     sf::Vector2f enemy=sf::Vector2f(pos.x-2.0,pos.y-2.0);
     if (isFree(enemy)) game::EnemiesCatalogue::loadYourEvilCopy(enemy);
     enemy=sf::Vector2f(pos.x-2.0,pos.y-1.0);
     if (isFree(enemy)) game::EnemiesCatalogue::loadYourEvilCopy(enemy);
     enemy=sf::Vector2f(pos.x+2.0,pos.y-2.0);
     if (isFree(enemy)) game::EnemiesCatalogue::loadYourEvilCopy(enemy);
     enemy=sf::Vector2f(pos.x+2.0,pos.y-1.0);
     if (isFree(enemy)) game::EnemiesCatalogue::loadYourEvilCopy(enemy);
     return(true);
 }
 bool AttackFuncts::NecromancersAttackGuardians(const game::Weapon *w,ecs::IEntity *attacker,ecs::IEntity *area)
 {
     area->GetComp<CompVisibility>()->visible=false;
     sf::Vector2i pos=attacker->GetComp<CompPos>()->v;
     sf::Vector2f enemy=sf::Vector2f(18,61);
     if (isFree(enemy)) game::EnemiesCatalogue::loadGuardian(enemy);
     enemy=sf::Vector2f(19,61);
     if (isFree(enemy)) game::EnemiesCatalogue::loadGuardian(enemy);
     enemy=sf::Vector2f(34,76);
     if (isFree(enemy)) game::EnemiesCatalogue::loadGuardian(enemy);
     return(true);
 }
 bool AttackFuncts::Attack(int attack_type,const game::Weapon *w,ecs::IEntity *attacker,ecs::IEntity *area)
 {
     bool success=true;
     if (!ConsumeManaStamina(w,attacker)) success=false; 
     else if (area==NULL) success=false;
     else
     {
        auto &effects =area->GetComp<CompPassiveEffects>()->list;
        effects.clear();
        if ((attacker==NULL)||(attacker->GetComp<CompParty>()==NULL)) 
            area->GetComp<CompParty>()->party=area->GetComp<CompParty>()->NEUTRAL;
        else
            area->GetComp<CompParty>()->party=attacker->GetComp<CompParty>()->party;
        
        area->GetComp<CompVisibility>()->visible=true;
        area->GetComp<CompVisibility>()->hideAfterOneTick=true;
        switch (attack_type)
        {
            case SIMPLE:success=AttackFuncts::SimpleAttack(w,attacker,area);
                        break;
            case MAGIC_BALL:success=AttackFuncts::MagicBalls(w,attacker,area);
                        break;
            case POISONOUS:success=AttackFuncts::PoisonousAttack(w,attacker,area);
                        break;
            case POISONOUS_WEAK:success=false;
                        break;
            case POISON_SPELL:success=AttackFuncts::Poison(w,attacker,area);
                        break;
            case FIERY:success=AttackFuncts::FieryAttack(w,attacker,area);
                        break;
            case CURSE:success=AttackFuncts::SpellCurse(w,attacker,area);
                        break;
            case NECROMANCERS_WEAK:success=AttackFuncts::NecromancersAttackWeak(w,attacker,area);
                        break;
            case NECROMANCERS_MEDIUM:success=AttackFuncts::NecromancersAttackMedium(w,attacker,area);
                        break;
            case NECROMANCERS_STRONG:success=AttackFuncts::NecromancersAttackStrong(w,attacker,area);
                        break;
            case NECROMANCERS_GUARDIANS:success=AttackFuncts::NecromancersAttackGuardians(w,attacker,area);
                        break;
            case FIREBALL_SPELL:success=AttackFuncts::FireBall(w,attacker,area);
                        break;
            case ARROW_SPELL:success=AttackFuncts::Arrow(w,attacker,area);
                        break;
            case BOLT_SPELL:success=AttackFuncts::Bolt(w,attacker,area);
                        break;
            
            default:success=false;
        }
        if (!success) area->GetComp<CompVisibility>()->visible=false;
        return(success);
     }
     
     
 }
}