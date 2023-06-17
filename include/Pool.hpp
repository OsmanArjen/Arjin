#ifndef POOL_HPP
#define POOL_HPP
#include <utility>
#include <algorithm>
#include <memory>
#include <cmath>
#include <type_traits>
#include <vector>
#include <cassert>
#include "iterator_traits.hpp"
#include "entity.hpp"

// they've been left as a syntax error for you to take care of them
//TODO: 19.02.2023: Change insert functionality and add it to the poolbase some part of it so that people can use PoolBase when they only wanna access the sparse and enttdense
//TODO: 18.02.2023: Fully add the allocator thing and also you figured it out it was to allocate new pages whenever we need and seperate them

// TODO: FIX THE ENTITYWORLD CLASS THE RESERVE FUNCTION AND ENTITYPOOL FUNCTIONALITY// check if this is still relevant
// TODO: FIX VIEW CLASS ACCORDING TO NEW POOLBASE STRUCTURE // check if this is still relevant
namespace arjin
{

constexpr unsigned int sparseEnttShift{12};
constexpr unsigned int denseEnttShift{12};

constexpr unsigned int sparsePageSize{2 ** sparseEnttShift};
constexpr unsigned int densePageSize{2 ** denseEnttShift};

constexpr EntityType::index_t sparsePageIndex(EntityType::index_t enttIdx)
{
	return (enttIdx >> sparseEnttShift);
}

constexpr EntityType::index_t sparseOffset(EntityType::index_t enttIdx)
{
	return (enttIdx & (sparsePageSize - 1));
}

constexpr EntityType::index_t densePageIndex(EntityType::index_t enttIdx)
{
	return (enttIdx >> denseEnttShift);
}

constexpr EntityType::index_t denseOffset(EntityType::index_t enttIdx)
{
	return (enttIdx & (densePageSize - 1));
}


template<typename pooldense_t>
class PoolEntityIterator
{
public:
	using difference_type   = typename pooldense_t::difference_type;
	using value_type        = typename pooldense_t::value_type;
	using pointer           = typename pooldense_t::const_pointer;
	using reference         = typename pooldense_t::const_reference;
	using iterator_category = std::random_access_iterator_tag;
public:
	constexpr PoolEntityIterator() = default;
	constexpr PoolEntityIterator(pooldense_t* ref, difference_type idx)
	: m_dense{ref}
	, m_currIdx{idx} {}

	constexpr PoolEntityIterator(const PoolEntityIterator<pooldense_t>& other)
	: PoolEntityIterator{other.m_dense, other.m_currIdx} {}


	constexpr PoolEntityIterator& operator++()
	{
		m_currIdx--;
		return *this;
	}

	constexpr PoolEntityIterator  operator++(int)
	{
		auto cpy = *this;
		--m_currIdx;
		return cpy;
	}

	constexpr PoolEntityIterator& operator--()
	{
		m_currIdx++;
		return *this;
	}

	constexpr PoolEntityIterator  operator--(int)
	{
		auto cpy = *this;
		++m_currIdx;
		return cpy;
	}

    constexpr PoolEntityIterator& operator+=(const difference_type value) noexcept
    {
        m_currIdx -= value;
        return *this;
    }

    constexpr PoolEntityIterator& operator-=(const difference_type value) noexcept
    {
        return (*this += -value);
    }

    constexpr PoolEntityIterator  operator+(const difference_type value) const noexcept
    {
        PoolEntityIterator copy = *this;
        return (copy += value);
    }

    constexpr PoolEntityIterator  operator-(const difference_type value) const noexcept
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

	constexpr bool operator==(const PoolEntityIterator& other) noexcept
	{
	    return m_currIdx == other.m_currIdx;
	}

	constexpr bool operator!=(const PoolEntityIterator& other) noexcept
	{
	    return !(m_currIdx == other.m_currIdx);
	}

	constexpr bool operator<(const PoolEntityIterator& other) noexcept
	{
	    return m_currIdx > other.m_currIdx;
	}

	constexpr bool operator>(const PoolEntityIterator& other) noexcept
	{
	    return m_currIdx < other.m_currIdx;
	}
	
	constexpr bool operator<=(const PoolEntityIterator& other) noexcept
	{
	    return !(other.m_currIdx > m_currIdx);
	}

	constexpr bool operator>=(const PoolEntityIterator& other) noexcept
	{
	    return !(other.m_currIdx < m_currIdx);
	}
private:
	const pooldense_t* m_dense;
	difference_type    m_currIdx;
};

template<typename pooldense_t>
class PoolDataIterator
{
public:
	using value_type        = typename pooldense_t::value_type;
	using difference_type   = typename pooldense_t::difference_type;
	using iterator_category = std::random_access_iterator_tag;
	using pointer           = value_type*;
	using reference         = value_type&;
public:
	constexpr PoolDataIterator() = default;
	constexpr PoolDataIterator(pooldense_t* ref, difference_type idx)
	: m_dense{ref}
	, m_currIdx{idx} {}

	constexpr PoolDataIterator(const PoolDataIterator<pooldense_t>& other)
	: PoolDataIterator{other.m_dense, other.m_currIdx} {}

	constexpr PoolDataIterator& operator++()
	{
		m_currIdx--;
		return *this;
	}

	constexpr PoolDataIterator  operator++(int)
	{
		auto cpy = *this;
		--m_currIdx;
		return cpy;
	}

	constexpr PoolDataIterator& operator--()
	{
		m_currIdx++;
		return *this;
	}

	constexpr PoolDataIterator  operator--(int)
	{
		auto cpy = *this;
		++m_currIdx;
		return cpy;
	}

    constexpr PoolDataIterator& operator+=(const difference_type value) noexcept
    {
        m_currIdx -= value;
        return *this;
    }

    constexpr PoolDataIterator& operator-=(const difference_type value) noexcept
    {
        return (*this += -value);
    }

    constexpr PoolDataIterator  operator+(const difference_type value) const noexcept
    {
        PoolDataIterator copy = *this;
        return (copy += value);
    }

    constexpr PoolDataIterator  operator-(const difference_type value) const noexcept
    {
        return (*this + -value);
    }

	constexpr reference operator[](const difference_type value)
	{
		const auto pos = (m_currIdx - value);
		return m_dense->data()[densePageIndex(pos)][denseOffset(pos)];
	}

	constexpr reference operator*()
	{
		return m_dense->data()[densePageIndex(m_currIdx)][denseOffset(m_currIdx)];
	}

	constexpr pointer   operator->()
	{
		return &(operator*());
	}

	constexpr bool operator==(const PoolDataIterator& other) noexcept
	{
	    return m_currIdx == other.m_currIdx;
	}
	
	constexpr bool operator!=(const PoolDataIterator& other) noexcept
	{
	    return !(m_currIdx == other.m_currIdx);
	}
	
	constexpr bool operator<(const PoolDataIterator& other) noexcept
	{
	    return m_currIdx > other.m_currIdx;
	}
	
	constexpr bool operator>(const PoolDataIterator& other) noexcept
	{
	    return m_currIdx < other.m_currIdx;
	}
	
	constexpr bool operator<=(const PoolDataIterator& other) noexcept
	{
	    return !(other.m_currIdx > m_currIdx);
	}

	constexpr bool operator>=(const PoolDataIterator& other) noexcept
	{
	    return !(other.m_currIdx < m_currIdx);
	}

private:
	pooldense_t*     m_dense;
	difference_type  m_currIdx;
};

//-Pool
template<typename allocator_t = std::allocator<EntityType::index_t>>
class PoolBase
{
public:
	using index_type  = EntityType::index_t;
	using alloc_traits = std::allocator_traits<allocator_t>;
	using sparse_t = std::vector<typename alloc_traits::pointer, typename alloc_traits::template rebind_alloc<typename alloc_traits::pointer>>;
	using dense_t  = std::vector<index_type, allocator_t>;
	using iterator = PoolEntityIterator<dense_t>;
	using const_iterator = iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
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

	/**
     * @brief Move assignment operator.
     * @param the other instance to move from
     */
	PoolBase& operator=(PoolBase&& other)
	{
		releaseSparse();
		m_sparse = std::move(other.m_sparse);
		m_entityDense = std::move(other.m_entityDense);
		return *this;
	}

	/*! @brief Default destructor. */
	virtual ~PoolBase()
	{
		releaseSparse();
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
		return (m_sparse.size() * sparsePageSize);
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
	index_type& ensureSparseRef(index_type enttIdx)
	{
		auto pageSize = sparsePageSize;
		auto pageidx = sparsePageIndex(enttIdx);
		auto currSize = m_sparse.size();
		if(!(pageidx < currSize))
		{
			m_sparse.resize((pageidx + 1u), nullptr);
			for(const auto newSize = m_sparse.size(); currSize < newSize; ++currSize)
			{
				m_sparse[currSize] = alloc_traits::allocate(m_entityDense.get_allocator(), pageSize);
            	std::uninitialized_fill(m_sparse[currSize], m_sparse[currSize] + pageSize, EntityType::nullidx);
			}
		}

		return sparseRef(enttIdx);
	}

	index_type& sparseRef(index_type idx)
	{
		assert((idx < extent()) && "Invalid entity");
		auto& item = m_sparse[sparsePageIndex(idx)][sparseOffset(idx)];
		assert((item == EntityType::nullidx) && "Invalid entity");
		return item;
	}

protected:
	sparse_t m_sparse;
	dense_t  m_entityDense;

private:
	void releaseSparse()
	{
		for(auto&& pageptr : m_sparse)
		{
			if(pageptr != nullptr)
			{
				std::destroy(pageptr, pageptr + sparsePageSize);
                alloc_traits::deallocate(m_entityDense.get_allocator(), pageptr, sparsePageSize);
                pageptr = nullptr;
			}
		}
	}
};

template<typename value_t, typename allocator_t = std::allocator<value_t>>
class Pool : public PoolBase<typename std::allocator_traits<allocator_t>::template rebind_alloc<EntityType::index_t>>
{
	//TODO: 19.02.2023: EDIT THE CODE WITH NEW ITERATOR AND PAGE SYSTEM; SINCE ITS NOW PTR NOT VECTOR

public:
	using index_type  = EntityType::index_t;
	using value_type  = value_t;
	using alloc_traits = std::allocator_traits<allocator_t>;
	static_assert(std::is_same_v<typename alloc_traits::value_type, value_t>, "Invalid value type");

	using page_ptr_t  = typename alloc_traits::pointer;
	using container_t = std::vector<typename alloc_traits::pointer, typename alloc_traits::template rebind_alloc<typename alloc_traits::pointer>>;

	using iterator = PoolDataIterator<container_t>;
	using const_iterator = PoolDataIterator<const container_t>;

	using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    
    using pool_base = PoolBase<typename std::allocator_traits<allocator_t>::template rebind_alloc<EntityType::index_t>>;
	using iterable       = Iterable<PairIterator<typename pool_base::iterator, iterator>>;
	using const_iterable = Iterable<PairIterator<typename pool_base::const_iterator, const_iterator>>;

public:
	/*! @brief Default constructor. */
	Pool() = default;

    /**
     * @brief Move constructor.
     * @param other to move from
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
		return {pair_iterator{typename pool_base::begin(), begin()}, 
	            pair_iterator{typename pool_base::end(),   end()}};
	}

	const_iterable ceach()
	{
		return {pair_iterator{typename pool_base::cbegin(), cbegin()}, 
	            pair_iterator{typename pool_base::cend(),   cend()}};
	}

	/**
	 * @warning
	 * Attempting to get the index of an entity that doesn't belong to the
	 * Pool results in undefined behavior.
	 */
	value_t& get(index_type entt)
	{
		return dataItemRef(typename pool_base::indexOf(entt));
	}

	const value_t& get(index_type entt) const
	{
		return dataItemRef(typename pool_base::indexOf(entt));
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

	constexpr allocator_t allocator()
	{
		return m_dataDense.get_allocator();
	}
	
	void reserve(index_type cap)
	{
		if(cap != 0u)
		{
			m_entityDense.reserve(cap);
			reservePages(densePageIndex(cap - 1u) + 1u);
		}
	}

	//-
	template<typename ...argtyps>
	void insert(index_type entt, argtyps&& ...args)
	{
		assert(!has(entt) && "Id already exists");

		index_type& entityRef    = ensureSparseRef(entt);
		page_ptr_t  currDataPage = dataCurrentPage();

		auto denseIdx = m_entityDense.size();
		auto offset = denseOffset(denseIdx);
		allocator_t typeAlloc = m_dataDense.get_allocator();

		m_entityDense.emplace_back(entt);
		alloc_traits::construct(typeAlloc, currDataPage[offset], std::forward<argtyps>(args)...);
		entityRef = denseIdx;
	}

	void erase(index_type entt)
	{
		// TODO: CHECK IF THIS CODE STILL WORKS OR THE SYNTAX IS FLAWLESS
		assert(has(entt) && "Id doesn't exist");

		auto backIdx = (m_entityDense.size() - 1u);
		index_type& sparseBackRef   = sparseRef(m_entityDense.back());
		index_type& sparseErasedRef = sparseRef(entt);

		auto& dataBackRef   = dataItemRef(backIdx);
		auto& dataErasedRef = dataItemRef(sparseErasedRef);

		sparseErasedRef = sparseBackRef;
		dataErasedRef   = dataBackRef;

		sparseBackRef = EntityType::nullidx;
		dataBackRef   = EntityType::nullidx;

		m_entityDense[sparseErasedRef] = m_entityDense.back();
		m_entityDense.pop_back();

	}

	void clear()
	{
		m_entityDense.clear();
		releasePages();
	}

private:
	value_type& dataItemRef(index_type idx)
	{
		assert((idx < (m_dataDense.size() * densePageSize)) && "Invalid entity")
		return dataPageRef(idx)[denseOffset(idx)];
	}

	page_ptr_t dataPageRef(index_type idx)
	{
		return m_dataDense[densePageIndex(idx)];
	}

	page_ptr_t dataCurrentPage()
	{
		auto pageidx = densePageIndex(m_entityDense.size());
		if(!(pageidx < m_dataDense.size()))
		{
			reservePages((pageidx + 1u));
		}

		return m_dataDense[pageidx];
	}

	void reservePages(index_type newcap)
	{
		auto pageSize = densePageSize;
		auto currSize = m_dataDense.size();
		m_dataDense.resize(newcap, nullptr);
		for(const auto newSize = m_dataDense.size(); currSize < newSize; ++currSize)
		{
			//page_type* newPage{new page_type{}};
			//newPage->reserve(densePageSize);
			m_dataDense[currSize] = alloc_traits::allocate(m_dataDense.get_allocator(), pageSize);
			std::uninitialized_fill(m_dataDense[currSize], m_dataDense[currSize] + pageSize, EntityType::nullidx);
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
	
private:
	using pair_iterator = PairIterator<typename pool_base::iterator>;
	container_t m_dataDense;
};
}

#endif // POOL_HPP