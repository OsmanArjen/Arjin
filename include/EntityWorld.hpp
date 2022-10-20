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
	using size_type       = EntityId::index_t;
	using iterator        = std::vector<EntityId>::const_iterator;
	using const_iterator  = std::vector<EntityId>::const_iterator;
public:
	EntityRange() = default;
	EntityRange(iterator begin, size_type size);

	iterator begin() const;
	iterator end()   const;

	const_iterator cbegin() const;
	const_iterator cend()   const;

	const EntityId& operator[](size_type idx) const;

private:
	iterator  m_begin;
	size_type m_size;
};



class EntityWorld
{
public:
	using size_type   = std::size_t;
	using index_type  = EntityId::index_t;
public:
	//--Constructor/Destructor
	EntityWorld() = default;
	EntityWorld(size_type npool);

	//--Functions
	//-Entity functions
	EntityId     create();
	EntityRange  bulkCreate(EntityId::index_t newcount);

	void         reserve(size_type count);
	void         release(const EntityId& enttId);
	void         isValid(const EntityId& enttId); 

	//-Component functions
	template<typename comptype, typename... argtyps>
	void  assign(const EntityId& enttId, argtyps&& ...args);

	template<typename comptype>
	bool  has(const EntityId& enttId);

	template<typename... comptypes>
	View<Pool<comptypes>...>  view();


private:
	using vec      = std::vector;
	using pool_ptr = std::unique_ptr<PoolBase<EntityId::index_t>>;
	//-
	vec<EntityId::index_t> m_entitySparse;	 
	vec<EntityId>          m_entityDense;	 
	EntityId::index_t      m_aliveCount;

	Pool<pool_ptr>         m_compPools;

	//-
	EntityId newEntity();
	
	template<typename comptype>
	ComponentId compTypeEnsure();

}; 

//-Template functions
template<typename comptype>
ComponentId EntityWorld::compTypeEnsure()
{
	const auto id = getComponentId<comptype>();

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
	assert(isValid(enttId) && "Invalid entity");
	
	const auto compid = compTypeEnsure<comptype>(); 
	Pool<comptype>* pool{m_compPools.get(compid)};

	assert(pool->has(enttId) && "Component already exists");	

	pool->insert(enttId.index, std::forward<argtyps>(args)...);

}

template<typename comptype>
bool EntityWorld::has(const EntityId& enttId)
{
	const auto compid = compTypeEnsure<comptype>();
	Pool<comptype>* pool{m_compPools.get(compid)};
	return pool->has(enttId);
}

template<typename... comptypes>
View<Pool<comptypes>...>  EntityWorld::view()
{
	static_assert(sizeof...(comptypes) > 0, "Cannot view without component types");
	return {m_compPools.get(compTypeEnsure<comptypes>())...};
}

#endif // ENTITY_WORLD_HPP