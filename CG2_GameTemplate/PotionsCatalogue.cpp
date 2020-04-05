#include "PotionsCatalogue.h"
#include <string.h>
namespace game
{
    PotionsCatalogue pc;
    stl::vector<Potion> ps;
void PotionsCatalogue::Init(int type)
{
    Potion p;
    int id;

    p.healthAmount=10.0;
    p.magicAmount=0.0;
    p.staminaAmount=0.0;
    p.overwhelming=false;
    id = gRenderer.AddTexture("tileset/Sliced/items_16x16/oryx_16bit_fantasy_items_01.png");
    p.name="Small health potion";
    p.description="Small health potion will heal you a little bit.";
    p.tileId=gRenderer.AddTile(id);
    ps.push_back(p);

    p.healthAmount=0.0;
    p.magicAmount=15.0;
    p.staminaAmount=0.0;
    p.overwhelming=false;
    id = gRenderer.AddTexture("tileset/Sliced/items_16x16/oryx_16bit_fantasy_items_02.png");
    p.name="Small magic potion";
    p.description="It will slightly recover your magic powers.";
    p.tileId=gRenderer.AddTile(id);
    ps.push_back(p);

    p.healthAmount=0.0;
    p.magicAmount=0.0;
    p.staminaAmount=20.0;
    p.overwhelming=false;
    id = gRenderer.AddTexture("tileset/Sliced/items_16x16/oryx_16bit_fantasy_items_03.png");
    p.name="Малое зелье энергии";
    p.description="Взбодрит вас";
    p.tileId=gRenderer.AddTile(id);
    ps.push_back(p);

    p.healthAmount=-25.0;
    p.magicAmount=0.0;
    p.staminaAmount=0.0;
    p.overwhelming=true;
    id = gRenderer.AddTexture("tileset/Sliced/items_16x16/oryx_16bit_fantasy_items_04.png");
    p.name="Малое ядовитое зелье";
    p.description="Не стоит его пить.";
    p.tileId=gRenderer.AddTile(id);
    ps.push_back(p);

    p.healthAmount=0.0;
    p.magicAmount=-15.0;
    p.staminaAmount=50.0;
    p.overwhelming=true;
    id = gRenderer.AddTexture("tileset/Sliced/items_16x16/oryx_16bit_fantasy_items_05.png");
    p.name="Малое высокоэнергетичное зелье";
    p.description="Восстановит ваши силы, забрав взамен магическую энергию.";
    p.tileId=gRenderer.AddTile(id);
    ps.push_back(p);

    p.healthAmount=10.0;
    p.magicAmount=10.0;
    p.staminaAmount=10.0;
    p.overwhelming=false;
    id = gRenderer.AddTexture("tileset/Sliced/items_16x16/oryx_16bit_fantasy_items_06.png");
    p.name="Малое святое зелье";
    p.description="Восстановит здоровье, магические силы и вернет бодрость духа";
    p.tileId=gRenderer.AddTile(id);
    ps.push_back(p);

}
Potion *PotionsCatalogue::getPotion(int ID)
{
    Potion *p=&ps[ID%ps.size()];
    return(p);
}
Potion *PotionsCatalogue::getRandom()
{
    int k=rand()%ps.size();
    return(&ps[k]);
}
}