#ifndef ENTITY_WORLD_HPP
#define ENTITY_WORLD_HPP
#include <memory>
#include <cstdint>
#include "Pool.hpp"
#include "View.hpp"
#include "TypeInfo.hpp"

/*
TODO
: IMPLEMENT COMPONENT REMOVE AND COMPLETE THAT SYSTEM AND MAYBE MAKE SINGLE HEADER
*/
class EntityRange
{
public:
	using size_type = EntityType::index_t;
	using iterator  = std::vector<EntityType>::const_iterator;
public:
	EntityRange() = default;
	EntityRange(iterator begin, size_type size)
	: m_begin(begin), m_size(size) {}

	iterator begin() const
	{
	    return m_begin;
	}

	iterator end() const
	{
	    return (m_begin + m_size);
	}

	const EntityType& operator[](size_type idx) const
	{
		assert((idx < m_size));
		return (m_begin + idx)*; 
	}

private:
	iterator  m_begin;
	size_type m_size;
};

class EntityWorld
{
public:
	using size_type   = std::size_t;
	using index_type  = EntityType::index_t;
public:
	EntityWorld() = default;
	
	EntityWorld(size_type npool)
	: m_compPools{npool} {}

	EntityType create()
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

	EntityRange bulkCreate(EntityType::index_t newcount)
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

	void reserve(size_type value)
	{
	    m_entitySparse.reserve(value);
	    m_entityDense.reserve(value);
	}

	void release(const EntityType& enttId)
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
	        std::swap(m_entityDense[currAlive - 1u], m_entityDense[denseIdx]);
	        std::swap(m_entitySparse[last.index],    m_entitySparse[enttId.index]);
	        m_aliveCount--;
	    }
	}

	void isValid(const EntityType& enttId)
	{
	    if ((enttId.index < 0u) || (enttId.index >= m_entitySparse.size()))
	        return false;

	    auto sparseIdx = m_entitySparse[enttId.index];
	    auto checkId   = m_entityDense[sparseIdx];
	 
	    return (enttId == checkId); 

	}

	template<typename comptype, typename ...argtyps>
	void assign(const EntityType& enttId, argtyps&& ...args)
	{
		assert(isValid(enttId) && "Invalid entity");
		
		const auto compid = compTypeEnsure<comptype>(); 
		Pool<comptype>* pool{m_compPools[compid]};

		assert(pool->has(enttId) && "Component already exists");	

		pool->insert(enttId.index, std::forward<argtyps>(args)...);

	}

	template<typename comptype>
	bool has(const EntityType& enttId)
	{
		const auto compid = compTypeEnsure<comptype>();
		Pool<comptype>* pool{m_compPools[compid]};
		return pool->has(enttId);
	}

	template<typename... comptypes>
	View<Pool<comptypes>...> view()
	{
		static_assert(sizeof...(comptypes) > 0, "Cannot view without component types");
		return {m_compPools[compTypeEnsure<comptypes>()]...};
	}

private:
	using vec      = std::vector;
	using pool_ptr = std::unique_ptr<PoolBase>;
	using pool_map = std::unordered_map<CompTypeId, pool_ptr>;

	//-
	pool_map    m_compPools;
	index_type  m_aliveCount;
	vec<index_type> m_entitySparse;	 
	vec<EntityType> m_entityDense;	 

	//-
	template<typename comptype>
	CompTypeId compTypeEnsure()
	{
		const auto id = getComponentId<comptype>();

		if(!(m_compPools.contains(id)))
		{
			m_compPools.insert({id, {new Pool<comptype>()}});
		}

		return id;
	}
	
	EntityType newEntity()
	{
	    EntityType newId{m_entityDense.size(), 0u};
	    
	    m_entitySparse.emplace_back(newId.index);
	    m_entityDense.emplace_back(newId);

	    return newId;
	}
}; 

#endif // ENTITY_WORLD_HPP