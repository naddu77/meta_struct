#pragma once
#include "TaggedTuple.h"
#include <nlohmann/json.hpp>

namespace nlohmann
{
	template <typename... Members>
	struct adl_serializer<NDataStructure::TaggedTuple<Members...>>
	{
		using TTuple = NDataStructure::TaggedTuple<Members... >;

		template <typename M>
		static auto get_from_json(json const& j)
		{
			using NDataStructure::tag;

			if constexpr (!M::HasDefaultInit())
			{
				return tag<M::FixedKey()> = j.at(std::data(M::Key())).get<typename M::value_type>();
			}
			else
			{
				if (j.contains(std::data(M::Key())))
				{
					return tag<M::FixedKey()> = std::optional<typename M::value_type>(
						j.at(std::data(M::Key())).get<typename M::value_type>()
					);
				}
				else
				{
					return tag<M::TagType::value> = std::optional<typename M::value_type>();
				}
			}
		}

		static TTuple from_json(json const& j)
		{
			return TTuple::ApplyStatic([&]<typename... M>(M* ...) {
				return TTuple(get_from_json<M>(j)...);
			});
		}

		static void to_json(json& j, TTuple const& t)
		{
			t.ForEach([&](auto& member) {
				j[std::data(member.Key())] = member.Value();
			});
		}
	};
}