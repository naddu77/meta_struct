#pragma once
#include "TaggedTuple.h"
#include <cassert>
#include <sqlite3.h>
#include <array>
#include <cstdint>
#include <exception>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <chrono>
#include <sstream>
#include <filesystem>

// Data �߰� �� �����ε��ؾ��� �޼���
// StringToType: Ÿ�� ����
// SqlType: Ŭ���� ���ø� Ư��ȭ
//  - ReadRowInto: Ÿ�Կ� �°� �б�
//  - BindImpl: Ÿ�Կ� �°� ���ε�
//  - ToConcrete: ��ü���� Ÿ������ ��ȯ

namespace NDatabase
{
	namespace Sqlite3
	{
		using NDataStructure::InternalTaggedTuple::FixedString;

		template <FixedString>
		struct StringToType;

		template <>
		struct StringToType<"integer">
		{
			using type = std::int64_t;
		};

		template <>
		struct StringToType<"int">
		{
			using type = int;
		};

		template <>
		struct StringToType<"ansi">
		{
			using type = std::string;
		};

		template <>
		struct StringToType<"text">
		{
			using type = std::u8string;
		};

		template <>
		struct StringToType<"utf16">
		{
			using type = std::u16string;
		};

		template <>
		struct StringToType<"real">
		{
			using type = double;
		};

		template <>
		struct StringToType<"bool">
		{
			using type = bool;
		};

		template <>
		struct StringToType<"date">
		{
			using type = std::chrono::system_clock::time_point;
		};

		template <>
		struct StringToType<"path">
		{
			using type = std::filesystem::path;
		};

		template <FixedString fs>
		using StringToType_t = typename StringToType<fs>::type;

		template <typename T>
		struct TypeToString;

		template <>
		struct TypeToString<std::int64_t>
		{
			static constexpr auto To()
			{
				return FixedString{ "integer" };
			}
		};

		template <>
		struct TypeToString<int>
		{
			static constexpr auto To()
			{
				return FixedString{ "int" };
			}
		};

		template <>
		struct TypeToString<std::string>
		{
			static constexpr auto To()
			{
				return FixedString{ "ansi" };
			}
		};

		template <>
		struct TypeToString<std::u8string>
		{
			static constexpr auto To()
			{
				return FixedString{ "text" };
			}
		};

		template <>
		struct TypeToString<std::u16string>
		{
			static constexpr auto To()
			{
				return FixedString{ "utf16" };
			}
		};

		template <>
		struct TypeToString<double>
		{
			static constexpr auto To()
			{
				return FixedString{ "double" };
			}
		};

		template <>
		struct TypeToString<bool>
		{
			static constexpr auto To()
			{
				return FixedString{ "bool" };
			}
		};

		template <>
		struct TypeToString<std::chrono::system_clock::time_point>
		{
			static constexpr auto To()
			{
				return FixedString{ "date" };
			}
		};

		template <>
		struct TypeToString<std::filesystem::path>
		{
			static constexpr auto To()
			{
				return FixedString{ "path" };
			}
		};

		template <typename T>
		class SqlType;

		template <>
		class SqlType<std::int64_t>
		{
		public:
			inline static bool ReadRowInto(sqlite3_stmt* stmt, int index, std::int64_t& v)
			{
				if (auto type{ sqlite3_column_type(stmt, index) }; type == SQLITE_INTEGER)
				{
					v = sqlite3_column_int64(stmt, index);

					return true;
				}
				else if (type == SQLITE_NULL)
				{
					return false;
				}
				else
				{
					return false;
				}
			}

			inline static bool BindImpl(sqlite3_stmt* stmt, int index, std::int64_t v)
			{
				auto r{ sqlite3_bind_int64(stmt, index, v) };

				return r == SQLITE_OK;
			}

			inline static auto ToConcrete(std::int64_t i)
			{
				return i;
			}
		};

		template <>
		class SqlType<int>
		{
		public:
			inline static bool ReadRowInto(sqlite3_stmt* stmt, int index, int& v)
			{
				if (auto type{ sqlite3_column_type(stmt, index) }; type == SQLITE_INTEGER)
				{
					v = sqlite3_column_int(stmt, index);

					return true;
				}
				else if (type == SQLITE_NULL)
				{
					return false;
				}
				else
				{
					return false;
				}
			}

			inline static bool BindImpl(sqlite3_stmt* stmt, int index, int v)
			{
				auto r{ sqlite3_bind_int64(stmt, index, v) };

				return r == SQLITE_OK;
			}

			inline static auto ToConcrete(int i)
			{
				return i;
			}
		};

		template <>
		class SqlType<double>
		{
		public:
			inline static bool ReadRowInto(sqlite3_stmt* stmt, int index, double& v)
			{
				if (auto type{ sqlite3_column_type(stmt, index) }; type == SQLITE_FLOAT)
				{
					v = sqlite3_column_double(stmt, index);

					return true;
				}
				else if (type == SQLITE_NULL)
				{
					return false;
				}
				else
				{
					return false;
				}
			}

			inline static bool BindImpl(sqlite3_stmt* stmt, int index, double v)
			{
				auto r{ sqlite3_bind_double(stmt, index, v) };

				return r == SQLITE_OK;
			}

			inline static auto ToConcrete(double d)
			{
				return d;
			}
		};

		template <>
		class SqlType<std::string_view>
		{
		public:
			inline static bool ReadRowInto(sqlite3_stmt* stmt, int index, std::string_view& v)
			{
				if (auto type{ sqlite3_column_type(stmt, index) }; type == SQLITE_TEXT)
				{
					auto ptr{ reinterpret_cast<char const*>(sqlite3_column_text(stmt, index)) };
					auto length{ static_cast<std::size_t>(sqlite3_column_bytes(stmt, index)) };

					v = std::string_view{ ptr, ptr ? length : 0 };

					return true;
				}
				else if (type == SQLITE_NULL)
				{
					return false;
				}
				else
				{
					return false;
				}
			}

			inline static bool BindImpl(sqlite3_stmt* stmt, int index, std::string_view v)
			{
				auto r{ sqlite3_bind_text(stmt, index, std::data(v), static_cast<int>(std::size(v)), SQLITE_TRANSIENT) };

				return r == SQLITE_OK;
			}

			inline static auto ToConcrete(std::string_view const& v)
			{
				return std::string{ v };
			}
		};

		template <>
		class SqlType<std::string>
		{
		public:
			inline static bool ReadRowInto(sqlite3_stmt* stmt, int index, std::string& v)
			{
				if (auto type{ sqlite3_column_type(stmt, index) }; type == SQLITE_TEXT)
				{
					auto ptr{ reinterpret_cast<char const*>(sqlite3_column_text(stmt, index)) };
					auto length{ static_cast<std::size_t>(sqlite3_column_bytes(stmt, index)) };

					if (ptr)
					{
						v = std::string{ ptr, length / sizeof(std::string::value_type) };
					}
					else
					{
						v.clear();
					}

					return true;
				}
				else if (type == SQLITE_NULL)
				{
					return false;
				}
				else
				{
					return false;
				}
			}

			inline static bool BindImpl(sqlite3_stmt* stmt, int index, std::string v)
			{
				auto r{ sqlite3_bind_text(stmt, index, std::data(v), static_cast<int>(std::size(v)) * sizeof(std::string::value_type), SQLITE_TRANSIENT) };

				return r == SQLITE_OK;
			}

			inline static auto ToConcrete(std::string const& v)
			{
				return v;
			}

			inline static auto ToConcrete(bool b)
			{
				return b;
			}
		};

		template <>
		class SqlType<std::u8string>
		{
		public:
			inline static bool ReadRowInto(sqlite3_stmt* stmt, int index, std::u8string& v)
			{
				if (auto type{ sqlite3_column_type(stmt, index) }; type == SQLITE_TEXT)
				{
					auto ptr{ reinterpret_cast<char const*>(sqlite3_column_text(stmt, index)) };
					auto length{ static_cast<std::size_t>(sqlite3_column_bytes(stmt, index)) };

					if (ptr)
					{
						v = std::u8string{ reinterpret_cast<char8_t const*>(ptr), length / sizeof(std::u8string::value_type) };
					}
					else
					{
						v.clear();
					}

					return true;
				}
				else if (type == SQLITE_NULL)
				{
					return false;
				}
				else
				{
					return false;
				}
			}

			inline static bool BindImpl(sqlite3_stmt* stmt, int index, std::u8string v)
			{
				auto r{ sqlite3_bind_text(stmt, index, reinterpret_cast<char const *>(v.c_str()), static_cast<int>(std::size(v)) * sizeof(std::u8string::value_type), SQLITE_TRANSIENT) };

				return r == SQLITE_OK;
			}

			inline static auto ToConcrete(std::u8string const& v)
			{
				return v;
			}
		};

		template <>
		class SqlType<std::wstring>
		{
		public:
			inline static bool ReadRowInto(sqlite3_stmt* stmt, int index, std::wstring& v)
			{
				if (auto type{ sqlite3_column_type(stmt, index) }; type == SQLITE_TEXT)
				{
					auto ptr{ reinterpret_cast<char const*>(sqlite3_column_text(stmt, index)) };
					auto length{ static_cast<std::size_t>(sqlite3_column_bytes(stmt, index)) };

					if (ptr)
					{
						v = std::wstring{ reinterpret_cast<wchar_t const*>(ptr), length / sizeof(std::wstring::value_type) };
					}
					else
					{
						v.clear();
					}

					return true;
				}
				else if (type == SQLITE_NULL)
				{
					return false;
				}
				else
				{
					return false;
				}
			}

			inline static bool BindImpl(sqlite3_stmt* stmt, int index, std::wstring v)
			{
				auto r{ sqlite3_bind_text(stmt, index, reinterpret_cast<char const*>(v.c_str()), static_cast<int>(std::size(v)) * sizeof(std::wstring::value_type), SQLITE_TRANSIENT) };

				return r == SQLITE_OK;
			}

			inline static auto ToConcrete(std::wstring const& v)
			{
				return v;
			}
		};

		template <>
		class SqlType<std::u16string>
		{
		public:
			inline static bool ReadRowInto(sqlite3_stmt* stmt, int index, std::u16string& v)
			{
				if (auto type{ sqlite3_column_type(stmt, index) }; type == SQLITE_TEXT)
				{
					auto ptr{ reinterpret_cast<char const*>(sqlite3_column_text(stmt, index)) };
					auto length{ static_cast<std::size_t>(sqlite3_column_bytes(stmt, index)) };

					if (ptr)
					{
						v = std::u16string{ reinterpret_cast<char16_t const*>(ptr), length / sizeof(std::u16string::value_type)};
					}
					else
					{
						v.clear();
					}

					return true;
				}
				else if (type == SQLITE_NULL)
				{
					return false;
				}
				else
				{
					return false;
				}
			}

			inline static bool BindImpl(sqlite3_stmt* stmt, int index, std::u16string v)
			{
				auto r{ sqlite3_bind_text(stmt, index, reinterpret_cast<char const*>(v.c_str()), static_cast<int>(std::size(v)) * sizeof(std::u16string::value_type), SQLITE_TRANSIENT) };

				return r == SQLITE_OK;
			}

			inline static auto ToConcrete(std::u16string const& v)
			{
				return v;
			}
		};

		template <>
		class SqlType<std::chrono::system_clock::time_point>
		{
		public:
			inline static bool ReadRowInto(sqlite3_stmt* stmt, int index, std::chrono::system_clock::time_point& v)
			{
				if (auto type{ sqlite3_column_type(stmt, index) }; type == SQLITE_TEXT)
				{
					auto ptr{ reinterpret_cast<char const*>(sqlite3_column_text(stmt, index)) };
					auto length{ static_cast<std::size_t>(sqlite3_column_bytes(stmt, index)) };

					if (ptr)
					{
						std::istringstream{ ptr } >> std::chrono::parse("%F %T", v);
					}

					return true;
				}
				else if (type == SQLITE_NULL)
				{
					return false;
				}
				else
				{
					return false;
				}
			}

			inline static bool BindImpl(sqlite3_stmt* stmt, int index, std::chrono::system_clock::time_point v)
			{
				auto str{ std::format("{}", v) };
				auto r{ sqlite3_bind_text(stmt, index, str.c_str(), static_cast<int>(std::size(str)), SQLITE_TRANSIENT) };

				return r == SQLITE_OK;
			}

			inline static auto ToConcrete(std::chrono::system_clock::time_point const& v)
			{
				return v;
			}
		};

		template <>
		class SqlType<bool>
		{
		public:
			inline static bool ReadRowInto(sqlite3_stmt* stmt, int index, bool& v)
			{
				if (auto type{ sqlite3_column_type(stmt, index) }; type == SQLITE_INTEGER)
				{
					v = static_cast<bool>(sqlite3_column_int(stmt, index));

					return true;
				}
				else if (type == SQLITE_NULL)
				{
					return false;
				}
				else
				{
					return false;
				}
			}

			inline static bool BindImpl(sqlite3_stmt* stmt, int index, bool v)
			{
				auto r{ sqlite3_bind_int(stmt, index, static_cast<int>(v)) };

				return r == SQLITE_OK;
			}

			inline static auto ToConcrete(bool b)
			{
				return b;
			}
		};

		template <>
		class SqlType<std::filesystem::path>
		{
		public:
			inline static bool ReadRowInto(sqlite3_stmt* stmt, int index, std::filesystem::path& v)
			{
				if (auto type{ sqlite3_column_type(stmt, index) }; type == SQLITE_TEXT)
				{
					auto ptr{ reinterpret_cast<char8_t const*>(sqlite3_column_text(stmt, index)) };
					auto length{ static_cast<std::size_t>(sqlite3_column_bytes(stmt, index)) / sizeof(std::u8string::value_type) };

					if (ptr)
					{
						v = std::u8string_view{ ptr, length };
					}
					else
					{
						v.clear();
					}

					return true;
				}
				else if (type == SQLITE_NULL)
				{
					return false;
				}
				else
				{
					return false;
				}
			}

			inline static bool BindImpl(sqlite3_stmt* stmt, int index, std::filesystem::path v)
			{
				auto str{ v.u8string() };
				auto r{ sqlite3_bind_text(stmt, index, reinterpret_cast<char const*>(str.c_str()), static_cast<int>(std::size(str)) * sizeof(typename decltype(str)::value_type), SQLITE_TRANSIENT)};

				return r == SQLITE_OK;
			}

			inline static auto ToConcrete(std::filesystem::path const& v)
			{
				return v;
			}
		};

		template <typename T>
		class SqlType<std::optional<T>>
		{
		public:
			template <typename T>
			inline static bool ReadRowInto(sqlite3_stmt* stmt, int index, std::optional<T>& v)
			{
				if (auto type{ sqlite3_column_type(stmt, index) }; type == SQLITE_NULL)
				{
					v = std::nullopt;

					return true;
				}
				else
				{
					v.emplace();

					return SqlType<T>::ReadRowInto(stmt, index, *v);
				}
			}

			template <typename T>
			inline static bool BindImpl(sqlite3_stmt* stmt, int index, std::optional<T> const& v)
			{
				if (v.has_value())
				{
					return SqlType<T>::BindImpl(stmt, index, *v);
				}
				else
				{
					auto r{ sqlite3_bind_null(stmt, index) };

					return r == SQLITE_OK;
				}
			}

			template <typename T>
			inline static auto ToConcrete(std::optional<T> const& o) -> std::optional<decltype(SqlType<T>::ToConcrete(std::declval<T>()))>
			{
				if (!o)
				{
					return std::nullopt;
				}
				else
				{
					return SqlType<T>::ToConcrete(*o);
				}
			}

			template <typename T>
			inline static auto ToConcrete(std::optional<T>&& o) -> std::optional<decltype(SqlType<T>::ToConcrete(std::declval<T>()))>
			{
				if (!o)
				{
					return std::nullopt;
				}
				else
				{
					return SqlType<T>::ToConcrete(std::move(*o));
				}
			}
		};

		template <FixedString fs, typename T>
		class NameAndType
		{
		public:
			using value_type = T;

			constexpr auto Name() const
			{
				return fs;
			}

			constexpr auto operator()() const
			{
				return fs;
			}

			constexpr auto Decl() const
			{
				using namespace NDataStructure::Literals;

				return "?/*:"_fs + fs + ":" + TypeToString<T>::To() + "*/";
			}

			constexpr auto Column() const
			{
				using namespace NDataStructure::Literals;

				return fs + "/*:" + TypeToString<T>::To() + "*/";
			}
		};

		template <FixedString fs, typename T>
		class NameAndType<fs, std::optional<T>>
		{
		public:
			using value_type = std::optional<T>;

			constexpr auto Name() const
			{
				return fs;
			}

			constexpr auto Decl() const
			{
				using namespace NDataStructure::Literals;

				return "?/*:"_fs + fs + ":" + TypeToString<T>::To() + "?*/";
			}

			constexpr auto Column() const
			{
				using namespace NDataStructure::Literals;

				return fs + "/*:" + TypeToString<T>::To() + "?*/";
			}
		};

		template <typename T, typename... GoodValues>
		void CheckSqliteReturn(T r, GoodValues... good_values)
		{
			auto success{ false };

			if constexpr (sizeof...(GoodValues) == 0)
			{
				success = r == SQLITE_OK;
			}
			else
			{
				success = ((r == good_values) || ...);
			}

			if (!success)
			{
				std::string error_message{ sqlite3_errstr(r) };

				assert(success);

				std::cerr << "SQLITE ERROR " << r << " " << error_message;

				throw std::runtime_error{ "sqlite error: " + error_message };
			}
		}

		template <typename RowType>
		auto ReadRow(sqlite3_stmt* stmt)
		{
			RowType row{};
			std::size_t count{ static_cast<std::size_t>(sqlite3_column_count(stmt)) };
			auto length{ std::size(row) };

			assert(length == count);

			if (length != count)
			{
				throw std::runtime_error{ "sqlite error: mismatch between read_row and sql columns" };
			}

			auto index{ 0 };

			row.ForEach([&](auto& m) mutable {
				SqlType<std::remove_cvref_t<decltype(m.Value())>>::ReadRowInto(stmt, index, m.Value());
				++index;
			});

			return row;
		}

		template <typename RowType>
		struct RowRange
		{
			RowType row;
			int last_result{};
			sqlite3_stmt* stmt;

			RowRange(sqlite3_stmt* stmt)
				: stmt{ stmt }
			{
				Next();
			}

			struct EndType
			{
				// Nothing
			};

			EndType end()
			{
				return {};
			}

			void Next()
			{
				last_result = sqlite3_step(stmt);

				CheckSqliteReturn(last_result, SQLITE_DONE, SQLITE_ROW);
			}

			struct RowIterator
			{
				RowRange* p;

				RowIterator& operator++()
				{
					p->Next();

					return *this;
				}

				bool operator!=(EndType)
				{
					return p->last_result == SQLITE_ROW;
				}

				bool operator==(EndType)
				{
					return p->last_result == SQLITE_ROW;
				}

				RowType& operator*()
				{
					p->row = ReadRow<RowType>(p->stmt);

					return p->row;
				}
			};

			bool HasError() const
			{
				return last_result != SQLITE_ROW && last_result != SQLITE_DONE;
			}

			RowIterator begin()
			{
				return { this };
			}
		};

		template <auto... Tags, typename... Ts, auto... Init>
		auto ToConcrete(NDataStructure::TaggedTuple<NDataStructure::Member<Tags, Ts, Init>...> const& t)
		{
			return NDataStructure::TaggedTuple{ (NDataStructure::tag<Tags> 
				= SqlType<std::remove_cvref_t<decltype(NDataStructure::Get<Tags>(t))>>::ToConcrete(NDataStructure::Get<Tags>(t)))...};
		}

		template <auto... Tags, typename... Ts, auto... Init>
		auto ToConcrete(NDataStructure::TaggedTuple<NDataStructure::Member<Tags, Ts, Init>...>&& t)
		{
			return NDataStructure::TaggedTuple{ (NDataStructure::tag<Tags> 
				= SqlType<std::remove_cvref_t<decltype(NDataStructure::Get<Tags>(std::move(t)))>>::ToConcrete(NDataStructure::Get<Tags>(std::move(t))))... };
		}	

		template <bool make_optional, typename Tag, typename T, auto Init>
		constexpr auto MaybeMakeOptional(NDataStructure::InternalTaggedTuple::MemberImpl<Tag, T, Init> m)
		{
			if constexpr (make_optional)
			{
				return NDataStructure::InternalTaggedTuple::MemberImpl<Tag, std::optional<T>, Init>{ std::nullopt };
			}
			else
			{
				return m;
			}
		}

		constexpr std::string_view start_group{ "{{" };
		constexpr std::string_view end_group{ "}}" };

		constexpr std::string_view delimiters{ ", ();" };
		constexpr std::string_view quotes{ "\"\'" };

		struct TypeSpecsCount
		{
			std::size_t fields;
			std::size_t params;

			constexpr auto operator<=>(TypeSpecsCount const&) const = default;
		};

		inline constexpr std::string_view start_comment{ "/*:" };
		inline constexpr std::string_view end_comment{ "*/" };

		template <FixedString query_string>
		constexpr auto GetTypeSpecCount()
		{
			constexpr auto sv{ query_string.ToStringView() };
			TypeSpecsCount count{};
			std::string_view str{ sv };

			while (!std::empty(str))
			{
				auto pos{ str.find(start_comment) };

				if (pos == str.npos)
				{
					break;
				}

				pos += std::size(start_comment);
				str = str.substr(pos);
				pos = str.find(end_comment);

				if (pos == str.npos)
				{
					break;
				}

				if (auto comment{ str.substr(0, pos) }; comment.find(":") == comment.npos)
				{
					++count.fields;
				}
				else
				{
					++count.params;
				}
			}

			return count;
		}

		template <typename First, typename Second>
		struct Pair
		{
			First first;
			Second second;

			constexpr auto operator<=>(Pair const&) const = default;
		};

		struct TypeSpec
		{
			Pair<std::size_t, std::size_t> name;
			Pair<std::size_t, std::size_t> type;
			bool optional;

			constexpr auto operator<=>(TypeSpec const&) const = default;
		};

		template <std::size_t N>
		struct TypeSpecs
		{
			constexpr auto operator<=>(TypeSpecs const&) const = default;
			TypeSpec data[N];

			static constexpr std::size_t size()
			{
				return N;
			}

			static constexpr bool empty()
			{
				return false;
			}

			constexpr auto const& operator[](std::size_t i) const
			{
				return data[i];
			}

			constexpr auto& operator[](std::size_t i)
			{
				return data[i];
			}
		};

		template <std::size_t Fields, std::size_t Params>
		struct CombinedTypeSpecs
		{
			TypeSpecs<Fields> fields;
			TypeSpecs<Params> params;
			
			auto operator<=>(CombinedTypeSpecs const&) const = default;
		};

		template <>
		struct TypeSpecs<0>
		{
			auto operator<=>(TypeSpecs const&) const = default;
			static constexpr std::size_t size()
			{
				return 0;
			}

			static constexpr bool empty()
			{
				return true;
			}
		};

		template <FixedString query_string>
		constexpr auto ParseTypeSpecs()
		{
			constexpr auto sv{ query_string.ToStringView() };
			constexpr auto ret_counts{ GetTypeSpecCount<query_string>() };
			CombinedTypeSpecs<ret_counts.fields, ret_counts.params> ret{};
			TypeSpecsCount counts{};
			auto in_range_inclusive{ [](char c, char b, char e) {
				return b <= c && c <= e;
			} };
			auto is_name{ [in_range_inclusive](char c) {
				return in_range_inclusive(c, 'A', 'Z') || in_range_inclusive(c, 'a', 'z') || in_range_inclusive(c, '0', '9') || c == '_' || c == '.';
			} };
			auto is_space{ [](char c) {
				return c == ' ' || c == '\n' || c == '\r' || c == '\t';
			} };
			auto const str{ sv };
			std::size_t offset{};

			while (offset != str.npos && offset < std::size(str))
			{
				auto pos{ str.find(start_comment, offset) };

				offset = pos + std::size(start_comment);
				
				if (pos == str.npos)
				{
					break;
				}

				auto end_pos{ str.find(end_comment, offset) };
				auto comment_begin{ pos + std::size(start_comment) };
				auto comment_end{ end_pos };
				auto comment{ str.substr(comment_begin, comment_end - comment_begin) };
				auto colon_pos{ comment.find(":") };

				if constexpr (ret_counts.fields > 0)
				{
					if (colon_pos == comment.npos)
					{
						auto prev_name_end{ static_cast<int>(pos + 1) };
						auto prev_name_begin{ 0 };

						// Todo: ���� �����ҵ�?
						for (auto rpos{ static_cast<int>(pos - 1) }; rpos > -1; --rpos)
						{
							auto c{ str[rpos] };

							if (is_name(c))
							{
								if (prev_name_end == pos + 1)
								{
									prev_name_end = rpos + 1;
								}
							}
							else
							{
								if (prev_name_end == pos + 1)
								{
									continue;
								}

								prev_name_begin = rpos + 1;

								break;
							}
						}

						TypeSpec& spec{ ret.fields[counts.fields] };

						spec.name.first = prev_name_begin;
						spec.name.second = prev_name_end - prev_name_begin;
						spec.type.first = comment_begin;
						spec.type.second = comment_end - comment_begin;
						auto optional{ false };
						
						if (auto type{ str.substr(comment_begin, comment_end - comment_begin) }; !std::empty(type) && type.ends_with("?"))
						{
							optional = true;
							--spec.type.second;
						}

						spec.optional = optional;

						auto name{ str.substr(spec.name.first, spec.name.second - spec.name.first) };

						++counts.fields;
					}
				}

				if constexpr (ret_counts.params > 0)
				{
					if (colon_pos != comment.npos)
					{
						auto name{ comment.substr(0, colon_pos) };
						auto type{ comment.substr(colon_pos + 1) };
						auto optional{ false };

						if (!std::empty(type) && type.ends_with("?"))
						{
							optional = true;
							type.remove_suffix(1);
						}

						TypeSpec& spec{ ret.params[counts.params] };

						spec.name.first = comment_begin;
						spec.name.second = std::size(name);
						spec.type.first = comment_begin + colon_pos + 1;
						spec.type.second = std::size(type);
						spec.optional = optional;
						++counts.params;
					}
				}
			}

			return ret;
		}

		template <FixedString query_string, TypeSpec ts>
		constexpr auto MakeMemberTypeSpec()
		{
			constexpr auto sv{ query_string.ToStringView() };
			constexpr FixedString<ts.name.second> name{ sv.substr(ts.name.first, ts.name.second) };
			constexpr FixedString<ts.type.second> type{ sv.substr(ts.type.first, ts.type.second) };

			return MaybeMakeOptional<ts.optional>(NDataStructure::tag<name> = (StringToType_t<type>{}));
		}

		template <FixedString query_string, TypeSpecs ts, std::size_t... Is>
		constexpr auto MakeMembersHelper(std::index_sequence<Is...>)
		{
			return NDataStructure::TaggedTuple{ MakeMemberTypeSpec<query_string, ts[Is]>()... };
		}

		template <FixedString query_string>
		constexpr auto MakeMembers()
		{
			constexpr auto ts{ ParseTypeSpecs<query_string>() };
			constexpr auto fields{ ts.fields };

			if constexpr (std::empty(fields))
			{
				return NDataStructure::TaggedTuple<>{};
			}
			else
			{
				return MakeMembersHelper<query_string, fields>(std::make_index_sequence<std::size(fields)>{});
			}
		}

		template <FixedString query_string>
		constexpr auto MakeParameters()
		{
			constexpr auto ts{ ParseTypeSpecs<query_string>() };
			constexpr auto params{ ts.params };

			return MakeMembersHelper<query_string, params>(std::make_index_sequence<std::size(params)>{});
		}

		template <typename T>
		std::true_type IsOptional(std::optional<T> const&);	// has no body

		std::false_type IsOptional(...);	// has no body

		template <typename PTuple>
		void DoBinding(sqlite3_stmt* stmt, PTuple p_tuple)
		{
			auto index{ 1 };

			p_tuple.ForEach([&](auto& m) mutable {
				using m_t = std::decay_t<decltype(m)>;
				using Tag = typename m_t::TagType;
				auto r{ SqlType<std::remove_cvref_t<decltype(m.Value())>>::BindImpl(stmt, index, m.Value()) };

				CheckSqliteReturn<bool>(r, true);

				++index;
			});
		}

		struct StmtCloser
		{
			void operator()(sqlite3_stmt* s)
			{
				if (s)
				{
					sqlite3_finalize(s);
				}
			}
		};

		using UniqueStmt = std::unique_ptr<sqlite3_stmt, StmtCloser>;

		template <FixedString query_string>
		class PreparedStatement
		{
			using RowType = decltype(MakeMembers<query_string>());
			using PTuple = decltype(MakeParameters<query_string>());

		public:
			PreparedStatement(sqlite3* sqldb)
			{
				auto sv{ query_string.ToStringView() };
				sqlite3_stmt* stmt;
				auto specs{ ParseTypeSpecs<query_string>() };
				auto rc{ sqlite3_prepare_v2(sqldb, std::data(sv), static_cast<int>(std::size(sv)), &stmt, 0) };

				CheckSqliteReturn(rc);
				this->stmt.reset(stmt);
			}

			RowRange<RowType> ExecuteRows() requires(std::empty(PTuple))
			{
				ResetStmt();

				return RowRange<RowType>(stmt.get());
			}

			RowRange<RowType> ExecuteRows(PTuple p_tuple)
			{
				ResetStmt();
				DoBinding(stmt.get(), std::move(p_tuple));

				return RowRange<RowType>(stmt.get());
			}

			std::optional<decltype(ToConcrete(std::declval<RowType>()))> ExecuteSingleRow(PTuple p_tuple)
			{
				auto rng{ ExecuteRows(std::move(p_tuple)) };
				
				if (auto begin{ std::begin(rng) }; begin != std::end(rng))
				{
					return ToConcrete(*begin);
				}
				else
				{
					return std::nullopt;
				}
			}

			std::optional<decltype(ToConcrete(std::declval<RowType>()))> ExecuteSingleRow()
				requires (PTuple::empty())
			{
				auto rng{ ExecuteRows() };
				
				if (auto begin{ std::begin(rng) }; begin != std::end(rng))
				{
					return ToConcrete(*begin);
				}
				else
				{
					return std::nullopt;
				}
			}

			void Execute(PTuple p_tuple) 
			{
				ResetStmt();
				DoBinding(stmt.get(), std::move(p_tuple));
				
				auto r{ sqlite3_step(stmt.get()) };

				CheckSqliteReturn(r, SQLITE_DONE);
			}

			void Execute()
				requires (PTuple::empty())
			{
				ResetStmt();
				
				auto r{ sqlite3_step(stmt.get()) };

				CheckSqliteReturn(r, SQLITE_DONE);
			}

		private:
			UniqueStmt stmt;

			void ResetStmt()
			{
				auto r{ sqlite3_reset(stmt.get()) };

				CheckSqliteReturn(r);
				r = sqlite3_clear_bindings(stmt.get());
				CheckSqliteReturn(r);
			}
		};

		template <FixedString fs, typename T>
		decltype(auto) Field(T&& t)
		{
			return NDataStructure::Get<fs>(std::forward<T>(t));
		}

		// NAT = NameAndType
		template <auto NAT, typename T>
			requires requires { NAT.Name(); }
		decltype(auto) Field(T&& t)
		{
			return NDataStructure::Get<NAT.Name()>(std::forward<T>(t));
		}

		template <FixedString fs, typename T>
		auto Bind(T&& t)
		{
			return NDataStructure::tag<fs> = std::forward<T>(t);
		}

		// NAT = NameAndType
		template <auto NAT, typename T>
			requires requires { NAT.Name(); }
		auto Bind(T&& t)
		{
			return NDataStructure::tag<NAT.Name()> = std::forward<T>(t);
		}

		//template <typename Tag>
		//struct ParamHelper
		//{
		//	template <typename T>
		//	auto operator=(T t)
		//	{
		//		return MakeMember<Tag, T>(std::move(t));
		//	}
		//};
	}

	using Sqlite3::Bind;
	using Sqlite3::Field;
	using Sqlite3::PreparedStatement;
	using Sqlite3::ToConcrete;
}