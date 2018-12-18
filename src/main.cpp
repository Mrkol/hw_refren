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
		str.push_back('a' + current);
	}

	return str;
}

std::pair<std::string, size_t> Refrain(std::string str)
{
	RefrainCalculator<SuffixAutomaton> calculator(str);

	auto[l, r, occ] = calculator.Calculate();

	return {str.substr(l, r - l), occ*(r-l)};
}

void WriteData(std::pair<std::string, size_t> data, std::ostream& out)
{
	out << data.second << std::endl;
	out << data.first.size() << std::endl;

	for (auto c : data.first)
	{
		out << size_t(c - 'a') << " ";
	}
	std::cout << std::endl;
}

int main()
{
	WriteData(Refrain(ReadData(std::cin)), std::cout);
	
	return 0;
}
