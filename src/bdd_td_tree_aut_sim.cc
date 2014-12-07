/*****************************************************************************
 *  VATA Tree Automata Library
 *
 *  Copyright (c) 2011  Ondra Lengal <ilengal@fit.vutbr.cz>
 *
 *  Description:
 *    Source file of computation of downward inclusion on top-down represented
 *    tree automaton.
 *
 *****************************************************************************/

// VATA headers
#include <vata/bdd_td_tree_aut.hh>

#include "gal/bdd_td_tree_aut_sim_expl.hh"
#include "gal/bdd_td_tree_aut_sim_computer.hh"
#include "gal/bdd_td_tree_aut_sim_efficient.hh"
#include "bdd_td_tree_aut_core.hh"
#include "explicit_tree_aut_core.hh"
#include "loadable_aut.hh"

using VATA::BDDTopDownTreeAut;
using VATA::Util::Convert;

typedef VATA::AutBase::StateDiscontBinaryRelation StateDiscontBinaryRelation;

namespace {

template<class SimComputer>
StateDiscontBinaryRelation computeSimulationExpl(
		const VATA::BDDTDTreeAutCore&  core)
{
		VATA::ExplicitTreeAutCore aut;
		VATA::BDDTopDownSimExpl::Translate(core, aut);
		return SimComputer::ComputeSimulation(aut);
}

}

StateDiscontBinaryRelation BDDTopDownTreeAut::ComputeSimulation(
	const SimParam&                params) const
{
	if (params.IsBddAlg())
	{
		return computeSimulationExpl<VATA::BDDTopDownSimComputer>(*(this->core_));
	}
	else if (params.ISBddAlgEfficient())
	{
		return computeSimulationExpl<VATA::BDDTopDownSimEfficient>(*(this->core_));
	}

	throw NotImplementedException(__func__);
}
