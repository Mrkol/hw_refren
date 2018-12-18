#pragma once
#ifndef REFREN_HPP
#define REFREN_HPP

#include <string>

template<typename Structure>
class RefrainCalculator
{
private:
	Structure structure;
	std::string stored;

public:
	RefrainCalculator(std::string str);

	std::tuple<size_t, size_t, size_t> Calculate();
};

template<typename Structure>
RefrainCalculator<Structure>::RefrainCalculator(std::string str)
	: structure(str), stored(str)
{

}

template<typename Structure>
std::tuple<size_t, size_t, size_t> RefrainCalculator<Structure>::Calculate()
{
	typename Structure::Iterator best;

	for (auto i = structure.begin(); i != structure.end(); ++i)
	{
		if (!best ||
			i.GetLength() * i.GetOccurances() > 
			best.GetLength() * best.GetOccurances())
		{
			best = i;
		}
	}

	auto[l, r] = best.GetSlice();
	return {l, r, best.GetOccurances()};
}


#endif // REFREN_HPP
