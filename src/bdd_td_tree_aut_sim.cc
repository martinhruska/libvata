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
#include "bdd_td_tree_aut_core.hh"
#include "loadable_aut.hh"

using VATA::BDDTopDownTreeAut;
using VATA::Util::Convert;

typedef VATA::AutBase::StateDiscontBinaryRelation StateDiscontBinaryRelation;

StateDiscontBinaryRelation BDDTopDownTreeAut::ComputeSimulation(
	const SimParam&                params) const
{
	if (params.GetBddAlg())
	{
		return BDDTopDownSimExpl::ComputeSimulation(*(this->core_));
	}

	throw NotImplementedException(__func__);
}
