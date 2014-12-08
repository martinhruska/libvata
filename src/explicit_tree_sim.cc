/*****************************************************************************
 *  VATA Tree Automata Library
 *
 *  Copyright (c) 2011  Jiri Simacek <isimacek@fit.vutbr.cz>
 *
 *  Description:
 *    Implemention of simulation computation for explicitly represented tree
 *    automata.
 *
 *****************************************************************************/

// VATA headers
#include <vata/aut_base.hh>

#include "explicit_tree_aut_core.hh"
#include "explicit_tree_transl.hh"

#include "gal/bdd_td_tree_aut_sim_computer.hh"
#include "gal/bdd_td_tree_aut_sim_efficient.hh"

using VATA::AutBase;
using VATA::ExplicitTreeAutCore;

using StateBinaryRelation  = AutBase::StateBinaryRelation;
using StateDiscontBinaryRelation  = AutBase::StateDiscontBinaryRelation;


StateDiscontBinaryRelation ExplicitTreeAutCore::ComputeSimulation(
	const VATA::SimParam&                  params) const
{
	switch (params.GetRelation())
	{
		case SimParam::e_sim_relation::TA_UPWARD:
		{
			return this->ComputeUpwardSimulation(params);
		}
		case SimParam::e_sim_relation::TA_DOWNWARD:
		{
			return this->ComputeDownwardSimulation(params);
		}
		default:
		{
			throw std::runtime_error("Unknown simulation parameters: " + params.toString());
		}
	}
}


StateDiscontBinaryRelation ExplicitTreeAutCore::ComputeUpwardSimulation(
	const SimParam&                        params) const
{
	assert(SimParam::e_sim_relation::TA_UPWARD == params.GetRelation());
	if (params.GetNumStates() != static_cast<size_t>(-1))
	{
		return this->ComputeUpwardSimulation(params.GetNumStates());
	}
	else
	{
		throw NotImplementedException(__func__);
	}
}


StateDiscontBinaryRelation ExplicitTreeAutCore::ComputeUpwardSimulation(
	size_t                                 size) const
{
	std::vector<std::vector<size_t>> partition;

	AutBase::StateBinaryRelation relation;

	StateToStateMap translMap;
	size_t stateCnt = 0;
	StateToStateTranslWeak transl(translMap, [&stateCnt](const StateType&)
		{return stateCnt++;});

	ExplicitLTS lts = this->TranslateUpward(partition, relation, VATA::Util::Identity(size), transl);
	StateBinaryRelation ltsSim = lts.computeSimulation(partition, relation, size);
	return StateDiscontBinaryRelation(ltsSim, translMap);
}


StateDiscontBinaryRelation ExplicitTreeAutCore::ComputeDownwardSimulation(
	const SimParam&                        params) const
{
	assert(SimParam::e_sim_relation::TA_DOWNWARD == params.GetRelation());
	if (params.GetNumStates() != static_cast<size_t>(-1))
	{
		if (params.IsBddAlg())
		{
			return BDDTopDownSimComputer::ComputeSimulation(*this);
		}
		else if (params.ISBddAlgEfficient())
		{
			return BDDTopDownSimEfficient::ComputeSimulation(*this);
		}
		return this->ComputeDownwardSimulation(params.GetNumStates());
	}
	else
	{
		throw NotImplementedException(__func__);
	}
}


StateDiscontBinaryRelation ExplicitTreeAutCore::ComputeDownwardSimulation(
	size_t                                 size) const
{
	StateToStateMap translMap;
	size_t stateCnt = 0;
	StateToStateTranslWeak transl(translMap, [&stateCnt](const StateType&)
		{return stateCnt++;});

	ExplicitLTS lts = this->TranslateDownward(transl);
	StateBinaryRelation ltsSim = lts.computeSimulation(size);
	return StateDiscontBinaryRelation(ltsSim, translMap);
}
