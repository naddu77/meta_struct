#pragma once
#include <algorithm>
#include <string>
#include <string_view>

template <std::size_t N>
struct FixedString
{
	constexpr FixedString() = default;

	constexpr FixedString(char const (&foo)[N + 1])
	{
		std::copy_n(std::begin(foo), N + 1, std::begin(data));
	}

	constexpr FixedString(std::string_view s)
	{
		std::copy_n(std::data(s), std::size(s), std::begin(data));
	}

	constexpr FixedString(std::string_view s1, std::string_view s2)
	{
		std::copy_n(std::data(s1), std::size(s1), std::begin(data));
		std::copy_n(std::data(s2), std::size(s2), std::next(std::begin(data), std::size(s1)));
	}

	constexpr FixedString(std::string const& s)
	{
		std::copy_n(std::data(s), std::size(s), std::begin(data));
	}

	auto operator<=>(FixedString const&) const = default;

	char data[N + 1]{};
	static std::size_t const length{ N };

	constexpr std::string_view ToStringView() const
	{
		return { &data[0], N };
	}

	constexpr std::size_t size() const noexcept
	{
		return N;
	}

	template <auto fs2>
	constexpr bool contains() const noexcept
	{
		return !std::empty(std::ranges::search(std::begin(data), std::end(data), std::begin(fs2.data), std::prev(std::end(fs2.data))));
	}

	constexpr auto operator[](std::size_t i) const
	{
		return data[i];
	}
};

template <std::size_t N>
FixedString(char const (&str)[N]) -> FixedString<N - 1>;

template <std::size_t N>
FixedString(FixedString<N>) -> FixedString<N>;

template <
	std::size_t N1,
	std::size_t N2
>
constexpr auto operator+(
	FixedString<N1> const& fs1,
	FixedString<N2> const& fs2)
{
	FixedString<N1 + N2> result{ fs1.ToStringView() };

	std::copy_n(fs2.data, N2, result.data + N1);

	return result;
}

template <
	std::size_t N1,
	std::size_t N2
>
constexpr auto operator+(
	FixedString<N1> const& fs1,
	char const(&str)[N2])
{
	FixedString<N1 + N2 - 1> result{ fs1.ToStringView() };

	std::copy_n(str, N2 - 1, result.data + N1);

	return result;
}

namespace Literals
{
	template <FixedString fs>
	constexpr auto operator""_fs()
	{
		return fs;
	}
}