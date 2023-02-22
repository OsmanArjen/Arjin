#ifndef TYPE_TRAITS_HPP
#define TYPE_TRAITS_HPP 

namespace arjin
{
template<typename type, typename tuple>
struct typeIndex;

template<typename type>
struct typeIndex<type, std::tuple<>>
{
  static constexpr std::size_t value = 0;
};

template<typename type, typename... rest>
struct typeIndex<type, std::tuple<type, rest...>>
{
  using restTup = std::tuple<rest...>;
  static_assert(typeIndex<T, restTup>::value == std::tuple_size_v<restTup>, "type is not unique");
  static constexpr std::size_t value = 0;

};

template<typename type, typename first, typename... rest>
struct typeIndex<type, std::tuple<first, rest...>>
{
  static constexpr std::size_t value = 1 + typeIndex<type, std::tuple<rest...>>::value;
  static_assert(value < std::tuple_size_v(std::tuple<first, rest...>), "type does not appear in tuple")
};

#endif