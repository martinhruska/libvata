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
}
	
template<class StateTupleSet, class StateType, class StateTuple>
class CondColApplyFunctor;	
		
class VATA::BDDTopDownSimExpl
{
private: // data types
	using StateDiscontBinaryRelation = VATA::AutBase::StateDiscontBinaryRelation;
	using StateType                  = BDDTDTreeAutCore::StateType;
	using StateTuple                 = BDDTDTreeAutCore::StateTuple;
	using StateTupleInd              = size_t;
	using TransMTBDD                 = BDDTDTreeAutCore::TransMTBDD;
	using BDD                        = BDDTDTreeAutCore::BDD;
	using StateTupleSet              = BDDTDTreeAutCore::StateTupleSet;
	using TupleStore                 = std::vector<StateTuple>;

	using SymbolType                 = std::string;
	
	using UsedSymbols = std::unordered_map<StateTupleInd, std::unordered_set<SymbolType>>;
	friend class StateToUsed;
	friend class SymbolTranslator;

public:
	static void Translate(
			const BDDTDTreeAutCore&              aut,
			ExplicitTreeAutCore&                 explAut);

private:
	static void loadUsedSymbols(
			const BDDTDTreeAutCore&                                     aut,
			VATA::StateToUsed&                                          stateToUsed,
			TupleStore&                                                 tupleStore);
};

#endif
