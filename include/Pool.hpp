#ifndef POOL_HPP
#define POOL_HPP
#include <vector>
#include <cassert>
#include <utility>
#include <algorithm>
#include <type_traits>
#include "TypeInfo.hpp"


// TODO: FIX THE ENTITYWORLD CLASS THE RESERVE FUNCTION AND ENTITYPOOL FUNCTIONALITY
// TODO: FIX VIEW CLASS ACCORDING TO NEW POOLBASE STRUCTURE
// TODO: IMPLEMENT THE NEW REVERSE VERSION OF THE ITERATORS
// TODO: FIND OUT HOW ITERATORS CAN BE CONSTEXPR

struct PoolTraits
{
	using index_type = EntityId::index_t;
	static constexpr unsigned int sparseEnttShift{12};
	static constexpr unsigned int denseEnttShift{12};

	static constexpr unsigned int sparsePageSize{2 ** sparseEnttShift};
	static constexpr unsigned int densePageSize{2 ** denseEnttShift};

	index_type sparsePageIndex(const index_type enttIdx)
	{
		return (enttIdx >> PoolTraits::sparseEnttShift);
	}

	index_type sparseOffset(const index_type enttIdx)
	{
		return (enttIdx & (PoolTraits::sparsePageSize - 1));
	}

	index_type densePageIndex(const index_type enttIdx)
	{
		return (enttIdx >> PoolTraits::denseEnttShift);
	}

	index_type denseOffset(const index_type enttIdx)
	{
		return (enttIdx & (PoolTraits::densePageSize - 1));
	}

};

//-EnttIterator
template<typename pooldense>
class EnttIterator
{
public:
	using value_type        = typename pooldense::value_type;
	using difference_type   = typename pooldense::difference_type;
	using iterator_category = std::random_access_iterator_tag;
	using pointer           = value_type*;
	using reference         = value_type&;
public:
	constexpr EnttIterator() = default;
	constexpr EnttIterator(pooldense* ref, difference_type idx)
	: m_dense{ref}
	, m_currIdx{idx} {}

	constexpr EnttIterator(const EnttIterator<pooldense>& other)
	: EnttIterator{other.m_dense, other.m_currIdx} {}


	constexpr EnttIterator& operator++()
	{
		m_currIdx--;
		return *this;
	}

	constexpr EnttIterator  operator++(int)
	{
		auto cpy = *this;
		--m_currIdx;
		return cpy;
	}

	constexpr EnttIterator& operator--()
	{
		m_currIdx++;
		return *this;
	}

	constexpr EnttIterator  operator--(int)
	{
		auto cpy = *this;
		++m_currIdx;
		return cpy;
	}

    constexpr EnttIterator& operator+=(const difference_type value) noexcept
    {
        m_currIdx -= value;
        return *this;
    }

    constexpr EnttIterator& operator-=(const difference_type value) noexcept
    {
        return (*this += -value);
    }

    constexpr EnttIterator  operator+(const difference_type value) const noexcept
    {
        EnttIterator copy = *this;
        return (copy += value);
    }

    constexpr EnttIterator  operator-(const difference_type value) const noexcept
    {
        return (*this + -value);
    }

	constexpr reference operator[](const difference_type value)
	{
		return m_dense->data()[m_currIdx - value];
	}

	constexpr reference operator*()
	{
		return m_dense->data()[m_currIdx];
	}

	constexpr pointer   operator->()
	{
		return m_dense->data() + m_currIdx;
	}

	constexpr bool operator==(const EnttIterator& other) noexcept
	{
	    return m_currIdx == other.m_currIdx;
	}
	

	constexpr bool operator!=(const EnttIterator& other) noexcept
	{
	    return !(m_currIdx == other.m_currIdx);
	}
	

	constexpr bool operator<(const EnttIterator& other) noexcept
	{
	    return m_currIdx > other.m_currIdx;
	}
	

	constexpr bool operator>(const EnttIterator& other) noexcept
	{
	    return m_currIdx < other.m_currIdx;
	}
	

	constexpr bool operator<=(const EnttIterator& other) noexcept
	{
	    return !(other.m_currIdx > m_currIdx);
	}
	

	constexpr bool operator>=(const EnttIterator& other) noexcept
	{
	    return !(other.m_currIdx < m_currIdx);
	}
private:
	const pooldense* m_dense;
	difference_type  m_currIdx;
};

//-CompIterator
template<typename pooldense>
class CompIterator
{
public:
	using value_type        = typename pooldense::value_type;
	using difference_type   = typename pooldense::difference_type;
	using iterator_category = std::random_access_iterator_tag;
	using pointer           = value_type*;
	using reference         = value_type&;
public:
	constexpr CompIterator() = default;
	constexpr CompIterator(pooldense* ref, difference_type idx)
	: m_dense{ref}
	, m_currIdx{idx} {}

	constexpr CompIterator(const CompIterator<pooldense>& other)
	: CompIterator{other.m_dense, other.m_currIdx} {}



	constexpr CompIterator& operator++()
	{
		m_currIdx--;
		return *this;
	}

	constexpr CompIterator  operator++(int)
	{
		auto cpy = *this;
		--m_currIdx;
		return cpy;
	}

	constexpr CompIterator& operator--()
	{
		m_currIdx++;
		return *this;
	}

	constexpr CompIterator  operator--(int)
	{
		auto cpy = *this;
		++m_currIdx;
		return cpy;
	}

    constexpr CompIterator& operator+=(const difference_type value) noexcept
    {
        m_currIdx -= value;
        return *this;
    }

    constexpr CompIterator& operator-=(const difference_type value) noexcept
    {
        return (*this += -value);
    }

    constexpr CompIterator  operator+(const difference_type value) const noexcept
    {
        CompIterator copy = *this;
        return (copy += value);
    }

    constexpr CompIterator  operator-(const difference_type value) const noexcept
    {
        return (*this + -value);
    }

	constexpr reference operator[](const difference_type value)
	{
		const auto pos = (m_currIdx - value);
		return m_dense->data()[PoolTraits::densePageIndex(pos)][PoolTraits::denseOffset(pos)];
	}

	constexpr reference operator*()
	{
		return m_dense->data()[PoolTraits::densePageIndex(m_currIdx)][PoolTraits::denseOffset(m_currIdx)];
	}

	constexpr pointer   operator->()
	{
		return &(operator*());

	}

	constexpr bool operator==(const CompIterator& other) noexcept
	{
	    return m_currIdx == other.m_currIdx;
	}
	

	constexpr bool operator!=(const CompIterator& other) noexcept
	{
	    return !(m_currIdx == other.m_currIdx);
	}
	

	constexpr bool operator<(const CompIterator& other) noexcept
	{
	    return m_currIdx > other.m_currIdx;
	}
	

	constexpr bool operator>(const CompIterator& other) noexcept
	{
	    return m_currIdx < other.m_currIdx;
	}
	

	constexpr bool operator<=(const CompIterator& other) noexcept
	{
	    return !(other.m_currIdx > m_currIdx);
	}
	

	constexpr bool operator>=(const CompIterator& other) noexcept
	{
	    return !(other.m_currIdx < m_currIdx);
	}
private:
	pooldense*       m_dense;
	difference_type  m_currIdx;
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
		using iterator_pair     = std::pair<itfirst_t, itsecond_t>;

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
		iterator_pair m_pair;

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
class PoolBase
{
public:
	using index_type     = EntityId::index_t;
	using container_t    = std::vector<index_t>;
	using iterator       = EnttIterator<container_t>;
	using const_iterator = EnttIterator<const container_t>;
	using traits         = PoolTraits<index_t>;
public:

	/*! @brief Default constructor. */
	PoolBase() = default;

	/**
     * @brief Move constructor.
     * @param the other instance to move from
     */
	PoolBase(PoolBase&& other)
	: m_sparse{std::move(other.m_sparse)}
	, m_enttDense{std::move(other.m_enttDense)} {}

	/*! @brief Default destructor. */
	virtual ~PoolBase()
	{
		releasePages();
	}

	/**
	 * @warning
	 * Attempting to get the entity that doesn't belong to the
	 * Pool results in undefined behavior.
	 */
	index_type entity(index_type idx)
	{
		assert(idx < m_enttDense.size() && "Index is out of bounds");
		return m_enttDense[idx];
	}

	/**
	 * @warning
	 * Attempting to get the entity that doesn't belong to the
	 * Pool results in undefined behavior.
	 */
	index_type indexOf(index_type entt)
	{
		return sparseRef(entt);
	}


	bool empty() const
	{
		return m_enttDense.empty();
	}

	bool has(index_type idx) const
	{ 
		return (idx < (m_sparse.size() * traits::sparsePageSize) && indexOf(idx) != EntityId::nullidx);
	}

	size_t size() const
	{
		return m_enttDense.size();
	}

	size_t capacity() const
	{
		return m_sparse.capacity();
	}


	iterator begin()
	{
		return {&m_enttDense, {m_enttDense.size() - 1u}};
	}

	iterator end()
	{
		return {&m_enttDense, -1};
	}

	const_iterator cbegin()
	{
		return begin();
	}

	const_iterator cend()
	{
		return end();
	}

protected:
	std::vector<container_t*> m_sparse;
	container_t               m_enttDense;

	index_type& ensureSparseRef(index_type enttIdx)
	{
		const auto enttpage{traits::sparsePageIndex(enttIdx)};

		if(!(enttpage < m_sparse.size()))
		{
			m_sparse.resize((enttpage + 1u), {new container_t{traits::sparsePageSize, EntityId::nullidx}});
		}

		return sparseRef(enttIdx);
	}

	index_type& sparseRef(index_type idx)
	{
		assert((idx < (m_sparse.size() * traits::sparsePageSize)) && "Invalid entity");
		return m_sparse[traits::sparsePageIndex(idx)][traits::sparseOffset(idx)];
	}

private:

	void releasePages()
	{
		for(auto* pageptr : m_sparse)
		{
			if(pageptr != nullptr)
			{
				delete pageptr;
			}
		}
	}
};


template<typename value_t>
class Pool : public PoolBase
{
public:
	using value_type     = value_t;
	using page_type      = std::vector<value_type>;
	using container_t    = std::vector<page_type*>;
	using iterator       = CompIterator<container_t>;
	using const_iterator = CompIterator<const container_t>;
	using iterable       = PoolIterable<PoolBase::iterator, iterator>;
public:
	/*! @brief Default constructor. */
	Pool() = default;

	/**
	 * @brief Constructs the container with a capacity.
	 * @param capacity of the container
	 */
	explicit Pool(index_type capacity)
	{
		reserve(capacity);
	}

    /**
     * @brief Move constructor.
     * @param the other instance to move from
     */
	Pool(Pool&& other)
	: PoolBase(std::move(other))
	, m_dataDense(std::move(other.m_dataDense)) {}


	/*! @brief Default destructor. */
	~Pool() override
	{
		releasePages();
	}

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
	value_t& get(index_type entt)
	{
		return dataDenseRef(PoolBase::indexOf(entt));
	}

	const value_t& get(index_type entt) const
	{
		return dataDenseRef(PoolBase::indexOf(entt));
	}

	iterator begin()
	{
		return {&m_dataDense, {m_dataDense.size() - 1u}};
	}

	iterator end()
	{
		return {&m_dataDense, -1};
	}

	const_iterator cbegin()
	{
		return begin();
	}

	const_iterator cend()
	{
		return end();
	}

	void reserve(index_type cap)
	{
		if(cap != 0u)
		{
			m_enttDense.reserve(cap);
			reservePages(traits::densePageIndex(cap - 1u) + 1u);
		}
	}

	//-
	template<typename ...argtyps>
	void insert(index_type entt, argtyps&& ...args)
	{
		assert(!has(entt) && "Id already exists");

		index_type& entityRef   = ensureSparseRef(entt);
		page_type&  dataPageRef = currentDensePage();

		const auto denseIdx = m_enttDense.size();
		m_enttDense.emplace_back(entt);
		dataPageRef.emplace_back(std::forward<argtyps>(args)...);
		entityRef = denseIdx;
	}

	void erase(index_type entt)
	{
		assert(has(entt) && "Id doesn't exist");

		auto& backRef = sparseRef(m_enttDense.back());
		auto& enttRef = sparseRef(entt);

		std::swap(m_enttDense.back(), m_enttDense[enttRef]);
		std::swap(dataDenseRef(m_enttDense.size() - 1), dataDenseRef(enttRef));

		backRef = enttRef;
		enttRef = EntityId::nullidx;

		m_enttDense.pop_back();
		m_dataDense.pop_back();
		
	}

	void clear()
	{
		m_enttDense.clear();
		m_dataDense.clear();
	}

private:
	container_t m_dataDense;

	value_type& dataDenseRef(index_type idx)
	{
		assert((idx < (m_dataDense.size() * traits::densePageSize)) && "Invalid entity")
		return m_dataDense[traits::densePageIndex(idx)][traits::denseOffset(idx)];
	}

	page_type& currentDensePage()
	{
		auto pageidx = traits::densePageIndex(m_enttDense.size());
		if(!(pageidx < m_dataDense.size()))
		{
			reservePages((pageidx + 1u));
		}

		return m_dataDense[pageidx];
	}

	void reservePages(index_type newcap)
	{
		auto currSize = m_dataDense.size();
		m_dataDense.resize(newcap, nullptr);
		for(const auto newSize = m_dataDense.size(); currSize < newSize; ++currSize)
		{
			page_type* newPage{new page_type{}};
			newPage->reserve(traits::densePageSize);

			m_dataDense[currSize] = newPage;
		}
	}

	void releasePages()
	{
		for(auto* pageptr : m_dataDense)
		{
			if(pageptr != nullptr)
			{
				delete pageptr;
			}
		}
	}

};


#endif // POOL_HPP