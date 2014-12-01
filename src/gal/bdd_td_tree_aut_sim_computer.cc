#include "bdd_td_tree_aut_sim_computer.hh"
#include "../explicit_tree_aut_core.hh"
#include <vata/aut_base.hh>

VATA::BDDTopDownSimComputer::StateDiscontBinaryRelation VATA::BDDTopDownSimComputer::ComputeSimulation(
        const ExplicitTreeAutCore& aut
)
{
    StateDiscontBinaryRelation prevSim;
	const size_t stateNumber = aut.GetStatesNumber();
    StateDiscontBinaryRelation sim = StateDiscontBinaryRelation(stateNumber*stateNumber, true, stateNumber);

//    while(prevSim != sim)
//    {
//        prevSim = sim;
//        for(auto trans&: aut)
//        {
//            trans.GetSymbol();
//        }
//    }

	return sim;
}


