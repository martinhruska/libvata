#include "bdd_td_tree_aut_sim_computer.hh"
#include <vata/aut_base.hh>

VATA::BDDTopDownSimComputer::StateDiscontBinaryRelation VATA::BDDTopDownSimComputer::ComputeSimulation(
        const ExplicitTreeAutCore& aut
)
{
    StateDiscontBinaryRelation sim = StateDiscontBinaryRelation(aut.states.size() * aut.states.size(), true, aut.states());
    StateDiscontBinaryRelation sim = StateDiscontBinaryRelation(aut.states.size() * aut.states.size(), true, aut.states());
}


