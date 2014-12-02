#ifndef _VATA_BDD_TD_SIM_COMPUTER_HH_
#define _VATA_BDD_TD_SIM_COMPUTER_HH_

#include <unordered_set>

namespace VATA
{
    class ExplicitTreeAutCore;
    class BDDTopDownSimExpl;
    class StateToUsed;
    class TupleStore;
	class BDDTopDownSimComputer;
	namespace Util
	{
		class DiscontBinaryRelation;
	}
}

class VATA::BDDTopDownSimComputer
{
private:
    //using StateDiscontBinaryRelation = VATA::AutBase::StateDiscontBinaryRelation;

public:
    typedef Util::DiscontBinaryRelation StateDiscontBinaryRelation;

public:
    static StateDiscontBinaryRelation ComputeSimulation(
            const ExplicitTreeAutCore& aut
    );

private:
	static void simRefinement(
        StateDiscontBinaryRelation &sim,
        const std::unordered_set<size_t>& lhs,
        const std::unordered_set<size_t>& rhs,
		const ExplicitTreeAutCore&        aut);

};

#endif
