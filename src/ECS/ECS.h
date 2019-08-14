#ifndef ECS_H
#define ECS_H

#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <bitset>
#include <array>

class Component;
class Entity;
class Manager;

using ComponentID = std::size_t;
using Group = std::size_t;

inline ComponentID getNewComponentID()
{
    static ComponentID lastID = 0u;
    return lastID++;
}

template <typename T>
inline ComponentID getComponentID() noexcept
{
    static_assert (std::is_base_of<Component, T>::value, "");
    static ComponentID typeID = getNewComponentID();
    return typeID;
}

constexpr std::size_t maxComponents = 32;
constexpr std::size_t maxGroups = 32;

using ComponentBitSet = std::bitset<maxComponents>;
using GroupBitSet = std::bitset<maxGroups>;

using ComponentArray = std::array<Component *, maxComponents>;

class Component
{
public:
    Entity *entity;

    virtual void init() {}
    virtual void update() {}
    virtual void draw() {}
    virtual ~Component() {}
};

class Entity
{
private:
    Manager& manager;
    bool active = true;
    std::vector<std::unique_ptr<Component>> components;

    ComponentArray componentArray;
    ComponentBitSet componentBitSet;
    GroupBitSet groupBitset;

public:
    Entity(Manager& mManager) : manager(mManager) {}

    void init()
    {
        for (auto &c : components)
            c->init();
    }
    void update()
    {
        for (auto &c : components)
            c->update();
    }
    void draw()
    {
        for (auto &c : components)
            c->draw();
    }
    bool isActive() const { return active; }
    void destroy() { active = false; }

    bool hasGroup(Group mGroup)
    {
        return groupBitset[mGroup];
    }
    template <typename T>
    bool hasComponent() const
    {
        return componentBitSet[getComponentID<T>()];
    }

    void addGroup(Group mGroup);
    void delGroup(Group mGroup)
    {
        groupBitset[mGroup] = false;
    }

    template <typename T, typename... TArgs>
    T &addComponent(TArgs &&... mArgs)
    {
        T *c(new T(std::forward<TArgs>(mArgs)...));
        c->entity = this;
        std::unique_ptr<Component> uPtr{c};
        components.emplace_back(std::move(uPtr));

        componentArray[getComponentID<T>()] = c;
        componentBitSet[getComponentID<T>()] = true;

        c->init();
        return *c;
    }

    template <typename T>
    T &getComponent() const
    {
        auto ptr(componentArray[getComponentID<T>()]);
        return *static_cast<T *>(ptr);
    }
};

class Manager
{
private:
    std::vector<std::unique_ptr<Entity>> entities;
    std::array<std::vector<Entity*>, maxGroups> groupedEntites;

public:
    void init()
    {
        for (auto &e : entities)
            e->init();
    }
    void update()
    {
        for (auto &e : entities)
            e->update();
    }
    void draw()
    {
        for (auto &e : entities)
            e->draw();
    }

    void refresh()
    {
        for( auto i(0u); i < maxGroups ; i++)
        {
            auto& v(groupedEntites[i]);
            v.erase(std::remove_if(std::begin(v), std::end(v),
                                   [i](Entity *mEntity) {
                                       return !mEntity->isActive() || !mEntity->hasGroup(i);
                                   }),
                    std::end(v));
        }

        entities.erase(std::remove_if(std::begin(entities), std::end(entities),
                                      [](const std::unique_ptr<Entity> &mEntity) {
                                          return !mEntity->isActive();
                                      }),
                       std::end(entities));
    }
    void AddToGroup(Entity* mEntity, Group mGroup)
    {
        groupedEntites[mGroup].emplace_back(mEntity);
    }
    std::vector<Entity*>& getGroup(Group mGroup)
    {
        return groupedEntites[mGroup];
    }

    Entity &addEntity()
    {
        Entity *e = new Entity(*this);
        std::unique_ptr<Entity> uPtr{e};
        entities.emplace_back(std::move(uPtr));
        return *e;
    }
};

#endif