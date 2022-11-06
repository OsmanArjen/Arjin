#ifndef TYPE_INFO_HPP
#define TYPE_INFO_HPP
#include <cstdint>
#include <limits>

namespace arjin
{
struct EntityType  
{
  using index_t   = std::uint32_t;
  using version_t = std::uint16_t;
  
  index_t   index;
  version_t version;

  inline static const index_t nullidx{std::numeric_limits<index_t>::max()};
};

using CompTypeId = std::size_t;
CompTypeId newComponentId()
{
	static CompTypeId idCount{0u};
	return idCount++;
}

template<class CompType>
CompTypeId getComponentId()
{
	// Static & const: to return the same id whenever same the type given
	static const CompTypeId uniqueId{newCompId()}; 
	return uniqueId;
}

}

#endif // TYPE_INFO_HPP