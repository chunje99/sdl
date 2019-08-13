#ifndef SDL_MAP_H
#define SDL_MAP_H

#include <string>

class Map
{
public:
    Map();
    ~Map();
    static void LoadMap(std::string path, int sizeX, int sizeY);
};

#endif