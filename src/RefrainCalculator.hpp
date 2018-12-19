#pragma once
#ifndef REFRAIN_CALCULATOR_HPP
#define REFRAIN_CALCULATOR_HPP
#include <algorithm>
#include "SuffixAutomaton.hpp"
#include "SuffixTree.hpp"

namespace details
{
	template<typename Structure>
	class BasicRefrainCalculator;
}

struct Refrain
{
	std::string Substring;
	size_t RefrainValue;
};

template<class Structure>
class details::BasicRefrainCalculator
{
protected:
	using Length = details::Length;
	using Index = details::Index;
	using Iterator = typename Structure::Iterator;

	Structure structure;
	std::string stored;
	std::vector<Iterator> sortedNodes;
	std::vector<Length> nodeOccurences;

	virtual void calculateBase() = 0;

	void precalculate()
	{
		sortedNodes.resize(structure.Size());
		for (auto it = structure.begin(); it != structure.end(); ++it)
			sortedNodes[it.GetIndex()] = it;
		
		nodeOccurences.assign(structure.Size(), 0);
		calculateBase();

		std::stable_sort(sortedNodes.begin(), sortedNodes.end(),
			[](Iterator a, Iterator b)
			{
				return a.GetLength() > b.GetLength();
			});

		for (auto i : sortedNodes)
		{
			if (i == structure.begin())
				continue;
			auto j = i;
			j.MoveToSuffixLink();
			nodeOccurences[j.GetIndex()] += nodeOccurences[i.GetIndex()];
		}
	}

	explicit BasicRefrainCalculator(const std::string& string)
		: structure(), stored(string)
	{
		structure.Append(string);
	}

public:

	Refrain Result()
	{
		precalculate();
		Iterator best;

		for (auto& i : sortedNodes)
		{
			if (!best ||
				i.GetLength() * nodeOccurences[i.GetIndex()] > 
				best.GetLength() * nodeOccurences[best.GetIndex()])
			{
				best = i;
			}
		}

		auto[l, r] = best.GetSlice();
		return {stored.substr(l, r - l), (r - l)*nodeOccurences[best.GetIndex()]};
	}
};

template<typename Structure>
class RefrainCalculator
{

};

template<>
class RefrainCalculator<SuffixAutomaton> : public details::BasicRefrainCalculator<SuffixAutomaton>
{
private:
	void calculateBase() override
	{
		for (auto it = structure.FullString();
			it != structure.begin(); it.MoveToSuffixLink())
		{
			nodeOccurences[it.GetIndex()] = 1;		
		}
	}

public:
	explicit RefrainCalculator(const std::string& string)
		: BasicRefrainCalculator(string)
	{

	}
};

template<>
class RefrainCalculator<SuffixTree> : public details::BasicRefrainCalculator<SuffixTree>
{
private:
	void calculateBase() override
	{
		for (auto it = structure.begin(); it != structure.end(); ++it)
		{
			auto r = std::get<1>(it.GetSlice());
			nodeOccurences[it.GetIndex()] = (r >= 0 && Length(r) == stored.size());	
		}
	}

public:
	explicit RefrainCalculator(const std::string& string)
		: BasicRefrainCalculator(string)
	{
		structure.Append('$');
	}
};

#endif // REFRAIN_CALCULATOR_HPP
