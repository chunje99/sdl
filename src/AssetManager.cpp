#include "AssetManager.h"

AssetManager::AssetManager(Manager *man) : manager(man)
{}

AssetManager::~AssetManager()
{}

void AssetManager::AddTexture(std::string id, const char* path)
{
    textures.emplace(id, CSurface::Load(path));
}

SDL_Texture* AssetManager::GetTexture(std::string id)
{
    return textures[id];
}
