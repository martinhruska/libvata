#ifndef _VATA_BDD_TD_SIM_EFFICIENT_HH_
#define _VATA_BDD_TD_SIM_EFFICIENT_HH_

#include <unordered_set>

namespace VATA
{
    class ExplicitTreeAutCore;
    class BDDTopDownSimExpl;
    class StateToUsed;
    class TupleStore;
	class BDDTopDownSimEfficient;
	namespace Util
	{
		class DiscontBinaryRelation;
	}
}

class VATA::BDDTopDownSimEfficient
{
public:
    typedef Util::DiscontBinaryRelation StateDiscontBinaryRelation;

public:
    static StateDiscontBinaryRelation ComputeSimulation(
            const ExplicitTreeAutCore& aut);
};

#endif
