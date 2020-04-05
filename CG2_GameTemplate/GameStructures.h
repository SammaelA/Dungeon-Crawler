#pragma once

#include "Renderer.h"
#include "ECS.h"
namespace game
{

struct TiledSurface : public ITiledSurface
{
    // Pos of the bottom left cornder
    // in world coordinates (measured in tiles).
    sf::Vector2i pos = sf::Vector2i(0, 0);
    sf::Vector2u size = sf::Vector2u(0, 0);
    unsigned **pattern = nullptr; // 2D matrix with tile IDs.

    bool Init(unsigned width, unsigned height, unsigned value);
    void Release();

    inline sf::Vector2i GetPos() const override { return pos; }
    inline sf::Vector2u GetSize() const override { return size; }
    inline unsigned GetTileId(int x, int y) const override
    {
        x -= pos.x;
        y -= pos.y;
        return x < size.x && x >= 0 && y < size.y && y >= 0 ? pattern[x][y] : 0;
    }
};

struct PosPattern : public ITiledSurface
{
    // Pos of the bottom left cornder
    // in world coordinates (measured in tiles).
    sf::Vector2i pos = sf::Vector2i(0, 0);
    sf::Vector2u size = sf::Vector2u(0, 0);
    unsigned tileId = 0;
    bool **pattern = nullptr;

    bool Init(unsigned width, unsigned height, bool value);
    void Release();

    inline sf::Vector2i GetPos() const override { return pos; }
    inline sf::Vector2u GetSize() const override { return size; }
    inline bool IsPosExists(int x, int y) const
    {
        x -= pos.x;
        y -= pos.y;
        return x < size.x && x >= 0 && y < size.y && y >= 0 ? unsigned(pattern[x][y]) : 0;
    }
    inline unsigned GetTileId(int x, int y) const override
    {
        x -= pos.x;
        y -= pos.y;
        return x < size.x && x >= 0 && y < size.y && y >= 0 ? unsigned(pattern[x][y]) * tileId : 0;
    }
};

struct Weapon
{
    sf::Vector2u areaSize = sf::Vector2u(0, 0);
    int shift = 0;
    // Interpreted as 2d array.
    // Pattern's initial direction id (0, 1).
    //const bool *pattern = nullptr;
    stl::vector<bool> pattern;
    float physDamage = 0.0f;
    float magicDamage = 0.0f;
    float staminaConsumption = 0.0f;
    float manaConsumption = 0.0f;
    float attack_speed=1.0f;
    unsigned tileId = 0;
    int attack_type=1;
    std::string name="Неопознанное оружие";
    std::string description="Никто не знает, что это такое";

};

struct Potion
{
    float healthAmount = 0.0f;
    float staminaAmount = 0.0f;
    float magicAmount = 0.0f;
    bool overwhelming=false;
    unsigned tileId = 0;
    std::string name="Неопознанное зелье";
    std::string description="Никто не знает, что оно сделает с тобой...";
};

class GameEffect {
public:
    virtual void OnCast(ecs::IEntity *entity) = 0;
    virtual bool OnTick() = 0;
    virtual ~GameEffect() {}
    ecs::IEntity *owner=nullptr;
};

}