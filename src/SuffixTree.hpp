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

	void MoveBy(char c);
	void MoveToSuffixLink();
	bool HasTransition(char c);

	Index GetIndex() const;
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

	Index calculateSuffixLink(Index node);

	Index realIndex(Index i);

	friend class details::SuffixTreeIterator;

	friend std::ostream& operator<<(std::ostream& out,
		SuffixTree& automaton);

public:
	using Iterator = details::SuffixTreeIterator;

	SuffixTree();

	void Append(char c);
	void Append(const std::string& str);

	Length Size() const;

	Iterator begin();
	Iterator end();
};

std::ostream& operator<<(std::ostream& out,
	SuffixTree& automaton);

#endif // SUFFIX_TREE_HPP
