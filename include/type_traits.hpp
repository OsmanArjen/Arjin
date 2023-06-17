#ifndef TYPE_TRAITS_HPP
#define TYPE_TRAITS_HPP 
#include <tuple>



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
  using tupleRest = std::tuple<rest...>;
  static_assert(typeIndex<type, tupleRest>::value == std::tuple_size_v<tupleRest>(), "type is not unique");
  static constexpr std::size_t value = 0;

};

template<typename type, typename first, typename... rest>
struct typeIndex<type, std::tuple<first, rest...>>
{
  static constexpr std::size_t value = 1 + typeIndex<type, std::tuple<rest...>>::value;
  static_assert(value < std::tuple_size_v<std::tuple<first, rest...>>(), "type does not appear in tuple");
};

template<typename... type>
struct typeList {
    static constexpr auto size = sizeof...(type);
};

}
#endif // TYPE_TRAITS_HPP