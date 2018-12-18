#include "SuffixTree.hpp"
#include <exception>
#include <algorithm>
#include <iostream>

SuffixTree::SuffixTree(std::string str)
	: nodes{{0,0,0,0,{},0,0}}, lastNotLeaf(0, 0, this)
{
	nodes.reserve(str.size()*2);
	for (auto c : str)
		append(c);
	append('$');
	calculate();
}

auto SuffixTree::realIndex(Index i)
	-> Index
{
	Index mod = stored.size() + 1;
	return ((i % mod) + mod) % mod;
}

details::SuffixTreeIterator SuffixTree::
	advance(details::SuffixTreeIterator state,
		Index from, Index to)
{
	Iterator it = state;
	for (Index i = from; i < to;)
	{
		if (it.isOnNode())
		{
			if (!it.HasTransition(stored[i]))
				return Iterator();
			
			it.Goto(stored[i]);
			i++;
			continue;
		}

		Index rest =
			realIndex(it.get().occuranceEnd) - it.currentPosition;
		if (to - i < rest)
			return Iterator(it.currentNode,
				it.currentPosition + to - i, this);

		i += rest;
		it.currentPosition += rest;
	}

	return it;
}

auto SuffixTree::splitState(details::SuffixTreeIterator state)
	-> Index
{
	if (state.isOnNode())
		return state.currentNode;

	auto& end = state.get();
	auto& start = nodes[end.parent];

	// root------start---middle---end

	nodes.push_back(
		{
			end.occuranceStart,
			end.parentEdgeStart,
			state.currentPosition,
			-1,
			{{stored[state.currentPosition], state.currentNode}},
			end.parent,
			0
		});
	end.parent = nodes.size() - 1;
	start.transitions[stored[end.parentEdgeStart]] = nodes.size() - 1;
	end.parentEdgeStart = state.currentPosition;

	return nodes.size() - 1;
}

auto SuffixTree::calculateSuffixLink(Index node)
	-> Index
{
	if (nodes[node].suffixLink != -1)
		return nodes[node].suffixLink;
	if (node == 0)
		return 0;

	Index suffix = calculateSuffixLink(nodes[node].parent);
	Iterator it(suffix, this);
	it = advance(it,
		(nodes[node].parent == 0) + nodes[node].parentEdgeStart,
		realIndex(nodes[node].occuranceEnd));
	nodes[node].suffixLink = splitState(it);

	return nodes[node].suffixLink;
}

void SuffixTree::append(char c)
{
	stored += c;
	while (true)
	{
		if (lastNotLeaf.HasTransition(c))
		{
			lastNotLeaf.Goto(c);
			break;
		}

		Index parent = splitState(lastNotLeaf);
		nodes.push_back(
			{
				Index(stored.size() -
					(1 + nodes[parent].occuranceEnd -
						nodes[parent].occuranceStart)),
				Index(stored.size() - 1),
				-1,
				-1,
				{},
				parent,
				0
			});
		nodes[parent].transitions[c] = nodes.size() - 1;
		lastNotLeaf = Iterator(calculateSuffixLink(parent), this);
		if (parent == 0)
			break;
	}
}

void SuffixTree::calculate()
{
	for (Length i = 0; i < nodes.size(); ++i)
		calculateSuffixLink(i);

	std::vector<Index> indices(nodes.size());
	for (Length i = 0; i < nodes.size(); ++i)
	{
		indices[i] = i;
		if (nodes[i].occuranceEnd == -1)
			nodes[i].occurances = 1;
		else
			nodes[i].occurances = 0;
	}

	std::stable_sort(indices.begin(), indices.end(),
		[this](Index a, Index b)
		{
			return
				realIndex(this->nodes[a].occuranceEnd) - this->nodes[a].occuranceStart
				>
				realIndex(this->nodes[b].occuranceEnd) - this->nodes[b].occuranceStart;
		});

	for (auto i : indices)
	{
		nodes[nodes[i].parent].occurances += nodes[i].occurances;
	}
}

auto SuffixTree::begin()
	-> Iterator
{
	return Iterator(1, this);
}

auto SuffixTree::end()
	-> Iterator
{
	return Iterator(nodes.size(), 0, this);
}

details::SuffixTreeIterator::SuffixTreeIterator(Index node,
	Index pos, SuffixTree* owner)
		: currentNode(node), currentPosition(pos), owner(owner)
{

}

details::SuffixTreeIterator::
	SuffixTreeIterator(Index node, SuffixTree* owner)
		: SuffixTreeIterator(node,
			owner->realIndex(owner->nodes[node].occuranceEnd), owner)
{

}

auto details::SuffixTreeIterator::get() const
	-> Node&
{
	return owner->nodes[currentNode];
}

bool details::SuffixTreeIterator::isOnNode() const
{
	return currentPosition == owner->realIndex(get().occuranceEnd);
}

auto details::SuffixTreeIterator::parentNodeLength()
	-> Length
{
	return owner->realIndex(get().occuranceEnd) - get().parentEdgeStart;
}

details::SuffixTreeIterator::SuffixTreeIterator()
	: SuffixTreeIterator(-1, -1, nullptr)
{
	
}

void details::SuffixTreeIterator::Goto(char c)
{
	if (isOnNode())
	{
		if (HasTransition(c))
		{
			currentNode = get().transitions[c];
			currentPosition = get().parentEdgeStart + 1;
			return;
		}

		GotoSuffix();
		return;
	}

	//suffix tree cannot handle this case,
	//at least i think so
	if (!HasTransition(c))
		throw std::runtime_error("Invalid goto");

	++currentPosition;
}

void details::SuffixTreeIterator::GotoSuffix()
{
	if (!isOnNode())
		throw std::runtime_error("Suffix goto only allowed on nodes.");


	currentNode = owner->calculateSuffixLink(currentNode);
	currentPosition = owner->realIndex(get().occuranceEnd);
}

bool details::SuffixTreeIterator::HasTransition(char c)
{
	if (isOnNode())
		return get().transitions.find(c) != get().transitions.end();

	return Length(owner->realIndex(currentPosition))
			 < owner->stored.size()
		&& owner->stored[currentPosition] == c;
}


auto details::SuffixTreeIterator::GetOccurances() const
	-> Length
{
	return get().occurances;
}

auto details::SuffixTreeIterator::GetLength() const
	-> Length
{
	return currentPosition - get().occuranceStart - get().transitions.empty();
}

auto details::SuffixTreeIterator::GetSlice()
	-> std::tuple<Index, Index>
{
	return {get().occuranceStart, get().occuranceStart + GetLength()};
}

auto details::SuffixTreeIterator::operator++()
	-> SuffixTreeIterator&
{
	++currentNode;
	currentPosition = *this ? owner->realIndex(get().occuranceEnd) : 0;


	return *this;
}

auto details::SuffixTreeIterator::operator++(int)
	-> SuffixTreeIterator
{
	SuffixTreeIterator copy = *this;
	++*this;
	return copy;
}

details::SuffixTreeIterator::operator bool() const
{
	return currentNode >= 0 && Length(currentNode) < owner->nodes.size();
}

bool details::SuffixTreeIterator::
	operator==(const SuffixTreeIterator& other) const
{
	return currentNode == other.currentNode
		&& currentPosition == other.currentPosition;
}

bool details::SuffixTreeIterator::
	operator!=(const SuffixTreeIterator& other) const
{
	return !(*this == other);
}


std::ostream& operator<<(std::ostream& out,
	SuffixTree& tree)
{
	for (size_t i = 0; i < tree.nodes.size(); ++i)
	{
		tree.calculateSuffixLink(i);
	}

	out << "digraph G" << std::endl;
	out << "{" << std::endl;
	out << "\trankdir = TB;" << std::endl;

	for (size_t i = 0; i < tree.nodes.size(); ++i)
	{
		auto& node = tree.nodes[i];
		out << "\t" << i << " [label=\"";
		out << i;
		out << "\"];" << std::endl;

		for (auto index : node.transitions)
		{
			out << "\t" << i << " -> " << index.second;

			auto& node = tree.nodes[index.second];
			out << " [label=\"";
			out << tree.stored.substr(node.parentEdgeStart,
				node.occuranceEnd - node.parentEdgeStart);
			out <<  "\"];" << std::endl;
		}

		// out << "\t" << i << " -> " << tree.nodes[i].suffixLink;
		// out << " [style=dotted, color=grey];" << std::endl;
	}

	out << "}" << std::endl;
	return out;
}


