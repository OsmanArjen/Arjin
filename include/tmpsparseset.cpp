#ifndef POOL_HPP
#define POOL_HPP
#include <vector>
#include <cassert>
#include <utility>
#include <type_traits>
#include "TypeInfo.hpp"


// TODO: FIX VIEW CLASS ACCORDING TO NEW POOLBASE STRUCTURE

//-PoolIterator
template<typename pooldense>
class PoolIterator
{
public:
	using value_type        = typename pooldense::value_type;
	using difference_type   = typename pooldense::difference_type;
	using iterator_category = std::random_access_iterator_tag;
	using pointer           = value_type*;
	using reference         = value_type&;
public:
	constexpr PoolIterator() = default;
	constexpr PoolIterator(pooldense* ref, difference_type idx)
	: m_dense{ref}
	, m_curr{idx} {}

	constexpr PoolIterator(const PoolIterator<pooldense>& other)
	: PoolIterator{other.m_dense, other.m_curr} {}



	constexpr PoolIterator& operator++()
	{
		m_curr++;
		return *this;
	}

	constexpr PoolIterator  operator++(int)
	{
		auto cpy = *this;
		++m_curr;
		return cpy;
	}

	constexpr PoolIterator& operator--()
	{
		m_curr--;
		return *this;
	}

	constexpr PoolIterator  operator--(int)
	{
		auto cpy = *this;
		--m_curr;
		return cpy;
	}

    constexpr PoolIterator& operator+=(const difference_type value) noexcept
    {
        m_curr += value;
        return *this;
    }

    constexpr PoolIterator& operator-=(const difference_type value) noexcept
    {
        return (*this += -value);
    }

    constexpr PoolIterator  operator+(const difference_type value) const noexcept
    {
        PoolIterator copy = *this;
        return (copy += value);
    }

    constexpr PoolIterator  operator-(const difference_type value) const noexcept
    {
        return (*this + -value);
    }

	constexpr reference operator[](const difference_type idx)
	{
		return (*m_dense)[m_curr + idx];
	}

	constexpr reference operator*()
	{
		return (*m_dense)[m_curr];
	}

	constexpr pointer   operator->()
	{
		return &(operator*());

	}

	constexpr bool operator==(const PoolIterator& other) noexcept
	{
	    return m_curr == other.m_curr;
	}
	

	constexpr bool operator!=(const PoolIterator& other) noexcept
	{
	    return !(m_curr == other.m_curr);
	}
	

	constexpr bool operator<(const PoolIterator& other) noexcept
	{
	    return m_curr < other.m_curr;
	}
	

	constexpr bool operator>(const PoolIterator& other) noexcept
	{
	    return m_curr > other.m_curr;
	}
	

	constexpr bool operator<=(const PoolIterator& other) noexcept
	{
	    return !(other.m_curr > m_curr);
	}
	

	constexpr bool operator>=(const PoolIterator& other) noexcept
	{
	    return !(other.m_curr < m_curr);
	}
	

private:
	pooldense*       m_dense;
	difference_type  m_curr;

};

//-PoolIterable
template<typename itfirst_t, typename itsecond_t>
class PoolIterable
{
public:
	struct PairIterator
	{
	public:
		using value_type        = std::pair<typename itfirst_t::value_type, typename itsecond_t::value_type>;
		using reference         = value_type;
		using difference_type   = std::ptrdiff_t;
		using iterator_category = std::input_iterator_tag;
		using it_pair           = std::pair<itfirst_t, itsecond_t>;

		constexpr PairIterator() = default;
		constexpr PairIterator(itfirst_t from, itsecond_t to)
			: m_pair{from, to} {}

		constexpr PairIterator& operator++()
		{
			++m_pair.first;
			++m_pair.second;
			return *this;
		}

		constexpr PairIterator operator++(int)
		{
			auto cpy = *this;
			++(*this);
	        return cpy;
	    }

	    constexpr value_type operator*() noexcept
	    {
	        return {*m_pair.first, *m_pair.second};
	    }

		constexpr bool operator==(const PairIterator& other) noexcept
		{
		    return m_pair.first == other.m_pair.first;
		}
		
		constexpr bool operator!=(const PairIterator& other) noexcept
		{
		    return !((*this) == other);
		}
		
	private:
		it_pair m_pair;

	};

public:
	constexpr PoolIterable() = default;
	constexpr PoolIterable(PairIterator first, PairIterator second)
	: m_first{std::move(first)}, m_second(std::move(second)) {}

	constexpr PairIterator begin()
	{
		return m_first;
	}

	constexpr PairIterator end()
	{
		return m_second;
	}

	constexpr PairIterator cbegin()
	{
		return m_first;
	}

	constexpr PairIterator cend()
	{
		return m_second;
	}

private:
	PairIterator  m_first;
	PairIterator  m_second;
};

//-Pool

template<typename idx_t>
class PoolBase
{
public:
	using idx_t            = idx_t;
	using container_t      = std::vector<idx_t>;
	using iterator         = PoolIterator<container_t>;
	using const_iterator   = PoolIterator<const container_t>;

	/**
	     * @warning
	     * Attempting to get the entity that doesn't belong to the
	     * Pool results in undefined behavior.
	*/
	idx_t entity(const idx_t& idx)
	{
		return m_dense[idx];
	}

	bool empty() const
	{
		return m_dense.empty();
	}

	bool has(const idx_t& idx) const
	{
		return (idx < m_sparse.size() && m_sparse[idx] != EntityId::null);
	}

	size_t size() const
	{
		return m_dense.size();
	}

	size_t capacity() const
	{
		return m_sparse.capacity();
	}

	iterator begin()
	{
		return {&m_dense, {}};
	}

	iterator end()
	{
		const auto endpos = static_cast<typename iterator::difference_type>(m_dense.size()); 
		return {&m_dense, endpos};
	}

	const_iterator cbegin()
	{
		return {&m_dense, {}};
	}

	const_iterator cend()
	{
		const auto endpos = static_cast<typename iterator::difference_type>(m_dense.size()); 
		return {&m_dense, endpos};
	}

protected:
	container_t m_sparse;
	container_t m_dense;

};

template<typename value_t, typename idx_t = EntityId::idx_t>
class Pool : public PoolBase<idx_t>
{
public:
	using container_t    = std::vector<value_t>;
	using iterator       = PoolIterator<container_t>;
	using const_iterator = PoolIterator<const container_t>;
	using iterable       = PoolIterable<PoolBase::iterator, iterator>;
public:
	//--Constructor
	Pool() = default;
	Pool(idx_t capacity)
	{
		reserve(capacity);
	}


	//--Functions
	//-
	iterable each()
	{
		return {{PoolBase::begin(), begin()}, 
	            {PoolBase::end(),   end()}};
	}

	/**
	     * @warning
	     * Attempting to get the index of an entity that doesn't belong to the
	     * Pool results in undefined behavior.
	*/
	value_t& get(const idx_t& idx)
	{
		return m_data[m_sparse[idx]];
	}

	const value_t& get(const idx_t& idx) const
	{
		return m_data[m_sparse[idx]];
	}

	void reserve(idx_t capacity)
	{
		m_data.reserve(capacity);
		m_dense.reserve(capacity); 
		m_sparse.resize(capacity, EntityId::null);
	}

	iterator begin()
	{
		return {&m_data, {}};
	}

	iterator end()
	{
		const auto endpos = static_cast<typename iterator::difference_type>(m_data.size()); 
		return {&m_data, endpos};
	}

	const_iterator cbegin()
	{
		return {&m_data, {}};
	}

	const_iterator cend()
	{
		const auto endpos = static_cast<typename iterator::difference_type>(m_data.size()); 
		return {&m_data, endpos};
	}

	//-
	template<typename ...argtyps>
	void insert(const idx_t& idx, argtyps&& ...args)
	{
		assert(!has(idx) && "Id already exists");

		const auto spidx{m_dense.size()};
		m_dense.emplace_back(idx);
		m_data.emplace_back(std::forward<argtyps>(args)...);

		m_sparse[idx] = spidx;
		
	}

	void erase(const idx_t& idx)
	{
		assert(has(idx) && "Id doesn't exist");

		const auto last = m_dense.back();
		std::swap(m_dense.back(), m_dense[m_sparse[idx]]);
		std::swap(m_data.back(),  m_data[m_sparse[idx]]);
		
		m_sparse[last] = m_sparse[idx];
		m_sparse[idx]  = EntityId::null;

		m_dense.pop_back();
		m_data.pop_back();
		
	}

	void clear()
	{
		m_dense.clear();
		m_data.clear();
	}

private:
	container_t   m_data;
};


#endif // POOL_HPP