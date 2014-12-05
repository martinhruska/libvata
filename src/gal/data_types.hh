#ifndef _VATA_DATA_TYPES_HH_
#define _VATA_DATA_TYPES_HH_

#include "bdd_td_tree_aut_sim_expl.hh"

#include <ostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace VATA {
	class SymbolTranslator;
	class StateToUsed;
	class TupleStore;
}

class VATA::SymbolTranslator :
	public std::unordered_map<int, std::string>
{
private:
	size_t cnt_;
	std::unordered_map<size_t, std::unordered_set<std::string>> items_;


public:
	SymbolTranslator() :
		cnt_(0), items_()
	{}


	size_t insertItem(const std::unordered_set<std::string>& item)
	{
		for (const auto& translated : items_)
		{
			if (translated.second.size() != item.size())
			{
				continue;
			}
			bool eq = true;
			for (const auto& sym : translated.second)
			{
				if (!item.count(sym))
				{
					eq = false;
					break;
				}
			}

			if (eq)
			{
				return translated.first;
			}
		}

		size_t ind = cnt_;
		items_.insert(std::make_pair(ind, std::unordered_set<std::string>(item)));
		cnt_++;

		return ind;
	}


	friend std::ostream& operator<<(std::ostream& os, const SymbolTranslator& trans)
	{
		for (const auto& item : trans.items_)
		{
			std::cerr << "{";
			for (const auto& sym : item.second)
			{
				std::cerr << sym << ", ";
			}
			std::cerr << "}:" << item.first << ", ";
		}

		return os;
	}
};


class VATA::StateToUsed : public std::unordered_map<
						  VATA::BDDTopDownSimExpl::StateType,
						  VATA::BDDTopDownSimExpl::UsedSymbols>
{

private:
	using StateType      = BDDTopDownSimExpl::StateType;
	using StateTupleInd  = size_t;
	using SymbolType     = BDDTopDownSimExpl::SymbolType;
	using UsedSymbols    = BDDTopDownSimExpl::UsedSymbols;


public:
	bool hasTuple(StateType state, const StateTupleInd tupleInd)
	{
		return (*this).count(state) && (*this)[state].count(tupleInd);
	}

	void insertSymbol(StateType state, const StateTupleInd tupleInd, SymbolType symbol)
	{
		if (!this->hasTuple(state, tupleInd))
		{
			(*this)[state][tupleInd] = std::unordered_set<std::string>();
		}
		(*this)[state][tupleInd].insert(symbol);
	}

	void insertState(StateType state)
	{
		if (!(*this).count(state))
		{
			(*this)[state] = UsedSymbols();
		}
	}
};


class VATA::TupleStore : public std::vector<VATA::BDDTopDownSimExpl::StateTuple>
{
private:
	using StateTuple  = BDDTopDownSimExpl::StateTuple;
public:
	int findTuple(const StateTuple& tuple)
	{
		for(size_t j = 0; j < this->size(); ++j)
		{
			bool eq = true;
			const auto& ts = this->at(j);
			if (ts.size() != tuple.size())
			{
				continue;
			}

			for(size_t i = 0; i < ts.size(); ++i)
			{ // compare two tuples
				eq &= tuple.at(i) == ts.at(i);
			}
			
			if (eq)
			{
				return j;
			}
		}

		return -1;
	}
};

#endif
