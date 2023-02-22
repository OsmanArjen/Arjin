#ifndef ENTITY_WORLD_HPP
#define ENTITY_WORLD_HPP
#include <memory>
#include <cstdint>
#include <type_traits>
#include <span>
#include "Pool.hpp"
#include "View.hpp"
#include "TypeInfo.hpp"

namespace arjen
{
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
	    index_type deadIdx = m_aliveCount++;
	    if(deadIdx < m_entityDense.size())
	    {
	        return m_entityDense[deadIdx];
	    }
	    else
	    {
	        m_entityDense.emplace_back(m_entitySparse.size(), 0);
	        m_entitySparse.push_back(m_entityDense.size() - 1);
	        return m_entityDense.back();
	    }
	}

	const std::span<EntityType> create(EntityType::index_t newcount)
	{
	    if(newcount == 0u) return {m_entityDense.begin(), 0u};
	    index_type entityCount = m_entityDense.size();
	    index_type currAlive = m_aliveCount;
	    index_type toCreate = newcount - (entityCount - currAlive);

	    m_aliveCount += newcount;
		for (size_type i = 0u; i < toCreate; ++i)
	    {
	        m_entityDense.emplace_back(m_entitySparse.size(), 0);
        	m_entitySparse.push_back(m_entityDense.size() - 1);
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

	    index_type denseIdx  = m_entitySparse[enttId.index];
	    index_type currAlive = m_aliveCount;
	    m_entityDense[denseIdx].version++;

	    if (denseIdx == (currAlive - 1u))
	    {
	        m_aliveCount--;
	    }
	    else if (denseIdx < currAlive)
	    {
	        EntityType last = m_entityDense[currAlive - 1u];
	        std::swap(m_entityDense[currAlive - 1u], m_entityDense[denseIdx]);
	        std::swap(m_entitySparse[last.index],    m_entitySparse[enttId.index]);
	        m_aliveCount--;
	    }
	}

	void isValid(const EntityType& enttId)
	{
	    if ((enttId.index < 0u) || (enttId.index >= m_entitySparse.size()))
	        return false;

	    index_type sparseIdx = m_entitySparse[enttId.index];
	    EntityType checkId   = m_entityDense[sparseIdx];
	 
	    return (enttId == checkId); 
	}

	template<typename comptype, typename ...argtyps>
	void assign(const EntityType& enttId, argtyps&& ...args)
	{
		assert(isValid(enttId) && "Invalid entity");
		auto& pool = poolEnsure<comptype>();
		pool.insert(enttId.index, std::forward<argtyps>(args)...);
	}

	template<typename comptype>
	void erase(const EntityType& enttId)
	{
		assert(isValid(enttId) && "Invalid entity");
		auto& pool = poolEnsure<comptype>();
		pool.erase(enttId.index);
	}
	
	template<typename comptype>
	bool has(const EntityType& enttId)
	{
		return poolEnsure<comptype>().has(enttId.index);
	}

	template<typename... comptypes>
	View<Pool<comptypes>...> each()
	{
		static_assert(sizeof...(comptypes) > 0, "Cannot view without component types");
		return {poolEnsure<std::remove_const_t(comptypes)>()...};
	}

private:
	template<typename comptype>
	auto& poolEnsure()
	{
		const auto id = getComponentId<comptype>();

		if(!(m_compPools.contains(id)))
		{
			m_compPools.insert({id, {new Pool<comptype>()}});
		}

		return static_cast<Pool<comptype>>(*m_compPools[id]);
	}
	
	EntityType newEntity()
	{
	    EntityType newId{m_entityDense.size(), 0u};
	    
	    m_entitySparse.emplace_back(newId.index);
	    m_entityDense.emplace_back(newId);

	    return newId;
	}

private:
	index_type m_aliveCount;
	std::vector<index_type> m_entitySparse;	 
	std::vector<EntityType> m_entityDense;	 
	std::unordered_map<CompTypeId, std::unique_ptr<PoolBase>> m_compPools;
	
}; 
}
#endif // ENTITY_WORLD_HPP