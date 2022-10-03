#include "EntityWorld.hpp"

//-EntityRange
EntityRange::EntityRange(EntityId* start, size_type size)
	: m_range(start), m_size(size) {}

const EntityId& EntityRange::at(size_type idx)
{
	assert(idx < m_size);
	return m_range[idx]; 
}


//-EntityWorld
EntityWorld::EntityWorld(const std::size_t npool)
	: m_compPools{npool}
{

}

EntityId EntityWorld::newId(const EntityId::idx_t denseidx)
{
    EntityId enttId{m_enttPool.nextIdx++, 0};
    m_enttPool.sparse[enttId.index] = denseIdx;
    m_enttPool.dense[denseIdx]      = enttId;

    return enttId;
}

EntityId EntityWorld::create()
{
    EntityId::idx_t deadLine = m_enttPool.aliveCount++;
    if(m_enttPool.aliveCount < m_enttPool.dense.size())
    {
        return m_enttPool.dense[deadLine];
    }
    else
    {
        return newId(deadLine);
    }

}

EntityRange EntityWorld::bulkCreate(const EntityId::idx_t newcount)
{
    int32_t entityCount = m_enttPool.dense.size();
    int32_t aliveCount  = m_enttPool.aliveCount;
    
    int32_t toCreate    = newcount - (entityCount - aliveCount);
    m_enttPool.aliveCount += newcount;

    if(toCreate > 0)
    {
    	m_enttPool.sparse.resize(entityCount + toCreate);
    	m_enttPool.dense.resize(entityCount  + toCreate);

	    for (int32_t i = 0; i < toCreate; ++i)
    	{
    		newId(aliveCount + i);
    	}
    } 

    return {&m_enttPool.dense.at(aliveCount), newcount}; // return pointer for the first created entity 
    
}

void EntityWorld::reserve(const std:uint32_t count)
{
	m_enttPool.sparse.reserve(count);
	m_enttPool.dense.reserve(count);
}

void EntityWorld::release(const EntityId& enttId)
{
    if(isAlive(enttId))
    {
        
        EntityId::idx_t denseIdx = m_enttPool.sparse[enttId.index];
        EntityId::idx_t count    = m_enttPool.aliveCount;
        
        m_enttPool.dense[denseIdx].version++;

        if (denseIdx == (count - 1))
        {
            m_enttPool.aliveCount--;
        }
        else if (denseIdx < count)
        {
            const auto last = m_dense[count - 1];
            std::swap(m_dense[count - 1],   m_dense[denseIdx]);
            std::swap(m_sparse[last.index], m_sparse[enttId.index]);
            m_enttPool.aliveCount--;
        }
    }
}

void EntityWorld::isAlive(const EntityId& enttId)
{
    if (enttId.index < 0 || enttId.index >= m_enttPool.sparse.size())
        return false;

    EntityId::idx_t denseIdx = m_enttPool.sparse[enttId.index];
    EntityId        poolId   = m_enttPool.dense[denseIdx];

    return (enttId == poolId); 

}

