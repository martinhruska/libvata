#ifndef _VATA_BDD_TD_SIM_EXPL_HH_
#define _VATA_BDD_TD_SIM_EXPL_HH_

#include <vata/aut_base.hh>
#include "../bdd_td_tree_aut_core.hh"

#include <unordered_map>
#include <unordered_set>

namespace VATA
{
	class BDDTopDownSimExpl;
	class StateToUsed;
}
		
class VATA::BDDTopDownSimExpl
{
private: // data types
	using StateDiscontBinaryRelation = VATA::AutBase::StateDiscontBinaryRelation;
	using StateType                  = BDDTDTreeAutCore::StateType;
	using StateTuple                 = BDDTDTreeAutCore::StateTuple;
	using TransMTBDD                 = BDDTDTreeAutCore::TransMTBDD;
	using BDD                        = BDDTDTreeAutCore::BDD;
	using StateTupleSet              = BDDTDTreeAutCore::StateTupleSet;

	using SymbolType                 = std::string;
	
	using UsedSymbols = std::unordered_map<const StateTuple*, std::unordered_set<SymbolType>>;
	friend class StateToUsed;
	friend class SymbolTranslator;

public:
	static StateDiscontBinaryRelation ComputeSimulation(
			const BDDTDTreeAutCore&              aut);

private:
	static void loadUsedSymbols(
			const BDDTDTreeAutCore&              aut,
			StateToUsed&                         stateToUsed);
};

#endif
