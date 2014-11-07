#include "bdd_td_tree_aut_sim_expl.hh"

#include "util/cond_col.hh"

VATA::BDDTopDownSimExpl::StateDiscontBinaryRelation VATA::BDDTopDownSimExpl::ComputeSimulation(
		const BDDTDTreeAutCore&              aut)
{
		CondColApplyFunctor<StateTupleSet, StateType, StateTuple> collector;

		for (auto stateBddPair : aut.GetStates())
		{	// for all states
			const StateType& state = stateBddPair.first;

			const TransMTBDD& transMtbdd = aut.GetMtbdd(state);

			for (auto strSymbol : aut.GetAlphabet()->GetSymbolDict())
			{	// iterate over all known symbols
				const std::string& symbol = strSymbol.first;
				// ignore rank

				BDD symbolBdd(strSymbol.second, true, false);

				collector.Clear();
				collector(transMtbdd, symbolBdd);

				for (const StateTuple& tuple : collector.GetAccumulator())
				{	// for each state tuple for which there is a transition
					std::vector<std::string> tupleStr;
					for (const StateType& tupState : tuple)
					{	// for each element in the tuple
					}
				}
			}
		}
	return StateDiscontBinaryRelation();
}
