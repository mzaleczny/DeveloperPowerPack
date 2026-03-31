#include <string>
#include <map>
#include <unordered_map>
#include <iostream>
#include <iomanip>
#include <algorithm>
using namespace std;

using TMap = map<int, float>;
using TUMap = unordered_map<int, float>;
using TMultiMap = multimap<float, int>;

TMap TestMap;
TUMap TestUMap;
TMultiMap MultiMap;


template <typename T>
void PrintMap(const T& map);

template <typename A, typename B>
std::pair<B, A> FlipPair(const std::pair<A, B>& p);

// Function below works only for map
template <typename A, typename B>
std::multimap<B, A> FlipMap(const std::map<A, B>& Src);

// Function below works for both map and  unordered_map
template <typename A, typename B, template <class, class, class...> class M, class... Args>
std::multimap<B, A> FlipMap(const M<A, B, Args...>& Src);


int main()
{
	srand(time(nullptr));

	while (TestMap.size() < 7)
	{
		TestMap[rand() % 10] = 1 + rand() % 9 + (rand() % 10) / 10.0f;
	}
	cout << " TestMap = ";
	PrintMap(TestMap);

	while (TestUMap.size() < 7)
	{
		TestUMap[rand() % 10] = 1 + rand() % 9 + (rand() % 10) / 10.0f;
	}
	cout << "TestUMap = ";
	PrintMap(TestUMap);

	while (MultiMap.size() < 7)
	{
		MultiMap.insert(std::pair<float, int>(1 + rand() % 9 + (rand() % 10) / 10.0f, rand() % 10));
	}
	cout << "MultiMap = ";
	PrintMap(MultiMap);

	MultiMap = FlipMap<int,float>(TestMap);
	cout << endl << "MultiMap of TestMap = ";
	PrintMap(MultiMap);

	MultiMap = FlipMap<int, float>(TestUMap);
	cout << endl << "MultiMap of TestUMap = ";
	PrintMap(MultiMap);

	return 0;
}



template <typename T>
void PrintMap(const T& map)
{
	cout << "[";
	for (T::const_iterator it = map.cbegin(); it != map.cend(); ++it)
	{
		if (it != map.begin())
		{
			cout << ", ";
		}
		cout << setprecision(3) << (*it).first << " => " << setprecision(3) << (*it).second;
	}
	cout << "]" << endl;
}

template <typename A, typename B>
std::pair<B, A> FlipPair(const std::pair<A, B>& p)
{
	return std::pair<B, A>(p.second, p.first);
}

template <typename A, typename B>
std::multimap<B, A> FlipMap(const std::map<A, B>& Src)
{
	std::multimap<B, A> Dst;
	std::transform(Src.begin(), Src.end(), std::inserter(Dst, Dst.begin()), FlipPair<A, B>);
	return std::move(Dst);
}

template <typename A, typename B, template <class, class, class...> class M, class... Args>
std::multimap<B, A> FlipMap(const M<A, B, Args...>& Src)
{
	std::multimap<B, A> Dst;
	std::transform(Src.begin(), Src.end(), std::inserter(Dst, Dst.begin()), FlipPair<A, B>);
	return std::move(Dst);
}
