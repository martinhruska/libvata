#include "bdd_td_tree_aut_sim_computer.hh"

#include "../explicit_tree_aut_core.hh"
#include <vata/aut_base.hh>

typedef VATA::ExplicitTreeAutCore::SymbolType SymbolType;
typedef VATA::ExplicitTreeAutCore::StateType  StateType;
typedef VATA::ExplicitTreeAutCore::StateTuple StateTuple;
typedef VATA::ExplicitTreeAutCore::TuplePtr   TuplePtr;
typedef VATA::BDDTopDownSimComputer::StateDiscontBinaryRelation StateDiscontBinaryRelation;

typedef std::vector<std::pair<SymbolType, TuplePtr>> SuperState;
typedef std::unordered_map<size_t, SuperState> AritySuperstates;

typedef std::unordered_map<SymbolType, std::unordered_set<StateType>> Parents;
typedef std::unordered_map<TuplePtr, Parents> InverseTrans;

namespace {

void initRel(
	VATA::BDDTopDownSimComputer::StateDiscontBinaryRelation& rel,
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


}

VATA::BDDTopDownSimComputer::StateDiscontBinaryRelation VATA::BDDTopDownSimComputer::ComputeSimulation(
        const ExplicitTreeAutCore& aut
)
{
	const size_t stateNumber = aut.GetStatesNumber();
    StateDiscontBinaryRelation sim(stateNumber, true, stateNumber);
    StateDiscontBinaryRelation prevSim(stateNumber, true, stateNumber);
	initRel(sim, true, stateNumber);
	initRel(prevSim, false, stateNumber);

    AritySuperstates container;
    InverseTrans parents;

    for (const auto &trans: aut)
    {
        const size_t arity = trans.GetChildren().size();
        if( container.count(arity) == 0)
        {
            container[arity] = SuperState();
        }
		const auto& tuplePtr = aut.FindTuplePtr(trans.GetChildren());
        container[arity].push_back(std::pair<SymbolType, const TuplePtr>(trans.GetSymbol(), tuplePtr));

		if (!parents.count(tuplePtr))
		{
			parents[tuplePtr] = Parents();
		}
		if (!parents[tuplePtr].count(trans.GetSymbol()))
		{
			parents[tuplePtr][trans.GetSymbol()] = std::unordered_set<StateType>();
		}
	
		parents[tuplePtr][trans.GetSymbol()].insert(trans.GetParent());
    }

    while(prevSim != sim)
    {
		for (size_t i = 0; i < sim.size(); ++i)
		{
			for (size_t j = 0; j < sim.size(); ++j)
			{
				prevSim.set(i,j,sim.get(i,j));
			}
		}

        for(const auto& arity_pair : container)
        {
            for(const auto& q : arity_pair.second)
            {
                auto tmp = std::unordered_set<StateType>();
                for(const auto& r : arity_pair.second)
                {
                    if (q.first != r.first) // symbols differ
                    {
                        continue;
                    }

					if (!areTuplesSimulated(*q.second, *r.second, sim))
					{
						continue;
					}

                    tmp.insert(parents[r.second][q.first].begin(), parents[r.second][q.first].end());
                }
                simRefinement(sim, parents[q.second][q.first], tmp);
            }
        }
    }
	return sim;
}


void VATA::BDDTopDownSimComputer::simRefinement(
        StateDiscontBinaryRelation &sim,
        const std::unordered_set<size_t>& lhs,
        const std::unordered_set<size_t>& rhs)
{
    for(auto q : lhs)
    {
        for(size_t i = 0; i < sim.size(); ++i)
        {
            if(sim.get(q, i))
            {
                if(rhs.count(i) == 0)
                {
                    sim.set(q, i, false);
                }
            }
        }
    }
}

