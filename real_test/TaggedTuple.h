#pragma once

#include "FixedString.h"
#include <algorithm>
#include <array>
#include <cstdint>
#include <optional>
#include <string_view>
#include <tuple>
#include <utility>

namespace NDataStructure
{
	namespace InternalTaggedTuple
	{
		template <typename Member>
		constexpr auto MemberTag()
		{
			return Member::fs.ToStringView();
		}

		template <typename... Members>
		struct TaggedTuple;

		template <typename S, typename M>
		struct ChopToHelper;

		template <typename... Members, typename M>
		struct ChopToHelper<TaggedTuple<Members...>, M>
		{
			template <FixedString fs>
			static constexpr std::size_t FindIndex()
			{
				std::array<std::string_view, sizeof...(Members)> ar{
					MemberTag<Members>()...
				};

				return std::distance(std::begin(ar), std::find_if(std::begin(ar), std::end(ar), [](auto e) {
					return e == fs.ToStringView();
				}));
			}

			static constexpr std::size_t index{ FindIndex<M::fs>() };
			decltype(std::make_index_sequence<index>()) sequence;
			using MemberTuple = std::tuple<Members...>;

			template <std::size_t... I>
			static constexpr auto Chop(std::index_sequence<I...>)
			{
				return std::type_identity<TaggedTuple<std::tuple_element_t<I, MemberTuple>...>>{};
			}

			using type = typename decltype(Chop(sequence))::type;
		};

		template <typename S, typename M>
		using Chopped = typename ChopToHelper<S, M>::type;

		template <typename T>
		struct DefaultInitImpl
		{
			static constexpr auto Init()
			{
				if constexpr (std::is_default_constructible_v<T>)
				{
					return T{};
				}
			}
		};

		template <typename T>
		struct DefaultInitImpl<T&>
		{
			static constexpr void Init()
			{
				// Nothing
			}
		};

		template <typename T>
		constexpr auto default_init{ [] { return DefaultInitImpl<T>::Init(); } };

		struct DummyConversion
		{
			// Nothing
		};

		struct NoConversion
		{
			NoConversion() = delete;
			NoConversion(NoConversion const&) = delete;
			NoConversion& operator=(NoConversion const&) = delete;
		};

		template <typename T>
		struct OptionalNoRef
		{
			using type = std::optional<T>;
		};

		template <typename T>
		struct OptionalNoRef<T&>
		{
			using type = NoConversion;
		};

		template <typename T>
		using OptionalNoRef_t = typename OptionalNoRef<T>::type;

		template <typename Tag, typename T, auto Init = default_init<T>>
		struct MemberImpl
		{
			static constexpr decltype(Init) init{ Init };
			T value;
			
			template <typename Self>
			constexpr MemberImpl(Self& self, DummyConversion) requires requires
			{
				{ Init() } -> std::convertible_to<T>;
			}
			: value{ Init() }
			{
				// Nothing
			}

			static constexpr bool HasDefaultInit() requires requires
			{
				{ Init() } -> std::same_as<void>;
			}
			{
				return false;
			}

			static constexpr bool HasDefaultInit()
			{
				return true;
			}

			using OptionalType = OptionalNoRef_t<T>;

			template <typename Self, typename U>
			constexpr MemberImpl(Self& self, U value_or) requires
				std::same_as<U, OptionalType> && requires
			{
				{ Init() } -> std::convertible_to<T>;
			}
			: value{ value_or.has_value() ? std::move(*value_or) : Init() }
			{
				// Nothing
			}

			template <typename Self>
			constexpr MemberImpl(Self& self, DummyConversion) requires requires
			{
				{ Init() } -> std::same_as<void>;
			}
			{
				static_assert(!std::is_same_v<decltype(Init()), void>, "Missing required argument.");
			}

			template <typename Self>
			constexpr MemberImpl(Self& self, DummyConversion) requires requires
			{
				{ Init(self) } -> std::convertible_to<T>;
			}
			: value{ Init(self) }
			{
				// Nothing
			}

			template <typename Self, typename U>
			constexpr MemberImpl(Self& self, U value_or) requires
				std::same_as<U, OptionalType> && requires
			{
				{ Init(self) } -> std::convertible_to<T>;
			}
			: value{ value_or.has_value() ? std::move(*value_or) : Init(self) }
			{
				// Nothing
			}

			constexpr MemberImpl(T value) requires(!std::is_reference_v<T>)
				: value{ std::move(value) }
			{
				// Nothing
			}

			constexpr MemberImpl(T value) requires(std::is_reference_v<T>)
				: value{ value }
			{
				// Nothing
			}

			template <typename Self>
			constexpr MemberImpl(Self&, T value) requires(!std::is_reference_v<T>)
				: value{ std::move(value) }
			{
				// Nothing
			}

			template <typename Self>
			constexpr MemberImpl(Self&, T value) requires(std::is_reference_v<T>)
				: value{ value }
			{
				// Nothing
			}

			constexpr MemberImpl() requires requires
			{
				{ Init() } -> std::convertible_to<T>;
			}
			: value{ Init() }
			{
				// Nothing
			}

			template <typename Self>
			constexpr MemberImpl() requires requires
			{
				{ Init() } -> std::convertible_to<T>;
			}
			: value{ Init() }
			{
				// Nothing
			}

			template <typename Self>
			constexpr MemberImpl(Self& self) requires requires
			{
				{ Init(self) } -> std::convertible_to<T>;
			}
			: value{ Init() }
			{
				// Nothing
			}

			constexpr MemberImpl(MemberImpl const&) = default;
			constexpr MemberImpl& operator=(MemberImpl const&) = default;
			constexpr MemberImpl(MemberImpl&&) noexcept = default;
			constexpr MemberImpl& operator=(MemberImpl&&) noexcept = default;

			template <typename Self, typename OtherT, auto OtherInit>
			constexpr MemberImpl(Self& self, MemberImpl<Tag, OtherT, OtherInit> const& other)
				: MemberImpl{ self, other.value }
			{
				// Nothing
			}

			template <typename Self, typename OtherT, auto OtherInit>
			constexpr MemberImpl(Self& self, MemberImpl<Tag, OtherT, OtherInit>& other)
				: MemberImpl{ self, other.value }
			{
				// Nothing
			}

			// Note: std::string_view는 std::string에 바로 대입이 되지 않으므로 이 생성자가 필요함
			template <typename Self, auto OtherInit>
			constexpr MemberImpl(Self& self, MemberImpl<Tag, std::string_view, OtherInit>& other)
				: MemberImpl{ self, std::string{ other.value } }
			{
				// Nothing
			}

			template <typename Self, typename OtherT, auto OtherInit>
			constexpr MemberImpl(Self& self, MemberImpl<Tag, OtherT, OtherInit>&& other) noexcept
				: MemberImpl{ self, std::move(other.value) }
			{
				// Nothing
			}

			template <typename OtherT, auto OtherInit>
			constexpr MemberImpl& operator=(MemberImpl<Tag, OtherT> const& other)
			{
				if (this != &other)
				{
					value = other.value;
				}

				return *this;
			}

			template <typename OtherT, auto OtherInit>
			constexpr MemberImpl& operator=(MemberImpl<Tag, OtherT>&& other) noexcept
			{
				if (this != &other)
				{
					value = std::move(other.value);
				}

				return *this;
			}

			//constexpr auto operator<=>(MemberImpl const&) const = default;	// Note: 이 부분은 내부 컴파일러 에러를 유발하여 주석 처리

			constexpr auto operator<=>(MemberImpl const& other) const
			{
				return value <=> other.value;
			}

			using TagType = Tag;
			using value_type = T;

			static constexpr std::string_view Key()
			{
				return Tag::value.ToStringView();
			}

			static constexpr auto FixedKey()
			{
				return Tag::value;
			}

			constexpr value_type& Value()&
			{
				return value;
			}

			constexpr value_type&& Value()&&
			{
				return std::move(value);
			}

			constexpr value_type const& Value() const&
			{
				return value;
			}

			constexpr value_type const&& Value() const&&
			{
				return std::move(value);
			}
		};

		template <FixedString fs>
		struct TupleTag
		{
			static constexpr decltype(fs) value{ fs };
			static constexpr auto is_tuple_tag_type{ true };

			template <typename T>
			constexpr auto operator=(T t) const
			{
				return MemberImpl<TupleTag<FixedString<std::size(fs)>(fs)>, T>{ std::move(t) };
			}

			template <typename T>
			constexpr decltype(auto) operator()(T&& t) const
			{
				return Get<fs>(std::forward<T>(t));
			}
		};
		
		template <typename T>
		struct IsTupleTag
			: std::false_type
		{
			// Nothing
		};

		template <typename T>
		requires requires 
		{ 
			{ T::is_tuple_tag_type };
		}
		struct IsTupleTag<T>
			: std::true_type
		{
			// Nothing
		};

		template <typename T>
		constexpr bool is_tuple_tag_v{ IsTupleTag<T>::value };

		struct Auto
		{
			// Nothing
		};

		template <typename Self, typename T, auto Init>
		struct TypeOrAuto
		{
			using type = T;
		};

		template <typename Self, auto Init> requires requires
		{
			{ Init() };
		}
		struct TypeOrAuto<Self, Auto, Init>
		{
			using type = decltype(Init());
		};

		template <typename Self, typename T, auto Init> requires requires
		{
			{ Init(std::declval<Self&>()) } -> std::convertible_to<T>;
		}
		struct TypeOrAuto<Self, T, Init>
		{
			using type = decltype(Init(std::declval<Self&>()));
		};

		template <typename Self, auto Init> requires requires
		{
			{ Init(std::declval<Self&>()) };
		}
		struct TypeOrAuto<Self, Auto, Init>
		{
			using type = decltype(Init(std::declval<Self&>()));
		};

		template <typename Self, typename T, auto Init>
		using TypeOrAuto_t = typename TypeOrAuto<Self, T, Init>::type;

		template <FixedString Fs, typename T, auto Init = default_init<T>>
		struct Member
		{
			using type = T;
			using FixedStringType = decltype(Fs);
			static constexpr FixedStringType fs{ Fs };
			static constexpr decltype(Init) init{ Init };
		};

		template <typename Self, typename Member>
		struct MemberToImpl
		{
			using type = MemberImpl<
				TupleTag<FixedString<std::size(Member::fs)>(Member::fs)>,
				TypeOrAuto_t<Chopped<Self, Member>, typename Member::type, Member::init>,
				Member::init
			>;
		};

		template <typename Self, typename Member>
		using MemberToImpl_t = typename MemberToImpl<Self, Member>::type;

		template <typename Tag, typename T>
		constexpr auto MakeMemberImpl(T t)
		{
			return MemberImpl<Tag, T>{ std::move(t) };
		}

		template <typename... Members>
		struct Parameters
			: Members...
		{
			constexpr operator DummyConversion()
			{
				return {};
			}
		};

		template <typename... Members>
		Parameters(Members&&...)->Parameters<std::decay_t<Members>...>;

		template <typename Self, typename... Members>
		struct TaggedTupleBase
			: MemberToImpl_t<Self, Members>...
		{
			template <typename... Args>
			constexpr TaggedTupleBase(Self& self, Parameters<Args...> p)
				: MemberToImpl_t<Self, Members>{ self, p }...
			{
				// Nothing
			}

			template <typename OtherSelf, typename... OtherMembers>
			constexpr TaggedTupleBase(TaggedTupleBase<OtherSelf, OtherMembers...>& other)
				: MemberToImpl_t<Self, Members>{ other, static_cast<MemberToImpl_t<OtherSelf, OtherMembers>&>(other) }...
			{
				// Nothing
			}

			template <typename OtherSelf, typename... OtherMembers>
			constexpr TaggedTupleBase(TaggedTupleBase<OtherSelf, OtherMembers...> const& other)
				: MemberToImpl_t<Self, Members>{ other, static_cast<MemberToImpl_t<OtherSelf, OtherMembers> const&>(other) }...
			{
				// Nothing
			}

			constexpr TaggedTupleBase(TaggedTupleBase const&) = default;
			constexpr TaggedTupleBase& operator=(TaggedTupleBase const&) = default;

			constexpr TaggedTupleBase(TaggedTupleBase&&) noexcept = default;
			constexpr TaggedTupleBase& operator=(TaggedTupleBase&&) noexcept = default;

			constexpr auto operator<=>(TaggedTupleBase const&) const = default;

			template <typename F>
			static constexpr auto ApplyStatic(F&& f)
			{
				return f(static_cast<MemberToImpl_t<Self, Members>*>(nullptr)...);
			}

			template <typename F>
			constexpr auto Apply(F&& f)&
			{
				return f(static_cast<MemberToImpl_t<Self, Members>&>(*this)...);
			}

			template <typename F>
			constexpr auto Apply(F&& f) const&
			{
				return f(static_cast<MemberToImpl_t<Self, Members> const&>(*this)...);
			}

			template <typename F>
			constexpr auto Apply(F&& f)&&
			{
				return f(static_cast<MemberToImpl_t<Self, Members>&&>(*this)...);
			}

			template <typename F>
			constexpr void ForEach(F&& f) &
			{
				auto function_object{ [&](auto&&... a) mutable {
					(f(std::forward<decltype(a)>(a)), ...);
				} };

				Apply(function_object);
			}

			template <typename F>
			constexpr void ForEach(F&& f) const&
			{
				auto function_object{ [&](auto&&... a) mutable {
					(f(std::forward<decltype(a)>(a)), ...);
				} };

				Apply(function_object);
			}

			template <typename F>
			constexpr void ForEach(F&& f) &&
			{
				auto function_object{ [&](auto&&... a) mutable {
					(f(std::forward<decltype(a)>(a)), ...);
				} };

				Apply(function_object);
			}

			static constexpr auto size() noexcept
			{
				return sizeof...(Members);
			}

			static constexpr bool empty() noexcept
			{
				return sizeof...(Members) == 0;
			}
		};

		template <typename Tag, typename T, auto Init>
		constexpr decltype(auto) GetImpl(MemberImpl<Tag, T, Init>& m)
		{
			return (m.Value());
		}

		template <typename Tag, typename T, auto Init>
		constexpr decltype(auto) GetImpl(MemberImpl<Tag, T, Init> const& m)
		{
			return (m.Value());
		}

		template <typename Tag, typename T, auto Init>
		constexpr decltype(auto) GetImpl(MemberImpl<Tag, T, Init>&& m)
		{
			return std::move(m.Value());
		}

		template <typename Tag, typename T, auto Init>
		constexpr decltype(auto) GetImpl(MemberImpl<Tag, T, Init> const&& m)
		{
			return std::move(m.Value());
		}

		template <FixedString fs, typename S>
		constexpr decltype(auto) Get(S&& s)
		{
			return GetImpl<
				TupleTag<fs>
			>(std::forward<S>(s));
		}

		template <typename... Members>
		struct TaggedTuple
			: TaggedTupleBase<TaggedTuple<Members...>, Members...>
		{
			using Super = TaggedTupleBase<TaggedTuple, Members...>;

			template <typename... Tag, typename... T, auto... Init>
			constexpr TaggedTuple(MemberImpl<Tag, T, Init>... args)
				: Super(*this, Parameters{ std::move(args)... })
			{
				// Nothing
			}

			constexpr TaggedTuple()
				: Super(*this, Parameters{})
			{
				// Nothing
			}

			template <typename... OtherMembers>
			constexpr TaggedTuple(TaggedTuple<OtherMembers...>& other)
				: Super(other)
			{
				// Nothing
			}

			template <typename... OtherMembers>
			constexpr TaggedTuple(TaggedTuple<OtherMembers...> const& other)
				: Super(other)
			{
				// Nothing
			}

			constexpr TaggedTuple(TaggedTuple const&) = default;
			constexpr TaggedTuple& operator=(TaggedTuple const&) = default;
			constexpr TaggedTuple(TaggedTuple&&) noexcept = default;
			constexpr TaggedTuple& operator=(TaggedTuple&&) noexcept = default;

			template <typename Tag>
			constexpr auto& operator[](Tag)
			{
				return Get<Tag::value>(*this);
			}

			template <typename Tag>
			constexpr auto& operator[](Tag) const
			{
				return Get<Tag::value>(*this);
			}
		};

		template <typename Tag, typename T, auto Init>
		T TaggedTupleValueTypeImpl(MemberImpl<Tag, T, Init>&);	// has no body

		template <typename Tag, typename T, auto Init>
		decltype(Init) TaggedTupleInitImpl(MemberImpl<Tag, T, Init>&);	// has no body

		template <FixedString fs, typename Tuple>
		using TaggedTupleValueType_t = decltype(TaggedTupleValueTypeImpl<TupleTag<fs>>(std::declval<Tuple&>()));

		template <FixedString fs, typename Tuple>
		inline constexpr auto tagged_tuple_init_v{ decltype(TaggedTupleInitImpl<TupleTag<fs>>(std::declval<Tuple&>())){} };

		template <typename MemberType>
		struct MemberImplToMember
		{
			using TagType = typename MemberType::TagType;
			static constexpr decltype(MemberType::init) init{ MemberType::init };
			static constexpr FixedString<std::size(TagType::value)> fs{ TagType::value.ToStringView() };
			using type = Member<fs, typename MemberType::value_type, init>;
		};

		template <typename T>
		using MemberImplToMember_t = typename MemberImplToMember<T>::type;

		template <typename... Tag, typename... T, auto... Init>
		TaggedTuple(MemberImpl<Tag, T, Init>...)->TaggedTuple<MemberImplToMember_t<MemberImpl<Tag, T, Init>>...>;

		template <typename TaggedTuple>
		struct TaggedTupleRef;

		template <auto... Tags, typename... Ts, auto... Init>
		struct TaggedTupleRef<TaggedTuple<Member<Tags, Ts, Init>...>>
		{
			using Self = TaggedTuple<Member<Tags, Ts, Init>...>;
			using type = TaggedTuple<Member<Tags, std::add_lvalue_reference_t<TypeOrAuto_t<Self, Ts, Init>>, Init>...>;
		};

		template <auto... Tags, typename... Ts, auto... Init>
		struct TaggedTupleRef<TaggedTuple<Member<Tags, Ts, Init> const...>>
		{
			using Self = TaggedTuple<Member<Tags, Ts, Init>...>;
			using type = TaggedTuple<Member<Tags, std::add_lvalue_reference_t<std::add_const_t<TypeOrAuto_t<Self, Ts, Init>>>, Init>...>;
		};

		template <typename TaggedTuple>
		using TaggedTupleRef_t = typename TaggedTupleRef<TaggedTuple>::type;

		template <FixedString fs>
		inline constexpr auto tag{ TupleTag<FixedString<std::size(fs)>(fs)>{} };

		enum class TagComparison
		{
			Equal,
			NotEqual,
			LessThan,
			GreaterThan,
			LessThanOrEqual,
			GreaterThanOrEqual
		};

		template <typename TagOrValue1, typename TagOrValue2, TagComparison comparison>
		struct TagComparatorPredicate
		{
			template <typename Value, typename TS>
			constexpr static const auto& GetValueForComparison(Value const& value, TS const&)
			{
				return value;
			}

			template <typename Tag, typename TS>
			requires is_tuple_tag_v<Tag>
			static constexpr auto const& GetValueForComparison(Tag const& tag, TS const& ts)
			{
				return tag(ts);
			}

			TagOrValue1 tag_or_value1;
			TagOrValue2 tag_or_value2;

			template <typename TS>
			bool operator()(TS const& ts) const
			{
				auto const& a{ GetValueForComparison(tag_or_value1, ts) };
				auto const& b{ GetValueForComparison(tag_or_value2, ts) };

				if constexpr (comparison == TagComparison::Equal)
				{
					return a == b;
				}
				
				if constexpr (comparison == TagComparison::NotEqual)
				{
					return a != b;
				}

				if constexpr (comparison == TagComparison::LessThan)
				{
					return a < b;
				}

				if constexpr (comparison == TagComparison::GreaterThan)
				{
					return a > b;
				}

				if constexpr (comparison == TagComparison::LessThanOrEqual)
				{
					return a <= b;
				}

				if constexpr (comparison == TagComparison::GreaterThanOrEqual)
				{
					return a >= b;
				}
			}
		};

		template <TagComparison comparison, typename T1, typename T2>
		constexpr auto MakeTagComparatorPredicate(T1 a, T2 b)
		{
			return TagComparatorPredicate<T1, T2, comparison>{ std::move(a), std::move(b) };
		}

		namespace TagRelops
		{
			template <typename A, typename B>
				requires is_tuple_tag_v<A> || is_tuple_tag_v<B>
			constexpr auto operator==(A a, B b)
			{
				return MakeTagComparatorPredicate<TagComparison::Equal>(a, b);
			}

			template <typename A, typename B>
				requires is_tuple_tag_v<A> || is_tuple_tag_v<B>
			constexpr auto operator!=(A a, B b)
			{
				return MakeTagComparatorPredicate<TagComparison::NotEqual>(a, b);
			}

			template <typename A, typename B>
				requires is_tuple_tag_v<A> || is_tuple_tag_v<B>
			constexpr auto operator<=(A a, B b)
			{
				return MakeTagComparatorPredicate<TagComparison::LessThanOrEqual>(a, b);
			}

			template <typename A, typename B>
				requires is_tuple_tag_v<A> || is_tuple_tag_v<B>
			constexpr auto operator>=(A a, B b)
			{
				return MakeTagComparatorPredicate<TagComparison::GreaterThanOrEqual>(a, b);
			}

			template <typename A, typename B>
				requires is_tuple_tag_v<A> || is_tuple_tag_v<B>
			constexpr auto operator<(A a, B b)
			{
				return MakeTagComparatorPredicate<TagComparison::LessThan>(a, b);
			}

			template <typename A, typename B>
				requires is_tuple_tag_v<A> || is_tuple_tag_v<B>
			constexpr auto operator>(A a, B b)
			{
				return MakeTagComparatorPredicate<TagComparison::GreaterThan>(a, b);
			}
		}
	}

	using InternalTaggedTuple::Auto;
	using InternalTaggedTuple::Get;
	using InternalTaggedTuple::Member;
	using InternalTaggedTuple::tag;
	using InternalTaggedTuple::TaggedTuple;
	using InternalTaggedTuple::tagged_tuple_init_v;
	using InternalTaggedTuple::TaggedTupleRef_t;
	using InternalTaggedTuple::TaggedTupleValueType_t;

	namespace TagRelops = InternalTaggedTuple::TagRelops;

	namespace Literals
	{
		template <FixedString fs>
		constexpr auto operator""_tag()
		{
			return tag<fs>;
		}
	}
}
