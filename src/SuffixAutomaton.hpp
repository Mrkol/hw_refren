#pragma once
#ifndef SUFFIX_AUTOMATON_HPP
#define SUFFIX_AUTOMATON_HPP

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <iostream>

class SuffixAutomaton;

namespace details
{
	using Index = int64_t;
	using Length = uint64_t;
	class SuffixAutomatonIterator;
}

class SuffixAutomaton
{
private:
	using Length = details::Length;
	using Index = details::Index;

	struct Node
	{
		Index suffixLink;
		Length length;
		bool cloned;
		Length occurances;
		Length firstOccuranceEnd;
		std::unordered_map<char, Index> transitions;
	};

	std::vector<Node> nodes;

	Index fullStringNode;

	void append(char newChar);

	void calculate();

	friend class details::SuffixAutomatonIterator;

	friend std::ostream& operator<<(std::ostream& out,
		const SuffixAutomaton& automaton);


public:
	using Iterator = details::SuffixAutomatonIterator;

	SuffixAutomaton();
	SuffixAutomaton(std::string str);

	Iterator begin();
	Iterator end();
};

class details::SuffixAutomatonIterator
{
private:
	Index currentNode;
	SuffixAutomaton* owner;

	SuffixAutomaton::Node& get() const;

	SuffixAutomatonIterator(Index index, SuffixAutomaton* owner);

	friend class ::SuffixAutomaton;

public:
	SuffixAutomatonIterator();

	void Goto(char c);
	void GotoSuffix();
	bool HasTransition(char c);

	Length GetOccurances() const;
	Length GetLength() const;
	std::tuple<Index, Index> GetSlice();

	SuffixAutomatonIterator& operator++();
	SuffixAutomatonIterator operator++(int);

	operator bool() const;

	bool operator==(const SuffixAutomatonIterator& other) const;
	bool operator!=(const SuffixAutomatonIterator& other) const;
};

std::ostream& operator<<(std::ostream& out,
	const SuffixAutomaton& automaton);

#endif // SUFFIX_AUTOMATON_HPP
