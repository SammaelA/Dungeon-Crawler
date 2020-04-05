#pragma once
#include "stl.h"
#include <SFML/Graphics.hpp>
namespace game {


extern std::map<std::string, int> gNamedTiles;
extern std::map<std::string, int> gNamedAnimations;
extern std::map<std::string, int> gNamedTextures;

bool loadResources(const std::string &path);

void setSpawnSingle(uint32_t pattern, std::function<bool(sf::Vector2f)> &&callback);
void setSpawnPattern(uint32_t pattern, std::function<bool(std::vector<sf::Vector2f>&)> &&callback);

bool loadLevelMap(const std::string& img_path);
bool loadAnimatedObjects(const std::string &s);
}


