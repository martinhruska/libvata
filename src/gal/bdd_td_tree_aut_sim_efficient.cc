#include "bdd_td_tree_aut_sim_efficient.hh"

#include "sim_efficient_types.hh"
#include "sim_efficient_types_functor.hh"

#include "../explicit_tree_aut_core.hh"
#include <vata/aut_base.hh>

using namespace VATA::EfficientTypes;
using namespace VATA::EfficientTypesFunctor;

namespace {

void initRel(
	VATA::BDDTopDownSimEfficient::StateDiscontBinaryRelation& rel,
	const bool defval,
	const size_t size)
{
	for (size_t i = 0; i < size; ++i)
	{
		for (size_t j = 0; j < size; ++j)
		{
			rel.set(i,j,defval);
		}
	}
}

void initQueueAndSim(
		Queue& queue,
		Sim&   sim,
		const StateSet& finals,
		const StateSet& all)
{
	for (const StateType& fstate : finals)
	{
		for (const StateType state : all)
		{
			if (finals.count(state))
			{
				continue;
			}

			queue.push_back(QueueItem(fstate, state));
			sim.set(fstate, state, false);
		}
	}
}

void initCounter(
		Counter& counter,
		const StateSet& stateSet,
		const SymbolSet& symbolSet)
{
	for (const auto& symbolToRanks : symbolSet)
	{
		const SymbolType& symbol = symbolToRanks.first; 
		addKeyToHash<RankToCounter>(counter, symbol);

		for (const auto& rank : symbolToRanks.second)
		{
			addKeyToHash<LStateToCounter>(counter[symbol], rank);
			for (const auto& lstate : stateSet)
			{
				addKeyToHash<RStateToCounter>(counter[symbol][rank], lstate);
				for (const auto& rstate : stateSet)
				{
					addValueToHash(counter[symbol][rank][lstate], 0, rstate);
				}
			}
		}
	}
}

void symbolsCheck(
		Queue& queue,
		Sim&   sim,
		const StateSet& all,
		const StateToSyms& stateToSyms)
{
	for (const StateType& lstate : all)
	{
		for (const StateType rstate : all)
		{
			if (!sim.get(lstate, rstate))
			{
				continue;
			}

			if ((stateToSyms.count(lstate) != stateToSyms.count(rstate)) ||
					(stateToSyms.count(lstate) && !isSubsetEq(stateToSyms.at(lstate), stateToSyms.at(rstate))))
			{
				queue.push_back(QueueItem(lstate, rstate));
				sim.set(lstate, rstate, false);
			}
		}
	}
}

bool isSimBreak(const Positions& qpos, const ParentToPos& pPos)
{
	for(const Position& pos : qpos)
	{
		for (const auto& symToPos : pPos)
		{
			if (symToPos.second.count(pos))
			{
				return true;
			}
		}
	}

	return false;
}

}

VATA::BDDTopDownSimEfficient::StateDiscontBinaryRelation VATA::BDDTopDownSimEfficient::ComputeSimulation(
        const ExplicitTreeAutCore& aut
)
{
	Card card;
	StateSet stateSet;
	SymbolSet symbolSet;
	ReverseTrans reverse;
	StatePos positions;
	StateToSyms stateToSyms;
	std::vector<ExplicitTreeAutCore::Transition> leafTrans;

    for (const auto& trans: aut)
    {
		const SymbolType& symbol = trans.GetSymbol();
		const StateType& parent = trans.GetParent();
		const RankType& rank = trans.GetChildren().size();

		addStateToSym(stateToSyms, parent, symbol);

		addCard(card, parent, symbol, rank);
		card[parent][symbol][rank] += 1;
		
		stateSet.insert(parent);

		const auto& children = trans.GetChildren();
		for (size_t i = 0; i < children.size(); ++i)
		{
			const StateType& state = children[i];
			stateSet.insert(state);
			addReverse(reverse, parent, state, symbol);
			addPositions(positions, parent, state, symbol, i);
		}
		if (children.size() == 0)
		{
			leafTrans.push_back(trans);
		}

		addKeyToHash<RankSet>(symbolSet, symbol);
		symbolSet[symbol].insert(rank);
	}
	
	const StateType mock = stateSet.size();
	for (const auto& trans : leafTrans)
	{
		const SymbolType& symbol = trans.GetSymbol();
		const StateType& parent = trans.GetParent();

		stateSet.insert(mock);
		addReverse(reverse, parent, mock, symbol);
		addPositions(positions, parent, mock, symbol, 0);
	}

	Counter counter;
	initCounter(counter, stateSet, symbolSet);
	
	const size_t stateNumber = stateSet.size();
    Sim sim(stateNumber, true, stateNumber);
	initRel(sim, true, stateNumber);
	Queue queue;
	initQueueAndSim(queue, sim, aut.GetFinalStates(), stateSet);
	
	symbolsCheck(queue, sim, stateSet, stateToSyms);

	while (queue.size())
	{
		const QueueItem item = queue.back();
		const StateType p = item.first;
		const StateType q = item.second;
		queue.pop_back();

		for (const auto& symRankPair : symbolSet)
		{
			const SymbolType& a = symRankPair.first;

			for (const RankType& rank : symRankPair.second)
			{
				for (const StateType& k : reverse[q][a])
				{
					if (isSimBreak(positions[q][a][k], positions[p][a]))
					{
						counter[a][rank][k][p] += 1;
					}

					if (counter[a][rank][k][p] == card[k][a][rank])
					{
						for (const StateType& l : reverse[p][a])
						{
							if (l != k && sim.get(l,k))
							{
								sim.set(l,k,false);
								queue.push_back(QueueItem(l,k));
							}
						}
					}
				}
			}
		}
	}

	sim.resizeRel(stateNumber-(stateSet.size() - mock));
	return sim;
}

