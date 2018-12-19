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
		Length firstOccurenceEnd;
		std::unordered_map<char, Index> transitions;
	};

	std::vector<Node> nodes;

	Index fullStringNode;

	Index cloneAndUpdate(details::SuffixAutomatonIterator which,
		details::SuffixAutomatonIterator prev, char newChar);

	friend class details::SuffixAutomatonIterator;

	friend std::ostream& operator<<(std::ostream& out,
		const SuffixAutomaton& automaton);


public:
	using Iterator = details::SuffixAutomatonIterator;

	SuffixAutomaton();

	void Append(char newChar);
	void Append(const std::string& string);

	Length Size();

	Iterator begin();
	Iterator end();

	Iterator FullString();
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

	void MoveBy(char c);
	void MoveToSuffixLink();
	bool HasTransition(char c);

	Index GetIndex() const;
	Length GetLength() const;
	std::tuple<Index, Index> GetSlice();

	std::vector<char> GetTransitions();

	SuffixAutomatonIterator& operator++();
	SuffixAutomatonIterator operator++(int);

	operator bool() const;

	bool operator==(const SuffixAutomatonIterator& other) const;
	bool operator!=(const SuffixAutomatonIterator& other) const;
};

std::ostream& operator<<(std::ostream& out,
	const SuffixAutomaton& automaton);

#endif // SUFFIX_AUTOMATON_HPP

