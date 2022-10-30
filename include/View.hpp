#ifndef VIEW_HPP
#define VIEW_HPP
#include <tuple>
#include <utility>
#include <algorithm>
#include <type_traits>

template<class pool_t, std::size_t pool_n>
class ViewEntityIterator
{
public:
	using pool_iterator     = typename pool_base_t::const_iterator;
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
		return std::apply([](const auto*... pool){return (pool->has(*m_it) && ...);}, m_pools)
	}

private:
	std::array<pool_t*, pool_n> m_pools;
	pool_iterator m_it;
	pool_iterator m_end;

};



// TODO FIX THIS
template<class it_t, class... pool_t>
class ViewPoolIterator
{
public:
	using iterator_type   = it_t;
	using difference_type = std::ptrdiff_t;
	using value_type = decltype(std::tuple_cat(std::make_tuple(*std::declval<it_t>()),std::forward_as_tuple(std::declval<pool_t>().get({}))...));
	using pointer    = value_type*;
	using reference  = value_type;
	using iterator_category = std::input_iterator_tag;
public:
	constexpr ViewPoolIterator() = default;
	ViewPoolIterator(iterator_type begin, std::tuple<pool_t*...> pools)
	: m_it{begin}
	, m_pools{pools} {}

	ViewPoolIterator &operator++() noexcept 
	{
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
    	return *m_it;
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
	iterator_type m_it;
};



template<class... pool_t>
class View
{
public:
	using pool_base_t = std::common_type<pool_t...>;
	using iterator    = ViewEntityIterator<pool_base_t, sizeof...(pool_t)>;
public:
	View() = default;

	View(pool_t&... pools)
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

	iterable get()
	{
		return {ViewPoolIterator{m_minPool.begin(), m_pools},
		        ViewPoolIterator{m_minPool.end(),   m_pools}};
	}

	void update()
	{
		//-Update the smallest pool
		m_minPool = findMinPool();
	}

	template<typename Func>
	void apply(Func fn)
	{
		apply(std::move(fn), std::index_sequence_for(pool_t...));
	}

private:
	pool_base_t* findMinPool()
	{
		return  std::apply([](const auto* curr, const auto*... other){ 
							((curr = curr->size() < other->size() ? curr : other), ...);
							return curr;}, m_pools);
	}
	

	template<typename Func, std::size_t... pidx>
	void apply(Func fn, std::index_sequence<pidx...>)
	{
		for(auto entt : (*m_minPool))
		{
			if(((std::get<pidx>(m_pools)->has(entt)) && ...))
			{
				std::apply(fn, std::tuple_cat(std::make_tuple(entt), tupleGet<pidx>(entt)...));
			}
		}
	}

	template<std::size_t pool>
	auto tupleGet(std::size_t entt)
	{
		return std::forward_as_tuple(std::get<pool>(m_pools)->get(entt));
	}


	template<typename tuple_t>
	constexpr auto tupleAsArray(tuple_t&& tuple)
	{
    	constexpr auto getArr = [](auto&&... x){return std::array{std::forward<decltype(x)>(x)...};};
    	return std::apply(getArr, std::forward<tuple_t>(tuple));
	}

private:
	std::tuple<pool_t*...> m_pools;
	pool_base_t*           m_minPool;
};

#endif // VIEW_HPP