#include <iostream>
#include "SuffixAutomaton.hpp"
#include "SuffixTree.hpp"
#include "RefrainCalculator.hpp"


int main()
{
	std::string str;
	std::cin >> str;

	SuffixTree tree(str);

	std::cout << tree;
	
	return 0;
}
