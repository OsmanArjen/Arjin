#ifndef ENTITY_HPP
#define ENTITY_HPP
#include <cstdint>
#include <exception>
#include <limits>

namespace arjin
{

// TODO: 21.02.2023: Implement it with bitwise operators and dont use look up to get index or version
struct EntityType  
{
  using index_t   = std::uint32_t;
  using version_t = std::uint16_t;
  
  index_t   index;
  version_t version;

  inline static const index_t nullidx{std::numeric_limits<index_t>::max()};
};

constexpr bool operator==(const EntityType &lhs, const EntityType &rhs) noexcept
{
  return (lhs.index == rhs.index) && (lhs.version == rhs.version);
}

}

#endif // ENTITY_HPP