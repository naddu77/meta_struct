#include "TaggedSqlite.h"
#include <iostream>
#include <chrono>
#include <format>
#include <string>
#include <string_view>
#include <filesystem>
#include <optional>

using namespace std::string_literals;
using namespace std::string_view_literals;
using namespace std::chrono_literals;
using namespace NDataStructure::Literals;

template <typename T, typename... Ts>
constexpr auto MakeNameList(T const& t, Ts const&... ts)
{
    return (t.Name() + ... + (", "_fs + MakeNameList(ts)));
}

template <typename T, typename... Ts>
constexpr auto MakeColumnList(T const& t, Ts const&... ts)
{
    return (t.Column() + ... + (", "_fs + MakeColumnList(ts)));
}

template <typename T, typename... Ts>
constexpr auto MakeDeclList(T const& t, Ts const&... ts)
{
    return (t.Decl() + ... + (", "_fs + MakeDeclList(ts)));
}

template <typename T>
inline constexpr bool IsStringType_v = std::is_constructible_v<std::string, T>
|| std::is_constructible_v<std::wstring, T>
|| std::is_constructible_v<std::u8string, T>
|| std::is_constructible_v<std::u16string, T>
|| std::is_constructible_v<std::u32string, T>;

template <typename T, typename From>
auto To(From const& v)
{
    if constexpr (requires { v.Name().ToStringView(); })
    {
        return std::filesystem::path{ v.Name().ToStringView() }.wstring();
    }
    else if constexpr (IsStringType_v<From>)
    {
        if constexpr (std::is_same_v<T, std::string>)
        {
            return std::filesystem::path{ v }.string();
        }
        else if constexpr (std::is_same_v<T, std::wstring>)
        {
            return std::filesystem::path{ v }.wstring();
        }
        else if constexpr (std::is_same_v<T, std::u8string>)
        {
            return std::filesystem::path{ v }.u8string();
        }
        else if constexpr (std::is_same_v<T, std::u16string>)
        {
            return std::filesystem::path{ v }.u16string();
        }
        else if constexpr (std::is_same_v<T, std::u32string>)
        {
            return std::filesystem::path{ v }.u32string();
        }
    }
    else if constexpr (requires { *v; })
    {
        if (v)
        {
            return To<T>(*v);
        }
        else
        {
            return To<T>(*v);
        }
    }
    else if constexpr (std::is_same_v<From, std::chrono::system_clock::time_point>)
    {
        return v;
    }
    else
    {
        return v;
    }
}

template <typename T, typename V>
constexpr void Print(T const& t, V const& v)
{
    std::wcout << std::format(L"{}: {}\n", To<std::wstring>(t), To<std::wstring>(v));
}

constexpr NDatabase::Sqlite3::NameAndType<"row", std::int64_t> row;
constexpr NDatabase::Sqlite3::NameAndType<"unique_key", std::u8string> unique_key;
constexpr NDatabase::Sqlite3::NameAndType<"type", int> type;
constexpr NDatabase::Sqlite3::NameAndType<"login_id", std::u8string> login_id;
constexpr NDatabase::Sqlite3::NameAndType<"name", std::u8string> name;
constexpr NDatabase::Sqlite3::NameAndType<"email", std::optional<std::u8string>> email;
constexpr NDatabase::Sqlite3::NameAndType<"profile", std::optional<std::u8string>> profile;
constexpr NDatabase::Sqlite3::NameAndType<"picture", std::optional<std::u8string>> picture;
constexpr NDatabase::Sqlite3::NameAndType<"refresh_token", std::optional<std::u8string>> refresh_token;
constexpr NDatabase::Sqlite3::NameAndType<"id_token", std::optional<std::u8string>> id_token;
constexpr NDatabase::Sqlite3::NameAndType<"access_token", std::optional<std::u8string>> access_token;
constexpr NDatabase::Sqlite3::NameAndType<"drive_letter", std::u8string> drive_letter;
constexpr NDatabase::Sqlite3::NameAndType<"drive_name", std::optional<std::u8string>> drive_name;
constexpr NDatabase::Sqlite3::NameAndType<"auto_login", bool> auto_login;
constexpr NDatabase::Sqlite3::NameAndType<"last_login_time", std::optional<std::chrono::system_clock::time_point>> last_login_time;
constexpr NDatabase::Sqlite3::NameAndType<"reserved", std::optional<std::u8string>> reserved;
constexpr NDatabase::Sqlite3::NameAndType<"reserved2", std::optional<std::u8string>> reserved2;

class AccountManager final
{
public:
    explicit AccountManager(std::string const& path_name)
    {
        assert(sqlite3_open(path_name.c_str(), &db) == 0);
    }

    ~AccountManager() = default;

    auto Select() 
    {
        return NDatabase::PreparedStatement<
            "SELECT "_fs
            + MakeColumnList(
                row,
                unique_key,
                type,
                login_id,
                name,
                email,
                profile,
                picture,
                refresh_token,
                id_token,
                access_token,
                drive_letter,
                drive_name,
                auto_login,
                last_login_time,
                reserved,
                reserved2
            )
            + " FROM accounts;"
        >{ db };
    }

private:
    sqlite3* db{ nullptr };
};

int main()
{
    setlocale(LC_ALL, "");

    AccountManager account_manager{ "CloudBaseAccount.db" };

    auto select_all{ account_manager.Select() };

    //sqlite3* sqldb{ nullptr };

    //sqlite3_open("CloudBaseAccount.db", &sqldb);

    //constexpr auto accounts{ "accounts"_fs };
    //constexpr NDatabase::Sqlite3::NameAndType<"row", std::int64_t> row;
    //constexpr NDatabase::Sqlite3::NameAndType<"unique_key", std::u8string> unique_key;
    //constexpr NDatabase::Sqlite3::NameAndType<"type", int> type;
    //constexpr NDatabase::Sqlite3::NameAndType<"login_id", std::u8string> login_id;
    //constexpr NDatabase::Sqlite3::NameAndType<"name", std::u8string> name;
    //constexpr NDatabase::Sqlite3::NameAndType<"email", std::optional<std::u8string>> email;
    //constexpr NDatabase::Sqlite3::NameAndType<"profile", std::optional<std::u8string>> profile;
    //constexpr NDatabase::Sqlite3::NameAndType<"picture", std::optional<std::u8string>> picture;
    //constexpr NDatabase::Sqlite3::NameAndType<"refresh_token", std::optional<std::u8string>> refresh_token;
    //constexpr NDatabase::Sqlite3::NameAndType<"id_token", std::optional<std::u8string>> id_token;
    //constexpr NDatabase::Sqlite3::NameAndType<"access_token", std::optional<std::u8string>> access_token;
    //constexpr NDatabase::Sqlite3::NameAndType<"drive_letter", std::u8string> drive_letter;
    //constexpr NDatabase::Sqlite3::NameAndType<"drive_name", std::optional<std::u8string>> drive_name;
    //constexpr NDatabase::Sqlite3::NameAndType<"auto_login", bool> auto_login;
    //constexpr NDatabase::Sqlite3::NameAndType<"last_login_time", std::optional<std::chrono::system_clock::time_point>> last_login_time;
    //constexpr NDatabase::Sqlite3::NameAndType<"reserved", std::optional<std::u8string>> reserved;
    //constexpr NDatabase::Sqlite3::NameAndType<"reserved2", std::optional<std::u8string>> reserved2;

    //NDatabase::PreparedStatement<
    //    "DROP TABLE "_fs + accounts + ";"
    //>{ sqldb }.Execute();

    //NDatabase::PreparedStatement<
    //    "CREATE TABLE "_fs + accounts + "("
    //    + row.Name() + " INTEGER NOT NULL PRIMARY KEY,"
    //    + unique_key.Name() + " TEXT NOT NULL,"
    //    + type.Name() + " INTEGER NOT NULL,"
    //    + login_id.Name() + " TEXT NOT NULL,"
    //    + name.Name() + " TEXT NOT NULL,"
    //    + email.Name() + " TEXT,"
    //    + profile.Name() + " TEXT,"
    //    + picture.Name() + " TEXT,"
    //    + refresh_token.Name() + " TEXT,"
    //    + id_token.Name() + " TEXT,"
    //    + access_token.Name() + " TEXT,"
    //    + drive_letter.Name() + " TEXT NOT NULL,"
    //    + drive_name.Name() + " TEXT,"
    //    + auto_login.Name() +" INTEGER NOT NULL,"
    //    + last_login_time.Name() + " TEXT,"
    //    + reserved.Name() + " TEXT,"
    //    + reserved2.Name() + " TEXT);"
    //>{ sqldb }.Execute();

    //NDatabase::PreparedStatement<
    //    "INSERT INTO accounts("_fs
    //    + MakeNameList(
    //        row,
    //        unique_key,
    //        type,
    //        login_id,
    //        name,
    //        email,
    //        profile,
    //        picture,
    //        refresh_token,
    //        id_token,
    //        access_token,
    //        drive_letter,
    //        drive_name,
    //        auto_login,
    //        last_login_time,
    //        reserved,
    //        reserved2
    //    )
    //    + ") VALUES("
    //    + MakeDeclList(
    //        row,
    //        unique_key,
    //        type, 
    //        login_id,
    //        name, 
    //        email,
    //        profile, 
    //        picture,
    //        refresh_token,
    //        id_token,
    //        access_token, 
    //        drive_letter, 
    //        drive_name, 
    //        auto_login,
    //        last_login_time,
    //        reserved,
    //        reserved2
    //    )
    //    + ");"
    //>{ sqldb }.Execute({
    //    NDatabase::Bind<row>(0),
    //    NDatabase::Bind<unique_key>(u8"1"), 
    //    NDatabase::Bind<type>(2),
    //    NDatabase::Bind<login_id>(u8"3"),
    //    NDatabase::Bind<name>(u8"4"),
    //    NDatabase::Bind<email>(u8"5"),
    //    NDatabase::Bind<profile>(u8"6"),
    //    NDatabase::Bind<picture>(u8"7"),
    //    NDatabase::Bind<refresh_token>(u8"8"),
    //    NDatabase::Bind<id_token>(u8"9"),
    //    NDatabase::Bind<access_token>(u8"10"),
    //    NDatabase::Bind<drive_letter>(u8"C:\\할로"),
    //    NDatabase::Bind<drive_name>(u8"헬로우"),
    //    NDatabase::Bind<auto_login>(true),
    //    NDatabase::Bind<last_login_time>(std::chrono::system_clock::now()),
    //    NDatabase::Bind<reserved>(u8"더블류 스트링"),
    //    NDatabase::Bind<reserved2>(u8"유니코드16")
    //});

    //NDatabase::PreparedStatement<
    //    "SELECT "_fs
    //    + MakeColumnList(
    //        row,
    //        unique_key,
    //        type,
    //        login_id,
    //        name,
    //        email,
    //        profile,
    //        picture,
    //        refresh_token,
    //        id_token,
    //        access_token,
    //        drive_letter,
    //        drive_name,
    //        auto_login,
    //        last_login_time,
    //        reserved,
    //        reserved2
    //    )
    //    + " FROM accounts;"
    //>select_all{ sqldb };

    for (auto& e : select_all.ExecuteRows({}))
    {
        Print(row, NDatabase::Field<row>(e));
        Print(unique_key, NDatabase::Field<unique_key>(e));
        Print(type, NDatabase::Field<type>(e));
        Print(login_id, NDatabase::Field<login_id>(e));
        Print(name, NDatabase::Field<name>(e));
        Print(email, NDatabase::Field<email>(e));
        Print(profile, NDatabase::Field<profile>(e));
        Print(picture, NDatabase::Field<picture>(e));
        Print(refresh_token, NDatabase::Field<refresh_token>(e));
        Print(id_token, NDatabase::Field<id_token>(e));
        Print(access_token, NDatabase::Field<access_token>(e));
        Print(drive_letter, NDatabase::Field<drive_letter>(e));
        Print(drive_name, NDatabase::Field<drive_name>(e));
        Print(auto_login, NDatabase::Field<auto_login>(e));
        Print(last_login_time, NDatabase::Field<last_login_time>(e));
        Print(reserved, NDatabase::Field<reserved>(e));
        Print(reserved2, NDatabase::Field<reserved2>(e));
    }
}
