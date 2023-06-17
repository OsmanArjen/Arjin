#ifndef VIEW_HPP
#define VIEW_HPP

#include <array>
#include <tuple>
#include <algorithm>
#include <utility>
#include <type_traits>
#include "iterator_traits.hpp"
#include "type_traits.hpp"

namespace arjin
{

template<class pool_t, std::size_t include_n, std::size_t exclude_n>
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
	ViewEntityIterator(std::pair<pool_iterator, pool_iterator> itpair, 
	                   std::array<pool_t*, include_n> included, 
	                   std::array<pool_t*, exclude_n> excluded)
	: m_included{included}
	, m_excluded{excluded}
	, m_it{itpair.first}
	, m_end{itpair.second} { findCurrentEntt(); }

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
		return std::apply([curr=*m_it](const auto*... pool){return (pool->has(curr) && ...);}, m_included) &&
		       std::apply([curr=*m_it](const auto*... pool){return (!pool->has(curr) && ...);}, m_excluded);
	}

private:
	std::array<const pool_t*, include_n> m_included;
	std::array<const pool_t*, exclude_n> m_excluded;
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
	using value_type = decltype(std::make_tuple(*std::declval<it_t>(), std::declval<pool_ts>().get({})...));
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


template<typename, typename>
class View;

template<class... include_ts, class... exclude_ts>
class View<typeList<include_ts...>, typeList<exclude_ts...>>
{
public:

	using pool_base_t  = std::common_type_t<include_ts..., exclude_ts...>;
	using pool_index_t = typename pool_base_t::index_type;

	using entity_iterator = ViewEntityIterator<pool_base_t, sizeof...(include_ts), sizeof...(exclude_ts)>;
	
	template<class... types>
	using pool_iterator = ViewPoolIterator<entity_iterator, types..., exclude_ts...>;

	template<typename type>
	static constexpr std::size_t pool_index = typeIndex<type, std::tuple<typename include_ts::value_type...>>::value;

public:
	View() = default;
	View(include_ts&... included, exclude_ts&... excluded)
	: m_included{&included...} 
	, m_excluded{&excluded...} {update();}

	// TODO: 22.02.2023: wtf bro fix excluding
	entity_iterator begin()
	{
		return {m_minPool->begin(), m_minPool->end(), poolTupleToArray(m_included), poolTupleToArray(m_excluded)};
	}

	entity_iterator end()
	{
		return {m_minPool->end(), m_minPool->end(), {}, {}};
	}

	Iterable<pool_iterator<include_ts...>> get()
	{
		return {{m_minPool->begin(), m_included},
		        {m_minPool->end(),   m_included}};
	}

	template<typename... types>
	Iterable<pool_iterator<types...>> get()
	{
		auto typePools = std::make_tuple(pool<types>()...);
		return {{m_minPool->begin(), typePools}, 
		        {m_minPool->end(),   typePools}};
	}

	template<typename... type>
	auto get(pool_index_t entt)
	{
		if constexpr(sizeof...(type) == 0)
		{
			return std::apply([entt](auto*... curr){return std::tuple_cat(curr->get(entt)...);}, m_included);
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

	bool has(pool_index_t entt)
	{
		return std::apply([entt](const auto*... pool){return (pool->has(entt)  && ...);}, m_included) &&
		       std::apply([entt](const auto*... pool){return (!pool->has(entt) && ...);}, m_excluded);
	}

	void update()
	{
		m_minPool = std::apply([](const pool_base_t* curr, const auto*... other) {
			((curr = curr->size() < other->size() ? curr : other), ...);
			return curr;
		}, m_included);
	}

	template<typename Func>
	void apply(Func fn)
	{
		m_minPool ? apply(std::move(fn), std::index_sequence_for<include_ts...>()): void();
	}

private:
	template<typename type>
	decltype(auto) pool()
	{
		return std::get<pool_index<type>>(m_included);
	}

	template<typename Func, std::size_t... pidx>
	void apply(Func fn, std::index_sequence<pidx...>)
	{
		for(auto entt : (*m_minPool))
		{
			if(((std::get<pidx>(m_included)->has(entt)) && ...) && (!m_excluded[pidx]->has(entt) && ...))
			{
				std::apply(fn, std::make_tuple(entt, std::get<pidx>(m_included)->get(entt)...));
			}
		}
	}

	template<typename tuple_t>
	constexpr auto poolTupleToArray(tuple_t&& tuple)
	{
    	constexpr auto asArray = [](const auto*... curr){return std::array<const pool_base_t, sizeof...(curr)>{curr...};};
    	return std::apply(asArray, tuple);
	}

private:
	std::tuple<include_ts*...> m_included;
	std::array<const pool_base_t*, sizeof...(exclude_ts)> m_excluded;
	pool_base_t* m_minPool;
};

template<typename... ts>
inline constexpr typeList<ts...> exclude{};
 
}
#endif // VIEW_HPP
