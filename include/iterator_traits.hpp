#ifndef ITERATOR_TRAITS_HPP
#define ITERATOR_TRAITS_HPP 
#include <iterator>
#include <type_traits>
#include <utility>

template<class itfirst_t, class itsecond_t = itfirst_t>
class PairIterator
{
public:
	using first_iterator    = itfirst_t;
	using second_iterator   = itsecond_t;
	using value_type        = std::pair<typename itfirst_t::value_type&, typename itsecond_t::value_type&>;
	using reference         = value_type;
	using difference_type   = std::ptrdiff_t;
	using iterator_category = std::input_iterator_tag;
	using iterator_pair     = std::pair<itfirst_t, itsecond_t>;
public:
	constexpr PairIterator() = delete;
	constexpr PairIterator(itfirst_t first, itsecond_t second)
		: m_pair{first, second} {}

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


template<class itfirst_t, class itsecond_t = itfirst_t>
class Iterable
{
public:
	using first_iterator  = itfirst_t;
	using second_iterator = itsecond_t;

	constexpr Iterable() = default;
	constexpr Iterable(itfirst_t first, itsecond_t second)
	: m_first{std::move(first)}, m_second(std::move(second)) {}

	constexpr first_iterator begin()
	{
		return m_first;
	}

	constexpr second_iterator end()
	{
		return m_second;
	}

	constexpr first_iterator cbegin()
	{
		return m_first;
	}

	constexpr second_iterator cend()
	{
		return m_second;
	}

private:
	first_iterator   m_first;
	second_iterator  m_second;
};

/*template<class itfirst_t, class itsecond_t = itfirst_t, class filter_t = void>
class Iterable
{
    // This version of the Iterable class has filtering functionality.
    // The Filter template parameter specifies the type of the filter functor
    // that will be used to filter the elements of the collection.
};

template<class itfirst_t, class itsecond_t>
class Iterable<itfirst_t, itsecond_t, void>
{
    // This version of the Iterable class does not have filtering functionality.
    // The void template parameter indicates that no filter functor will be used.
};*/

#endif // ITERATOR_TRAITS_HPP