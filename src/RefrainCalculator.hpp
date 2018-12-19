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
	virtual Index length(Iterator j) = 0;

	void sortIterators(std::vector<Iterator>& vector, Length max)
	{
		std::vector<std::vector<Iterator>> buckets(max);
		for (auto it : vector)
			buckets[it.GetLength()].push_back(it);

		vector.clear();

		for (auto& vec : buckets)
			for (auto& el : vec)
				vector.push_back(el);
	}

	void precalculate()
	{
		nodeOccurences.assign(structure.Size(), 0);
		calculateBase();

		sortedNodes.resize(structure.Size());
		for (auto it = structure.begin(); it != structure.end(); ++it)
			sortedNodes[it.GetIndex()] = it;

		sortIterators(sortedNodes, stored.size() + 2);
		std::reverse(sortedNodes.begin(), sortedNodes.end());

		for (auto i : sortedNodes)
		{
			auto transitions = i.GetTransitions();
			for (auto c : transitions)
			{
				auto j = i;
				j.MoveBy(c);

				nodeOccurences[i.GetIndex()] += nodeOccurences[j.GetIndex()];
			}
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

		for (auto i = structure.begin(); i != structure.end(); ++i)
		{
			if (!best ||
				length(i) * nodeOccurences[i.GetIndex()] > 
				length(best) * nodeOccurences[best.GetIndex()])
			{
				best = i;
			}
		}

		return {stored.substr(std::get<0>(best.GetSlice()), length(best)),
			length(best)*nodeOccurences[best.GetIndex()]};
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

	Index length(Iterator i) override
	{
		return i.GetLength();
	}

public:
	explicit RefrainCalculator(const std::string& string)
		: BasicRefrainCalculator(string)
	{
		// std::cout << structure;
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
			nodeOccurences[it.GetIndex()] = (r >= 0 && Length(r) == stored.size() + 1);	
		}
	}

	Index length(Iterator i) override
	{
		Index r = std::get<1>(i.GetSlice());
		return i.GetLength() - (r >= 0 && Length(r) == stored.size() + 1);
	}

public:
	explicit RefrainCalculator(const std::string& string)
		: BasicRefrainCalculator(string)
	{
		structure.Append('$');
	}
};

#endif // REFRAIN_CALCULATOR_HPP
