#include <catch.hpp>
#include "SoaVector.h"
#include "ToFromNlohmannJson.h"

using namespace NDataStructure;
using namespace std::string_literals;

TEST_CASE("Construction", "[TaggedStruct]")
{
	TaggedTuple<
		Member<"hello", Auto, [] { return 5; }>,
		Member<"world", std::string, [] { return "world"; }>,
		Member<"test", Auto, [](auto& t) { return 2 * Get<"hello">(t) + std::size(Get<"world">(t)); }>,
		Member<"last", int>
	> ts{ tag<"hello"> = 1 };

	REQUIRE(Get<"hello">(ts) == 1);
	REQUIRE(Get<"world">(ts) == "world");
	REQUIRE(Get<"test">(ts) == 7);
	REQUIRE(Get<"last">(ts) == 0);
}

TEST_CASE("Copy", "[TaggedStruct]")
{
	TaggedTuple<
		Member<"hello", Auto, [] { return 5; }>,
		Member<"world", std::string, [] { return "world"; }>,
		Member<"test", Auto, [](auto& t) { return 2 * Get<"hello">(t) + std::size(Get<"world">(t)); }>,
		Member<"last", int>
	> t{ tag<"hello"> = 1 };

	auto ts{ t };

	REQUIRE(Get<"hello">(ts) == 1);
	REQUIRE(Get<"world">(ts) == "world");
	REQUIRE(Get<"test">(ts) == 7);
	REQUIRE(Get<"last">(ts) == 0);
}

TEST_CASE("ConstructionUdl", "[TaggedStruct]")
{
	using namespace Literals;

	TaggedTuple<
		Member<"hello", Auto, [] { return 5; }>,
		Member<"world", std::string, [](auto& self) { return std::to_string(Get<"hello">(self)); }>,
		Member<"test", Auto, [](auto& t) { return 2 * Get<"hello">(t) + std::size(Get<"world">(t)); }>,
		Member<"last", int>
	> ts{ "world"_tag = "Universe", "hello"_tag = 1 };

	REQUIRE(ts["hello"_tag] == 1);
	REQUIRE(ts["world"_tag] == "Universe");
}

TEST_CASE("Ctad", "[TaggedStruct]")
{
	using namespace Literals;

	TaggedTuple ctad{ tag<"a"> = 15, "b"_tag = std::string{ "Hello ctad" } };

	REQUIRE(Get<"a">(ctad) == 15);
	REQUIRE(Get<"b">(ctad) == "Hello ctad");
}

TEST_CASE("ConstAccess", "[TaggedStruct]")
{
	using namespace Literals;

	TaggedTuple const ctad{ tag<"a"> = 15, "b"_tag = std::string{ "Hello ctad" } };

	REQUIRE(Get<"a">(ctad) == 15);
	REQUIRE(Get<"b">(ctad) == "Hello ctad");
}

TEST_CASE("MutableAccess", "[TaggedStruct]")
{
	using namespace Literals;

	TaggedTuple ctad{ tag<"a"> = 15, "b"_tag = std::string{ "Hello ctad" } };

	REQUIRE(Get<"a">(ctad) == 15);
	REQUIRE(Get<"b">(ctad) == "Hello ctad");

	Get<"a">(ctad) = 10;

	REQUIRE(Get<"a">(ctad) == 10);
	REQUIRE(Get<"b">(ctad) == "Hello ctad");
}

TEST_CASE("NamedArguments", "[TaggedStrcut]")
{
	using namespace Literals;
	using TestArguments = TaggedTuple<
		Member<"a", int, []{}>,	// Required argument.
		Member<"b", Auto, [](auto& t) { return Get<"a">(t) + 2; }>,
		Member<"c", Auto, [](auto& t) { return Get<"b">(t) + 2; }>,
		Member<"d", Auto, []() { return 5; }>
	>;

	auto func{ [](TestArguments args) {
		REQUIRE(Get<"a">(args) == 5);
		REQUIRE(Get<"b">(args) == 7);
		REQUIRE(Get<"c">(args) == 9);
		REQUIRE("d"_tag(args) == 1);
	} };

	func({ "d"_tag = 1, "a"_tag = 5 });
}

TEST_CASE("RelopsPredicate", "[TaggedStruct]")
{
	using namespace TagRelops;
	using namespace Literals;

	TaggedTuple ctad{ tag<"a"> = 15, tag<"b"> = "Hello ctad"s };
	auto predicate{ tag<"a"> == 15 };

	REQUIRE(predicate(ctad));
	REQUIRE_FALSE((tag<"a"> != 15)(ctad));
	REQUIRE((tag<"b"> == "Hello ctad")(ctad));
	REQUIRE_FALSE((tag<"a"> < 15)(ctad));
	REQUIRE_FALSE((tag<"a"> > 15)(ctad));
	REQUIRE((tag<"a"> <= 15)(ctad));
	REQUIRE((tag<"a"> >= 15)(ctad));

	REQUIRE_FALSE((15 < tag<"a">(ctad)));
	REQUIRE_FALSE((15 > tag<"a">(ctad)));
	REQUIRE((15 <= tag<"a">(ctad)));
	REQUIRE((15 >= tag<"a">(ctad)));
	REQUIRE((15 == tag<"a">(ctad)));
	REQUIRE_FALSE((15 != tag<"a">(ctad)));
}

TEST_CASE("Apply", "[TaggedStruct]")
{
	TaggedTuple<
		Member<"hello", Auto, [] { return 5; }>,
		Member<"world", std::string, [] { return "world"; }>,
		Member<"test", Auto, [](auto& t) { return 2 * Get<"hello">(t) + std::size(Get<"world">(t)); }>,
		Member<"last", int>
	> ts{ tag<"hello"> = 1 };

	auto f{ [](auto&&... m) {
		auto tup{ std::tie(m...) };

		REQUIRE(std::get<0>(tup).Key() == "hello");
		REQUIRE(std::get<0>(tup).Value() == 1);
		REQUIRE(std::get<1>(tup).Key() == "world");
		REQUIRE(std::get<1>(tup).Value() == "world");
		REQUIRE(std::get<2>(tup).Key() == "test");
		REQUIRE(std::get<2>(tup).Value() == 7);
		REQUIRE(std::get<3>(tup).Key() == "last");
		REQUIRE(std::get<3>(tup).Value() == 0);
	} };

	ts.Apply(f);
}

TEST_CASE("SoaVector", "[Basic]")
{
	using Person = TaggedTuple<
		Member<"name", std::string>,
		Member<"address", std::string>,
		Member<"id", std::int64_t>,
		Member<"score", double>
	>;

	SoaVector<Person> v;

	v.push_back({ 
		tag<"name"> = "John",
		tag<"address"> = "somewhere",
		tag<"id"> = 1,
		tag<"score"> = 10.5
	});
	v.push_back({ 
		tag<"name"> = "Jane",
		tag<"address"> = "there",
		tag<"id"> = 2,
		tag<"score"> = 12.5
	});

	REQUIRE(Get<"name">(v[1]) == "Jane");

	auto scores{ Get<"score">(v) };

	REQUIRE(*std::max_element(std::begin(scores), std::end(scores)) == 12.5);
}

TEST_CASE("BasicRoundTrip", "[Json]")
{
	using Person = TaggedTuple<
		Member<"name", std::string>,
		Member<"address", std::string>,
		Member<"id", std::int64_t>,
		Member<"score", double>
	>;

	Person person{
		tag<"name"> = "John",
		tag<"address"> = "Somewhere",
		tag<"id"> = 1,
		tag<"score"> = 15
	};

	nlohmann::json j = person;		// Uniform Initialization을 하면 제대로 변환되지 안흠
	//nlohmann::json j{ person };	// 테스트를 통과하지 못함

	auto person2{ j.get<Person>() };

	REQUIRE(person == person2);
}

TEST_CASE("Defaults", "[Json]")
{
	using Person = TaggedTuple<
		Member<"name", std::string>,
		Member<"address", std::string>,
		Member<"id", std::int64_t>,
		Member<"score", double, [] { return 100.0; }>
	>;

	Person person{
		tag<"name"> = "John",
		tag<"address"> = "Somewhere",
		tag<"id"> = 1,
		tag<"score"> = 15
	};

	nlohmann::json j = person;

	auto person2{ j.get<Person>() };

	REQUIRE(person == person2);

	j.erase("score");

	auto person3{ j.get<Person>() };

	REQUIRE(person2 != person3);
	REQUIRE(Get<"score">(person3) == 100);
	
	Get<"score">(person3) = 15;

	REQUIRE(person2 == person3);
}

TEST_CASE("DefaultSelf", "[Json]")
{
	using Person = TaggedTuple<
		Member<"name", std::string>,
		Member<"address", std::string>,
		Member<"id", std::int64_t, []{}>,
		Member<"score", double, [](auto& self) { return Get<"id">(self) + 1.0; }>
	>;

	Person person{
		tag<"name"> = "John",
		tag<"address"> = "Somewhere",
		tag<"id"> = 1,
		tag<"score"> = 15.0
	};

	nlohmann::json j = person;

	auto person2{ j.get<Person>() };

	REQUIRE(person == person2);

	j.erase("score");

	auto person3{ j.get<Person>() };

	REQUIRE(person != person3);
	REQUIRE(Get<"score">(person3) == Get<"id">(person3) + 1.0);

	Get<"score">(person3) = 15.0;

	REQUIRE(person == person3);
}

TEST_CASE("DefaultConstructed", "[TaggedTuple][Constexpr]")
{
	using Tup = TaggedTuple<
		Member<"a", int, [] { return 1; }>,
		Member<"b", double, [] { return 5.0; }>,
		Member<"c", int, [](auto& self) { return Get<"a">(self) + 1; }>
	>;

	constexpr Tup t{};

	using namespace Literals;

	static_assert(Get<"a">(t) == 1);
	static_assert(Get<"b">(t) == 5.0);
	static_assert(Get<"c">(t) == 2);
}

TEST_CASE("WithParameters", "[TaggedTuple][Constexpr]")
{
	using Tup = TaggedTuple<
		Member<"a", int, [] { return 1; }>,
		Member<"b", double, [] { return 5.0; }>,
		Member<"c", int, [](auto& self) { return Get<"a">(self) + 1; }>
	>;

	constexpr Tup t{ tag<"a"> = 5 };

	using namespace Literals;

	static_assert(Get<"a">(t) == 5);
	static_assert(Get<"b">(t) == 5.0);
	static_assert(Get<"c">(t) == 6);
}