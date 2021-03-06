#include "SuffixAutomaton.hpp"
#include <algorithm>


SuffixAutomaton::SuffixAutomaton()
	: nodes(1, {-1, 0, 0, {}}),
	fullStringNode(0)
{

}

auto SuffixAutomaton::Size()
	-> Length
{
	return nodes.size();
}

void SuffixAutomaton::Append(const std::string& string)
{
	nodes.reserve(2*string.size());

	for (auto c : string)
	{
		Append(c);
	}
}

void SuffixAutomaton::Append(char newChar)
{
	Iterator it(fullStringNode, this);
	fullStringNode = nodes.size();
	nodes.push_back({0, it.GetLength() + 1, it.GetLength() + 1, {}});
	Node& newNode = nodes.back();

	for (;it && !it.HasTransition(newChar); it.MoveToSuffixLink())
		it.get().transitions[newChar] = fullStringNode;

	if (!it)
	{
		newNode.suffixLink = 0;
		return;
	}

	Iterator next = it;
	next.MoveBy(newChar);

	if (next.GetLength() != it.GetLength() + 1)
	{
		newNode.suffixLink = cloneAndUpdate(next, it, newChar);
		return;
	}

	newNode.suffixLink = next.currentNode;
}

auto SuffixAutomaton::cloneAndUpdate(Iterator which, Iterator prev, char newChar)
	-> Index
{
	Node clone = which.get();
	clone.length = prev.GetLength() + 1;
	which.get().suffixLink = nodes.size();

	for (; prev && prev.HasTransition(newChar)
		&& prev.get().transitions[newChar] == which.currentNode;
			prev.MoveToSuffixLink())
		prev.get().transitions[newChar] = nodes.size();

	nodes.push_back(clone);
	return nodes.size() - 1;
}

auto SuffixAutomaton::begin()
	-> Iterator
{
	return Iterator(0, this);
}

auto SuffixAutomaton::FullString()
	-> Iterator
{
	return Iterator(fullStringNode, this);
}

auto SuffixAutomaton::end()
	-> Iterator
{
	return Iterator(nodes.size(), this);
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
void details::SuffixAutomatonIterator::MoveBy(char c)
{
	currentNode = HasTransition(c)
		? get().transitions[c] : get().suffixLink;
}

void details::SuffixAutomatonIterator::MoveToSuffixLink()
{
	currentNode = get().suffixLink;
}

bool details::SuffixAutomatonIterator::HasTransition(char c)
{
	return get().transitions.find(c) != get().transitions.end();
}

auto details::SuffixAutomatonIterator::GetIndex() const
	-> Index
{
	return currentNode;
}

auto details::SuffixAutomatonIterator::GetLength() const
	-> Length
{
	return get().length;
}

std::vector<char> details::SuffixAutomatonIterator::GetTransitions()
{
	std::vector<char> result;
	for (auto it = get().transitions.begin();
		it != get().transitions.end(); ++it)
	{
		result.push_back(it->first);
	}
	return result;
}

auto details::SuffixAutomatonIterator::GetSlice()
	-> std::tuple<Index, Index>
{
	return {get().firstOccurenceEnd - get().length, get().firstOccurenceEnd};
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

