#include "MetaStruct.h"
#include <iostream>

template <typename MetaStruct>
void Print(std::ostream& os, MetaStruct const& ms)
{
    MetaStructApply([&](auto const&... m) {
        auto print_item{ [&](auto& m) {
            std::cout << m.Tag().ToStringView() << ":" << m.value << "\n";
        } };

        (std::invoke(print_item, m), ...);
    }, ms);
}

using SubStrArgs = MetaStruct <
    Member<"str", std::string const&, required>,
    Member<"offset", std::size_t, [] { return 0; }>,
    Member<"count", std::size_t, [](auto& self) { return std::size(Get<"str">(self)) - Get<"offset">(self); }>
>;

auto SubStr(SubStrArgs args)
{
    return Get<"str">(args).substr(Get<"offset">(args), Get<"count">(args));
}

using NameAndIdArgs = MetaStruct<
    Member<"name", std::string_view>,
    Member<"id", int const&>
>;

void PrintNameId(NameAndIdArgs args)
{
    std::cout << "Name is " << Get<"name">(args) << " and id is " << Get<"id">(args) << "\n";
}

template <typename P>
void DisplayPerson(P const& p) requires(requires {
    { p.id };
    { p.name };
    { p.score };
})
{
    std::cout << "The person has an id of " << p.id << " and name " << p.name << " and scored " << p.score << "\n";
}

using PersonRef = MetaStruct<
    Member<"name", std::string_view, required>,
    Member<"id", int const&, required>,
    Member<"score", int const&, required>
>;

void DisplayPersonMeta(PersonRef p)
{
    std::cout << "The person has an id of " << Get<"id">(p) << " and name " << Get<"name">(p) << " and scored " << Get<"score">(p) << "\n";
}

int main()
{
    // 1
    {
        using Person = MetaStruct<
            Member<"id", int>,
            Member<"name", std::string>
        >;

        Person p;

        Get<"id">(p) = 1;
        Get<"name">(p) = "John";

        std::cout << Get<"id">(p) << " " << Get<"name">(p) << "\n";
    }

    // 2. Constructed
    {
        using Person = MetaStruct<
            Member<"id", int>,
            Member<"name", std::string>
        >;

        Person p{ arg<"id"> = 1, arg<"name"> = "John" };

        std::cout << p << std::endl;

        p = Person{ arg<"name"> = "John", arg<"id"> = 1 };

        std::cout << p << std::endl;
    }

    // 3. Init
    {
        using Person = MetaStruct<
            Member<"id", int>,
            Member<"name", std::string, [] { return "John"; }>
        >;

        Person p;

        std::cout << p << std::endl;
    }

    // 4. Init Self
    {
        using Person = MetaStruct<
            Member<"id", int>,
            Member<"score", int, [](auto& self) { return Get<"id">(self) + 1; }>,
            Member<"name", std::string, [] { return "John"; }>
        >;

        Person p;

        std::cout << p << std::endl;
    }

    // 5. Print
    {
        using Person = MetaStruct<
            Member<"id", int>,
            Member<"score", int, [](auto& self) { return Get<"id">(self) + 1; }>,
            Member<"name", std::string, [] { return "John"; }>
        >;

        MetaStructApply<Person>([]<typename... M>(M*...) {
            std::cout << "The tags are: ";
            ((std::cout << M::Tag().ToStringView() << " "), ...);
            std::cout << "\n";
        });

        Person p;

        MetaStructApply([&](auto const&... m) {
            ((std::cout << m.Tag().ToStringView() << ":" << m.value << "\n"), ...);
            }, p);
    }

    // 6. Optional arguments
    {
        using Person = MetaStruct<
            Member<"id", int>,
            Member<"score", int, [](auto& self) { return Get<"id">(self) + 1; }>,
            Member<"name", std::string, [] { return "John"; }>
        > ;

        MetaStructApply<Person>([]<typename... M>(M*...) {
            std::cout << "The tags are: ";
            ((std::cout << M::Tag().ToStringView() << " "), ...);
            std::cout << "\n";
        });

        Person p{ arg<"id"> = 2 };

        std::cout << Get<"id">(p) << " " << Get<"name">(p) << " " << Get<"score">(p) << "\n";

        Person p2{ arg<"id"> = 2, arg<"score"> = std::optional<int>{} };

        Print(std::cout, p2);

        Person p3{ arg<"id"> = 2, arg<"score"> = std::optional<int>{ 500 } };

        Print(std::cout, p3);
    }

    // 7. Required named args
    {
        using Person = MetaStruct<
            Member<"id", int, required>,
            Member<"name", std::string, required>,
            Member<"score", int, [](auto& self) { return Get<"id">(self) + 1; }>
        >;

        MetaStructApply<Person>([]<typename... M>(M*...) {
            std::cout << "The tags are: ";

            auto print_tag{ [](auto t) { std::cout << t.ToStringView() << " "; } };

            (std::invoke(print_tag, M::Tag()), ...);
            std::cout << "\n";
        });

        Person p{ arg<"id"> = 2, arg<"name"> = "John" };

        std::cout << Get<"id">(p) << " " << Get<"name">(p) << " " << Get<"score">(p) << "\n";

        Person p2{ arg<"name"> = "JRB", arg<"id"> = 2, arg<"score"> = std::optional<int>() };

        Print(std::cout, p2);

        std::string s{ "Hello World" };

        auto pos{ s.find(' ') };
        auto all{ SubStr({ arg<"str"> = std::ref(s) }) };
        auto first{ SubStr({ arg<"str"> = std::ref(s), arg<"count"> = pos }) };
        auto second{ SubStr({ arg<"str"> = std::ref(s), arg<"offset"> = pos + 1 }) };

        std::cout << all << "\n" << first << "\n" << second << "\n";
    }

    // 8. Conversion
    {
        using Person = MetaStruct<
            Member<"id", int, required>,
            Member<"name", std::string, required>,
            Member<"score", int, [](auto& self) { return Get<"id">(self) + 1; }>
        > ;

        Person p{ arg<"id"> = 2, arg<"name"> = "John" };

        using NameAndId = MetaStruct<
            Member<"name", std::string_view>,
            Member<"id", int>
        >;

        Print(std::cout, p);

        NameAndId n = p;

        Print(std::cout, n);

        PrintNameId(p);
        PrintNameId(n);
    }

    // 9. Attributes
    {
        enum class Encoding
            : int
        {
            Fixed = 0,
            Variable = 1
        };

        using Person = MetaStruct<
            Member<"id", int, required, { arg<"encoding"> = Encoding::Variable }>,
            Member<"name", std::string, required>,
            Member<"score", int, [](auto& self) { return Get<"id">(self) + 1; }>
        > ;

        constexpr auto attributes{ GetAttributes<"id", Person>() };

        if constexpr (Has<"encoding">(attributes) && Get<"encoding">(attributes) == Encoding::Variable)
        {
            std::cout << "Encoding was variable";
        }
        else
        {
            std::cout << "Encoding was fixed";
        }

        Person p{ arg<"id"> = 2, arg<"name"> = "John" };

        using NameAndId = MetaStruct<
            Member<"name", std::string_view>,
            Member<"id", int>
        >;

        Print(std::cout, p);

        NameAndId n{ p };

        Print(std::cout, n);

        PrintNameId(p);
        PrintNameId(n);
    }

    // 10. Applications
    {
        struct MyPerson
        {
            std::string name;
            int id{ 0 };
            int score{ 0 };
        };

        struct YourPerson
        {
            int id{ 0 };
            int score{ 0 };
            std::string name;
        };

        using MyPersonMeta = MetaStruct<
            Member<"id", int>,
            Member<"name", std::string>,
            Member<"score", int>
        >;

        using YourPersonMeta = MetaStruct<
            Member<"id", int>,
            Member<"score", int>,
            Member<"name", std::string>
        >;

        MyPerson p1;
        YourPerson p2;

        MyPersonMeta pm1;
        YourPersonMeta pm2;

        DisplayPerson(p1);
        DisplayPerson(p2);

        DisplayPersonMeta(pm1);
        DisplayPersonMeta(pm2);
    }
    
    // 11. tuple-like
    {
        MetaStruct ms{ arg<"id"> = 2, arg<"score"> = 100, arg<"name"> = "NaDDu" };

        std::cout << ms << std::endl;

        std::cout << ms.get<0>() << std::endl;
        std::cout << std::get<0>(ms) << std::endl;

        auto& [id, score, name] { ms };

        std::cout << id << '\n';
        std::cout << score << '\n';
        std::cout << name << '\n';

        id = 3;

        std::cout << ms << '\n';
    }

    // Fixed string
    {
        constexpr std::string_view string_view_str{ "string_view_str" };
        constexpr FixedString<std::size(string_view_str)> fs{ string_view_str };
        constexpr FixedString fs2{ "string_view_sts" };

        static_assert(fs != fs2);
        static_assert(fs < fs2);
    }
}   
