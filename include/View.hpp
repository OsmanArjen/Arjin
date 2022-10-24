#ifndef VIEW_HPP
#define VIEW_HPP
#include <tuple>
#include <utility>
#include <algorithm>

template<>
class ViewIterator
{
public:
	ViewIterator();
};

template<class... pool_t>
class View
{
public:
	using iterator;
	using const_iterator;
	using pool_base_t = std::common_type<pool_t...>;
public:
	View() = default;

	View(pool_t&... pools)
	: m_viewPools{&pools...} 
	, m_minPool{findMinPool()} {}

	iterator begin()
	{
		return {};
	}

	iterator end()
	{
		return {};
	}

	void update()
	{
		//-Update the smallest pool
		m_minPool = findMinPool();
	}

	template<typename Func>
	void each(Func fn)
	{
		each(std::move(fn), std::index_sequence_for(pool_t...));
	}

private:
	std::tuple<pool_t*...> m_viewPools;
	pool_base_t*           m_minPool;

	//-
	pool_base_t* findMinPool()
	{
		return  std::min({&pools...},
		            [](const pool_base_t* fs, const pool_base_t* sc){
		            	return fs->size() < sc->size();
		            });
	}

	template<typename Func, std::size_t... pidx>
	void each(Func fn, std::index_sequence<pidx...>)
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
	auto tupleGet(pool_base_t::idx_t idx)
	{
		return std::forward_as_tuple(std::get<pidx>(m_viewPools)->get(idx));
	}

};

#endif // VIEW_HPP