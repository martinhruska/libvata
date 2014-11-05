#ifndef _VATA_BDD_TD_SIM_EXPL_HH_
#define _VATA_BDD_TD_SIM_EXPL_HH_

#include <vata/aut_base.hh>

namespace VATA
{
	class BDDTopDownSimExpl;

	class BDDTDTreeAutCore;
}
		
class VATA::BDDTopDownSimExpl
{
private: // data types
	typedef VATA::AutBase::StateDiscontBinaryRelation StateDiscontBinaryRelation;

public:
	static StateDiscontBinaryRelation ComputeSimulation(
			const BDDTDTreeAutCore&              aut);
};

#endif
