#pragma once
#ifndef SUFFIX_TREE_HPP
#define SUFFIX_TREE_HPP

#include <string>
#include <vector>
#include <unordered_map>


namespace details
{
	using Index = int64_t;
	using Length = uint64_t;
	class SuffixTreeIterator;

	struct Node
	{
		Index occuranceStart;
		Index parentEdgeStart;
		Index occuranceEnd;
		Index suffixLink;
		std::unordered_map<char, Index> transitions;
		Index parent;
		Length occurances;
	};
}

class SuffixTree;

class details::SuffixTreeIterator
{
private:
	Index currentNode;
	//indexed from the string start
	Index currentPosition;

	SuffixTree* owner;

	details::Node& get() const;

	SuffixTreeIterator(Index node, Index pos, SuffixTree* owner);
	SuffixTreeIterator(Index node, SuffixTree* owner);

	friend class ::SuffixTree;

	bool isOnNode() const;
	Length parentNodeLength();

public:
	SuffixTreeIterator();

	void Goto(char c);
	void GotoSuffix();
	bool HasTransition(char c);

	Length GetOccurances() const;
	Length GetLength() const;
	std::tuple<Index, Index> GetSlice();

	SuffixTreeIterator& operator++();
	SuffixTreeIterator operator++(int);

	operator bool() const;

	bool operator==(const SuffixTreeIterator& other) const;
	bool operator!=(const SuffixTreeIterator& other) const;
};

class SuffixTree
{
private:
	using Length = details::Length;
	using Index = details::Index;

	std::vector<details::Node> nodes;

	details::SuffixTreeIterator lastNotLeaf;

	std::string stored;

	details::SuffixTreeIterator advance(details::SuffixTreeIterator state,
		Index from, Index to);

	Index splitState(details::SuffixTreeIterator state);

	void append(char c);

	Index calculateSuffixLink(Index node);

	Index realIndex(Index i);

	void calculate();

	friend class details::SuffixTreeIterator;

	friend std::ostream& operator<<(std::ostream& out,
		SuffixTree& automaton);

public:
	using Iterator = details::SuffixTreeIterator;

	SuffixTree(std::string str);

	Iterator begin();
	Iterator end();
};

std::ostream& operator<<(std::ostream& out,
	SuffixTree& automaton);

#endif // SUFFIX_TREE_HPP
