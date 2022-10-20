#pragma once
#include <algorithm>
#include <cstddef>
#include <optional>
#include <string_view>
#include <type_traits>
#include <ranges>
#include <iostream>
#include <string_view>

template <std::size_t N>
struct FixedString
{
	constexpr FixedString(char const (&str)[N + 1])
	{
		std::ranges::copy_n(str, N + 1, data);
	}

	constexpr explicit(true) FixedString(std::string_view s)
	{
		std::ranges::copy(s, std::begin(data));
	}

	constexpr std::string_view ToStringView() const
	{
		return { data };
	}

	constexpr auto operator<=>(FixedString const&) const = default;

	constexpr std::size_t size() const
	{
		return N;
	}

	char data[N + 1]{};
};

template <std::size_t N>
FixedString(char const (&str)[N])->FixedString<N - 1>;

namespace Internal
{
	template <typename... Ts>
	struct Inherit
		: Ts...
	{

	};

	template <
		typename TDefault,
		FixedString,
		template <FixedString, typename> typename
	>
	auto MapLookup(...)->TDefault;

	template <
		typename,
		FixedString TKey,
		template <FixedString, typename> typename TArg,
		typename TValue
	>
	auto MapLookup(TArg<TKey, TValue>*)->TArg<TKey, TValue>;

	template <
		typename TDefault,
		typename,
		template <typename, typename> typename
	>
	auto MapLookup(...)->TDefault;

	template <
		typename,
		typename TKey,
		template <typename, typename> typename TArg,
		typename TValue
	>
	auto MapLookup(TArg<TKey, TValue>*)->TArg<TKey, TValue>;
}

template <typename T, FixedString TKey, typename TDefault, template <FixedString, typename> typename TArg>
using MapLookup = decltype(Internal::MapLookup<TDefault, TKey, TArg>(static_cast<T*>(nullptr)));

namespace Internal
{
	template <FixedString tag, typename T>
	struct TagAndValue
	{
		constexpr static auto Tag()
		{
			return tag;
		}

		using value_type = T;

		T value;
	};

	template <typename... Members>
	struct MetaStruct;

	struct NoConversion
	{
		// Nothing
	};

	template <typename... TagsAndValues>
	struct Params
		: TagsAndValues...
	{
		constexpr operator NoConversion() const
		{
			return NoConversion{};
		}
	};

	template <typename... TagsAndValues>
	Params(TagsAndValues...)->Params<TagsAndValues...>;

	template <typename T>
	struct DefaultInit
	{
		constexpr DefaultInit() = default;

		constexpr auto operator<=>(DefaultInit const&) const = default;

		constexpr auto operator()() const
		{
			if constexpr (std::is_default_constructible_v<T>)
			{
				return T{};
			}
		}
	};

	template <FixedString Tag, typename T, typename Self, typename F>
	constexpr auto CallInit(Self&, F& f) requires(requires {
		{ f() } -> std::convertible_to<T>;
	})
	{
		return f();
	}

	template <FixedString Tag, typename T, typename Self, typename F>
	constexpr auto CallInit(Self& self, F& f) requires(requires {
		{ f(self) } -> std::convertible_to<T>;
	})
	{
		return f(self);
	}

	template <FixedString Tag, typename T, typename Self, typename F>
	constexpr auto CallInit(Self& self, F& f) requires(requires {
		{ f() } -> std::same_as<void>;
	})
	{
		static_assert(!std::is_same_v<std::invoke_result_t<F>, void>,
			"Required argument not specified");
	}

	inline constexpr auto required{ [] {} };

	template <FixedString tag, typename T, auto init = DefaultInit<T>(), MetaStruct attributes = {}>
	struct Member
	{
		constexpr static auto Tag()
		{
			return tag;
		}

		constexpr static auto Init()
		{
			return init;
		}

		constexpr static auto Attributes()
		{
			return attributes;
		}

		using value_type = T;

		T value;

		constexpr Member() = default;

		template <typename Self, typename OtherT>
		constexpr Member(Self&, TagAndValue<tag, OtherT> tv)
			: value(std::move(tv.value))
		{

		}

		template <typename Self>
		constexpr Member(Self& self, NoConversion)
			: value(CallInit<tag, T>(self, init))
		{

		}

		template <typename Self>
		constexpr Member(Self& self, TagAndValue<tag, std::optional<std::remove_reference_t<T>>> tv_or) requires(!std::is_reference_v<T>)
			: value(tv_or.value.has_value() ? std::move(*tv_or.value) : CallInit<tag, T>(self, init))
		{

		}

		template <typename Self, typename OtherT, auto OtherInit, auto OtherAtrributes>
		requires(std::is_convertible_v<OtherT, T>)
		constexpr Member(Self&, Member<tag, OtherT, OtherInit, OtherAtrributes> const& other)
			: value(other.value)
		{

		}

		template <typename Self, typename OtherT, auto OtherInit, auto OtherAtrributes>
		requires(std::is_convertible_v<OtherT, T>)
		constexpr Member(Self&, Member<tag, OtherT, OtherInit, OtherAtrributes>&& other) noexcept
			: value(std::move(other.value))
		{

		}

		template <typename Self, typename OtherT, auto OtherInit, auto OtherAttributes>
		requires(std::is_convertible_v<OtherT, T>)
		constexpr Member& operator=(Member<tag, OtherT, OtherInit, OtherAttributes> const& other)
		{
			if (this != &other)
			{
				value = other.value;
			}

			return *this;
		}

		template <typename Self, typename OtherT, auto OtherInit, auto OtherAttributes>
		requires(std::is_convertible_v<OtherT, T>)
		constexpr Member& operator=(Member<tag, OtherT, OtherInit, OtherAttributes>&& other) noexcept
		{
			if (this != &other)
			{
				value = std::move(other.value);
			}

			return *this;
		}
		
		constexpr Member& operator=(T&& t)
		{
			value = std::forward<T>(t);

			return *this;
		}

		constexpr Member(Member&&) noexcept = default;
		constexpr Member(Member const&) = default;

		constexpr Member& operator=(Member&&) noexcept = default;
		constexpr Member& operator=(Member const&) = default;

		constexpr auto operator<=>(Member const&) const = default;
	};

	template <typename... Members>
	struct MetaStructImpl
		: Members...
	{
		template <typename... Args>
		constexpr MetaStructImpl(Params<Args...> p)
			: Members(*this, std::move(p))...
		{

		}

		constexpr MetaStructImpl()
			: Members(*this, NoConversion{})...
		{

		}

		constexpr MetaStructImpl(MetaStructImpl&&) noexcept = default;
		constexpr MetaStructImpl(MetaStructImpl const&) = default;
		constexpr MetaStructImpl& operator=(MetaStructImpl&&) noexcept = default;
		constexpr MetaStructImpl& operator=(MetaStructImpl const&) = default;

		template <typename... OtherMembers>
		constexpr MetaStructImpl(MetaStructImpl<OtherMembers...>&& other) noexcept
			: Members(*this, std::move(other))...
		{

		}

		template <typename... OtherMembers>
		constexpr MetaStructImpl(MetaStructImpl<OtherMembers...> const& other)
			: Members(*this, other)...
		{

		}

		template <typename... OtherMembers>
		constexpr MetaStructImpl& operator=(MetaStructImpl<OtherMembers...>&& other) noexcept
		{
			if (this != &other)
			{
				((static_cast<Members&>(*this) = std::move(other)), ...);
			}

			return *this;
		}

		template <typename... OtherMembers>
		constexpr MetaStructImpl& operator=(MetaStructImpl<OtherMembers...> const& other)
		{
			if (this != &other)
			{
				((static_cast<Members&>(*this) = other), ...);
			}

			return *this;
		}

		constexpr operator NoConversion() const
		{
			return NoConversion{};
		}

		constexpr auto operator<=>(MetaStructImpl const&) const = default;
	};

	template <typename... Members>
	struct MetaStruct
		: MetaStructImpl<Members...>
	{
		using Super = MetaStructImpl<Members...>;

		template <typename... TagsAndValues>
		constexpr MetaStruct(TagsAndValues... tags_and_values)
			: Super(Params(std::move(tags_and_values)...))
		{

		}

		constexpr MetaStruct() = default;
		constexpr MetaStruct(MetaStruct&&) noexcept = default;
		constexpr MetaStruct(MetaStruct const&) = default;
		constexpr MetaStruct& operator=(MetaStruct&&) noexcept = default;
		constexpr MetaStruct& operator=(MetaStruct const&) = default;

		template <typename... OtherMembers>
		constexpr MetaStruct(MetaStruct<OtherMembers...>&& other) noexcept
			: Super(std::move(other))
		{

		}

		template <typename... OtherMembers>
		constexpr MetaStruct(MetaStruct<OtherMembers...> const& other)
			: Super(other)
		{

		}

		template <typename... OtherMembers>
		constexpr MetaStruct& operator=(MetaStruct<OtherMembers...>&& other) noexcept
		{
			if (this != &other)
			{
				static_cast<Super&>(*this) = std::move(other);
			}

			return *this;
		}

		template <typename... OtherMembers>
		constexpr MetaStruct& operator=(MetaStruct<OtherMembers...> const& other)
		{
			if (this != &other)
			{
				static_cast<Super&>(*this) = other;
			}

			return *this;
		}

		constexpr auto operator<=>(MetaStruct const&) const = default;

		template <std::size_t N>
		auto& get()
		{
			auto id_type{ [] <auto... Ns>(std::index_sequence<Ns...>) {
				return Inherit<std::pair<std::integral_constant<std::size_t, Ns>, Members>...>{};
			}(std::make_index_sequence<sizeof...(Members)>{}) };
			
			using T = typename decltype(MapLookup<void, std::integral_constant<std::size_t, N>, std::pair>(&id_type))::second_type;

			return static_cast<T&>(*this);
		}

		template <std::size_t N>
		auto const& get() const
		{
			auto id_type{ [] <auto... Ns>(std::index_sequence<Ns...>) {
				return Inherit<std::pair<std::integral_constant<std::size_t, Ns>, Members>...>{};
			}(std::make_index_sequence<sizeof...(Members)>{}) };

			using T = typename decltype(MapLookup<void, std::integral_constant<std::size_t, N>, std::pair>(&id_type))::second_type;

			return static_cast<T const&>(*this);
		}
	};

	template <typename... TagsAndValues>
	MetaStruct(TagsAndValues...)->MetaStruct<Member<TagsAndValues::Tag(), typename TagsAndValues::value_type>...>;

	template <FixedString Tag>
	struct ArgType
	{
		template <typename T>
		constexpr auto operator=(T t) const
		{
			return TagAndValue<Tag, T>{ std::move(t) };
		}
	};

	template <FixedString Tag>
	inline constexpr auto arg{ ArgType<Tag>{} };

	template <typename F, typename... Members>
	constexpr decltype(auto) MetaStructApply(F&& f, MetaStructImpl<Members...>& m)
	{
		return std::invoke(std::forward<F>(f), static_cast<Members&>(m)...);
	}

	template <typename F, typename... Members>
	constexpr decltype(auto) MetaStructApply(F&& f, MetaStructImpl<Members...> const& m)
	{
		return std::invoke(std::forward<F>(f), static_cast<Members const&>(m)...);
	}

	template <typename F, typename... Members>
	constexpr decltype(auto) MetaStructApply(F&& f, MetaStructImpl<Members...>&& m)
	{
		return std::invoke(std::forward<F>(f), static_cast<Members&&>(m)...);
	}

	template <typename MetaStructImpl>
	struct ApplyStaticImpl;

	template <typename... Members>
	struct ApplyStaticImpl<MetaStructImpl<Members...>>
	{
		template <typename F>
		constexpr static decltype(auto) Apply(F&& f)
		{
			return std::invoke(std::forward<F>(f), static_cast<Members*>(nullptr)...);
		}
	};

	template <typename MetaStruct, typename F>
	constexpr auto MetaStructApply(F&& f)
	{
		return ApplyStaticImpl<typename MetaStruct::Super>::Apply(std::forward<F>(f));
	}

	template <typename MetaStruct, typename F>
	constexpr void MetaStructForEach(F&& f, MetaStruct&& ms)
	{
		MetaStructApply([&](auto&... m) mutable {
			(std::invoke(std::forward<F>(f), m), ...);
		}, std::forward<MetaStruct>(ms));
	}

	template <FixedString Tag, typename T, auto Init, auto Attrs>
	constexpr decltype(auto) GetImpl(Member<Tag, T, Init, Attrs>& m)
	{
		return (m.value);
	}

	template <FixedString Tag, typename T, auto Init, auto Attrs>
	constexpr decltype(auto) GetImpl(Member<Tag, T, Init, Attrs> const& m)
	{
		return (m.value);
	}

	template <FixedString Tag, typename T, auto Init, auto Attrs>
	constexpr decltype(auto) GetImpl(Member<Tag, T, Init, Attrs>&& m)
	{
		return std::move(m.value);
	}

	template <FixedString Tag, typename MetaStruct>
	constexpr decltype(auto) Get(MetaStruct&& s)
	{
		return GetImpl<Tag>(std::forward<MetaStruct>(s));
	}

	template <FixedString Tag, typename T, auto Init, auto Attrs>
	constexpr Member<Tag, T, Init, Attrs> GetMemberImpl(Member<Tag, T, Init, Attrs> const& m);

	template <FixedString Tag, typename MetaStruct>
	constexpr auto GetAttributes(MetaStruct&& s)
	{
		return decltype(GetMemberImpl(s))::Attributes();
	}

	template <FixedString Tag, typename MetaStruct>
	constexpr auto GetAttributes()
	{
		return decltype(GetMemberImpl<Tag>(std::declval<MetaStruct&>()))::Attributes();
	}

	template <FixedString Tag, typename T, auto Init, auto Attrs>
	constexpr std::true_type HasImpl(Member<Tag, T, Init, Attrs> const&);

	template <FixedString Tag, typename T, auto Init, auto Attrs>
	constexpr std::false_type HasImpl(NoConversion);

	template <FixedString Tag, typename MetaStruct>
	constexpr bool Has(MetaStruct&& s)
	{
		return decltype(HasImpl<Tag>(s))::value;
	}

	template <FixedString Tag, typename MetaStruct>
	constexpr bool Has()
	{
		return decltype(HasImpl<Tag>(std::declval<MetaStruct&>()))::value;
	}

	template <typename... Members>
	constexpr std::size_t MetaStructSizeImpl(MetaStruct<Members...> const*)
	{
		return sizeof...(Members);
	}

	template <typename... Members>
	constexpr std::size_t MetaStructSize(MetaStruct<Members...> const&)
	{
		return sizeof...(Members);
	}

	template <typename MetaStruct>
	constexpr std::size_t MetaStructSize()
	{
		return MetaStructSizeImpl(static_cast<MetaStruct const*>(nullptr));
	}
}

using Internal::arg;
using Internal::Get;
using Internal::GetAttributes;
using Internal::Has;
using Internal::Member;
using Internal::MetaStruct;
using Internal::MetaStructApply;
using Internal::MetaStructSize;
using Internal::required;
using Internal::DefaultInit;

namespace std
{
	template <typename... Members>
	struct tuple_size<MetaStruct<Members...>>
		: std::integral_constant<std::size_t, sizeof...(Members)>
	{

	};

	template <std::size_t N, typename... Members>
	struct tuple_element<N, MetaStruct<Members...>>
	{
		using type = decltype(std::declval<MetaStruct<Members...>>().template get<N>());
	};

	template <std::size_t N, typename... Members>
	auto const& get(MetaStruct<Members...> const& nt) noexcept
	{
		return nt.template get<N>();

	}

	template <std::size_t N, typename... Members>
	auto& get(MetaStruct<Members...>& nt) noexcept
	{
		return nt.template get<N>();
	}
}

template <std::size_t N>
std::ostream& operator<<(std::ostream& os, FixedString<N> const& fixed_string)
{
	os << fixed_string.data;

	return os;
}

template <FixedString tag, typename T, auto init, MetaStruct attributes>
std::ostream& operator<<(std::ostream& os, Member<tag, T, init, attributes> const& member)
{
	os << member.Tag() << ":" << member.value;

	return os;
}

template <typename... Members>
std::ostream& operator<<(std::ostream& os, MetaStruct<Members...> const& s)
{
	MetaStructApply([&](auto const&... m) {
		((os << m << "\n"), ...);
	}, s);

	return os;
}
