/*****************************************************************************
 *  VATA Tree Automata Library
 *
 *  Copyright (c) 2014 Martin Hruska, Petr Sebek <{xhrusk16, xsebek06}@fit.vutbr.cz>
 *
 *  Description:
 *  Computing simulation using modification of Ilie Navaro algorithm for NTA
 *
 *  Nothe:
 *  Part of project for GAL lecture
 *
 *****************************************************************************/

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

/*
 * Class containg method for computing simulation over NTA
 */
class VATA::BDDTopDownSimEfficient
{
public:
    typedef Util::DiscontBinaryRelation StateDiscontBinaryRelation;

public:
	/*
	 * Computes simulation over given automata using naive algorithm.
	 * @param[in] aut NTA over which state is simulation computed
	 * @return Simulation relation over states of aut
	 */
    static StateDiscontBinaryRelation ComputeSimulation(
            const ExplicitTreeAutCore& aut);
};

#endif
