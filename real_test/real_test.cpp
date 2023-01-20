#include "TaggedSqlite.h"
#include <iostream>
#include <chrono>
#include <format>
#include <string>
#include <string_view>
#include <filesystem>

using namespace std::string_literals;
using namespace std::string_view_literals;
using namespace std::chrono_literals;

int main()
{
    setlocale(LC_ALL, "");
    sqlite3* sqldb{ nullptr };
    
    sqlite3_open("account.db", &sqldb);

    NDatabase::PreparedStatement<
        "DROP TABLE accounts;"
    >{ sqldb }.Execute();

    NDatabase::PreparedStatement<
        "CREATE TABLE accounts("
        "row INTEGER NOT NULL PRIMARY KEY,"
        "unique_key TEXT NOT NULL,"
        "type INTEGER NOT NULL,"
        "login_id TEXT NOT NULL,"
        "name TEXT NOT NULL,"
        "email TEXT,"
        "profile TEXT,"
        "picture TEXT,"
        "refresh_token TEXT,"
        "id_token TEXT,"
        "access_token TEXT,"
        "drive_letter TEXT NOT NULL,"
        "drive_name TEXT,"
        "auto_login INTEGER NOT NULL,"
        "last_login_time TEXT,"
        "reserved TEXT,"
        "reserved2 TEXT);"
    >{ sqldb }.Execute();

    NDatabase::PreparedStatement<
        "INSERT INTO accounts VALUES("
        "?/*:row:integer*/,"
        "?/*:unique_key:ansi*/,"
        "?/*:type:integer*/,"
        "?/*:login_id:ansi*/,"
        "?/*:name:ansi*/,"
        "?/*:email:ansi?*/,"
        "?/*:profile:ansi?*/,"
        "?/*:picture:ansi?*/,"
        "?/*:refresh_token:ansi?*/,"
        "?/*:id_token:ansi?*/,"
        "?/*:access_token:ansi?*/,"
        "?/*:drive_letter:path*/,"
        "?/*:drive_name:text?*/,"
        "?/*:auto_login:bool*/,"
        "?/*:last_login_time:ansi?*/,"
        "?/*:reserved:ansi?*/,"
        "?/*:reserved2:utf16?*/"
        ");"
    >{ sqldb }.Execute({
        NDatabase::Bind<"row">(0),
        NDatabase::Bind<"unique_key">("1"),
        NDatabase::Bind<"type">(2),
        NDatabase::Bind<"login_id">("3"),
        NDatabase::Bind<"name">("4"),
        NDatabase::Bind<"email">("5"),
        NDatabase::Bind<"profile">("6"),
        NDatabase::Bind<"picture">("7"),
        NDatabase::Bind<"refresh_token">("8"),
        NDatabase::Bind<"id_token">("9"),
        NDatabase::Bind<"access_token">("10"),
        NDatabase::Bind<"drive_letter">("C:\\할로"),
        NDatabase::Bind<"drive_name">(u8"헬로우"),
        NDatabase::Bind<"auto_login">(13),
        NDatabase::Bind<"last_login_time">(std::format("{}", std::chrono::system_clock::now())),
        NDatabase::Bind<"reserved">("더블류 스트링"),
        NDatabase::Bind<"reserved2">(u"유니코드16")
    });

    NDatabase::PreparedStatement<
        "SELECT "
        "row/*:integer*/,"
        "unique_key/*:ansi*/,"
        "type/*:integer*/,"
        "login_id/*:ansi*/,"
        "name/*:ansi*/,"
        "email/*:ansi?*/,"
        "profile/*:ansi?*/,"
        "picture/*:ansi?*/,"
        "refresh_token/*:ansi?*/,"
        "id_token/*:ansi?*/,"
        "access_token/*:ansi?*/,"
        "drive_letter/*:path*/,"
        "drive_name/*:text?*/,"
        "auto_login/*:integer*/,"
        "last_login_time/*:date?*/,"
        "reserved/*:ansi?*/,"
        "reserved2/*:utf16?*/"
        " FROM accounts;"
    >select_all{ sqldb };

    for (auto& row : select_all.ExecuteRows({}))
    {
        std::cout << NDatabase::Field<"unique_key">(row) << std::endl;
        std::cout << NDatabase::Field<"auto_login">(row) << std::endl;
        std::cout << NDatabase::Field<"reserved">(row).value_or("NULL") << std::endl;
        std::cout << NDatabase::Field<"drive_letter">(row)  << std::endl;
        std::wcout << std::filesystem::path{ NDatabase::Field<"reserved2">(row).value_or(u"NULL") }.wstring() << std::endl;
        std::wcout << std::filesystem::path{ NDatabase::Field<"drive_name">(row).value_or(u8"NULL") }.wstring() << std::endl;
        std::cout << std::chrono::floor<std::chrono::seconds>(NDatabase::Field<"last_login_time">(row).value_or(std::chrono::system_clock::now() - 1h)) << std::endl;
    }
}
