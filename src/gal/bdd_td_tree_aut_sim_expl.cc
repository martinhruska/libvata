#include "bdd_td_tree_aut_sim_expl.hh"

#include "../util/cond_col.hh"
#include "data_types.hh"
#include <vata/explicit_tree_aut.hh>

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


	template <class StateTuple, class SymbolType, class StateType>
	void translate(
			const VATA::StateToUsed&   used,
			VATA::SymbolTranslator&    translator,
			VATA::ExplicitTreeAut&     aut)
	{
		for (const auto& item : used)
		{
			std::unordered_set<const StateTuple*> addedTuples;
			const StateType& parent = item.first;
			const auto& tupleToSyms = item.second;
			
			for (const auto& a : tupleToSyms)
			{
				for (const auto& b : tupleToSyms)
				{
					if (a.first == b.first)
					{ // pointer to state tuples are same -> continue
						continue;
					}

					auto isect = [](
							const SymbolType& a,
							const std::unordered_set<SymbolType>& b) -> bool {
						return b.count(a);
					};
					std::unordered_set<SymbolType> intersect;
					setOp(a.second, b.second, intersect, isect);

					const size_t translSym = translator.insertItem(intersect);
					aut.AddTransition(*a.first, translSym, parent);
					addedTuples.insert(a.first);
				}
			}

			for (const auto& i : tupleToSyms)
			{
				const StateTuple *tuple = i.first;
				if (addedTuples.count(tuple))
				{
					continue;
				}

				for (const auto& sym : i.second)
				{
					const size_t translSym =
						translator.insertItem(std::unordered_set<std::string>({sym}));
					aut.AddTransition(*tuple, translSym, parent);
				}
			}
		}
	}
}


void VATA::BDDTopDownSimExpl::loadUsedSymbols(
		const BDDTDTreeAutCore&               aut,
		VATA::StateToUsed&                    stateToUsed)
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
				stateToUsed.insertSymbol(state, &tuple, symbol);
			}
		}
	}
}


VATA::BDDTopDownSimExpl::StateDiscontBinaryRelation VATA::BDDTopDownSimExpl::ComputeSimulation(
		const BDDTDTreeAutCore&              aut)
{

	StateToUsed stateToUsed;
	loadUsedSymbols(aut, stateToUsed);
	
	SymbolTranslator translator;
	ExplicitTreeAut explAut;
	translate<StateTuple, SymbolType, StateType>(stateToUsed, translator, explAut);
	
	return StateDiscontBinaryRelation();
}
