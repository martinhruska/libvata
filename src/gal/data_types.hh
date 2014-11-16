#ifndef _VATA_DATA_TYPES_HH_
#define _VATA_DATA_TYPES_HH_

#include "bdd_td_tree_aut_sim_expl.hh"

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>

namespace VATA {
	class SymbolTranslator;
	class StateToUsed;
}

class VATA::SymbolTranslator :
	public std::unordered_map<int, std::string>
{
private:
	size_t cnt_;
	std::unordered_map<size_t, std::unordered_set<std::string>> items_;
	std::unordered_map<size_t, std::string> translator_;


private:
	static const std::unordered_set<std::string>& syms(
			const std::pair<size_t, std::unordered_set<std::string>>& item)
	{
		return item.second;
	}
	
	
	static size_t index(
			const std::pair<size_t, std::unordered_set<std::string>>& item)
	{
		return item.first;
	}

public:
	SymbolTranslator() :
		cnt_(0), items_(), translator_()
	{}


	size_t insertItem(const std::unordered_set<std::string>& item)
	{
		for (const auto& translated : items_)
		{
			bool eq = true;
			for (const auto& sym : syms(translated))
			{
				if (!item.count(sym))
				{
					eq = false;
					break;
				}
			}

			if (eq)
			{
				return index(translated);
			}
		}

		size_t ind = cnt_;
		items_[ind] = std::unordered_set<std::string>(item);
		cnt_++;

		return ind;
	}
};


class VATA::StateToUsed : public std::unordered_map<
						  VATA::BDDTopDownSimExpl::StateType,
						  VATA::BDDTopDownSimExpl::UsedSymbols>
{

private:
	using StateType   = BDDTopDownSimExpl::StateType;
	using StateTuple  = BDDTopDownSimExpl::StateTuple;
	using SymbolType  = BDDTopDownSimExpl::SymbolType;
	using UsedSymbols = BDDTopDownSimExpl::UsedSymbols;


public:
	bool hasTuple(StateType state, const StateTuple* tuple)
	{
		return (*this).count(state) && (*this)[state].count(tuple);
	}

	void insertSymbol(StateType state, const StateTuple* tuple, SymbolType symbol)
	{
		if (!this->hasTuple(state, tuple))
		{
			(*this)[state][tuple] = std::unordered_set<std::string>();
		}
		(*this)[state][tuple].insert(symbol);
	}

	void insertState(StateType state)
	{
		if (!(*this).count(state))
		{
			(*this)[state] = UsedSymbols();
		}
	}
};

#endif
