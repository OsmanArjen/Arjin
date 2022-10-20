#include "EntityWorld.hpp"

//-EntityRange
EntityRange::EntityRange(iterator begin, size_type size)
	: m_begin(begin), m_size(size) {}

iterator EntityRange::begin() const
{
    return m_begin;
}

iterator EntityRange::end() const
{
    return (m_begin + m_size);
}

const_iterator EntityRange::cbegin() const 
{
    return begin();
}

const_iterator EntityRange::cend() const 
{
    return end();
}

const EntityId& EntityRange::operator[](size_type idx) const 
{
	assert((idx < m_size));
	return (m_begin + idx)*; 
}


//-EntityWorld
EntityWorld::EntityWorld(size_type npool)
	: m_compPools{npool} {}

EntityId EntityWorld::create()
{
    auto deadIdx = m_aliveCount++;
    if(deadIdx < m_entityDense.size())
    {
        return m_entityDense[deadIdx];
    }
    else
    {
        return newEntity();
    }

}

EntityRange EntityWorld::bulkCreate(index_type newcount)
{
    if(newcount == 0u) return {m_entityDense.begin(), 0u};
    auto entityCount = m_entityDense.size();
    auto currAlive   = m_aliveCount;
    auto toCreate    = newcount - (entityCount - currAlive);

    m_aliveCount += newcount;
	for (size_type i = 0u; i < toCreate; ++i)
    {
        newEntity();
    } 

    return {m_entityDense.begin() + currAlive, newcount}; 
}

EntityId EntityWorld::newEntity()
{
    EntityId newId{m_entityDense.size(), 0u};
    
    m_entitySparse.emplace_back(newId.index);
    m_entityDense.emplace_back(newId);

    return newId;
}

void EntityWorld::reserve(size_type value)
{
    m_entitySparse.reserve(value);
    m_entityDense.reserve(value);
}


void EntityWorld::release(const EntityId& enttId)
{
    assert(isValid(enttId) && "Invalid entity");

    auto denseIdx  = m_entitySparse[enttId.index];
    auto currAlive = m_aliveCount;
    m_entityDense[denseIdx].version++;

    if (denseIdx == (currAlive - 1u))
    {
        m_aliveCount--;
    }
    else if (denseIdx < currAlive)
    {
        auto last = m_entityDense[currAlive - 1u];
        std::swap(m_entityDense[currAlive - 1u],   m_entityDense[denseIdx]);
        std::swap(m_entitySparse[last.index],      m_entitySparse[enttId.index]);
        m_aliveCount--;
    }
}

void EntityWorld::isValid(const EntityId& enttId)
{
    if ((enttId.index < 0u) || (enttId.index >= m_entitySparse.size()))
        return false;

    auto sparseIdx = m_entitySparse[enttId.index];
    auto checkId   = m_entityDense[sparseIdx];
 
    return (enttId == checkId); 

}

