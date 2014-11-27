#ifndef _VATA_BDD_TD_SIM_COMPUTER_HH_
#define _VATA_BDD_TD_SIM_COMPUTER_HH_

#include <vata/aut_base.hh>
#include "../explicit_tree_aut_core.hh"



namespace VATA
{
    class ExplicitTreeAutCore;
    class BDDTopDownSimExpl;
    class StateToUsed;
    class TupleStore;
}

class VATA::BDDTopDownSimComputer
{
private:
    using StateDiscontBinaryRelation = VATA::AutBase::StateDiscontBinaryRelation;

public:
    using ExplicitTreeAutCore = VATA::ExplicitTreeAutCore;
    using StateDiscontBinaryRelation = VATA::Util::DiscontBinaryRelation;

public:
    static StateDiscontBinaryRelation ComputeSimulation(
            const ExplicitTreeAutCore& aut
    );
};

#endif
