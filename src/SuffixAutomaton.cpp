#include "SuffixAutomaton.hpp"
#include <algorithm>


SuffixAutomaton::SuffixAutomaton()
	: nodes(1, {-1, 0, false, 1, 0, {}}),
	fullStringNode(0)
{

}

SuffixAutomaton::SuffixAutomaton(std::string string)
	: SuffixAutomaton()
{
	nodes.reserve(2*string.size());

	for (auto c : string)
	{
		append(c);
	}

	calculate();
}

void SuffixAutomaton::append(char newChar)
{
	Iterator it(fullStringNode, this);
	fullStringNode = nodes.size();
	nodes.push_back({0, it.GetLength() + 1, false, 0, it.GetLength() + 1, {}});
	Node& newNode = nodes.back();



	while (it && !it.HasTransition(newChar))
	{
		it.get().transitions[newChar] = fullStringNode;
		it.GotoSuffix();
	}

	if (!it)
	{
		newNode.suffixLink = 0;
		return;
	}

	Iterator next = it;
	next.Goto(newChar);
	if (next.GetLength() == it.GetLength() + 1)
	{
		newNode.suffixLink = next.currentNode;
		return;
	}

	Node clone = next.get();
	clone.cloned = true;
	clone.length = it.GetLength() + 1;
	next.get().suffixLink = nodes.size();
	newNode.suffixLink = nodes.size();

	while (it && it.HasTransition(newChar)
		&& it.get().transitions[newChar] == next.currentNode)
	{
		it.get().transitions[newChar] = nodes.size();
		it.GotoSuffix();
	}

	nodes.push_back(clone);
}

auto SuffixAutomaton::begin()
	-> Iterator
{
	return Iterator(0, this);
}

auto SuffixAutomaton::end()
	-> Iterator
{
	return Iterator(nodes.size(), this);
}

void SuffixAutomaton::calculate()
{
	std::vector<Index> indices(nodes.size());
	for (Length i = 0; i < nodes.size(); ++i)
	{
		indices[i] = i;
		if (nodes[i].cloned)
			nodes[i].occurances = 0;
		else
			nodes[i].occurances = 1;
	}

	std::stable_sort(indices.begin(), indices.end(),
		[this](Index a, Index b)
		{
			return this->nodes[a].length > this->nodes[b].length;
		});

	for (auto i : indices)
	{
		nodes[nodes[i].suffixLink].occurances += nodes[i].occurances;
	}
}

details::SuffixAutomatonIterator::
	SuffixAutomatonIterator(Index index, SuffixAutomaton* owner)
	: currentNode(index), owner(owner)
{

}

details::SuffixAutomatonIterator::SuffixAutomatonIterator()
	: currentNode(-1), owner(nullptr)
{

}

auto details::SuffixAutomatonIterator::get() const
	-> SuffixAutomaton::Node&
{
	return owner->nodes[currentNode];
}
void details::SuffixAutomatonIterator::Goto(char c)
{
	currentNode = HasTransition(c)
		? get().transitions[c] : get().suffixLink;
}

void details::SuffixAutomatonIterator::GotoSuffix()
{
	currentNode = get().suffixLink;
}

bool details::SuffixAutomatonIterator::HasTransition(char c)
{
	return get().transitions.find(c) != get().transitions.end();
}

auto details::SuffixAutomatonIterator::GetOccurances() const
	-> Length
{
	return get().occurances;
}

auto details::SuffixAutomatonIterator::GetLength() const
	-> Length
{
	return get().length;
}

auto details::SuffixAutomatonIterator::GetSlice()
	-> std::tuple<Index, Index>
{
	return {get().firstOccuranceEnd - get().length, get().firstOccuranceEnd};
}

auto details::SuffixAutomatonIterator::operator++()
	-> SuffixAutomatonIterator&
{
	++currentNode;

	return *this;
}

auto details::SuffixAutomatonIterator::operator++(int)
	-> SuffixAutomatonIterator
{
	SuffixAutomatonIterator copy = *this;
	++currentNode;
	return copy;
}

details::SuffixAutomatonIterator::operator bool() const
{
	return currentNode >= 0
		&& static_cast<Length>(currentNode) < owner->nodes.size();
}

bool details::SuffixAutomatonIterator::
	operator==(const SuffixAutomatonIterator& other) const
{
	return currentNode == other.currentNode;
}

bool details::SuffixAutomatonIterator::
	operator!=(const SuffixAutomatonIterator& other) const
{
	return !(*this == other);
}

std::ostream& operator<<(std::ostream& out,
	const SuffixAutomaton& automaton)
{
	out << "digraph G" << std::endl;
	out << "{" << std::endl;
	out << "\trankdir = TB;" << std::endl;

	for (size_t i = 0; i < automaton.nodes.size(); ++i)
	{
		auto& node = automaton.nodes[i];
		out << "\t" << i << " [label=\"";
		out << i;
		out << "," << automaton.nodes[i].length;
		out << "," << automaton.nodes[i].occurances;
		out << "\"];" << std::endl;

		for (auto index : node.transitions)
		{
			out << "\t" << i << " -> " << index.second;

			out << " [label=\"";
			out << index.first <<  "\"];" << std::endl;
		}

		out << "\t" << i << " -> " << node.suffixLink;
		out << " [style=dotted, color=grey];" << std::endl;
	}

	out << "}" << std::endl;
	return out;
}
