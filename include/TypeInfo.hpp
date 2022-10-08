#ifndef TYPE_INFO_HPP
#define TYPE_INFO_HPP
#include <cstdint>
#include <limits>
//-Entity
struct EntityId  
{
  using idx_t  = std::uint32_t;
  using vers_t = std::uint16_t;
  
  idx_t  index;
  vers_t version;

  inline static const idx_t nullidx{std::numeric_limits<idx_t>::max()};
};

//-Component
using CompTypeId = std::size_t;
CompTypeId newCompId()
{
	static CompTypeId idCount{0u};
	return idCount++;
}

template<class CompType>
CompTypeId getCompId()
{
	// Static & const: to return the same id whenever same the type given
	static const CompTypeId uniqueId{newCompId()}; 
	return uniqueId;
}

#endif // TYPE_INFO_HPP