#include "EcsSystems.h"
#include "Game.h"
#include "Renderer.h"
#include "Utils.h"
#include <climits>

static const sf::Vector2i cellCount=sf::Vector2i(CELL_X,CELL_Y);
static const sf::Vector2i cellSize=sf::Vector2i(WIDTH/cellCount.x,HEIGHT/cellCount.y);
static const sf::Vector2i mapCellCount=sf::Vector2i(CELL_X,CELL_Y-4);
static const sf::Vector2i mapPos=sf::Vector2i(0,2);
static const sf::Vector2i healthPos=sf::Vector2i(1,0);
static const sf::Vector2i manaPos=sf::Vector2i(5,0);
static const sf::Vector2i staminaPos=sf::Vector2i(10,0);
static const sf::Vector2i curWeaponPos = sf::Vector2i(0,11);
static const sf::Vector2i weaponsPos = sf::Vector2i(6,11);
static const sf::Vector2i potionsPos = sf::Vector2i(6,13);
static const sf::Vector2i curItemNamePos = sf::Vector2i(0,13);
static const sf::Vector2i curItemDescriptionPos = sf::Vector2i(16,11);
static const sf::Vector2i wSize = sf::Vector2i(WIDTH,HEIGHT);
static const int itemsBarLen=9;
static const int fontSize=cellSize.x/2;
static const int SmallFontSize=cellSize.x/2;
static const int baseInfoTextLen=26;
static const sf::Color textColor=sf::Color(0X000000FFU);
static stl::map<stl::string, int> guiTextures;
std::string makeFullWeaponDescription(const game::Weapon *w,int maxStrLen)
{
	
    std::string base;
    char buffer[1024+1];
    base.append(w->name.c_str());
    base.append("\n");
    std::snprintf(buffer,128, "Physical damage:%.1f\n", w->physDamage);
    base.append(buffer);
    std::snprintf(buffer,128, "Magical damage:%.1f\n", w->magicDamage);
    base.append(buffer);
    std::snprintf(buffer,128, "Mana consumption:%.1f\n", w->manaConsumption);
    base.append(buffer);
    std::snprintf(buffer,128, "Stamina Consumptuion:%.1f\n", w->staminaConsumption);
    base.append(buffer);
    std::snprintf(buffer,128, "Attack speed:%.1f\n", w->attack_speed*100);
    base.append(buffer);
    int k=-1;
    for (int i=0;i<w->description.length();i++)
    {
        k++;
        buffer[k]=w->description[i];
        if (k==1024-2)
        {
            
            buffer[k+1]='\n';
            buffer[k+2]='\0';
            base.append(buffer);
            k=-1;
        }
        
        
    }
    buffer[k+1]='\n';
    buffer[k+2]='\0';
    base.append(buffer);
    return(base);
}
std::string makeFullPotionDescription(const game::Potion *w,int maxStrLen)
{
    std::string base;
    char buffer[1024+1];
    base.append(w->name.c_str());
    base.append("\n");
    std::snprintf(buffer,128, "Health :%.1f\n", w->healthAmount);
    base.append(buffer);
    std::snprintf(buffer,128, "Mana:%.1f\n", w->magicAmount);
    base.append(buffer);
    std::snprintf(buffer,128, "Stamina:%.1f\n", w->staminaAmount);
    base.append(buffer);
    std::snprintf(buffer,128, "Overwhelming:%s\n", (w->overwhelming)?"true":"false");
    base.append(buffer);
    int k=-1;
    for (int i=0;i<w->description.length();i++)
    {
        k++;
        buffer[k]=w->description[i];
        if (k==1024-2)
        {
            
            buffer[k+1]='\n';
            buffer[k+2]='\0';
            base.append(buffer);
            k=-1;
        }
        
    }
    buffer[k+1]='\n';
    buffer[k+2]='\0';
    base.append(buffer);
    return(base);
}
void GameGuiRenderSys::OnEvent(const game::EventRenderWindowWasCreated &evt)
{
    evt.wnd->setSize(sf::Vector2u(wSize));
}

void GameGuiRenderSys::OnEvent(const game::EventRendererWasInited &evt)
{
    guiTextures.emplace("backBlock", game::gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_1969.png"));
    guiTextures.emplace("inventoryPanel", game::gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_1925.png"));  
    guiTextures.emplace("emptySlot", game::gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_657.png"));  
    guiTextures.emplace("chosenSlot", game::gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_1663.png"));  
    guiTextures.emplace("notChosenSlot", game::gRenderer.AddTexture("tileset/Sliced/world_24x24/oryx_16bit_fantasy_world_1416.png"));
    guiTextures.emplace("start_tex", game::gRenderer.AddTexture("start.png"));
    guiTextures.emplace("finish_tex", game::gRenderer.AddTexture("finish.png"));
    game::gRenderer.GuiAddTexturedRect("start_tex",4,{0,0},
    {wSize.x,wSize.y},guiTextures["start_tex"],{0,0},wSize);
    game::gRenderer.GuiAddTexturedRect("finish_tex",4,{0,0},
    {wSize.x,wSize.y},guiTextures["finish_tex"],{0,0},wSize);
    game::gRenderer.GuiAddTexturedRect("backBlock", -1,
        { 0, 0 },
        { wSize.x, wSize.y }, guiTextures["backBlock"],
        { 0, 0 }, wSize);
    game::gRenderer.GuiAddTexturedRect("inventoryPanel", 1,
        { 0, cellSize.y*11 },
        { wSize.x ,cellSize.y*5}, guiTextures["inventoryPanel"],
        { 0, cellSize.y*11 },
        { wSize.x ,cellSize.y*5});
    game::gRenderer.GuiAddTexturedRect("infoPanel", 1,
        { 0, 0 },
        { wSize.x ,cellSize.y}, guiTextures["inventoryPanel"],
        { 0, cellSize.y*11 },
        { wSize.x ,cellSize.y*5});
    game::gRenderer.GuiAddText("healthLabel", 2, {healthPos.x*cellSize.x,healthPos.y*cellSize.y},
        "Health: ", fontSize, textColor);
    game::gRenderer.GuiAddText("staminaLabel", 2, {staminaPos.x*cellSize.x,staminaPos.y*cellSize.y},
        "Stamina: ",fontSize, textColor);
    game::gRenderer.GuiAddText("manaLabel", 2, {manaPos.x*cellSize.x,manaPos.y*cellSize.y},
        "Mana: ",fontSize, textColor);

    game::gRenderer.GuiAddText("healthBar", 2, {(int)((healthPos.x+2.33)*cellSize.x),(healthPos.y)*cellSize.y}, "100.0",fontSize,textColor);
    game::gRenderer.GuiAddText("staminaBar", 2, {(int)((staminaPos.x+2.7)*cellSize.x),(staminaPos.y)*cellSize.y}, "100.0",fontSize,textColor);
    game::gRenderer.GuiAddText("manaBar", 2, {(int)((manaPos.x+1.9)*cellSize.x),(manaPos.y)*cellSize.y}, "100.0",fontSize,textColor);

    game::gRenderer.GuiAddText("curItemInfo", 3,{(curItemNamePos.x)*cellSize.x,(curItemNamePos.y)*cellSize.y},""
    ,SmallFontSize,textColor);

    game::gRenderer.GuiAddText("curDescriptionInfo", 3,{(curItemDescriptionPos.x)*cellSize.x,
    (curItemDescriptionPos.y)*cellSize.y},"",SmallFontSize/2,textColor);


    for (int i = 0; i < itemsBarLen; ++i)
    {
        constexpr int bufSize = 16;
        char buf[bufSize] = {};
        std::snprintf(buf, bufSize, "weapon%d", i);
        game::gRenderer.GuiAddTile(buf, 3, {cellSize.x*(weaponsPos.x+i),cellSize.y*weaponsPos.y},
            cellSize, 0);
    }
    for (int i = 0; i < itemsBarLen; ++i)
    {
        constexpr int bufSize = 16;
        char buf[bufSize] = {};
        std::snprintf(buf, bufSize, "potion%d", i);
        game::gRenderer.GuiAddTile(buf, 3, {cellSize.x*(potionsPos.x+i),cellSize.y*potionsPos.y},
            cellSize, 0);
    }
    game::gRenderer.GuiAddTile("WeaponBar",2,{cellSize.x*curWeaponPos.x,cellSize.y*curWeaponPos.y},2*cellSize,0);
    game::gRenderer.GuiAddTile("PotionBar",2,{cellSize.x*(curWeaponPos.x+2),cellSize.y*curWeaponPos.y},2*cellSize,0);

    game::gRenderer.GuiAddTile("WeaponChosen",3,{cellSize.x*curWeaponPos.x,cellSize.y*curWeaponPos.y},2*cellSize,0);
    game::gRenderer.GuiAddTile("PotionChosen",3,{cellSize.x*(curWeaponPos.x+2),cellSize.y*curWeaponPos.y},2*cellSize,0);

    game::gRenderer.SetVisionAreaSize(sf::Vector2f(mapCellCount.x,mapCellCount.y));
    game::gRenderer.SetCameraViewport(cellSize.x*mapPos.x, cellSize.x*mapPos.x, mapCellCount.x*cellSize.x, mapCellCount.y*cellSize.y);
    game::gRenderer.SetCameraCenterPos(sf::Vector2f(113.0f,85.0f));
}

void GameGuiRenderSys::OnUpdate()
{
    if (game::gstate==STATE_START)
    {
        game::gRenderer.GuiChangeSize("finish_tex",{1,1});
        game::gRenderer.GuiChangeSize("start_tex",{wSize.x,wSize.y});
    }
    if (game::gstate==STATE_GAME)
    {
        game::gRenderer.GuiChangeSize("finish_tex",{1,1});
        game::gRenderer.GuiChangeSize("start_tex",{1,1});
    }
    if (game::gstate==STATE_FINISH)
    {
        game::gRenderer.GuiChangeSize("start_tex",{1,1});
        game::gRenderer.GuiChangeSize("finish_tex",{wSize.x,wSize.y});
    }
    ecs::Apply<CompHealth, CompStamina, CompMana, CompOwnedItems, TagPlayer>([&](IEntity *ent)
    {
        if (ent->GetComp<CompHealth>()->value<=0.0f){
            game::gstate=STATE_FINISH;
        }
        constexpr int bufSize = 1024;
        char buf[bufSize] = {};
        std::snprintf(buf, bufSize, "%.1f", ent->GetComp<CompHealth>()->value);
        game::gRenderer.GuiChangeText("healthBar", buf);
        std::snprintf(buf, bufSize, "%.1f", ent->GetComp<CompStamina>()->value);
        game::gRenderer.GuiChangeText("staminaBar", buf);
        std::snprintf(buf, bufSize, "%.1f", ent->GetComp<CompMana>()->value);
        game::gRenderer.GuiChangeText("manaBar", buf);
        
        CompOwnedItems *items = ent->GetComp<CompOwnedItems>();
        
        
            
        
        int i=0,k;
        for (i=0;i<itemsBarLen;i++)
        {
            k=i+items->weaponHotbarStartPosition*itemsBarLen;
            if (k>=items->weapons.size()) break;
            std::snprintf(buf, bufSize, "weapon%d", i);
            game::gRenderer.GuiChangeTile(buf, items->weapons[k]->tileId);
        }
        for (;i<itemsBarLen;i++)
        {
            std::snprintf(buf, bufSize, "weapon%d", i);
            game::gRenderer.GuiChangeTile(buf, game::gRenderer.AddTile(guiTextures["emptySlot"]));
        }


        for (i=0;i<itemsBarLen;i++)
        {
            k=i+items->potionHotbarStartPosition*itemsBarLen;
            if (k>=items->potions.size()) break;
            std::snprintf(buf, bufSize, "potion%d", i);
            game::gRenderer.GuiChangeTile(buf, items->potions[k]->tileId);
        }
        for (;i<itemsBarLen;i++)
        {
            std::snprintf(buf, bufSize, "potion%d", i);
            game::gRenderer.GuiChangeTile(buf, game::gRenderer.AddTile(guiTextures["emptySlot"]));
        }
        if (items->curWeaponId >= 0)
        {
            game::gRenderer.GuiChangeTile("WeaponBar", items->weapons[items->curWeaponId]->tileId);
            if (!items->potionActive)
            {
                game::gRenderer.GuiChangeText("curItemInfo",items->weapons[items->curWeaponId]->name.c_str());
                game::gRenderer.GuiChangeText("curDescriptionInfo",
                makeFullWeaponDescription(items->weapons[items->curWeaponId],baseInfoTextLen).c_str());
            }
                
        }
        else
        {
            game::gRenderer.GuiChangeTile("WeaponBar", game::gRenderer.AddTile(guiTextures["emptySlot"]));
            if (!items->potionActive)
            {
                game::gRenderer.GuiChangeText("curItemInfo","");
                game::gRenderer.GuiChangeText("curDescriptionInfo","");
            }
        }
        if (items->curPotionId >= 0)
        {
            game::gRenderer.GuiChangeTile("PotionBar", items->potions[items->curPotionId]->tileId);
            if (items->potionActive)
            {
                game::gRenderer.GuiChangeText("curItemInfo",items->potions[items->curPotionId]->name.c_str());
                game::gRenderer.GuiChangeText("curDescriptionInfo",
                makeFullPotionDescription(items->potions[items->curPotionId],baseInfoTextLen).c_str());
            }
        }
        else
        {
            game::gRenderer.GuiChangeTile("PotionBar", game::gRenderer.AddTile(guiTextures["emptySlot"]));
            if (items->potionActive)
            {
                game::gRenderer.GuiChangeText("curItemInfo","");
                game::gRenderer.GuiChangeText("curDescriptionInfo","");
            }
        }
        if (items->potionActive)
        {
            game::gRenderer.GuiChangeTile("WeaponChosen",game::gRenderer.AddTile(guiTextures["notChosenSlot"]));
            game::gRenderer.GuiChangeTile("PotionChosen",game::gRenderer.AddTile(guiTextures["chosenSlot"]));
        }
        else
        {
            game::gRenderer.GuiChangeTile("WeaponChosen",game::gRenderer.AddTile(guiTextures["chosenSlot"]));
            game::gRenderer.GuiChangeTile("PotionChosen",game::gRenderer.AddTile(guiTextures["notChosenSlot"]));
        }
        
        
    });
    game::gRenderer.DrawGui(0, INT_MAX);
    ecs::Apply<CompFloatingText, CompPos>([&](IEntity *text)
    {   
        auto pos = text->GetComp<CompPos>()->v;
        game::gRenderer.DrawText(text->GetComp<CompFloatingText>()->tid,
            game::gRenderer.GetAbsPos(sf::Vector2f(pos)));
    });
}

void GameBackgroundGuiRenderSys::OnUpdate()
{
    game::gRenderer.DrawGui(INT_MIN, -1);
}