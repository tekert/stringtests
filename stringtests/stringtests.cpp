// stringtests.cpp : Defines the entry point for the application.
//

//#define ENABLE_BOOST_TESTS 0

#include "stringtests.h"

#include <string>
#include <string_view>
#include <functional>
#include <algorithm>

#if ENABLE_BOOST_TESTS
#include <boost/utility/string_view.hpp>
#include <boost/tokenizer.hpp>
#endif

#include <benchmark/benchmark.h>

// std:string
std::string LoremIpsumStr{ "Lorem ipsum dolor sit amet, consectetur adipiscing elit, "
"sed do eiusmod tempor incididuntsuperlongwordsuper ut labore et dolore magna aliqua. Ut enim ad minim veniam, "
"quis nostrud exercitation ullamco laboris nisi ut aliquipsuperlongword ex ea commodo consequat. Duis aute "
"irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. "
"Excepteur sint occaecat cupidatatsuperlongword non proident, sunt in culpa qui officia deserunt mollit anim id est laborum." };

// stringview
std::string_view LoremIpsumStrv;

// delims used to split the string (repeated delims are used to measure performance in other areas)
auto g_delim = " orzabcd<eeeeeeeeeeeeeeeeeeee";

#if ENABLE_BOOST_TESTS
// boost::string_view type
boost::string_view LoremIpsumbStrv;
const boost::string_view g_bdelim{ g_delim };
#endif


// Custom made functions

std::vector<std::string_view> SplitSV_custom1(const std::string_view str, const std::string_view delims = " or")
{
	std::vector<std::string_view> result;
	result.reserve(str.length() / 2);

	int index = 0;
	int length;

	for (auto i = 0; i < str.size(); i++)
	{
		for (int a = 0; a < delims.size(); a++)
		{
			if (str[i] == delims[a])
			{
				length = i - index;

				if (length != 0)
					result.emplace_back(str.data() + index, length);

				index = i + 1;

				break;
			}
		}
	}

	result.emplace_back(str.data() + index, str.size() - index);

	return result;
}

std::vector<std::string_view> SplitSV_custom2(const std::string_view str, const std::string_view delims = " or")
{
	std::vector<std::string_view> result;
	result.reserve(str.length() / 2);

	auto index = 0;
	int length;
	const char* stri = str.data();

	for (auto i = 0; i < str.size(); i++)
	{
		for (int a = 0; a < delims.size(); a++)
		{
			if (stri[i] == delims[a])
			{
				length = i - index;

				if (length != 0)
					result.emplace_back(stri + index, length);

				index = i + 1;

				break;
			}
		}
	}

	result.emplace_back(stri + index, str.size() - index);

	return result;
}

constexpr auto findfirst_custom(const char* first1, const char* last1, const char* first2, const char* last2)
{
	for (; first1 != last1; ++first1)
	{
		for (auto mid2 = first2; mid2 != last2; ++mid2)
		{
			if (*first1 == *mid2)
				return first1;
		}
	}
	return last1;
}

std::vector<std::string_view> SplitSV_custom3(const std::string_view str, const std::string_view delims = " or")
{
	std::vector<std::string_view> output;
	output.reserve(str.size() / 2);

	auto fromi = str.data();
	auto delimfist = delims.data();

	for (auto foundat = fromi, last = fromi + str.size(); foundat != last && fromi != last; fromi = foundat + 1)
	{
		foundat = findfirst_custom(fromi, last, delimfist, delimfist + delims.size());

		if (fromi != foundat)
			output.emplace_back(fromi, foundat - fromi);
	}

	return output;
}

// Playing with intructions
std::vector<std::string_view> splitSV_custom4a(const std::string_view str, const std::string_view delimeters = "or")
{
	std::vector<std::string_view> res;
	res.reserve(str.length() / 2);
	const char* ptr = str.data();
	size_t size = 0;

	for (const char c : str)
	{
		for (const char d : delimeters)
		{
			if (c == d)
			{
				if (size)
					res.emplace_back(ptr, size);
				ptr += size + 1;
				size = 0;
				goto next;
			}
		}
		++size;
	next: continue;
	}

	if (size)
		res.emplace_back(ptr, size);
	return res;
}

// Removing gotos.
std::vector<std::string_view> splitSV_custom4b(const std::string_view str, const std::string_view delimeters = "or")
{
	std::vector<std::string_view> res;
	res.reserve(str.length() / 2);
	const char* ptr = str.data();
	size_t size = 0;

	for (const char c : str)
	{
		for (const char d : delimeters)
		{
			if (c == d)
			{
				if (size)
					res.emplace_back(ptr, size);
				ptr += size + 1;
				size = -1; // extra intruction every time this if happens (bad)
				break;
			}
		}
		++size;
	}

	if (size)
		res.emplace_back(ptr, size);
	return res;
}

static bool IsDelim(char tst, const char* DELIMS)
{
	//const char* DELIMS = " \n\t\r\f";
	//const char* DELIMS = g_delim;
	do // Delimiter string cannot be empty, so don't check for it
	{
		if (tst == *DELIMS)
			return true;
		++DELIMS;
	} while (*DELIMS);

	return false;
}

std::vector<std::string> split_Cway(const std::string& str, const std::string& delims = "or")
{
	std::vector<std::string> output;
	output.reserve(str.length() / 2);

	const char* p = str.c_str();
	const char* delim = delims.c_str();

	while (*p && IsDelim(*p, delim))
		++p;

	const char* pTok = p;

	while (*p)
	{
		do
		{
			++p;
		} while (!IsDelim(*p, delim) && *p);

		output.emplace_back(pTok, p - pTok);

		while (*p && IsDelim(*p, delim))
			++p;

		pTok = p;
	}

	return output;
}

#if ENABLE_BOOST_TESTS
//uses string
std::vector<std::string> split_S_boostToken(const std::string& strv, const std::string_view delims = "or")
{
	std::vector<std::string> output;
	output.reserve(strv.length() / 2);
	//boost::char_separator<char> sep{ " 1234abc" };
	boost::char_separator<char> sep{ delims.data() };
	boost::tokenizer<boost::char_separator<char>> tok{ strv, sep };

	for (const auto& t : tok)
	{
		output.emplace_back(t);
	}

	return output;
}
#endif



// uses string::find_first_of
std::vector<std::string> splitS(const std::string& str, const std::string& delims = "or")
{
	std::vector<std::string> output;
	output.reserve(str.length() / 2);
	size_t first = 0;

	while (first < str.size())
	{
		const auto second = str.find_first_of(delims, first);

		if (first != second)
		{
			output.emplace_back(str.substr(first, second - first));
		}

		if (second == std::string::npos)
			break;

		first = second + 1;
	}

	return output;
}

// uses std::find_first_of with iterators
std::vector<std::string> splitS_Std(const std::string& str, const std::string& delims = "or")
{
	std::vector<std::string> output;
	output.reserve(str.length() / 2);
	auto first = std::cbegin(str);

	while (first != std::cend(str))
	{
		const auto second = std::find_first_of(first, std::cend(str),
			std::cbegin(delims), std::cend(delims));

		if (first != second)
			output.emplace_back(first, second);

		if (second == std::cend(str))
			break;

		first = std::next(second);
	}

	return output;
}

// uses std::find_first_of with pointers
std::vector<std::string> splitS_Std_Ptr(const std::string& str, const std::string_view delims = "or")
{
	std::vector<std::string> output;
	output.reserve(str.size() / 2);

	for (auto first = str.data(), second = str.data(), last = first + str.size(); second != last && first != last; first = second + 1) {
		second = std::find_first_of(first, last, std::cbegin(delims), std::cend(delims));

		if (first != second)
			output.emplace_back(first, second);
	}

	return output;
}

// uses string_view::find_first_of
std::vector<std::string_view> split_SV(const std::string_view strv, const std::string_view delims = "or")
{
	std::vector<std::string_view> output;
	output.reserve(strv.length() / 2);
	size_t first = 0;

	while (first < strv.size())
	{
		const auto second = strv.find_first_of(delims, first);

		if (first != second)
			output.emplace_back(strv.substr(first, second - first));

		if (second == std::string_view::npos)
			break;

		first = second + 1;
	}

	return output;
}

//uses std::find_first_of
std::vector<std::string_view> split_SV_Std(const std::string_view strv, const std::string_view delims = "or")
{
	std::vector<std::string_view> output;
	output.reserve(strv.length() / 2);
	auto first = strv.begin();

	while (first != strv.end())
	{
		const auto second = std::find_first_of(first, std::cend(strv), std::cbegin(delims), std::cend(delims));

		if (first != second)
			output.emplace_back(strv.substr(std::distance(strv.begin(), first), std::distance(first, second)));

		if (second == strv.end())
			break;

		first = std::next(second);
	}

	return output;
}

// uses std::find_first_of with Pointers
std::vector<std::string_view> split_SV_Ptr(const std::string_view str, const std::string_view delims = "or")
{
	std::vector<std::string_view> output;
	output.reserve(str.size() / 2);

	for (auto first = str.data(), second = str.data(), last = first + str.size(); second != last && first != last; first = second + 1)
	{
		second = std::find_first_of(first, last, std::cbegin(delims), std::cend(delims));

		if (first != second)
			output.emplace_back(first, second - first);
	}

	return output;

}
/*
// Testing something
std::vector<std::string_view> split_SVb_Std(std::string_view str, std::string_view delims = "or")
{
	std::vector<std::string_view> output;
	output.reserve(str.size() / 2);

	for (auto fromi = str.data(), foundat = str.data(), last = fromi + str.size(); foundat != last && fromi != last; fromi = foundat + 1)
	{
		foundat = std::find_first_of(fromi, last, std::cbegin(delims), std::cend(delims));

		if (fromi != foundat)
			output.emplace_back(fromi, foundat - fromi);
	}

	return output;
}
*/

#if ENABLE_BOOST_TESTS
//uses boost::string_view::find_first_of
std::vector<boost::string_view> split_SV_Boost(const boost::string_view strv, const boost::string_view delims = "or")
{
	std::vector<boost::string_view> output;
	output.reserve(strv.length() / 2);
	size_t first = 0;

	while (first < strv.size())
	{
		const auto second = strv.find_first_of(delims, first);

		if (first != second)
			output.emplace_back(strv.substr(first, second - first));

		if (second == boost::string_view::npos)
			break;

		first = second + 1;
	}

	return output;
}
#endif

// Google Benchmark defs

static void bench_split_Cway(benchmark::State& state) {
	for (auto _ : state) {
		auto v = split_Cway(LoremIpsumStr, g_delim);
		benchmark::DoNotOptimize(v);
	}
}

static void bench_split_SV(benchmark::State& state) {
	for (auto _ : state) {
		auto v = split_SV(LoremIpsumStrv, g_delim);
		benchmark::DoNotOptimize(v);
	}
}


#if ENABLE_BOOST_TESTS
static void bench_split_S_boostToken(benchmark::State& state) {
	for (auto _ : state) {
		auto v = split_S_boostToken(LoremIpsumStr, g_delim);
		benchmark::DoNotOptimize(v);
	}
}

static void bench_split_SV_Boost(benchmark::State& state) {
	for (auto _ : state) {
		auto v = split_SV_Boost(LoremIpsumbStrv, g_bdelim);
		benchmark::DoNotOptimize(v);
	}
}
#endif


static void bench_split_SV_Ptr(benchmark::State& state) {
	for (auto _ : state) {
		auto v = split_SV_Ptr(LoremIpsumStrv, g_delim);
		benchmark::DoNotOptimize(v);
	}
}

static void bench_split_SV_Std(benchmark::State& state) {
	for (auto _ : state) {
		auto v = split_SV_Std(LoremIpsumStrv, g_delim);
		benchmark::DoNotOptimize(v);
	}
}

static void bench_splitS(benchmark::State& state) {
	for (auto _ : state) {
		auto v = splitS(LoremIpsumStr, g_delim);
		benchmark::DoNotOptimize(v);
	}
}

static void bench_splitS_Std(benchmark::State& state) {
	for (auto _ : state) {
		auto v = splitS_Std(LoremIpsumStr, g_delim);
		benchmark::DoNotOptimize(v);
	}
}

static void bench_splitS_Std_Ptr(benchmark::State& state) {
	for (auto _ : state) {
		auto v = splitS_Std_Ptr(LoremIpsumStr, g_delim);
		benchmark::DoNotOptimize(v);
	}
}

static void bench_SplitSV_custom1(benchmark::State& state) {
	for (auto _ : state) {
		auto v = SplitSV_custom1(LoremIpsumStrv, g_delim);
		benchmark::DoNotOptimize(v);
	}
}

static void bench_SplitSV_custom2(benchmark::State& state) {
	for (auto _ : state) {
		auto v = SplitSV_custom2(LoremIpsumStrv, g_delim);
		benchmark::DoNotOptimize(v);
	}
};

static void bench_SplitSV_custom3(benchmark::State& state) {
	for (auto _ : state) {
		auto v = SplitSV_custom3(LoremIpsumStrv, g_delim);
		benchmark::DoNotOptimize(v);
	}
}

static void bench_splitSV_custom4a(benchmark::State& state) {
	for (auto _ : state) {
		auto v = splitSV_custom4a(LoremIpsumStrv, g_delim);
		benchmark::DoNotOptimize(v);
	}
}

static void bench_splitSV_custom4b(benchmark::State& state) {
	for (auto _ : state) {
		auto v = splitSV_custom4b(LoremIpsumStrv, g_delim);
		benchmark::DoNotOptimize(v);
	}
}


// Custom splits
BENCHMARK(bench_split_Cway);
BENCHMARK(bench_SplitSV_custom1);
BENCHMARK(bench_SplitSV_custom2);
BENCHMARK(bench_SplitSV_custom3);
BENCHMARK(bench_splitSV_custom4a);
BENCHMARK(bench_splitSV_custom4b);

// StringView standard splits
BENCHMARK(bench_split_SV);
BENCHMARK(bench_split_SV_Std);
BENCHMARK(bench_split_SV_Ptr);

// String standard splits
BENCHMARK(bench_splitS);
BENCHMARK(bench_splitS_Std);
BENCHMARK(bench_splitS_Std_Ptr);

// Boost splits
#if ENABLE_BOOST_TESTS
BENCHMARK(bench_split_S_boostToken);
BENCHMARK(bench_split_SV_Boost);
#endif



//std::string needle = "aliquipsuperlongword";
std::string needle = g_delim;
const std::string testString{ LoremIpsumStrv };
static void BM_boyer_moore_searcher(benchmark::State& state)
{
	for (auto _ : state)
	{
		auto it = std::search(testString.begin(), testString.end(),
			std::boyer_moore_searcher(needle.begin(), needle.end()));

		if (it == testString.end())
			std::cout << "The string " << needle << " not found\n";


	}
}

static void BM_boyer_moore_horspool_searcher(benchmark::State& state)
{
	for (auto _ : state)
	{
		auto it = std::search(testString.begin(), testString.end(),
			std::boyer_moore_horspool_searcher(needle.begin(), needle.end()));

		if (it == testString.end())
			std::cout << "The string " << needle << " not found\n";

	}
}

static void BM_default_searcher(benchmark::State& state)
{
	for (auto _ : state)
	{
		auto it = std::search(testString.begin(), testString.end(), std::default_searcher(needle.begin(), needle.end()));

		if (it == testString.end())
			std::cout << "The string " << needle << " not found\n";

	}
}

void setup_strings()
{
	// make it bigger
	for (int i = 0; i < 10; i++)
		LoremIpsumStr.append(LoremIpsumStr);

	// convert to other types
	LoremIpsumStrv  = LoremIpsumStr;
#if ENABLE_BOOST_TESTS
	LoremIpsumbStrv = LoremIpsumStr;
#endif
}

int main(int argc, char* argv[])
{
	std::cout << "Split String tests" << std::endl;

	std::vector<const char*> new_argv(argv, argv + argc);
	new_argv.push_back("--benchmark_repetitions=1"); argc += 1;
	//new_argv.push_back("--benchmark_min_time=1"); argc += 1;
	new_argv.push_back(nullptr);
	argv = const_cast<char**>(new_argv.data()); // or &new_argv[0] if you are using an old compiler

	setup_strings();

	//BENCHMARK(BM_boyer_moore_searcher);
	//BENCHMARK(BM_boyer_moore_horspool_searcher);
	//BENCHMARK(BM_default_searcher);

	::benchmark::Initialize(&argc, argv);
	::benchmark::RunSpecifiedBenchmarks();
	::benchmark::Shutdown();

	/*
	auto v = splitSV_custom4a(LoremIpsumStrv, g_delim);
	for (auto a : v)
		std::cout << a << " ";
	std::cout << std::endl;
	v = splitSV_custom4b(LoremIpsumStrv, g_delim);
	for (auto a : v)
		std::cout << a << " ";
		*/

	//system("PAUSE");

	return 0;
}