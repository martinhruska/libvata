#include "bdd_td_tree_aut_sim_expl.hh"

#include "../util/cond_col.hh"
#include "data_types.hh"
#include "../explicit_tree_aut_core.hh"

#include <iostream>
#include <string>

namespace {

	template <class Op, class SymbolSet>
	SymbolSet setOp(
			const SymbolSet&                       a,
			const SymbolSet&                       b,
			Op                                     op)
	{
		SymbolSet res;
		for (const auto& ia : a)
		{
			if (op(ia, b))
			{
				res.insert(ia);
			}
		}

		return res;
	}


	template<class StateType, class StateTuple, class SymbolSet>
	void addTrans(
			const StateType            parent,
			const StateTuple&          tuple,
			const SymbolSet&           syms,
			SymbolSet&                 processedSymbols,
			VATA::SymbolTranslator&    translator,
			VATA::ExplicitTreeAutCore& aut)
	{
		if (!syms.empty())
		{
			const size_t translSym = translator.insertItem(syms);
			aut.AddTransition(tuple, translSym, parent);
			processedSymbols.insert(syms.begin(), syms.end());
		}
	}


	/**
	 * Translates set of the symbols to a new symbol when the translated set is common
	 * for more transitions. Eq. when there is
	 * a(q1) -> p, b(q1) -> p, a(q2) -> p, b(q2) -> p,
	 * then {a,b} is mapped to, e.g., A
	 */
	template<class SymbolType, class StateType, class TupleToSyms>
	void translateParent(
			const StateType            parent,
			const TupleToSyms&         tupleToSyms,
			const VATA::TupleStore&    tupleStore, 
			VATA::SymbolTranslator&    translator,
			VATA::ExplicitTreeAutCore& aut)
	{
		for (const auto& a : tupleToSyms)
		{
			std::unordered_set<SymbolType> processedSymbols;
			for (const auto& b : tupleToSyms)
			{
				if (a.first == b.first)
				{
					continue;
				}

				auto isect = [](
						const SymbolType& lhsItem,
						const std::unordered_set<SymbolType>& rhs) -> bool {
					return rhs.count(lhsItem);
				};
				addTrans(parent, tupleStore.at(a.first),
						setOp(a.second, b.second, isect), processedSymbols,
						translator, aut);
			}

			auto diff = [](
					const SymbolType& lhsItem,
					const std::unordered_set<SymbolType>& rhs) -> bool {
				return !rhs.count(lhsItem);
			};
			addTrans(parent, tupleStore.at(a.first),
						setOp(a.second, processedSymbols, diff), processedSymbols,
						translator, aut);
		}
	}


	template <class StateTupleInd, class SymbolType, class StateType>
	void internalTranslate(
			const VATA::StateToUsed&   used,
			VATA::SymbolTranslator&    translator,
			VATA::ExplicitTreeAutCore& aut,
			const VATA::TupleStore&    tupleStore)
	{
		for (const auto& item : used)
		{
			const StateType& parent = item.first;
			const auto& tupleToSyms = item.second;
			
			translateParent<SymbolType>(
					parent, tupleToSyms, tupleStore, translator, aut);
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


// Documentation in a header file
void VATA::BDDTopDownSimExpl::loadUsedSymbols(
		const BDDTDTreeAutCore&               aut,
		VATA::StateToUsed&                    stateToUsed,
		TupleStore&                           tupleStore)
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
				int ind = tupleStore.findTuple(tuple);
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
	internalTranslate<StateTupleInd, SymbolType, StateType>(
			stateToUsed, translator, explAut, tupleStore);

	//std::cerr << translator << std::endl;
	//printExpl(explAut);
}
