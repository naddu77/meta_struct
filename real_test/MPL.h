#pragma once
#include <type_traits>

template <typename T, template <typename...> typename TTP>
struct IsSameTemplateTemplateParameter
	: std::false_type
{

};

template <template <typename...> typename TTP1, template <typename...> typename TTP2, typename... Ts>
struct IsSameTemplateTemplateParameter<TTP1<Ts...>, TTP2>
	: std::is_same<TTP1<Ts...>, TTP2<Ts...>>
{

};

template <typename T, template <typename...> typename TTP>
inline constexpr bool IsSameTemplateTemplateParameter_v{ IsSameTemplateTemplateParameter<std::remove_cvref_t<T>, TTP>::value };

template <typename T, template <typename...> typename TTP>
concept TemplateTemplateParameter = IsSameTemplateTemplateParameter_v<T, TTP>;