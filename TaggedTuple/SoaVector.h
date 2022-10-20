#pragma once
#include <span>
#include <vector>
#include "TaggedTuple.h"

namespace NDataStructure
{
	template <typename TT>
	class SoaVector;

	template <auto... Tags, typename... Ts, auto... Inits>
	class SoaVector<TaggedTuple<Member<Tags, Ts, Inits>...>>
	{
		using TT = TaggedTuple<Member<Tags, Ts, Inits>...>;

		TaggedTuple<Member<Tags, std::vector<TaggedTupleValueType_t<Tags, TT>>>...> vs;

	public:
		SoaVector() = default;
		
		
		decltype(auto) Vectors()
		{
			return (vs);
		}

		decltype(auto) Vectors() const
		{
			return (vs);
		}

		void push_back(TT t)
		{
			(Get<Tags>(vs).push_back(Get<Tags>(t)), ...);
		}

		void pop_back()
		{
			(Get<Tags>(vs).pop_back(), ...);
		}

		void clear()
		{
			(Get<Tags>(vs).clear(), ...);
		}
		
		std::size_t size() const
		{
			return std::size(First());
		}

		bool empty() const
		{
			return std::empty(First());
		}

		auto operator[](std::size_t i)
		{
			return TaggedTupleRef_t<TT>((tag<Tags> = std::ref(Get<Tags>(vs)[i]))...);
		}

		auto operator[](std::size_t i) const
		{
			return TaggedTupleRef_t<TT>((tag<Tags> = std::ref(Get<Tags>(vs)[i]))...);;
		}

		auto front()
		{
			return (*this)[0];
		}

		auto back()
		{
			return (*this)[size() - 1];
		}

	private:
		template <auto Tag, auto...>
		decltype(auto) FirstHelper()
		{
			return Get<Tag>(vs);
		}

		template <auto Tag, auto...>
		decltype(auto) FirstHelper() const
		{
			return Get<Tag>(vs);
		}

		decltype(auto) First()
		{
			return FirstHelper<Tags...>();
		}

		decltype(auto) First() const
		{
			return FirstHelper<Tags...>();
		}
	};

	template <typename Tag, typename TT>
	decltype(auto) GetImpl(SoaVector<TT>& s)
	{
		return std::span{ Get<Tag::value>(s.Vectors()) };
	}

	template <typename Tag, typename TT>
	auto GetImpl(SoaVector<TT> const& s)
	{
		return std::span{ Get<Tag::value>(s.Vectors()) };
	}

	template <typename Tag, typename TT>
	auto GetImpl(SoaVector<TT>&& s)
	{
		return std::span{ Get<Tag::value>(std::move(s.Vectors())) }; 
	}
}