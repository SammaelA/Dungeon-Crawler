#include "stl.h"
#include "EcsSystems.h"
#include "Renderer.h"

#define LAYERS_COUNT CompDrawTileInfo::LAYERS_COUNT


stl::map<CompDrawTileInfo *, CompPos *> singleObjects[LAYERS_COUNT];
stl::map<CompDrawTileInfo *, CompPosPattern *> patternObjects[LAYERS_COUNT];
std::map<CompAnimInfo *, CompPos *> animatedObects[LAYERS_COUNT];

void GameRenderSys::OnUpdate()
{
    sf::Vector2f camPos = sf::Vector2f(0, 0);
    
    /*std::vector<CompFloatingText *> text;*/

    ecs::Apply<CompPos>([&](ecs::IEntity *ent)
    {   
        CompPos *pos = ent->GetComp<CompPos>();
        if (ent->GetComp<TagPlayer>())
                camPos = sf::Vector2f(pos->v) + sf::Vector2f(0.5f, 0.5f);
        
        CompVisibility *vis = ent->GetComp<CompVisibility>();
        if (vis)
        {
            if (!vis->visible)
                return;
            if (vis->hideAfterOneTick)
                vis->visible = false;
        }
        CompDrawTileInfo *info = ent->GetComp<CompDrawTileInfo>();
        if(info)
        {
            if (info->layer < LAYERS_COUNT)
            {
                singleObjects[info->layer].emplace(info, pos);
            }
        } else {
            CompAnimInfo *ainf = ent->GetComp<CompAnimInfo>();
            if(ainf && ainf->layer < LAYERS_COUNT)
            {
                animatedObects[ainf->layer].emplace(ainf, pos);
            }
        }
    });

    ecs::Apply<CompPosPattern, CompDrawTileInfo>([&](ecs::IEntity *ent)
    {
        CompVisibility *vis = ent->GetComp<CompVisibility>();
        if (vis)
        {
            if (!vis->visible)
                return;
            if (vis->hideAfterOneTick)
                vis->visible = false;
        }
        CompDrawTileInfo *info = ent->GetComp<CompDrawTileInfo>();
        if (info->layer < LAYERS_COUNT)
        {
            CompPosPattern *pattern = ent->GetComp<CompPosPattern>();
            pattern->positions.tileId = info->tileId;
            patternObjects[info->layer].emplace(info, pattern);
        }
    });
    /*
    ecs::Apply<CompFloatingText>([&](IEntity *ent)
    {
        text.push_back(ent->GetComp<CompFloatingText>());
    }); */

    game::gRenderer.SetCameraCenterPos(camPos);
    game::gRenderer.BeginDrawWorld();
    for (int i = 0; i < LAYERS_COUNT; ++i)
    {
        for (auto &surf : patternObjects[i])
            game::gRenderer.DrawTiledSurface(surf.second->positions);
        for (auto &obj : singleObjects[i])
        {
            game::gRenderer.DrawSingleTile(obj.first->tileId, sf::Vector2f(obj.second->v.x, obj.second->v.y));
        }
        for(auto &obj : animatedObects[i]){
            game::gRenderer.DrawAnimationFrame(
                obj.first->id, obj.first->frame, 
                sf::Vector2f(obj.second->v)
            );
            obj.first->update();
        }
        singleObjects[i].clear();
        patternObjects[i].clear();
        animatedObects[i].clear();
    }
}