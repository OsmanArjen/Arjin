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

class PoolTraits
{
	using index_type = EntityType::index_t;
	static constexpr unsigned int sparseEnttShift{12};
	static constexpr unsigned int denseEnttShift{12};

	static constexpr unsigned int sparsePageSize{2 ** sparseEnttShift};
	static constexpr unsigned int densePageSize{2 ** denseEnttShift};

	constexpr index_type sparsePageIndex(index_type enttIdx)
	{
		return (enttIdx >> PoolTraits::sparseEnttShift);
	}

	constexpr index_type sparseOffset(index_type enttIdx)
	{
		return (enttIdx & (PoolTraits::sparsePageSize - 1));
	}

	constexpr index_type densePageIndex(index_type enttIdx)
	{
		return (enttIdx >> PoolTraits::denseEnttShift);
	}

	constexpr index_type denseOffset(index_type enttIdx)
	{
		return (enttIdx & (PoolTraits::densePageSize - 1));
	}

};

//-EnttIterator
template<typename pooldense_t>
class EnttIterator
{
public:
	using value_type        = typename pooldense_t::value_type;
	using difference_type   = typename pooldense_t::difference_type;
	using iterator_category = std::random_access_iterator_tag;
	using pointer           = value_type*;
	using reference         = value_type&;
public:
	constexpr EnttIterator() = default;
	constexpr EnttIterator(pooldense_t* ref, difference_type idx)
	: m_dense{ref}
	, m_currIdx{idx} {}

	constexpr EnttIterator(const EnttIterator<pooldense_t>& other)
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
	const pooldense_t* m_dense;
	difference_type  m_currIdx;
};

//-CompIterator
template<typename pooldense_t>
class CompIterator
{
public:
	using value_type        = typename pooldense_t::value_type;
	using difference_type   = typename pooldense_t::difference_type;
	using iterator_category = std::random_access_iterator_tag;
	using pointer           = value_type*;
	using reference         = value_type&;
public:
	constexpr CompIterator() = default;
	constexpr CompIterator(pooldense_t* ref, difference_type idx)
	: m_dense{ref}
	, m_currIdx{idx} {}

	constexpr CompIterator(const CompIterator<pooldense_t>& other)
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
	pooldense_t*       m_dense;
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
	using index_type     = EntityType::index_t;
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
	, m_entityDense{std::move(other.m_entityDense)} {}

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
		assert(idx < m_entityDense.size() && "Index is out of bounds");
		return m_entityDense[idx];
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
		return m_entityDense.empty();
	}

	bool has(index_type idx) const
	{ 
		return (idx < extent() && indexOf(idx) != EntityType::nullidx);
	}

	size_t extent() const 
	{
		return (m_sparse.size() * traits::sparsePageSize);
	}

	size_t size() const
	{
		return m_entityDense.size();
	}

	size_t capacity() const
	{
		return m_sparse.capacity();
	}


	iterator begin()
	{
		return {&m_entityDense, {m_entityDense.size() - 1u}};
	}

	iterator end()
	{
		return {&m_entityDense, -1};
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
	container_t               m_entityDense;
private:
	index_type& ensureSparseRef(index_type enttIdx)
	{
		auto pageidx  = traits::sparsePageIndex(enttIdx);
		auto currSize = m_sparse.size();
		if(!(pageidx < currSize))
		{
			m_sparse.resize((pageidx + 1u), nullptr);
			for(const auto newSize = m_sparse.size(); currSize < newSize; ++currSize)
			{
				m_sparse[currSize] = {new container_t{traits::sparsePageSize, EntityType::nullidx}};
			}
		}

		return sparseRef(enttIdx);
	}

	index_type& sparseRef(index_type idx)
	{
		assert((idx < extent()) && "Invalid entity");
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
     * @brief Move constructor.
     * @param the other instance to move from
     */
	Pool(Pool&& other)
	: PoolBase(std::move(other))
	, m_dataDense(std::move(other.m_dataDense)) {}

	/**
	 * @brief Constructs the container with a capacity.
	 * @param capacity of the container
	 */
	explicit Pool(index_type capacity)
	{
		reserve(capacity);
	}

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
			m_entityDense.reserve(cap);
			reservePages(traits::densePageIndex(cap - 1u) + 1u);
		}
	}

	//-
	template<typename ...argtyps>
	void insert(index_type entt, argtyps&& ...args)
	{
		assert(!has(entt) && "Id already exists");

		index_type& entityRef    = ensureSparseRef(entt);
		page_type&  currDataPage = denseCurrentPage();

		const auto denseIdx = m_entityDense.size();
		m_entityDense.emplace_back(entt);
		currDataPage.emplace_back(std::forward<argtyps>(args)...);
		entityRef = denseIdx;
	}

	void erase(index_type entt)
	{
		assert(has(entt) && "Id doesn't exist");

		auto& backRef   = sparseRef(m_entityDense.back());
		auto& erasedRef = sparseRef(entt);
		auto  backIdx   = (m_entityDense.size() - 1);

		std::swap(m_entityDense.back(),    m_entityDense[erasedRef]);
		std::swap(dataDenseRef(backIdx), dataDenseRef(erasedRef));

		backRef   = erasedRef;
		erasedRef = EntityType::nullidx;

		m_entityDense.pop_back();
		densePageRef(backIdx).pop_back();
		
	}

	void clear()
	{
		m_entityDense.clear();
		releasePages();
	}

private:
	container_t m_dataDense;

	value_type& dataDenseRef(index_type idx)
	{
		assert((idx < (m_dataDense.size() * traits::densePageSize)) && "Invalid entity")
		return densePageRef(idx)[traits::denseOffset(idx)];
	}

	page_type& densePageRef(index_type idx)
	{
		return m_dataDense[traits::densePageIndex(idx)];
	}

	page_type& denseCurrentPage()
	{
		auto pageidx = traits::densePageIndex(m_entityDense.size());
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
		for(auto&& pageptr : m_dataDense)
		{
			if(pageptr != nullptr)
			{
				delete pageptr;
				pageptr = nullptr;
			}
		}
	}

};


#endif // POOL_HPP