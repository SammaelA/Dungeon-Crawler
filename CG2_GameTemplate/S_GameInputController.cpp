#include "stl.h"
#include "EcsSystems.h"
#include "Utils.h"
#include "GameStructures.h"
#include "Game.h"
#include "GameEffects.h"
#include "AttackFunctions.h"
#include "WeaponsCatalogue.h"
static stl::queue<game::EventSfml> inputPool;

extern stl::vector<game::Weapon> gGameWeapons;

void GameInputControllerSys::OnEvent(const game::EventSfml &evt)
{
    inputPool.push(evt);
}
static void TakePotion(CompOwnedItems *items, ecs::IEntity *player)
{
    float h=items->potions[items->curPotionId]->healthAmount;
    float m=items->potions[items->curPotionId]->magicAmount;
    float s=items->potions[items->curPotionId]->staminaAmount;
    player->GetComp<CompHealth>()->value+=h;
    player->GetComp<CompMana>()->value+=m;
    player->GetComp<CompStamina>()->value+=s;
    if (!items->potions[items->curPotionId]->overwhelming)
    {
        if (player->GetComp<CompHealth>()->value>100)
        {player->GetComp<CompHealth>()->value=100.0;}
        if (player->GetComp<CompMana>()->value>100)
        {player->GetComp<CompMana>()->value=100.0;}
        if (player->GetComp<CompStamina>()->value>100)
        {player->GetComp<CompStamina>()->value=100.0;}
    }
}

void GameInputControllerSys::OnUpdate()
{
    auto players = ecs::gEntityManager.GetEntitiesByType(ecs::GetEntityTid<Player>());
    if (!players)
        return;
    IEntity *player = players->at(0).get();
    if (!player)
        return;

    // Yes, input is hardcoded.
    // You may change such realization, if you want.
    
    CompOwnedItems *items = player->GetComp<CompOwnedItems>();

    bool needAttackChange = false;
    bool needAttack = false;
    bool needShowAttack = false;
    bool turnOnCD = false;

    sf::Vector2i moveDir(0, 0);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
    {
        player->GetComp<CompArmor>()->magicResist=0.99;
        player->GetComp<CompArmor>()->physResist=0.99;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::T))
    {
        for (int i=0;i<10;i++)
        {
            player->GetComp<CompOwnedItems>()->weapons.push_back(game::wc.getWeapon(i));
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Y))
    {
        player->GetComp<CompPos>()->v=sf::Vector2i(19,58);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract))
    {
        if (items->potionActive)
        {
            if (items->potionHotbarStartPosition>0)
                items->potionHotbarStartPosition--;
        }
        else
        {
            if (items->weaponHotbarStartPosition>0)
                items->weaponHotbarStartPosition--;
        }
        
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Add))
    {
        if (items->potionActive)
        {
            items->potionHotbarStartPosition++;
        }
        else
        {
            items->weaponHotbarStartPosition++;
        }
        
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
    {
        //P - кнопка принятия зелья. Если зелье в руке - ты пьешь его. Иначе potionActive
        //ставится в единицу и следующее нажание на клавишу номера выберет соответствующее зелье
        //из инвентаря 
        if (items->potionActive)
        {
        if (items->curPotionId!=-1)
        {
            
            TakePotion(items,player);
            items->potions.erase(items->potions.begin()+items->curPotionId);
            items->curPotionId=-1;
            
        }
        else
        {
            items->potionActive=false;
        }
        }
        else
        {
            items->potionActive=true;
        }   
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    {
        if (game::gstate==STATE_START)
        {
            game::gstate=STATE_GAME;
        }
        
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        moveDir += sf::Vector2i(0, 1);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        moveDir += sf::Vector2i(-1, 0);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        moveDir += sf::Vector2i(0, -1);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        moveDir += sf::Vector2i(1, 0);
    player->GetComp<CompMoveDir>()->v=moveDir;
    
    while (inputPool.size())
    {
        needAttackChange = false;
        needAttack = false;
        needShowAttack = false;

        game::EventSfml &evt = inputPool.front();
        
        switch (evt.type)
        {
        case game::EventSfml::KeyPressed:
            switch (evt.key.code)
            {
            case sf::Keyboard::Up:
                player->GetComp<CompDir>()->v = sf::Vector2i(0, 1);
                needAttackChange = true;
                needShowAttack = true;
                needAttack = true;
                break;
            case sf::Keyboard::Down:
                player->GetComp<CompDir>()->v = sf::Vector2i(0, -1);
                needAttackChange = true;
                needShowAttack = true;
                needAttack = true;
                break;
            case sf::Keyboard::Left:
                player->GetComp<CompDir>()->v = sf::Vector2i(-1, 0);
                needAttackChange = true;
                needShowAttack = true;
                needAttack = true;
                break;
            case sf::Keyboard::Right:
                player->GetComp<CompDir>()->v = sf::Vector2i(1, 0);
                needAttackChange = true;
                needShowAttack = true;
                needAttack = true;
                break;
            }
            if (evt.key.code >= sf::Keyboard::Num1 && evt.key.code <= sf::Keyboard::Num9)
            {
                unsigned weapId = (evt.key.code - sf::Keyboard::Num1);
                if (items->potionActive)
                {
                    weapId+=items->potionHotbarStartPosition*9;
                    if (weapId<items->potions.size())
                    {
                        items->curPotionId=weapId;
                    }
                    items->potionActive=false;
                }
                else
                {
                    weapId+=items->weaponHotbarStartPosition*9;
                    if (weapId < items->weapons.size())
                    {
                        items->curWeaponId = weapId;
                        needAttackChange = true;
                    }
                }
            }
            break;
        
        case game::EventSfml::KeyReleased:
            switch (evt.key.code)
            {
            case sf::Keyboard::Up:
            case sf::Keyboard::Down:
            case sf::Keyboard::Left:
            case sf::Keyboard::Right:
                turnOnCD = true;
                break;
            }
            break;
        }
        if(turnOnCD)
        {
            auto attack = ecs::gEntityManager.GetEntityByDesc(
                player->GetComp<CompCurrentAttackArea>()->ent);
            if(attack)
            {
                auto cd = attack->GetComp<CompCoolDown>();
                if(!cd->is_updating)
                    cd->reset();
            }
        }
        if (needAttack && items->curWeaponId != -1)
        {
            IEntity *attackArea = ecs::gEntityManager.GetEntityByDesc(
                player->GetComp<CompCurrentAttackArea>()->ent);
            bool attack_res=(items->curWeaponId>=0) |(items->curWeaponId<items->weapons.size());
            if (attack_res)
            {
                const game::Weapon *w=items->weapons[items->curWeaponId];
                game::AttackFuncts::Attack(w->attack_type,w,player,attackArea);
            }
            
            
        }
        inputPool.pop();
    }
}