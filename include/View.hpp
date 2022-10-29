#ifndef VIEW_HPP
#define VIEW_HPP
#include <tuple>
#include <utility>
#include <algorithm>
#include <type_traits>

template<typename pool_t, std::size_t typecount>
class ViewIterator
{
public:
	using type_iterator     = typename pool_t::const_iterator;
	using value_type        = typename type_iterator::value_type;
	using difference_type   = typename type_iterator::difference_type;
	using pointer           = typename type_iterator::value_type*;
	using reference         = typename type_iterator::value_type&;
	using iterator_category = std::forward_iterator_tag;
public:
	ViewIterator(type_iterator begin, type_iterator end, std::array<pool_t,typecount> pools)
	: m_pools{pools}
	, m_it{begin}
	, m_end{end} { findCurrentEntt(); }

    ViewIterator &operator++() noexcept 
    {
    	++m_it;
    	findCurrentEntt();
    	return *this;
    }

    ViewIterator operator++(int) noexcept
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

	bool operator==(const ViewIterator& other) noexcept
	{
	    return m_it == other.m_it;
	}
	

	bool operator!=(const ViewIterator& other) noexcept
	{
	    return !(m_it == other.m_it);
	}

private:
	std::array<pool_t,typecount> m_pools;
	type_iterator m_it;
	type_iterator m_end;
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

};

template<class... pool_t>
class View
{
public:
	using pool_base_t = std::common_type<pool_t...>;
	using iterator    = ViewIterator<pool_base_t, sizeof...(pool_t)>;
public:
	View() = default;

	View(pool_t&... pools)
	: m_pools{&pools...} 
	, m_minPool{findMinPool()} {}

	iterator begin()
	{
		return {};
	}

	iterator end()
	{
		return {};
	}


	iterable each()
	{
		// todo
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
	std::tuple<pool_t*...> m_pools;
	pool_base_t*           m_minPool;

	//-
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

	pool_base_t* findMinPool()
	{
		return  std::apply([](const auto* curr, const auto*... other){ 
							((curr = curr->size() < other->size() ? curr : other), ...);
							return curr;}, m_pools);
	}
};

#endif // VIEW_HPP