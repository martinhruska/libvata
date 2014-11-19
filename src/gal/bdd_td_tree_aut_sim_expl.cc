#include "bdd_td_tree_aut_sim_expl.hh"

#include "../util/cond_col.hh"
#include "data_types.hh"
#include "../explicit_tree_aut_core.hh"

#include <iostream>
#include <string>

namespace {

	template <class Op>
	void setOp(
			const std::unordered_set<std::string>& a,
			const std::unordered_set<std::string>& b,
			std::unordered_set<std::string>&       res,
			Op                                     op)
	{
		for (const auto& ia : a)
		{
			if (op(ia, b))
			{
				res.insert(ia);
			}
		}
	}


	template<class SymbolType, class StateType, class TupleToSyms, class TupleStore, class ExploredTuples>
	void translateIntersections(
			const StateType            parent,
			const TupleToSyms&         tupleToSyms,
			const TupleStore&          tupleStore, 
			ExploredTuples&            exploredTuples,
			VATA::SymbolTranslator&    translator,
			VATA::ExplicitTreeAutCore& aut)
	{
		for (const auto& a : tupleToSyms)
		{ // translate all common symbols for more transitions
			for (const auto& b : tupleToSyms)
			{
				auto isect = [](
						const SymbolType& lhsItem,
						const std::unordered_set<SymbolType>& rhs) -> bool {
					return rhs.count(lhsItem);
				};
				std::unordered_set<SymbolType> intersect;
				setOp(a.second, b.second, intersect, isect);
				if (intersect.empty())
				{
					continue;
				}

				const size_t translSym = translator.insertItem(intersect);
				aut.AddTransition(tupleStore.at(a.first), translSym, parent);
				exploredTuples.insert(a.first); // mark that it was translated
			}
		}
	}


	template <class TupleToSyms, class StateType, class TupleStore, class ExploredTuples>
	void translateSingleSymbols(
			const StateType            parent,
			const TupleToSyms&         tupleToSyms,
			const TupleStore&          tupleStore, 
			ExploredTuples&            exploredTuples,
			VATA::SymbolTranslator&    translator,
			VATA::ExplicitTreeAutCore& aut)
	{
		for (const auto& i : tupleToSyms)
		{ // add to the result automata symbols that has not been translated yet.
			const auto& tupleInd = i.first;
			if (exploredTuples.count(tupleInd))
			{
				continue;
			}

			for (const auto& sym : i.second)
			{
				const size_t translSym =
					translator.insertItem(std::unordered_set<std::string>({sym}));
				aut.AddTransition(tupleStore.at(tupleInd), translSym, parent);
			}
		}
	}

	template <class StateTupleInd, class SymbolType, class StateType, class TupleStore>
	void translate(
			const VATA::StateToUsed&   used,
			VATA::SymbolTranslator&    translator,
			VATA::ExplicitTreeAutCore& aut,
			const TupleStore&          tupleStore)
	{
		for (const auto& item : used)
		{
			std::unordered_set<StateTupleInd> addedTuples;
			const StateType& parent = item.first;
			const auto& tupleToSyms = item.second;
			
			translateIntersections<SymbolType>(
					parent, tupleToSyms, tupleStore, addedTuples,  translator, aut);

			translateSingleSymbols(parent, tupleToSyms, tupleStore, addedTuples, translator, aut);
			
		}
	}

	void printExpl(
		const VATA::ExplicitTreeAutCore &aut)
	{
		for (const auto& trans : aut)
		{
			std::cerr << trans.GetParent() << " " << trans.GetSymbol() << " (";
			for (const auto state : trans.GetChildren()) std::cerr << state << " ";
			std::cerr << ")\n";
		}
	}
}

template<class Tuple, class TupleStore>
int findTuple(
		Tuple&                                                     tuple,
		TupleStore&                                                tupleStore)
{
	for(size_t j = 0; j < tupleStore.size(); ++j)
	{
		bool eq = true;
		const auto& ts = tupleStore.at(j);
		if (ts.size() != tuple.size())
		{
			continue;
		}

		for(size_t i = 0; i < ts.size(); ++i)
		{
			eq &= tuple.at(i) == ts.at(i);
		}
		
		if (eq)
		{
			return j;
		}
	}

	return -1;
}

void VATA::BDDTopDownSimExpl::loadUsedSymbols(
		const BDDTDTreeAutCore&                                    aut,
		VATA::StateToUsed&                                         stateToUsed,
		TupleStore&                                                tupleStore)
{
	CondColApplyFunctor<StateTupleSet, StateType, StateTuple> collector;

	for (auto stateBddPair : aut.GetStates())
	{	// for all states
		const StateType& state = stateBddPair.first;

		const TransMTBDD& transMtbdd = aut.GetMtbdd(state);

		stateToUsed.insertState(state);

		for (auto strSymbol : aut.GetAlphabet()->GetSymbolDict())
		{	// iterate over all known symbols
			const std::string& symbol = strSymbol.first;
			// ignore rank

			BDD symbolBdd(strSymbol.second, true, false);

			collector.Clear();
			collector(transMtbdd, symbolBdd);

			for (const StateTuple& tuple : collector.GetAccumulator())
			{	// for each state tuple for which there is a transition
				int ind = findTuple(tuple, tupleStore);
				if (ind < 0)
				{
					tupleStore.push_back(StateTuple(tuple));
					ind = tupleStore.size() - 1;
				}
				
				stateToUsed.insertSymbol(state, ind, symbol);
			}
		}
	}
}


void VATA::BDDTopDownSimExpl::Translate(
		const BDDTDTreeAutCore&              aut,
		ExplicitTreeAutCore&                 explAut)
{

	StateToUsed stateToUsed;
	TupleStore tupleStore;
	loadUsedSymbols(aut, stateToUsed, tupleStore);
	
	SymbolTranslator translator;
	translate<StateTupleInd, SymbolType, StateType, TupleStore>(
			stateToUsed, translator, explAut, tupleStore);

	std::cerr << translator << std::endl;
	printExpl(explAut);
}
