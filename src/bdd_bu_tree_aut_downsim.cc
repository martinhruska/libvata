/*****************************************************************************
 *  Vojnar's Army Tree Automata Library
 *
 *  Copyright (c) 2011  Ondra Lengal <ilengal@fit.vutbr.cz>
 *
 *  Description:
 *    Source file of computation of downward inclusion on bottom-up
 *    represented tree automaton.
 *
 *****************************************************************************/

// VATA headers
#include <vata/vata.hh>
#include <vata/bdd_bu_tree_aut.hh>
#include <vata/bdd_bu_tree_aut_op.hh>
#include <vata/mtbdd/apply3func.hh>

// Standard library headers
#include <stack>

using VATA::BDDBottomUpTreeAut;
using VATA::BDDTopDownTreeAut;
using VATA::Util::Convert;

typedef VATA::AutBase::StateBinaryRelation StateBinaryRelation;
typedef VATA::AutBase::StateType StateType;
typedef VATA::AutBase::StateToStateMap StateToStateMap;
typedef VATA::AutBase::StateToStateTranslator StateToStateTranslator;

typedef BDDTopDownTreeAut::StateTuple StateTuple;
typedef BDDTopDownTreeAut::StateTupleSet StateTupleSet;

typedef BDDBottomUpTreeAut::StateSet StateSet;

typedef std::map<StateType, size_t> CounterElementMap;
typedef VATA::MTBDDPkg::OndriksMTBDD<CounterElementMap> CounterMTBDD;
typedef std::unordered_map<StateTuple, CounterMTBDD, boost::hash<StateTuple>>
	CounterHT;

typedef BDDTopDownTreeAut::TransMTBDD TopDownMTBDD;

typedef std::pair<StateTuple, StateTuple> RemoveElement;
typedef std::stack<RemoveElement, std::list<RemoveElement>> RemoveList;

typedef BDDBottomUpTreeAut::TransTable BUTransTable;

template <class ArbitraryFunction>
void forAllTuplesWithMatchingStatesDo(
	const BUTransTable& tuples, const StateType& lhsState,
	const StateType& rhsState, ArbitraryFunction func)
{
	for (auto lhsTupleBddPair : tuples)
	{
		const StateTuple& lhsTuple = lhsTupleBddPair.first;

		std::vector<size_t> matchedPositions;
		for (size_t i = 0; i < lhsTuple.size(); ++i)
		{
			if (lhsTuple[i] == lhsState)
			{
				matchedPositions.push_back(i);
			}
		}

		if (matchedPositions.empty())
		{
			continue;
		}

		for (auto rhsTupleBddPair : tuples)
		{
			const StateTuple& rhsTuple = rhsTupleBddPair.first;

			if (rhsTuple.size() != rhsTuple.size())
			{
				continue;
			}

			size_t i;
			for (i = 0; i < matchedPositions.size(); ++i)
			{
				if ((rhsTuple[matchedPositions[i]] == lhsState) &&
					(rhsTuple[matchedPositions[i]] == rhsState))
				{
					break;
				}
			}

			if (i == matchedPositions.size())
			{
				continue;
			}

			func(lhsTuple, rhsTuple);
		}
	}
}

inline bool componentWiseSim(const StateBinaryRelation& sim,
	const StateTuple& lhsTuple, const StateTuple& rhsTuple)
{
	for (size_t i = 0; i < lhsTuple.size(); ++i)
	{
		if (!sim.get(lhsTuple[i], rhsTuple[i]))
		{
			return false;
		}
	}

	return true;
}


GCC_DIAG_OFF(effc++)
class InitCntApplyFctor :
	public VATA::MTBDDPkg::Apply2Functor<InitCntApplyFctor, StateTupleSet,
	CounterElementMap, CounterElementMap>
{
GCC_DIAG_ON(effc++)

private:  // data members

	const StateType& state_;

public:   // methods

	InitCntApplyFctor(const StateType& state) :
		state_(state)
	{ }

	CounterElementMap ApplyOperation(const StateTupleSet& lhs,
		const CounterElementMap& rhs)
	{
		CounterElementMap result = rhs;
		if (!result.insert(std::make_pair(state_, lhs.size())).second)
		{
			assert(false);
		}

		return result;
	}
};


GCC_DIAG_OFF(effc++)
class InitRefineApplyFctor :
	public VATA::MTBDDPkg::VoidApply2Functor<InitRefineApplyFctor, StateTupleSet,
	StateTupleSet>
{
GCC_DIAG_ON(effc++)

private:  // data members

	bool& isSim_;

public:   // methods

	InitRefineApplyFctor(bool& isSim) :
		isSim_(isSim)
	{ }

	void ApplyOperation(const StateTupleSet& lhs, const StateTupleSet& rhs)
	{
		if (!lhs.empty() && rhs.empty())
		{
			isSim_ = false;
			stopProcessing();
		}
	}
};

GCC_DIAG_OFF(effc++)
class RefineApplyFctor :
	public VATA::MTBDDPkg::Apply3Functor<RefineApplyFctor, StateSet, StateSet,
	CounterElementMap, CounterElementMap>
{
GCC_DIAG_ON(effc++)

private:  // data members

	const BUTransTable& transTable_;
	StateBinaryRelation& sim_;
	RemoveList& remove_;

public:   // methods

	RefineApplyFctor(const BUTransTable& transTable, StateBinaryRelation& sim,
		RemoveList& remove) :
		transTable_(transTable),
		sim_(sim),
		remove_(remove)
	{ }

	CounterElementMap ApplyOperation(const StateSet& upR, const StateSet& upQ,
		const CounterElementMap& cntQ)
	{
		if (upR.empty())
		{
			return cntQ;
		}

		VATA_LOGGER_INFO("upR = " + Convert::ToString(upR) + ", upQ = " + Convert::ToString(upQ) + ", cntQ = " + Convert::ToString(cntQ));
		CounterElementMap result = cntQ;

		for (const StateType& s : upR)
		{
			CounterElementMap::iterator itCnt;
			if ((itCnt = result.find(s)) == result.end())
			{
				assert(false);    // fail gracefully
			}

			if (--(itCnt->second) == 0)
			{
				for (const StateType& p : upQ)
				{
					if (sim_.get(p, s))
					{	// if 's' simulates 'p'
						forAllTuplesWithMatchingStatesDo(
							transTable_, p, s,
							[this](const StateTuple& pTuple, const StateTuple& sTuple){
								if (componentWiseSim(sim_, pTuple, sTuple))
								{
									remove_.push(std::make_pair(pTuple, sTuple));
								}}
							);

						// 's' no longer simulates 'p'
						sim_.set(p, s, false);
					}
				}

			}

		}

		return cntQ;
	}

};



StateBinaryRelation VATA::ComputeDownwardSimulation(
	const BDDBottomUpTreeAut& aut)
{
	BDDBottomUpTreeAut newAut;

	StateType stateCnt = 0;
	StateToStateMap stateMap;
	StateToStateTranslator stateTrans(stateMap,
		[&stateCnt](const StateType&){return stateCnt++;});

	aut.ReindexStates(newAut, stateTrans);

	return ComputeDownwardSimulation(newAut, stateCnt);
}

StateBinaryRelation VATA::ComputeDownwardSimulation(
	const BDDBottomUpTreeAut& aut, const size_t& states)
{
	StateBinaryRelation sim(states);

	BDDTopDownTreeAut topDownAut = aut.GetTopDownAut();

	CounterMTBDD initCnt((CounterElementMap()));

	StateType firstState;
	InitCntApplyFctor initCntFctor(firstState);

	bool isSim;
	InitRefineApplyFctor initRefFctor(isSim);

	RemoveList remove;

	for (auto firstStateBddPair : topDownAut.GetStates())
	{
		firstState = firstStateBddPair.first;
		const TopDownMTBDD& firstBdd = firstStateBddPair.second;

		initCnt = initCntFctor(firstBdd, initCnt);

		for (auto secondStateBddPair : topDownAut.GetStates())
		{
			const StateType& secondState = secondStateBddPair.first;
			const TopDownMTBDD& secondBdd = secondStateBddPair.second;
			isSim = true;
			initRefFctor(firstBdd, secondBdd);
			if (isSim)
			{
				sim.set(firstState, secondState, true);
			}
			else
			{	// prune
				forAllTuplesWithMatchingStatesDo(
					aut.GetTransTable(), firstState, secondState,
					[&remove](const StateTuple& firstTuple, const StateTuple& secondTuple){
						remove.push(std::make_pair(firstTuple, secondTuple));}
					);
			}
		}
	}

	CounterHT cnt;
	for (auto tupleBddPair : aut.GetTransTable())
	{
		cnt.insert(std::make_pair(tupleBddPair.first, initCnt));
	}

	RefineApplyFctor refineFctor(aut.GetTransTable(), sim, remove);

	while (!remove.empty())
	{
		RemoveElement elem = remove.top();
		remove.pop();

		CounterHT::iterator itCnt;
		if ((itCnt = cnt.find(elem.first)) == cnt.end())
		{
			assert(false);       // fail gracefully
		}

		itCnt->second = refineFctor(aut.GetMtbdd(elem.second),
			aut.GetMtbdd(elem.first), itCnt->second);
	}

	return sim;
}
