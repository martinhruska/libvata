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

	/**
	 * Translates bdd top down automata to explicit representation
	 * @param[in]  aut BDD top down automata to be converted
	 * @param[out] explAut create explicit automaton
	 */
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
