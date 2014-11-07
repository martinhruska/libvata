#ifndef _VATA_BDD_TD_SIM_EXPL_HH_
#define _VATA_BDD_TD_SIM_EXPL_HH_

#include <vata/aut_base.hh>
#include "bdd_td_tree_aut_core.hh"

namespace VATA
{
	class BDDTopDownSimExpl;
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

public:
	static StateDiscontBinaryRelation ComputeSimulation(
			const BDDTDTreeAutCore&              aut);
};

#endif
