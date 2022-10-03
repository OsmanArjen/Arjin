#ifndef ENTITY_WORLD_HPP
#define ENTITY_WORLD_HPP
#include <memory>
#include <cstdint>
#include "Pool.hpp"
#include "View.hpp"
#include "TypeInfo.hpp"

struct EntityRange
{
public:
	using size_type = std::uint32_t;
	EntityRange() = default;
	EntityRange(EntityId* start, size_type size);
	const EntityId& at(size_type idx);
private:
	EntityId* m_range;
	size_type m_size;
};



class EntityWorld
{

public:
	//--Constructor/Destructor
	EntityWorld() = default;
	EntityWorld(const std::size_t npool);

	//--Functions
	//-Entity functions
	EntityId     create();
	EntityRange  bulkCreate(const EntityId::idx_t newcount);

	void         reserve(const EntityId::idx_t count);
	void         release(const EntityId& enttId);
	void         isAlive(const EntityId& enttId); 

	//-Component functions
	template<typename comptype, typename... argtyps>
	void  assign(const EntityId& enttId, argtyps&& ...args);

	template<typename comptype>
	bool  has(const EntityId& enttId);

	template<typename... comptypes>
	View<Pool<comptypes>...>  view();


private:
	using vec = std::vector;
	using pstorage_t = Pool<std::unique_ptr<PoolBase<EntityId::idx_t>>>;


	//-
	struct
	{
		EntityId::idx_t nextIdx;
		EntityId::idx_t aliveCount;

		vec<EntityId::idx_t>    sparse;
		vec<EntityId> dense; 

	}	m_enttPool;	 

	// -
	pstorage_t m_compPools;

	//-
	EntityId  newId(const EntityId::idx_t denseidx);
	
	template<typename comptype>
	void ensure();

}; 

//-Template functions
template<typename comptype>
CompTypeId EntityWorld::ensure()
{
	const auto id = getCompId<comptype>();

	if(id >= m_compPools.capacity())
	{
		m_compPools.reserve(id + 1);
	}

	if(!(m_compPools.has(id)))
	{
		m_compPools.insert(id, {new Pool<comptype>()});
	}

	return id;
}

template<typename comptype, typename ...argtyps>
void EntityWorld::assign(const EntityId& enttId, argtyps&& ...args)
{
	assert(isAlive(enttId) && "Invalid entity");
	
	const auto id = ensure<comptype>(); 
	Pool<comptype>* pool{m_compPools.get(id)};

	assert(pool->has(enttId) && "Component already exists");	

	pool->insert(enttId.index, std::forward<argtyps>(args)...);

}

template<typename comptype>
bool EntityWorld::has(const EntityId& enttId)
{
	const auto id = ensure<comptype>();
	Pool<comptype>* pool{m_compPools.get(id)};
	return pool->has(enttId);
}

template<typename... comptypes>
View<Pool<comptypes>...>  EntityWorld::view()
{
	static_assert(sizeof...(comptypes) > 0, "Cannot view without component types");
	return {m_compPools.get(ensure<comptypes>)...};
}

#endif // ENTITY_WORLD_HPP