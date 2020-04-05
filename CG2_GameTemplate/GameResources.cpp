#include "GameResources.h"
#include "Renderer.h"
#include <fstream>
#include <iostream>
#include <cctype>
namespace game {

std::map<std::string, int> gNamedTiles; /*Нужная вещь*/
std::map<std::string, int> gNamedAnimations;
std::map<std::string, int> gNamedTextures;

using handler_type = bool(*)(const std::vector<std::string> &);

static bool split_on_commands(const std::string &s, std::vector<std::string>& commands){
    enum state { WORD, SPACE, BRACES};
    state st = SPACE;
    std::string temp;
    for(char c : s){
        switch (st)
        {
        case SPACE:
            if(std::isspace(c)) continue;
            if(c == '"') { st = BRACES; continue; }
            /*default*/ st = WORD; temp.push_back(c); break;
        case WORD:
            if(std::isspace(c)){
                st = SPACE;
                if(temp.size()){
                    //std::cout << "insert " << temp << "\n";
                    commands.push_back(temp);
                    temp.clear();
                }
                continue;
            }
            if(c == '"'){
                st = BRACES;
                if(temp.size()){
                    //std::cout << "insert " << temp << "\n";
                    commands.push_back(temp);
                    temp.clear();
                }
                continue;
            }
            temp.push_back(c);
            break;
        case BRACES:
            if(c == '"'){
                st = SPACE;
                if(temp.size()){
                    //std::cout << "insert " << temp << "\n";
                    commands.push_back(temp);
                    temp.clear();
                }
                continue;
            }
            temp.push_back(c); break;
        default:
            std::cout << "unexpected symbol " << c << "\n";
            return false;
            break;
        }
    }
    if(temp.size()){
        //std::cout << "insert " << temp << "\n";
        commands.push_back(temp);
    }
    return true;
}

static bool texture_add(const std::vector<std::string>& vector){
    if(vector.size() != 3){
        std::cout << "bad command format\n";
        return false;
    }
    const auto& name = vector[1];
    const auto& path = vector[2];
    int id = gRenderer.AddTexture(path.c_str());
    if(id == -1){
        std::cout << "bad load path : " << path << "\n";
        return false;
    }
    auto itr = gNamedTextures.find(name);
    if(itr == gNamedTextures.end()){
        gNamedTextures[name] = id;
        return true;
    } else {
        std::cout << "reinserting texture " << name <<"\n";
        return false;
    }
}

static bool tile_add(const std::vector<std::string>& vector){
    if(vector.size() != 3){
        std::cout << "bad command format\n";
        return false;
    }
    const auto& name = vector[1];
    const auto& texname = vector[2];
    auto itr = gNamedTextures.find(texname);
    if(itr == gNamedTextures.end()){
        std::cout << "bad texture name " << texname << "\n";
        return false;
    }
    auto itr2 = gNamedTiles.find(name);
    if(itr2 == gNamedTiles.end()){
        gNamedTiles[name] = gRenderer.AddTile(itr->second);
        return true;
    } else {
        std::cout << "reinserting tile " << name << "\n";
        return false;
    }
}

static bool tile_cut(const std::vector<std::string>& vector){
    if(vector.size() != 7){
        std::cout << "bad command format\n";
        return false;
    }
    auto& name = vector[1];
    auto& tex = vector[2];
    auto x = std::atoi(vector[3].c_str()), y = std::atoi(vector[4].c_str()),
         w = std::atoi(vector[5].c_str()), h = std::atoi(vector[6].c_str());
    auto itr = gNamedTiles.find(name);
    if(itr != gNamedTiles.end()){
        std::cout << "reinserting tile " << name << "\n";
        return false;
    }
    if(gNamedTextures.find(tex) == gNamedTextures.end()){
        std::cout << "bad texture name " << tex << "\n";
        return false; 
    }
    gNamedTiles[name] = gRenderer.AddTile(gNamedTextures[tex], 
        sf::Vector2i(x, y), sf::Vector2i(w, h));
    return true;
}

static bool animation_add(const std::vector<std::string>& vector){
    if(vector.size() < 3){
        std::cout << "bad command format\n";
        return false;
    }

    std::vector<int> anim_idx;

    auto vec_itr = vector.begin();
    vec_itr++;
    const auto& name = *vec_itr;
    auto nitr = gNamedAnimations.find(name);
    if(nitr != gNamedAnimations.end()){
        std::cout << "reinserting animation " << name << "\n";
        return false;
    }

    vec_itr++;
    while(vec_itr != vector.end()){
        auto itr = gNamedTiles.find(*vec_itr);
        if(itr == gNamedTiles.end()){
            std::cout << "bad tile name " << *vec_itr << "\n";
            return false;
        }
        anim_idx.push_back(itr->second);
        vec_itr++;
    }
    int id = gRenderer.AddAnimation(std::move(anim_idx));
    gNamedAnimations[name] = id;
    return true;
}

bool loadResources(const std::string &s){
    std::ifstream file;
    file.open(s);
    if(!file){
        std::cout << "loadResources : bad file " << s << "\n";
        return false; 
    }
    std::string strbuf;
    std::vector<std::string> commands;
    int stringnum = 0;
    while(file){
        stringnum++;
        std::getline(file, strbuf);
        if(!strbuf.size()) continue;
        if(!split_on_commands(strbuf, commands)){
            std::cout <<"string " << stringnum << " - parse error\n";
        }
        handler_type handler = nullptr;
        if(commands.size()){
            auto name = commands[0];
            if(name == "texture")
                handler = texture_add;
            else if(name == "tile")
                handler = tile_add;
            else if(name == "animation")
                handler = animation_add;
            else if(name == "tilecut")
                handler = tile_cut;
            else {
                std::cout << "bad command: line " << stringnum << " file " << s << "\n";
                return false;
            }
            if(!handler(commands)){
                std::cout << "bad command: line " << stringnum << " file " << s << "\n";
                return false;
            }
        }
        commands.clear();
    }
    return true;
} 
static std::map<uint32_t, std::function<bool(sf::Vector2f)>> singleObjs;
static std::map<uint32_t, std::function<bool(std::vector<sf::Vector2f>&)>> paternObjs;

void setSpawnSingle(uint32_t pattern, std::function<bool(sf::Vector2f)> &&callback){
    singleObjs[pattern] = callback;
}

void setSpawnPattern(uint32_t pattern, std::function<bool(std::vector<sf::Vector2f>&)> &&callback){
    paternObjs[pattern] = callback;
}

bool loadLevelMap(const std::string& img_path){
    std::map<uint32_t, std::vector<sf::Vector2f>> mapColors;
    sf::Image texturemap;
    if(!texturemap.loadFromFile(img_path)){
        std::cout << "load level error : bad file " << img_path << "\n";
        return false;
    }
    auto size = texturemap.getSize();
    for(auto y = 0; y < size.y; y++){
        for(auto x = 0; x < size.x; x++){
            auto color = texturemap.getPixel(x, size.y - y - 1).toInteger();
            color >>= 8;
            mapColors[color].push_back(sf::Vector2f(x, y));
        }
    }
    for(auto& elem : singleObjs){
        for(auto pos : mapColors[elem.first]){
            if(!elem.second(pos)){
                std::cout << "creating error\n";
                return false;
            }
        }
    }
    for(auto& elem : paternObjs){
        if(!elem.second(mapColors[elem.first])){
            std::cout << "creating error\n";
            return false;
        }
    }
    return true;
}

}