#include "Entity.hpp"
#include "EntityWorld.hpp"

Entity::Entity(EntityWorld& world, EnttId id) 
    : m_world(world)
    , m_id(id),
{

}

EntityWorld& Entity::getWorld()
{
    return m_world
}

EnttId Entity::getId()
{
    return m_id;
}
