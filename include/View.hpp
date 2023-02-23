#ifndef VIEW_HPP
#define VIEW_HPP
#include <pair>
#include <tuple>
#include <utility>
#include <algorithm>
#include <type_traits>
#include "type_traits.hpp"

namespace arjin
{
template<class pool_t, std::size_t pool_n, std::size_t exclude_n, typename filter_t = void>
class ViewEntityIterator
{
public:
	using pool_iterator     = typename pool_t::const_iterator;
	using value_type        = typename pool_iterator::value_type;
	using difference_type   = typename pool_iterator::difference_type;
	using pointer           = typename pool_iterator::value_type*;
	using reference         = typename pool_iterator::value_type&;
	using iterator_category = std::forward_iterator_tag;
public:
	constexpr ViewEntityIterator() = default;
	ViewEntityIterator(std::pair<pool_iterator> itpair, 
	                   std::array<pool_t*, pool_n> value, 
	                   std::array<pool_t*, pool_n> excluded,
	                   filter_t fn = void)
	: m_pools{value}
	, m_excluded{excluded}
	, m_filter{fn}
	, m_it{begin}
	, m_end{end} { findCurrentEntt(); }

    pool_iterator getEnd() const
    {
        return m_end;
    }

    ViewEntityIterator &operator++() noexcept 
    {
    	++m_it;
    	findCurrentEntt();
    	return *this;
    }

    ViewEntityIterator operator++(int) noexcept
    {
        auto cpy = *this;
        ++(*this);
        return cpy;
    }

    pointer operator->() const noexcept {
        return &*m_it;
    }

    reference operator*() const noexcept {
        return *m_it;
    }

	bool operator==(const ViewEntityIterator& other) noexcept
	{
	    return m_it == other.m_it;
	}
	

	bool operator!=(const ViewEntityIterator& other) noexcept
	{
	    return !(m_it == other.m_it);
	}

private:
	void findCurrentEntt()
	{
		while(m_it != m_end && !checkEntity()) 
		{
			m_it++;
		}
	}

	bool checkEntity()
	{
		// TODO: 6.12.2022: implement filtering here if needed
		return std::apply([curr=*m_it](const auto*... pool){return (pool->has(curr) && ...);}, m_pools) &&
		       std::apply([curr=*m_it](const auto*... pool){return (!pool->has(curr) && ...);}, m_excluded) &&
		       m_filter(*m_it);
	}

private:
	std::array<pool_t*, pool_n> m_pools;
	std::array<pool_t*, exclude_n> m_excluded;
	filter_t m_filter;
	pool_iterator m_it;
	pool_iterator m_end;

};


template<class it_t, class... pool_ts>
class ViewPoolIterator
{
public:
	using entity_iterator_t = it_t;
	using difference_type   = std::ptrdiff_t;
	using iterator_category = std::input_iterator_tag;
	using value_type = decltype(std::make_tuple(*std::declval<it_t>(), declval<pool_ts>().get({})...));
	using pointer    = value_type*;
	using reference  = value_type;
public:
	constexpr ViewPoolIterator() = default;
	ViewPoolIterator(entity_iterator_t begin, const std::tuple<pool_ts*...>& pools)
	: m_it{begin}
	, m_pools{pools} {}

	ViewPoolIterator &operator++() noexcept 
	{
		++m_it;
		return *this;
	}

	ViewPoolIterator operator++(int) noexcept
	{
	    auto cpy = *this;
	    ++(*this);
	    return cpy;
	}

	pointer operator->() const noexcept
	{
	    return &*m_it;
	}

	reference operator*() const noexcept
	{
    	return std::apply([curr = *m_it](auto*... pool){
    	           return std::make_tuple(curr, pool->get(curr)...);
    	       }, m_pools);
    }

	bool operator==(const ViewPoolIterator& other) noexcept
	{
	    return m_it == other.m_it;
	}
		
	bool operator!=(const ViewPoolIterator& other) noexcept
	{
		return !(m_it == other.m_it);
	}
	
private:
	entity_iterator_t m_it;
	std::tuple<pool_ts*...> m_pools;
};


template<class... pool_ts, class... exclude_ts>
class View
{
public:
	using pool_base_t  = std::common_type_t<pool_ts..., exclude_ts...>;
	using pool_index_t = typename pool_base_t::index_type;

	using entity_iterator = ViewEntityIterator<pool_base_t, sizeof...(pool_ts), sizeof...(exclude_ts)>;
	
	template<class... pool_ts>
	using pool_iterator = ViewPoolIterator<entity_iterator, pool_ts..., exclude_ts...>;

	template<typename type>
	constexpr auto pool_index = typeIndex<type, std::tuple<pool_ts::value_type...>>::value;

public:
	View() = default;

	View(pool_ts&... pools, exclude_ts&... excluded)
	: m_pools{&pools...} 
	, m_excluded{&excluded...} {update();}

	// TODO: 22.02.2023: wtf bro fix excluding
	entity_iterator begin()
	{
		return {m_minPool.begin(), m_minPool.end(), tupleAsArray(m_pools), tupleAsArray(m_excluded)};
	}

	entity_iterator end()
	{
		return {m_minPool.end(), m_minPool.end(), {}, {}};
	}

	template<typename filter_fn>
	Iterable<entity_iterator> filter(filter_fn func)
	{
		// return a iterable of entity iterators with 
		return;
	}

	Iterable<pool_iterator<pool_ts...>> get()
	{
		return {{m_minPool.begin(), m_pools},
		        {m_minPool.end(),   m_pools}};
	}

	template<typename... types>
	Iterable<pool_iterator<types...>> get()
	{
		auto typePools = std::make_tuple(pool<types>()...);
		return {{m_minPool.begin(), typePools}, 
		        {m_minPool.end(),   typePools}};
	}

	template<typename... type>
	auto get(pool_base_t::index_type entt)
	{
		if constexpr(sizeof...(type) == 0)
		{
			return std::apply([entt](auto*... curr){return std::tuple_cat(curr->get(entt)...);}, m_pools);
		}
		else if constexpr(sizeof...(type) == 1)
		{
			return (pool<type>()->get(entt), ...);
		}
		else
		{
			return std::tuple_cat(pool<type>()->get(entt)...);
		}
	}

	bool has(pool_base_t::index_type entt)
	{
		return std::apply([](const auto*... pool){return (pool->has(entt)  && ...);}, m_pools) &&
		       std::apply([](const auto*... pool){return (!pool->has(entt) && ...);}, m_excluded);
	}

	void update()
	{
		m_minPool = std::apply([](const pool_base_t* curr, const auto*... other) {
			((curr = curr->size() < other->size() ? curr : other), ...);
			return curr;
		}, m_pools);
	}

	template<typename Func>
	void apply(Func fn)
	{
		apply(std::move(fn), std::index_sequence_for(pool_ts...));
	}

private:
	template<typename type>
	decltype(auto) pool()
	{
		return std::get<pool_index<type>>(m_pools);
	}

	template<typename Func, std::size_t... pidx>
	void apply(Func fn, std::index_sequence<pidx...>)
	{
		for(auto entt : (*m_minPool))
		{
			if(((std::get<pidx>(m_pools)->has(entt)) && ...))
			{
				std::apply(fn, std::make_tuple(entt, std::get<pidx>(m_pools)->get(entt)...));
			}
		}
	}

	template<typename tuple_t>
	constexpr auto tupleAsArray(tuple_t&& tuple)
	{
    	constexpr auto getArr = [](auto&&... x){return std::array{std::forward<decltype(x)>(x)...};};
    	return std::apply(getArr, std::forward<tuple_t>(tuple));
	}

private:
	std::tuple<pool_ts*...>    m_pools;
	std::tuple<exclude_ts*...> m_excluded;
	pool_base_t* m_minPool;
};
}
#endif // VIEW_HPP