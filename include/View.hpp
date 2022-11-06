#ifndef VIEW_HPP
#define VIEW_HPP
#include <tuple>
#include <utility>
#include <algorithm>
#include <type_traits>
#include "TypeTraits.hpp"

namespace arjin
{
template<class pool_t, std::size_t pool_n>
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
	ViewEntityIterator(pool_iterator begin, pool_iterator end, std::array<pool_t*, pool_n> pools)
	: m_pools{pools}
	, m_it{begin}
	, m_end{end} { findCurrentEntt(); }

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
		while(m_it != m_end && !hasAll()) 
		{
			m_it++;
		}
	}

	bool hasAll()
	{
		return std::apply([curr=*m_it](const auto*... pool){return (pool->has(curr) && ...);}, m_pools)
	}

private:
	std::array<pool_t*, pool_n> m_pools;
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
	ViewPoolIterator(entity_iterator_t begin, std::tuple<pool_ts*...> pools)
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


template<class... pool_ts, class... exclude_t>
class View
{
public:
	using pool_base_t  = std::common_type<pool_ts...>;
	using pool_index_t = typename pool_base_t::index_type;
	using iterator     = ViewEntityIterator<pool_base_t, sizeof...(pool_ts)>;

	template<class... poolts>
	using iterable = Iterable<ViewPoolIterator<iterator, poolts...>>;

	template<typename type>
	constexpr auto pool_index = typeIndex<type, std::tuple<pool_ts::value_type...>>::value;
public:
	View() = default;

	View(pool_ts&... pools)
	: m_pools{&pools...} 
	, m_minPool{findMinPool()} {}

	iterator begin()
	{
		return {m_minPool.begin(), m_minPool.end(), tupleAsArray(m_pools)};
	}

	iterator end()
	{
		return {m_minPool.end(), m_minPool.end(), tupleAsArray(m_pools)};
	}

	iterable<pool_ts...> get()
	{
		return {{m_minPool.begin(), m_pools},
		        {m_minPool.end(),   m_pools}};
	}

	template<typename... type>
	iterable<type...> get()
	{
		auto poolsToIter = std::make_tuple(pool<type>()...);
		return {{m_minPool.begin(), poolsToIter},
		        {m_minPool.end(),   poolsToIter}};
	}

	template<typename type>
	auto get(pool_base_t::index_type entt)
	{
		pool<type>()->get(entt);
	}

	bool has(pool_base_t::index_type entt)
	{
		return std::apply([](const auto*... pool){return (pool->has(entt)  && ...);}, m_pools) &&
		       std::apply([](const auto*... pool){return (!pool->has(entt) && ...);}, m_excluded);
	}

	void update()
	{
		//-Update the smallest pool
		m_minPool = findMinPool();
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

	pool_base_t* findMinPool()
	{
		return std::apply([](const auto* curr, const auto*... other){ 
					((curr = curr->size() < other->size() ? curr : other), ...);
					return curr;
				}, m_pools);
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