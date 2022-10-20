#ifndef VIEW_HPP
#define VIEW_HPP
#include <tuple>
#include <utility>
#include <algorithm>

template<class... pool_t>
class View
{
public:
	using pool_base_t = std::common_type<pool_t...>;
public:
	View() = default;
	View(pool_t&... pools)
	: m_viewPools{&pools...} 
	, m_minPool{std::min({&pools...},
	    [](const pool_base_t* fs, const pool_base_t* sc){
	    	return fs->size() < sc->size();
	    })} {}

	template<typename Func>
	void each(Func fn);
private:
	std::tuple<pool_t*...> m_viewPools;
	const pool_base_t*     m_minPool;

	template<typename Func, std::size_t... pidx>
	void eachImpl(Func fn, std::index_sequence<pidx...>);

	template<std::size_t pidx>
	auto tupleGet(pool_base_t::idx_t idx);
};

template<typename Func>
void View::each(Func fn)
{
	eachImpl(std::move(fn), std::index_sequence_for(pool_t...));
}

template<typename Func, std::size_t... pidx>
void View::eachImpl(Func fn, std::index_sequence<pidx...>)
{
	for(auto entt : (*m_minPool))
	{
		if((std::get<pidx>(m_viewPools)->has(entt)) && ...)
		{
			std::apply(fn, std::tuple_cat(std::make_tuple(entt), tupleGet<pidx>(entt)...));
		}
	}
}

template<std::size_t pidx>
auto View::tupleGet(pool_base_t::idx_t idx)
{
	return std::forward_as_tuple(std::get<pidx>(m_viewPools)->get(idx));
}


#endif // VIEW_HPP