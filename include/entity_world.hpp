#ifndef ENTITY_WORLD_HPP
#define ENTITY_WORLD_HPP
#include <memory>
#include <cstdint>
#include <tuple>
#include <type_traits>
#include <span>
#include <unordered_map>

#include "pool.hpp"
#include "type_traits.hpp"
#include "view.hpp"
#include "entity.hpp"

// TODO: 6.07.2023: Find a way to simplfy the interface of allocators and also i guess we forgot that we need to initiate them with a type
// TODO: 6.07.2023: Make the entity type templated and use bitwise
// TODO: 6.07.2023: Create an entity namespace and define the bitwise operators for version and index

namespace arjin
{

template<typename allocator_t>
class EntityWorld
{
public:
	using alloc_traits = std::allocator_traits<allocator_t>;
	using size_type   = std::size_t;
	using index_type  = EntityType::index_t;
public:
	EntityWorld() = default;
	
	EntityWorld(size_type npool)
	: m_compPools{npool} {}

	EntityType create()
	{
	    index_type deadIdx = m_aliveCount++;
	    if(deadIdx < m_tableDense.size())
	    {
	        return m_tableDense[deadIdx];
	    }
	    else
	    {
	        m_tableDense.emplace_back(m_tableSparse.size(), 0);
	        m_tableSparse.push_back(m_tableDense.size() - 1);
	        return m_tableDense.back();
	    }
	}

	const std::span<EntityType> create(EntityType::index_t newcount)
	{
	    if(newcount == 0u) return {m_tableDense.begin(), 0u};
	    index_type entityCount = m_tableDense.size();
	    index_type currAlive = m_aliveCount;
	    index_type toCreate = newcount - (entityCount - currAlive);

	    m_aliveCount += newcount;
		for (size_type i = 0u; i < toCreate; ++i)
	    {
	        m_tableDense.emplace_back(m_tableSparse.size(), 0);
        	m_tableSparse.push_back(m_tableDense.size() - 1);
	    } 

	    return {m_tableDense.begin() + currAlive, newcount}; 
	}

	void reserve(size_type value)
	{
	    m_tableSparse.reserve(value);
	    m_tableDense.reserve(value);
	}

	void release(const EntityType& enttId)
	{
	    assert(valid(enttId) && "Invalid entity");

	    index_type denseIdx  = m_tableSparse[enttId.index];
	    index_type currAlive = m_aliveCount;
	    m_tableDense[denseIdx].version++;

	    if (denseIdx == (currAlive - 1u))
	    {
	        m_aliveCount--;
	    }
	    else if (denseIdx < currAlive)
	    {
	        EntityType last = m_tableDense[currAlive - 1u];
	        std::swap(m_tableDense[currAlive - 1u], m_tableDense[denseIdx]);
	        std::swap(m_tableSparse[last.index],    m_tableSparse[enttId.index]);
	        m_aliveCount--;
	    }
	}

	bool valid(const EntityType& enttId)
	{
	    if ((enttId.index < 0u) || (enttId.index >= m_tableSparse.size()))
	        return false;

	    EntityType checkId = m_tableDense[m_tableSparse[enttId.index]];
	    return (enttId.version == checkId.version); 
	}

	template<typename comptype, typename ...argtyps>
	void assign(const EntityType& enttId, argtyps&& ...args)
	{
		assert(valid(enttId) && "Invalid entity");
		auto& pool = poolEnsure<comptype>();
		pool.insert(enttId.index, std::forward<argtyps>(args)...);
	}

	template<typename comptype>
	void erase(const EntityType& enttId)
	{
		assert(valid(enttId) && "Invalid entity");
		auto& pool = poolEnsure<comptype>();
		pool.erase(enttId.index);
	}
	
	template<typename comptype>
	bool has(const EntityType& enttId)
	{
		return poolEnsure<comptype>().has(enttId.index);
	}

	template<typename... get_ts, typename... excluded_ts>
	View<typeList<Pool<get_ts>>..., typeList<Pool<excluded_ts>>...> 
	each(typeList<excluded_ts...> = exclude<>)
	{
		static_assert(sizeof...(get_ts) > 0, "Cannot view without component types");
		return {poolEnsure<std::remove_const_t<get_ts>()>()..., 
		        poolEnsure<std::remove_const_t<excluded_ts>()>()...};
	}

private:
	using CompTypeId = std::size_t;

	template<typename comptype>
	auto& poolEnsure()
	{
		const auto id = getComponentId<comptype>();

		if(!(m_compPools.count(id)))
		{
			m_compPools.insert({id, {new Pool<comptype>()}});
		}

		return static_cast<Pool<comptype>&>(*m_compPools[id]);
	}

	template<class comptype>
	CompTypeId getComponentId()
	{
		static const CompTypeId uniqueId{newComponentId()}; 
		return uniqueId;
	}

	CompTypeId newComponentId()
	{
		static CompTypeId idCount{0u};
		return idCount++;
	}

private:
	index_type m_aliveCount;
	std::vector<index_type> m_tableSparse;	 
	std::vector<EntityType> m_tableDense;	 
	std::unordered_map<CompTypeId, std::unique_ptr<PoolBase<>>> m_compPools;
	
}; 
}
#endif // ENTITY_WORLD_HPP