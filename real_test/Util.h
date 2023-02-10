#pragma once
#include "FixedString.h"
#include <cstdint>

template<std::intmax_t N>
class ToStringType
{
	constexpr static unsigned int Digit10() noexcept
	{
		unsigned int len{ N > 0 ? 1 : 2 };

		for (auto n = N; n; len++, n /= 10);

		return len;
	}

public:
	constexpr ToStringType() noexcept
	{
		auto ptr = buf.data + Digit10();
		*--ptr = '\0';

		if (N != 0) {
			for (auto n = N; n; n /= 10)
				*--ptr = "0123456789"[(N < 0 ? -1 : 1) * (n % 10)];
			if (N < 0)
				*--ptr = '-';
		}
		else {
			buf.data[0] = '0';
		}
	}

	constexpr operator FixedString<Digit10()>()
	{
		return buf;
	}

private:
	FixedString<Digit10()> buf;
};

template<std::intmax_t N>
constexpr unsigned int Digit10() noexcept
{
	unsigned int len{ N > 0 ? 1 : 2 };  // Note: For minus

	for (auto n = N; n; len++, n /= 10);

	return len;
}

template <int N>
constexpr auto IntergralToString()
{
	FixedString<Digit10<N>()> buf;
	auto ptr{ buf.data + Digit10<N>() };

	*--ptr = '\0';

	if (N != 0)
	{
		for (auto n{ N }; n; n /= 10)
		{
			*--ptr = "0123456789"[(N < 0 ? -1 : 1) * (n % 10)];
		}

		if (N < 0)
		{
			*--ptr = '-';
		}
	}
	else
	{
		buf.data[0] = '0';
	}

	return buf;
}