/*****************************************************************************
 *  VATA Tree Automata Library
 *
 *  Copyright (c) 2014 Martin Hruska, Petr Sebek <{xhrusk16, xsebek06}@fit.vutbr.cz>
 *
 *  Description:
 *  Conversion of bdd td NTA to explicit NTA reducing number of used symbols.
 *
 *  Nothe:
 *  Part of project for GAL lecture
 *
 *****************************************************************************/

#ifndef _VATA_BDD_TD_SIM_EXPL_HH_
#define _VATA_BDD_TD_SIM_EXPL_HH_

#include <vata/aut_base.hh>
#include "../bdd_td_tree_aut_core.hh"

#include <unordered_map>
#include <unordered_set>

namespace VATA
{
	class ExplicitTreeAutCore;
	class BDDTopDownSimExpl;
	class StateToUsed;
	class TupleStore;
}
	
template<class StateTupleSet, class StateType, class StateTuple>
class CondColApplyFunctor;	

/*
 * Class contains methods for conversion of BDD Top Down NTA to
 * explicitly represented one
 */
class VATA::BDDTopDownSimExpl
{
	friend class StateToUsed;
	friend class SymbolTranslator;
	friend class TupleStore;

private: // data types
	using StateDiscontBinaryRelation = VATA::AutBase::StateDiscontBinaryRelation;
	using StateTupleInd              = size_t;
	using TransMTBDD                 = BDDTDTreeAutCore::TransMTBDD;
	using BDD                        = BDDTDTreeAutCore::BDD;
	using StateTupleSet              = BDDTDTreeAutCore::StateTupleSet;
	using SymbolType                 = std::string;
	
public: // data types
	using StateType                  = BDDTDTreeAutCore::StateType;
	using UsedSymbols                = std::unordered_map<StateTupleInd, std::unordered_set<SymbolType>>;
	using StateTuple                 = BDDTDTreeAutCore::StateTuple;
	
public:

	static void Translate(
			const BDDTDTreeAutCore&              aut,
			ExplicitTreeAutCore&                 explAut);

private:

	/**
	 * Method creates a structure mapping parent state to its children tuples
	 * and these tuples to the symbols under whom is possible to go from the parent to a children tuple.
	 */
	static void loadUsedSymbols(
			const BDDTDTreeAutCore&             aut,
			VATA::StateToUsed&                  stateToUsed,
			TupleStore&                         tupleStore);
};

#endif
