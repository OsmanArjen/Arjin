#ifndef TYPE_INFO_HPP
#define TYPE_INFO_HPP
#include <cstdint>
#include <limits>
//-Entity
struct EntityId  
{
  using index_t   = std::uint32_t;
  using version_t = std::uint16_t;
  
  index_t  index;
  version_t version;

  inline static const index_t nullidx{std::numeric_limits<index_t>::max()};
};

//-Component
using ComponentId = std::size_t;
ComponentId newComponentId()
{
	static ComponentId idCount{0u};
	return idCount++;
}

template<class CompType>
ComponentId getComponentId()
{
	// Static & const: to return the same id whenever same the type given
	static const ComponentId uniqueId{newCompId()}; 
	return uniqueId;
}

#endif // TYPE_INFO_HPP