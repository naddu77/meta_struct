#include "TaggedSqlite.h"
#include "SoaVector.h"
#include "UnitTest.h"
#include <iostream>

//using NDataStructure::Get;
//using NDataStructure::TaggedTuple;
//using namespace NDataStructure::Literals;
//using NDataStructure::Auto;
//using NDataStructure::Member;
//using NDataStructure::tag;
//
//using TestArguments = TaggedTuple<
//    Member<"a", int, []{}>,
//    Member<"b", Auto, [](auto& t) { return Get<"a">(t) + 2; }>,
//    Member<"c", Auto, [](auto& t) { return Get<"b">(t) + 2; }>,
//    Member<"d", Auto, []() { return 5; }>
//>;
//
//void Test(TestArguments args)
//{
//    std::cout << Get<"a">(args) << "\n";
//    std::cout << Get<"b">(args) << "\n";
//    std::cout << Get<"c">(args) << "\n";
//    std::cout << Get<"d">(args) << "\n";
//}
//
//template <typename T>
//void Print(T const& t)
//{
//    auto f{ [](auto&&... xs) {
//        auto print{ [](auto& m) {
//            std::cout << m.Key() << ": " << m.Value() << "\n";
//        } };
//
//        (std::invoke(print, xs), ...);
//    } };
//
//    std::cout << "{\n";
//    t.Apply(f);
//    std::cout << "}\n";
//}
//
//template <typename TaggedTuple>
//auto MakeRef(TaggedTuple& t)
//{
//    return NDataStructure::TaggedTupleRef_t<TaggedTuple>(t);
//}
//
//using NDataStructure::SoaVector;
//
//void SoaVectorExample()
//{
//    using Person = TaggedTuple<
//        Member<"name", std::string>,
//        Member<"address", std::string>,
//        Member<"id", std::int64_t>,
//        Member<"score", double>
//    >;
//
//    SoaVector<Person> v;
//
//    v.push_back(Person{ tag<"name"> = "John", tag<"address"> = "somewhere", tag<"id"> = 1, tag<"score"> = 10.5 });
//    v.push_back(Person{ tag<"name"> = "Jane", tag<"address"> = "there", tag<"id"> = 2, tag<"score"> = 12.5 });
//
//    assert(Get<"name">(v[1]) == "Jane");
//
//    auto scores{ Get<"score">(v) };
//
//    assert(*std::max_element(std::begin(scores), std::end(scores)) == 12.5);
//}

int main()
{
    // Unit Test
    UnitTest();

    //// TaggedTuple
    //SoaVectorExample();
    //TaggedTuple<
    //    Member<"hello", Auto, []{ return 5;}>,
    //    Member<"world", std::string, [](auto& self) { return Get<"hello">(self); }>,
    //    Member<"test", Auto, [](auto& t) { return 2 * Get<"hello">(t) + std::size(Get<"world">(t)); }>,
    //    Member<"last", int>
    //> ts{ tag<"world"> = "Universe", tag<"hello"> = 1};

    //auto ref_ts{ MakeRef(ts) };

    //Print(ref_ts);

    //using T = decltype(ts);
    //T t2{ tag<"world"> = "JRB" };

    //SoaVector<T> v;

    //v.push_back(t2);

    //Print(v[0]);

    //auto v0{ Get<"world">(v) };

    //v0.front() = "Changed again";

    //std::cout << Get<"world">(v).front() << '\n';

    //std::cout << Get<"hello">(ts) << "\n";
    //std::cout << Get<"world">(ts) << "\n";
    //std::cout << Get<"test">(ts) << "\n";
    //std::cout << Get<"hello">(t2) << "\n";
    //std::cout << Get<"world">(t2) << "\n";
    //std::cout << Get<"test">(t2) << "\n";

    //Test({ tag<"c"> = 1, tag<"a"> = 5 });
    //Test({ tag<"a"> = 1 });
    //Test({ "a"_tag = 1 });

    //TaggedTuple ctad{ "a"_tag = 15, "b"_tag = std::string{ "Hello ctad" } };

    //std::cout << ts["world"_tag] << "\n";
    //std::cout << ts[tag<"world">] << "\n";
    //std::cout << ctad["a"_tag] << "\n";

    //// Sqlite3
    //using NDatabase::Bind;
    //using NDatabase::Field;
    //using NDatabase::PreparedStatement;

    //sqlite3* sqldb;
    //sqlite3_open(":memory:", &sqldb);

    //PreparedStatement<
    //    "CREATE TABLE customers("
    //    "id INTEGER NOT NULL PRIMARY KEY, "
    //    "name TEXT NOT NULL"
    //    ");"  //
    //>{ sqldb }
    //.Execute();

    //PreparedStatement <
    //    "CREATE TABLE orders("
    //    "id INTEGER NOT NULL PRIMARY KEY,"
    //    "item TEXT NOT NULL, "
    //    "customerid INTEGER NOT NULL,"
    //    "price REAL NOT NULL, "
    //    "discount_code TEXT "
    //    ");"  //
    //>{ sqldb }
    //.Execute();

    //PreparedStatement<
    //    "INSERT INTO customers(name) "
    //    "VALUES(? /*:name:text*/);"  //
    //>{ sqldb }
    //.Execute({ Bind<"name">("John") });

    //auto customer_id_or{ PreparedStatement<
    //    "select id/*:integer*/ from customers "
    //    "where name = ? /*:name:text*/;"  //
    //>{ sqldb }
    //.ExecuteSingleRow({ Bind<"name">("John") }) };

    //if (!customer_id_or)
    //{
    //    std::cerr << "Unable to find customer name\n";

    //    return 1;
    //}

    //auto customer_id{ Field<"id">(customer_id_or.value()) };

    //PreparedStatement<
    //    "INSERT INTO orders(item , customerid , price, discount_code ) "
    //    "VALUES (?/*:item:text*/, ?/*:customerid:integer*/, ?/*:price:real*/, "
    //    "?/*:discount_code:text?*/ );"  //
    //> insert_order{ sqldb };

    //insert_order.Execute({
    //    Bind<"item">("Phone"),
    //    Bind<"price">(1444.44),
    //    Bind<"customerid">(customer_id)
    //});
    //insert_order.Execute({
    //    Bind<"item">("Laptop"),
    //    Bind<"price">(1300.44),
    //    Bind<"customerid">(customer_id)
    //});
    //insert_order.Execute({
    //    Bind<"customerid">(customer_id),
    //    Bind<"price">(2000),
    //    Bind<"item">("MacBook"),
    //    Bind<"discount_code">("BIGSALE")
    //});

    //PreparedStatement<
    //    "SELECT orders.id /*:integer*/, name/*:text*/, item/*:text*/, price/*:real*/, "
    //    "discount_code/*:text?*/ "
    //    "FROM orders JOIN customers ON customers.id = customerid "
    //    "WHERE price > ?/*:min_price:real*/;"
    //> select_orders{ sqldb };

    //while (true)
    //{
    //    std::cout << "Enter min price.\n";

    //    auto min_price{ 0.0 };

    //    std::cin >> min_price;

    //    for (auto& row : select_orders.ExecuteRows({ Bind<"min_price">(min_price) }))
    //    {
    //        std::cout << Field<"orders.id">(row) << " ";
    //        std::cout << Field<"price">(row) << " ";
    //        std::cout << Field<"name">(row) << " ";
    //        std::cout << Field<"item">(row) << " ";
    //        std::cout << Field<"item">(row) << " ";
    //        std::cout << Field<"discount_code">(row).value_or("<NO CODE>") << "\n";
    //    }
    //}
}
