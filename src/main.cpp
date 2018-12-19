#include <iostream>
#include "SuffixAutomaton.hpp"
#include "SuffixTree.hpp"
#include "RefrainCalculator.hpp"


std::string ReadData(std::istream& in)
{
	size_t count = 0;
	size_t alpha_size = 0;
	in >> count >> alpha_size;
	std::string str;
	str.reserve(count);
	for (size_t i = 0; i < count; ++i)
	{
		size_t current = 0;
		in >> current;
		str.push_back('a' + current - 1);
	}

	return str;
}

void WriteData(const Refrain& data, std::ostream& out)
{
	out << data.RefrainValue << std::endl;
	out << data.Substring.size() << std::endl;

	for (auto c : data.Substring)
	{
		out << size_t(c - 'a' + 1) << " ";
	}
	out << std::endl;
}

void SolveTask()
{
	WriteData(RefrainCalculator<SuffixTree>(ReadData(std::cin)).Result(), std::cout);
}

int main()
{
	SolveTask();

	return 0;
}
