#include "TaggedSqlite.h"
#include <iostream>
#include <chrono>
#include <format>
#include <string>
#include <string_view>
#include <filesystem>
#include <optional>
#include <sstream>
#include <iomanip>

using namespace std::string_literals;
using namespace std::string_view_literals;
using namespace std::chrono_literals;
using namespace NDataStructure::Literals;
using NDatabase::SQLite3Manager;
using Literals::operator""_fs;

class AccountManager final
{
public:
    static inline constexpr auto accounts{ "accounts"_fs };
    static inline constexpr NDatabase::NameAndType<"row", std::int64_t, "INTEGER NOT NULL PRIMARY KEY"> row;
    static inline constexpr NDatabase::NameAndType<"unique_key", std::u8string, "TEXT NOT NULL"> unique_key;
    static inline constexpr NDatabase::NameAndType<"type", int, "INTEGER NOT NULL"> type;
    static inline constexpr NDatabase::NameAndType<"login_id", std::u8string, "TEXT NOT NULL"> login_id;
    static inline constexpr NDatabase::NameAndType<"name", std::u8string, "TEXT NOT NULL"> name;
    static inline constexpr NDatabase::NameAndType<"email", std::optional<std::u8string>, "TEXT"> email;
    static inline constexpr NDatabase::NameAndType<"profile", std::optional<std::u8string>, "TEXT"> profile;
    static inline constexpr NDatabase::NameAndType<"picture", std::optional<std::u8string>, "TEXT"> picture;
    static inline constexpr NDatabase::NameAndType<"refresh_token", std::optional<std::u8string>, "TEXT"> refresh_token;
    static inline constexpr NDatabase::NameAndType<"id_token", std::optional<std::u8string>, "TEXT"> id_token;
    static inline constexpr NDatabase::NameAndType<"access_token", std::optional<std::u8string>, "TEXT"> access_token;
    static inline constexpr NDatabase::NameAndType<"drive_letter", std::u8string, "TEXT NOT NULL"> drive_letter;
    static inline constexpr NDatabase::NameAndType<"drive_name", std::optional<std::u8string>, "TEXT"> drive_name;
    static inline constexpr NDatabase::NameAndType<"auto_login", bool, "INTEGER NOT NULL"> auto_login;
    static inline constexpr NDatabase::NameAndType<"last_login_time", std::optional<std::chrono::system_clock::time_point>, "TEXT"> last_login_time;
    static inline constexpr NDatabase::NameAndType<"reserved", std::optional<std::u8string>, "TEXT"> reserved;
    static inline constexpr NDatabase::NameAndType<"reserved2", std::optional<std::u16string>, "TEXT"> reserved2;
    static inline constexpr NDatabase::NameAndType<"profile_picture", std::optional<std::vector<unsigned char>>, "BLOB"> profile_picture;

    explicit AccountManager(std::filesystem::path const& path_name)
        : sql{ path_name }
    {
        
    }

    ~AccountManager() = default;

    int Version() const
    {
        return sql.Version();
    }

    template <int version>
    void Version() const
    {
        sql.Version<version>();
    }

    bool ExistTable() const
    {
        return sql.ExistTable<accounts>();
    }

    void Create() const
    {
        sql.Create<accounts,
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
            reserved2,
            profile_picture
        >();
    }

    void Drop() const
    {
        sql.Drop<accounts>();
    }

    auto PreparedInsert() const
    {
        return sql.PreparedInsert<accounts,
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
            reserved2,
            profile_picture
        >();
    }

    auto PreparedSelect() const
    {
        return sql.PreparedSelect<accounts,
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
            reserved2,
            profile_picture
        >();
    }

    template <auto NAT, auto... NATS>
    constexpr auto PreparedSelect()
    {
        return sql.PreparedSelectWithWhere<
            accounts,
            SQLite3Manager::MakeColumnList(
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
                reserved2,
                profile_picture
            ),
            ((NAT.Name() + " = " + NAT.Decl()) + ... + (" AND " + NATS.Name() + " = " + NATS.Decl()))
        >();
    }

    auto PreparedDelete() const
    {
        return sql.PreparedDeleteWithWhere<accounts,
            unique_key.Name() + " = " + unique_key.Decl()
        >();
    }

    auto PreparedInsertOrReplace() const
    {
        return sql.PreparedInsertOrReplace<accounts,
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
            reserved2,
            profile_picture
        >();
    }

private:
    SQLite3Manager sql;
};

int main()
{
    setlocale(LC_ALL, "");

    AccountManager account_manager{ "account.db" };

    std::cout << std::format("Current version: {}\n", account_manager.Version());
    account_manager.Version<38>();
    std::cout << std::format("Update version: {}\n", account_manager.Version());

    std::vector<unsigned char> v{ 'a', 'b', 'c', 'd' };

    account_manager.Drop();
    account_manager.Create();
    account_manager.PreparedInsert().Execute({
        NDatabase::Bind<AccountManager::row>(0),
        NDatabase::Bind<AccountManager::unique_key>(u8"1"), 
        NDatabase::Bind<AccountManager::type>(2),
        NDatabase::Bind<AccountManager::login_id>(u8"3"),
        NDatabase::Bind<AccountManager::name>(u8"4"),
        NDatabase::Bind<AccountManager::email>(u8"5"),
        NDatabase::Bind<AccountManager::profile>(u8"6"),
        NDatabase::Bind<AccountManager::picture>(u8"7"),
        NDatabase::Bind<AccountManager::refresh_token>(u8"8"),
        NDatabase::Bind<AccountManager::id_token>(u8"9"),
        NDatabase::Bind<AccountManager::access_token>(u8"10"),
        NDatabase::Bind<AccountManager::drive_letter>(u8"C:\\할로"),
        NDatabase::Bind<AccountManager::drive_name>(u8"헬로우"),
        NDatabase::Bind<AccountManager::auto_login>(true),
        NDatabase::Bind<AccountManager::last_login_time>(std::chrono::system_clock::now()),
        NDatabase::Bind<AccountManager::reserved>(u8"유니코드8"),
        NDatabase::Bind<AccountManager::reserved2>(u"유니코드16"),
        NDatabase::Bind<AccountManager::profile_picture>(v)
    });
    account_manager.PreparedInsert().Execute({
        NDatabase::Bind<AccountManager::row>(1),
        NDatabase::Bind<AccountManager::unique_key>(u8"2"),
        NDatabase::Bind<AccountManager::type>(3),
        NDatabase::Bind<AccountManager::login_id>(u8"4"),
        NDatabase::Bind<AccountManager::name>(u8"5"),
        NDatabase::Bind<AccountManager::email>(u8"6"),
        NDatabase::Bind<AccountManager::profile>(u8"7"),
        NDatabase::Bind<AccountManager::picture>(u8"8"),
        NDatabase::Bind<AccountManager::refresh_token>(u8"9"),
        NDatabase::Bind<AccountManager::id_token>(u8"10"),
        NDatabase::Bind<AccountManager::access_token>(u8"11"),
        NDatabase::Bind<AccountManager::drive_letter>(u8"C:\\발로"),
        NDatabase::Bind<AccountManager::drive_name>(u8"발로우"),
        NDatabase::Bind<AccountManager::auto_login>(false),
        NDatabase::Bind<AccountManager::last_login_time>(std::chrono::system_clock::now() - 1h),
        NDatabase::Bind<AccountManager::reserved>(u8"UTF8"),
        NDatabase::Bind<AccountManager::reserved2>(u"UTF16"),
        NDatabase::Bind<AccountManager::profile_picture>(std::vector<unsigned char>{ 'e', 'f', 'g', 'h'})
    });
    account_manager.PreparedInsert().Execute({
        NDatabase::Bind<AccountManager::row>(2),
        NDatabase::Bind<AccountManager::unique_key>(u8"3"),
        NDatabase::Bind<AccountManager::type>(4),
        NDatabase::Bind<AccountManager::login_id>(u8"5"),
        NDatabase::Bind<AccountManager::name>(u8"6"),
        NDatabase::Bind<AccountManager::email>(u8"7"),
        NDatabase::Bind<AccountManager::profile>(u8"8"),
        NDatabase::Bind<AccountManager::picture>(u8"9"),
        NDatabase::Bind<AccountManager::refresh_token>(u8"10"),
        NDatabase::Bind<AccountManager::id_token>(u8"11"),
        NDatabase::Bind<AccountManager::access_token>(u8"12"),
        NDatabase::Bind<AccountManager::drive_letter>(u8"C:\\탈로"),
        NDatabase::Bind<AccountManager::drive_name>(u8"탈로우"),
        NDatabase::Bind<AccountManager::auto_login>(true),
        NDatabase::Bind<AccountManager::last_login_time>(std::chrono::system_clock::now() - 24h),
        NDatabase::Bind<AccountManager::reserved>(u8"유티에프8"),
        NDatabase::Bind<AccountManager::reserved2>(u"유티에프16"),
        NDatabase::Bind<AccountManager::profile_picture>(std::vector<unsigned char>{ 'i', 'j', 'k', 'l'})
    });

    auto select{ account_manager.PreparedSelect<
        AccountManager::auto_login
    >() };

    for (auto& e : select.ExecuteRows({
        NDatabase::Bind<AccountManager::auto_login>(true)
        }))
    {
        SQLite3Manager::Print<AccountManager::row>(e);
        SQLite3Manager::Print<AccountManager::unique_key>(e);
        SQLite3Manager::Print<AccountManager::type>(e);
        SQLite3Manager::Print<AccountManager::login_id>(e);
        SQLite3Manager::Print<AccountManager::name>(e);
        SQLite3Manager::Print<AccountManager::email>(e);
        SQLite3Manager::Print<AccountManager::profile>(e);
        SQLite3Manager::Print<AccountManager::picture>(e);
        SQLite3Manager::Print<AccountManager::refresh_token>(e);
        SQLite3Manager::Print<AccountManager::id_token>(e);
        SQLite3Manager::Print<AccountManager::access_token>(e);
        SQLite3Manager::Print<AccountManager::drive_letter>(e);
        SQLite3Manager::Print<AccountManager::drive_name>(e);
        SQLite3Manager::Print<AccountManager::auto_login>(e);
        SQLite3Manager::Print<AccountManager::last_login_time>(e);
        SQLite3Manager::Print<AccountManager::reserved>(e);
        SQLite3Manager::Print<AccountManager::reserved2>(e);
        SQLite3Manager::Print<AccountManager::profile_picture>(e);
    }

    account_manager.PreparedInsertOrReplace().Execute({
        NDatabase::Bind<AccountManager::row>(2),
        NDatabase::Bind<AccountManager::unique_key>(u8"3"),
        NDatabase::Bind<AccountManager::type>(18),
        NDatabase::Bind<AccountManager::login_id>(u8"5"),
        NDatabase::Bind<AccountManager::name>(u8"6"),
        NDatabase::Bind<AccountManager::email>(u8"7"),
        NDatabase::Bind<AccountManager::profile>(u8"8"),
        NDatabase::Bind<AccountManager::picture>(u8"9"),
        NDatabase::Bind<AccountManager::refresh_token>(u8"10"),
        NDatabase::Bind<AccountManager::id_token>(u8"11"),
        NDatabase::Bind<AccountManager::access_token>(u8"12"),
        NDatabase::Bind<AccountManager::drive_letter>(u8"C:\\탈로"),
        NDatabase::Bind<AccountManager::drive_name>(u8"탈로우"),
        NDatabase::Bind<AccountManager::auto_login>(true),
        NDatabase::Bind<AccountManager::last_login_time>(std::chrono::system_clock::now() - 24h),
        NDatabase::Bind<AccountManager::reserved>(u8"유티에프8"),
        NDatabase::Bind<AccountManager::reserved2>(u"유티에프16"),
        NDatabase::Bind<AccountManager::profile_picture>(std::vector<unsigned char>{ 'i', 'j', 'k', 'l'})
    });

    account_manager.PreparedDelete().Execute({
        NDatabase::Bind<AccountManager::unique_key>(u8"1")
    });

    //// Note: 직접 사용
    //SQLite3Manager sql{ "account.db" };

    //auto select_all{ sql.PrepareStatement<
    //    "SELECT reserved/*:text*/ FROM accounts"
    //>() };

    //for (auto& e : select_all.ExecuteRows({}))
    //{
    //    sql.Print<AccountManager::reserved>(e);
    //}

    //if (auto opt{ account_manager.PrepareSelect_GetRow().ExecuteSingleRow({
    //    NDatabase::Bind<unique_key>(u8"1")
    //}) })
    //{
    //    auto row_value{ NDatabase::Field<row>(*opt) };

    //    std::cout << row_value << std::endl;
    //    std::cout << typeid(opt).name() << std::endl;

    //    //auto unique_key_value{ NDatabase::Field<unique_key>(*opt) };

    //    //auto type_value{ NDatabase::Field<type>(*opt) };

    //    //auto reserved_value{ NDatabase::Field<reserved>(*opt) };

    //    //auto profile_picture_value{ NDatabase::Field<profile_picture>(*opt) };

    //    //std::ranges::copy(profile_picture_value, std::ostream_iterator<unsigned char>(std::cout, "\n"));
    //}
    //else
    //{
    //    std::cout << "Not exist!\n";
    //}

   
}
