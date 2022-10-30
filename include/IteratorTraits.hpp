#ifndef ITERATOR_TRAITS_HPP
#define ITERATOR_TRAITS_HPP 
#include <iterator>
#include <type_traits>
#include <utility>

template<class itfirst_t, class itsecond_t>
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
	constexpr PairIterator() = default;
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


template<class itfirst_t, class itsecond_t>
class Iterable
{
public:
	using first_iterator = itfirst_t;
	using second_iterator = itfirst_t;

	constexpr Iterable() = default;
	constexpr Iterable(itfirst_t first, itsecond_t second)
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

#endif // ITERATOR_TRAITS_HPP