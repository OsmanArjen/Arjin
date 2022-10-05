#ifndef POOL_HPP
#define POOL_HPP
#include <vector>
#include <cassert>
#include <utility>
#include <type_traits>
#include "TypeInfo.hpp"


// TODO: FIX VIEW CLASS ACCORDING TO NEW POOLBASE STRUCTURE

/*
 * Create a base iterator named PoolIterator and make it abstract then derive it from enttiterator and compiterator 
 * with pure virtual functions+ pre defined functions
 * and define enttiterator with compiterator
 */

/* 

TODO: IMPLEMENT PAGING AND FIX THIS

ALSO FIND A WAY TO IMPLEMENT PAGING TO COMPONENT DENSE TOO

maybe EnttIterator and DataIterator(or CompIterator)
*/

// todo: generally get the things together and make it work may god be with you, myself,  mashallah to you be emanet to god amin
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
	, m_curr{idx} {}

	constexpr EnttIterator(const EnttIterator<pooldense>& other)
	: EnttIterator{other.m_dense, other.m_curr} {}



	constexpr EnttIterator& operator++()
	{
		m_curr++;
		return *this;
	}

	constexpr EnttIterator  operator++(int)
	{
		auto cpy = *this;
		++m_curr;
		return cpy;
	}

	constexpr EnttIterator& operator--()
	{
		m_curr--;
		return *this;
	}

	constexpr EnttIterator  operator--(int)
	{
		auto cpy = *this;
		--m_curr;
		return cpy;
	}

    constexpr EnttIterator& operator+=(const difference_type value) noexcept
    {
        m_curr += value;
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

	constexpr bool operator==(const EnttIterator& other) noexcept
	{
	    return m_curr == other.m_curr;
	}
	

	constexpr bool operator!=(const EnttIterator& other) noexcept
	{
	    return !(m_curr == other.m_curr);
	}
	

	constexpr bool operator<(const EnttIterator& other) noexcept
	{
	    return m_curr < other.m_curr;
	}
	

	constexpr bool operator>(const EnttIterator& other) noexcept
	{
	    return m_curr > other.m_curr;
	}
	

	constexpr bool operator<=(const EnttIterator& other) noexcept
	{
	    return !(other.m_curr > m_curr);
	}
	

	constexpr bool operator>=(const EnttIterator& other) noexcept
	{
	    return !(other.m_curr < m_curr);
	}
	

private:
	pooldense*       m_dense;
	difference_type  m_curr;

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
	, m_curr{idx} {}

	constexpr CompIterator(const CompIterator<pooldense>& other)
	: CompIterator{other.m_dense, other.m_curr} {}



	constexpr CompIterator& operator++()
	{
		m_curr++;
		return *this;
	}

	constexpr CompIterator  operator++(int)
	{
		auto cpy = *this;
		++m_curr;
		return cpy;
	}

	constexpr CompIterator& operator--()
	{
		m_curr--;
		return *this;
	}

	constexpr CompIterator  operator--(int)
	{
		auto cpy = *this;
		--m_curr;
		return cpy;
	}

    constexpr CompIterator& operator+=(const difference_type value) noexcept
    {
        m_curr += value;
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

	constexpr bool operator==(const CompIterator& other) noexcept
	{
	    return m_curr == other.m_curr;
	}
	

	constexpr bool operator!=(const CompIterator& other) noexcept
	{
	    return !(m_curr == other.m_curr);
	}
	

	constexpr bool operator<(const CompIterator& other) noexcept
	{
	    return m_curr < other.m_curr;
	}
	

	constexpr bool operator>(const CompIterator& other) noexcept
	{
	    return m_curr > other.m_curr;
	}
	

	constexpr bool operator<=(const CompIterator& other) noexcept
	{
	    return !(other.m_curr > m_curr);
	}
	

	constexpr bool operator>=(const CompIterator& other) noexcept
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
	using idx_type       = idx_t;
	using idx_vec        = std::vector<idx_t>
	using container_t    = idx_vec;
	using iterator       = PoolIterator<container_t>;
	using const_iterator = PoolIterator<const container_t>;

	static constexpr unsigned int spEnttShift{12};
	static constexpr unsigned int sparsePage{2 ** spEnttShift};

public:

	PoolBase() = default;
	PoolBase(PoolBase&& other)
	: m_sparse{std::move(other.m_sparse)}
	, m_enttDense{std::move(other.m_enttDense)} {}

	virtual ~PoolBase()
	{
		releasePages();
	}

	idx_type entity(const idx_type& idx)
	{
		assert(idx < m_enttDense.size() && "Index is out of bounds");
		return m_enttDense[idx];
	}

	/**
	     * @warning
	     * Attempting to get the entity that doesn't belong to the
	     * Pool results in undefined behavior.
	*/
	idx_type index(const idx_type& idx)
	{
		return sparseIdx(idx);
	}


	bool empty() const
	{
		return m_enttDense.empty();
	}

	bool has(const idx_type& idx) const
	{
		return (idx < (m_sparse.size() * PoolBase::sparsePage) && sparseIdx(idx) != EntityId::nullidx);
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
		return {&m_enttDense, {}};
	}

	iterator end()
	{
		const auto endpos = static_cast<typename iterator::difference_type>(m_enttDense.size()); 
		return {&m_enttDense, endpos};
	}

	const_iterator cbegin()
	{
		return {&m_enttDense, {}};
	}

	const_iterator cend()
	{
		const auto endpos = static_cast<typename iterator::difference_type>(m_enttDense.size()); 
		return {&m_enttDense, endpos};
	}

protected:
	std::vector<idx_vec*> m_sparse;
	container_t           m_enttDense;

	// Todo: will allocate new pages for sparse if needed and will return things to use the sparse set im tired do this
	idx_type& sparseEnsure(const idx_type& enttIdx)
	{
		const auto enttpage{page(enttIdx)};

		if(!(enttpage < m_sparse.size()))
		{
			m_sparse.resize((enttpage + 1), nullptr);
			m_sparse[enttpage] = {new idx_vec{PoolBase::sparsePage, EntityId::nullidx}};
		}

		return sparseIdx(enttIdx);
	}

	idx_type& sparseIdx(const idx_type& enttIdx) const 
	{
		return m_sparse[page(enttIdx)][offset(enttIdx)];
	}

	idx_type page(const idx_type& enttIdx)
	{
		return (enttIdx >> PoolBase::spEnttShift);
	}

	idx_type offset(const idx_type& enttIdx)
	{
		return (enttIdx & (PoolBase::sparsePage - 1));
	}
	
private:

	void releasePages()
	{
		for(auto&& pageptr : m_sparse)
		{
			if(pageptr != nullptr)
			{
				delete pageptr;
			}
		}
	}
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
	Pool(idx_type capacity)
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
	value_t& get(const idx_type& idx)
	{
		return m_dataDense[sparseIdx(idx)];
	}

	const value_t& get(const idx_type& idx) const
	{
		return m_dataDense[sparseIdx(idx)];
	}

	void reserve(idx_type capacity)
	{

		m_dataDense.reserve(capacity);
		m_enttDense.reserve(capacity); //TODO: IMPLEMENT PAGING TO IT 
		//m_sparse.resize(capacity, EntityId::nullidx); we might not need this but look incase
	}

	iterator begin()
	{
		return {&m_dataDense, {}};
	}

	iterator end()
	{
		const auto endpos = static_cast<typename iterator::difference_type>(m_dataDense.size()); 
		return {&m_dataDense, endpos};
	}

	const_iterator cbegin()
	{
		return {&m_dataDense, {}};
	}

	const_iterator cend()
	{
		const auto endpos = static_cast<typename iterator::difference_type>(m_dataDense.size()); 
		return {&m_dataDense, endpos};
	}

	//-
	template<typename ...argtyps>
	void insert(const idx_type& entt, argtyps&& ...args)
	{
		assert(!has(entt) && "Id already exists");

		auto  denseIdx = m_enttDense.size();
		auto& spentt   = sparseEnsure(entt);

		m_enttDense.emplace_back(entt);
		m_dataDense.emplace_back(std::forward<argtyps>(args)...);
		spentt = denseIdx;
		
		
	}

	void erase(const idx_type& entt)
	{
		assert(has(entt) && "Id doesn't exist");

		const auto last = m_enttDense.back();
		auto& splast = sparseIdx(last);
		auto& spentt = sparseIdx(entt);

		std::swap(m_enttDense.back(),  m_enttDense[spentt]);
		std::swap(m_dataDense.back(),  m_dataDense[spentt]);

		splast  = spentt;
		spentt  = EntityId::nullidx;

		m_enttDense.pop_back();
		m_dataDense.pop_back();
		
	}

	void clear()
	{
		m_enttDense.clear();
		m_dataDense.clear();
	}

private:
	container_t   m_dataDense;
};


#endif // POOL_HPP