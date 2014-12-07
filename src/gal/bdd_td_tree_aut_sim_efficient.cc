#include "bdd_td_tree_aut_sim_efficient.hh"

#include "../explicit_tree_aut_core.hh"
#include <vata/aut_base.hh>

typedef VATA::ExplicitTreeAutCore::SymbolType SymbolType;
typedef VATA::ExplicitTreeAutCore::StateType  StateType;
typedef VATA::ExplicitTreeAutCore::StateTuple StateTuple;
typedef VATA::ExplicitTreeAutCore::TuplePtr   TuplePtr;
typedef VATA::BDDTopDownSimEfficient::StateDiscontBinaryRelation StateDiscontBinaryRelation;
typedef StateDiscontBinaryRelation Sim;

typedef size_t RankType;

// State positions
typedef size_t Position;
typedef std::unordered_set<Position> Positions;
typedef std::unordered_map<StateType, Positions> ParentToPos;
typedef std::unordered_map<SymbolType, ParentToPos> SymToPos;
typedef std::unordered_map<StateType, SymToPos> StatePos;


// Reverse trans
typedef std::unordered_set<StateType> ParentsReverse;
typedef std::unordered_map<SymbolType, ParentsReverse> SymToParents;
typedef std::unordered_map<StateType, SymToParents> ReverseTrans;

// Cardinality
typedef std::unordered_map<RankType, size_t> RankToCard;
typedef std::unordered_map<SymbolType, RankToCard> SymToCard;
typedef std::unordered_map<StateType, SymToCard> Card;

// Counter
typedef std::unordered_map<StateType, size_t> RStateToCounter;
typedef std::unordered_map<StateType, RStateToCounter> LStateToCounter;
typedef std::unordered_map<RankType, LStateToCounter> RankToCounter;
typedef std::unordered_map<SymbolType, RankToCounter> Counter;

// Queue
typedef std::pair<StateType, StateType> QueueItem;
typedef std::vector<QueueItem> Queue;

typedef std::unordered_set<RankType> RankSet;
typedef std::unordered_map<StateType, RankSet> SymbolSet;
typedef std::unordered_set<StateType> StateSet;

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

bool areTuplesSimulated(
	const StateTuple&                 lhs,
	const StateTuple&                 rhs,
	const StateDiscontBinaryRelation& sim)
{
	assert(lhs.size() == rhs.size());
	for (size_t i = 0; i < lhs.size(); ++i)
	{
		if (!sim.get(lhs.at(i), rhs.at(i)))
		{
			return false;
		}
	}

	return true;
}

template<class ValueType, class Key, class Hash>
void addKeyToHash(Hash& hash, const Key& key)
{
	if (!hash.count(key))
	{
		hash[key] = ValueType();
	}
}

template<class ValueType, class Key, class Hash>
void addValueToHash(Hash& hash, const ValueType& value, const Key& key)
{
	if (!hash.count(key))
	{
		hash[key] = value;
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


void addCard(
		Card& card,
		const StateType& parent,
		const SymbolType& symbol,
		const RankType&  rank)
{
	addKeyToHash<SymToCard>(card, parent);
	addKeyToHash<RankToCard>(card[parent], symbol);
	addValueToHash(card[parent][symbol], 0, rank);
}

void addReverse(
		ReverseTrans& reverse,
		const StateType& parent,
		const StateType& state,
		const SymbolType& symbol)
{
	addKeyToHash<SymToParents>(reverse, state);
	addKeyToHash<ParentsReverse>(reverse[state], symbol);
	reverse[state][symbol].insert(parent);
}


void addPositions(
		StatePos& positions,
		const StateType& parent,
		const StateType& state,
		const SymbolType& symbol,
		const Position pos)
{
	addKeyToHash<SymToPos>(positions, state);
	addKeyToHash<ParentToPos>(positions[state], symbol);
	addKeyToHash<Positions>(positions[state][symbol], parent);
	positions[state][symbol][parent].insert(pos);
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
	std::vector<ExplicitTreeAutCore::Transition> leafTrans;

    for (const auto& trans: aut)
    {
		const SymbolType& symbol = trans.GetSymbol();
		const StateType& parent = trans.GetParent();
		const RankType& rank = trans.GetChildren().size();

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
    Sim sim(stateNumber, true, stateNumber-1);
	initRel(sim, true, stateNumber);
	Queue queue;
	initQueueAndSim(queue, sim, aut.GetFinalStates(), stateSet);

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
							if (sim.get(l,k))
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

	for (const auto& symToRank : counter)
	{
		const SymbolType& a = symToRank.first;
		for (const auto& rankToLstate : symToRank.second)
		{
			const RankType& rank = rankToLstate.first;
			for (const auto& lstateToRstate : rankToLstate.second)
			{
				const StateType& q = lstateToRstate.first;
				for (const auto& rstateToCounter : lstateToRstate.second)
				{
					const StateType& p = rstateToCounter.first;
					const size_t& count = rstateToCounter.second;
					if (count == card[q][a][rank])
					{
						for (const StateType& l : reverse[p][a])
						{
							if (sim.get(l,q))
							{
								sim.set(l,q,false);
							}
						}
					}
				}
			}
		}
	}

	sim.resizeRel(stateNumber-1);
	return sim;
}

