#include "bdd_td_tree_aut_sim_computer.hh"

#include "../explicit_tree_aut_core.hh"
#include <vata/aut_base.hh>

typedef VATA::ExplicitTreeAutCore::SymbolType SymbolType;
typedef VATA::ExplicitTreeAutCore::StateType  StateType;
typedef VATA::ExplicitTreeAutCore::StateTuple StateTuple;

typedef std::vector<std::pair<SymbolType, const StateTuple*>> SuperState;
typedef std::unordered_map<size_t, SuperState> AritySuperstates;

typedef std::unordered_map<const StateTuple*, std::unordered_set<StateType>> Parents;

VATA::BDDTopDownSimComputer::StateDiscontBinaryRelation VATA::BDDTopDownSimComputer::ComputeSimulation(
        const ExplicitTreeAutCore& aut
)
{
    StateDiscontBinaryRelation prevSim;
	const size_t stateNumber = aut.GetStatesNumber();
    StateDiscontBinaryRelation sim = StateDiscontBinaryRelation(stateNumber*stateNumber, true, stateNumber);

    AritySuperstates container;
    Parents parents;

    for (const auto &trans: aut)
    {
        const size_t arity = trans.GetChildren().size();
        if( container.count(arity) == 0)
        {
            container[arity] = SuperState();
        }
        parents[&trans.GetChildren()].insert(trans.GetParent());
        container[arity].push_back(std::pair<SymbolType, const StateTuple*>(trans.GetSymbol(), &trans.GetChildren()));
    }

    while(prevSim != sim)
    {
        prevSim = sim;
        for(const auto& arity_pair : container)
        {
            for(const auto& q : arity_pair.second)
            {
                auto tmp = std::unordered_set<size_t>();
                for(const auto& r : arity_pair.second)
                {
                    if (q.first != r.first) // symbols differ
                    {
                        continue;
                    }
                    tmp.insert(parents[r.second].begin(), parents[r.second].end());
                }
                simRefinement(sim, parents[q.second], tmp);
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
    for(auto q: lhs)
    {
        for(size_t i = 0; i<sim.size(); i++)
        {
            if(sim.get(q, i))
            {
                if(rhs.count(i) == 0)
                {
                    sim.set(q, i, 0);
                }
            }
        }
    }
}

