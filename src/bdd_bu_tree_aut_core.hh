/*****************************************************************************
 *  VATA Tree Automata Library
 *
 *  Copyright (c) 2013  Ondra Lengal <ilengal@fit.vutbr.cz>
 *
 *  Description:
 *    Header file for the core of an BDD-based bottom-up tree automaton.
 *
 *****************************************************************************/

#ifndef _VATA_BDD_BU_TREE_AUT_CORE_HH_
#define _VATA_BDD_BU_TREE_AUT_CORE_HH_

// VATA headers
#include <vata/bdd_bu_tree_aut.hh>
#include <vata/util/aut_description.hh>

// MTBDD headers
#include "mtbdd/apply1func.hh"
#include "mtbdd/apply2func.hh"
#include "mtbdd/ondriks_mtbdd.hh"
#include "mtbdd/void_apply2func.hh"

#include "symbolic_tree_aut_base_core.hh"
#include "util/bdd_bu_trans_table.hh"
#include "util/cond_col.hh"
#include "bdd_bu_tt_wrapper.hh"
#include "explicit_tree_aut_core.hh"


GCC_DIAG_OFF(effc++)
class VATA::BDDBUTreeAutCore : public SymbolicTreeAutBaseCore
{
GCC_DIAG_ON(effc++)

public:   // data types

	using StateSet        = VATA::Util::OrdVector<StateType>;
	using StateTupleSet   = Util::OrdVector<StateTuple>;
	using StateHT         = std::unordered_set<StateType>;

	using MTBDDHandle     = size_t;
	using TransTable      = Util::TransTableWrapper<MTBDDHandle, StateSet>;
	using TransMTBDD      = TransTable::TransMTBDD;

private:  // data types

	using BDD             = MTBDDPkg::OndriksMTBDD<bool>;

	GCC_DIAG_OFF(effc++)    // suppress missing virtual destructor warning
	class UnionApplyFunctor :
		public VATA::MTBDDPkg::Apply2Functor<UnionApplyFunctor, StateSet,
		StateSet, StateSet>
	{
	GCC_DIAG_ON(effc++)
	public:   // methods

		StateSet ApplyOperation(const StateSet& lhs, const StateSet& rhs)
		{
			return lhs.Union(rhs);
		}
	};

private:  // constants

	static const MTBDDHandle DEFAULT_HANDLE = 0;

private:  // data members

	TransTable transTable_;
	FinalStateSet finalStates_;

private:  // methods


	template <
		class StateTranslFunc,
		class SymbolTranslFunc
		>
	void loadFromAutDescSymbolic(
		const AutDescription&     /* desc */,
		StateTranslFunc&          /* stateTransl */,
		SymbolTranslFunc&         /* symbolTransl */,
		const std::string&        /* params */)
	{
		throw NotImplementedException(__func__);
	}


	template <
		class StateBackTranslFunc>
	AutDescription dumpToAutDescSymbolic(
		StateBackTranslFunc     stateBackTransl,
		const AlphabetType&     /*alphabet */) const
	{
		AutDescription desc;

		// copy final states
		for (const StateType& fst : finalStates_)
		{	// copy final states
			desc.finalStates.insert(stateBackTransl(fst));
			desc.states.insert(stateBackTransl(fst));
		}

		// copy states, transitions and symbols
		for (auto tupleBddPair : this->GetTransTable())
		{	// for all states
			const StateTuple& children = tupleBddPair.first;

			std::vector<std::string> tupleStr;

			for (const StateType& state : children)
			{
				std::string stateStr = stateBackTransl(state);

				tupleStr.push_back(stateStr);
				desc.states.insert(stateStr);
			}

			const TransMTBDD& transMtbdd = tupleBddPair.second;
			TransMTBDD::SymVarToValueList paths = transMtbdd.GetPaths();

			for (const std::pair<SymbolicVarAsgn, TransMTBDD::DataType> path : paths)
			{
				for (const StateType& state : path.second)
				{
					std::string stateStr = stateBackTransl(state);
					std::string symbolStr = path.first.ToString();

					desc.transitions.insert(AutDescription::Transition(tupleStr,
						symbolStr, stateStr));
				}
			}
		}

		return desc;
	}


	template <
		class StateBackTranslFunc>
	AutDescription dumpToAutDescExplicit(
		StateBackTranslFunc       stateBackTransl,
		const AlphabetType&       alphabet) const
	{
		AutDescription desc;

		// copy final states
		for (const StateType& fst : finalStates_)
		{	// copy final states
			desc.finalStates.insert(stateBackTransl(fst));
			desc.states.insert(stateBackTransl(fst));
		}

		CondColApplyFunctor<StateSet, StateType, StateType> collector;

		// copy states, transitions and symbols
		for (auto tupleBddPair : this->GetTransTable())
		{	// for all states
			const StateTuple& children = tupleBddPair.first;

			std::vector<std::string> tupleStr;

			for (const StateType& state : children)
			{
				std::string stateStr = stateBackTransl(state);

				tupleStr.push_back(stateStr);
				desc.states.insert(stateStr);
			}

			const TransMTBDD& transMtbdd = tupleBddPair.second;

			for (auto strSymbol : alphabet->GetSymbolDict())
			{	// iterate over all known symbols
				const std::string& symbol = strSymbol.first;
				// ignore rank

				BDD symbolBdd(strSymbol.second, true, false);

				collector.Clear();
				collector(transMtbdd, symbolBdd);

				for (const StateType& state : collector.GetAccumulator())
				{	// for each state tuple for which there is a transition
					std::string stateStr = stateBackTransl(state);

					desc.transitions.insert(AutDescription::Transition(tupleStr, symbol,
						stateStr));
				}
			}
		}

		return desc;
	}


	ExplicitTreeAutCore translateToExplicit(
			std::unordered_map<size_t, SymbolicVarAsgn>* transl=nullptr) const;


	template <
		class StateTranslFunc,
		class SymbolTranslFunc
		>
	void loadFromAutDescExplicit(
		const AutDescription&      desc,
		StateTranslFunc&           stateTransl,
		SymbolTranslFunc&          symbolTransl,
		const std::string&         /* params */)
	{
		for (const AutDescription::State& fst : desc.finalStates)
		{	// traverse final states
			finalStates_.insert(stateTransl(fst));
		}

		for (const AutDescription::Transition& trans : desc.transitions)
		{	// traverse the transitions
			const AutDescription::StateTuple& childrenStr = trans.first;
			const std::string& symbolStr = trans.second;
			const AutDescription::State& parentStr = trans.third;

			// translate the parent state
			StateType parent = stateTransl(parentStr);

			// translate children
			StateTuple children;
			for (const std::string& tupState : childrenStr)
			{	// for all children states
				children.push_back(stateTransl(tupState));
			}

			// translate the symbol
			SymbolType symbol = symbolTransl(symbolStr);

			this->AddTransition(children, symbol, parent);
		}
	}


public:   // methods

	explicit BDDBUTreeAutCore(
		AlphabetType&       alphabet = SymbolicTreeAutBaseCore::globalAlphabet_) :
		SymbolicTreeAutBaseCore(alphabet),
		transTable_(),
		finalStates_()
	{ }


	BDDBUTreeAutCore(
		const BDDBUTreeAutCore&           aut) :
		SymbolicTreeAutBaseCore(aut),
		transTable_(aut.transTable_),
		finalStates_(aut.finalStates_)
	{ }


	BDDBUTreeAutCore(
		BDDBUTreeAutCore&&                aut) :
		SymbolicTreeAutBaseCore(aut),
		transTable_(std::move(aut.transTable_)),
		finalStates_(std::move(aut.finalStates_))
	{ }


	explicit BDDBUTreeAutCore(
		const TransTable&               transTable);


	BDDBUTreeAutCore& operator=(
		const BDDBUTreeAutCore&         rhs);


	BDDBUTreeAutCore& operator=(
		BDDBUTreeAutCore&&              rhs);


	const TransTable& GetTransTable() const
	{
		return transTable_;
	}

	const TransMTBDD& GetMtbdd(
		const StateTuple&          children) const
	{
		return transTable_.GetMtbdd(children);
	}

	void SetMtbdd(
		const StateTuple&      children,
		const TransMTBDD&      mtbdd)
	{
		transTable_.SetMtbdd(children, mtbdd);
	}

	static bool ShareTransTable(
		const BDDBUTreeAutCore&       lhs,
		const BDDBUTreeAutCore&       rhs)
	{
		return lhs.transTable_.GetTable() == rhs.transTable_.GetTable();
	}

	template <class OperationFunc>
	static void ForeachUpSymbolFromTupleAndTupleSetDo(
		const BDDBUTreeAutCore&     lhs,
		const BDDBUTreeAutCore&     rhs,
		const StateTuple&           lhsTuple,
		const StateTupleSet&        rhsTupleSet,
		OperationFunc&              opFunc)
	{
		GCC_DIAG_OFF(effc++)
		class OperationApplyFunctor :
			public VATA::MTBDDPkg::VoidApply2Functor<OperationApplyFunctor,
			StateSet, StateSet>
		{
		GCC_DIAG_ON(effc++)

		private:  // data members

			OperationFunc& opFunc_;

		public:   // methods

			OperationApplyFunctor(OperationFunc& opFunc) :
				opFunc_(opFunc)
			{ }

			inline void ApplyOperation(const StateSet& lhs, const StateSet& rhs)
			{
				auto AccessElementF = [](const StateType& state){return state;};
				opFunc_(lhs, AccessElementF, rhs, AccessElementF);

				if (opFunc_.IsProcessingStopped())
				{	// in case the operator wants to stop processing
					this->stopProcessing();
				}
			}
		};

		UnionApplyFunctor unioner;
		TransMTBDD rhsUnionMtbdd((StateSet()));

		// collect the RHS's MTBDDs leaves
		for (auto tuple : rhsTupleSet)
		{
			rhsUnionMtbdd = unioner(rhsUnionMtbdd, rhs.GetMtbdd(tuple));
		}

		// create apply functor
		OperationApplyFunctor opApplyFunc(opFunc);

		// perform the apply operation
		opApplyFunc(lhs.GetMtbdd(lhsTuple), rhsUnionMtbdd);
	}


	template <class Index>
	TransMTBDD ReindexStates(
		BDDBUTreeAutCore&          dstAut,
		Index&                     stateTrans) const;


	BDDBUTreeAutCore ReindexStates(
		StateToStateTranslWeak&    stateTrans) const;


protected:// methods


	template <
		class StateTranslFunc,
		class SymbolTranslFunc>
	void loadFromAutDescInternal(
		const AutDescription&       desc,
		StateTranslFunc&            stateTransl,
		SymbolTranslFunc&           symbolTransl,
		const std::string&          params = "")
	{
		if (params == "symbolic")
		{
			loadFromAutDescSymbolic(desc, stateTransl, symbolTransl, params);
		}
		else
		{
			loadFromAutDescExplicit(desc, stateTransl, symbolTransl, params);
		}
	}


	template <
		class StateBackTranslFunc>
	AutDescription dumpToAutDescInternal(
		StateBackTranslFunc                        stateBackTransl,
		const AlphabetType&                        alphabet,
		const std::string&                         params = "") const
	{
		AutDescription desc;
		if ("symbolic" == params)
		{
			desc = dumpToAutDescSymbolic(stateBackTransl, alphabet);
		}
		else
		{
			desc = dumpToAutDescExplicit(stateBackTransl, alphabet);
		}

		return desc;
	}


public:   // methods


	std::string DumpToDot() const;


	void SetStateFinal(
		const StateType&       state)
	{
		finalStates_.insert(state);
	}


	bool IsStateFinal(
		const StateType&       state) const
	{
		return (finalStates_.cend() != finalStates_.find(state));
	}


	const StateHT& GetFinalStates() const
	{
		return finalStates_;
	}



	void AddTransition(
		const StateTuple&      children,
		SymbolType             symbol,
		const StateType&       parent);


	BDDBUTreeAutCore RemoveUselessStates() const;


	/**
	 * @brief  Removes bottom-up unreachable states of the automaton
	 *
	 * Removes bottom-up unreachable states of the automaton. If @p
	 * reachableStates is provided, the bottom-up reachable states are collected
	 * into this container.
	 *
	 * @param[out]  reachableStates  Container for bottom-up reachable states (or
	 *                               @p nullptr)
	 *
	 * @returns  The copy of the automaton without bottom-up unreachable states
	 */
	BDDBUTreeAutCore RemoveUnreachableStates(
		StateHT*               reachableStates = nullptr) const;


	static BDDBUTreeAutCore Union(
		const BDDBUTreeAutCore&        lhs,
		const BDDBUTreeAutCore&        rhs,
		AutBase::StateToStateMap*      pTranslMapLhs = nullptr,
		AutBase::StateToStateMap*      pTranslMapRhs = nullptr);


	static BDDBUTreeAutCore UnionDisjointStates(
		const BDDBUTreeAutCore&        lhs,
		const BDDBUTreeAutCore&        rhs);


	static BDDBUTreeAutCore Intersection(
		const BDDBUTreeAutCore&        lhs,
		const BDDBUTreeAutCore&        rhs,
		AutBase::ProductTranslMap*     pTranslMap = nullptr);


	static bool CheckInclusion(
		const BDDBUTreeAutCore&     smaller,
		const BDDBUTreeAutCore&     bigger)
	{
		InclParam inclParam;
		// TODO: set some more sensible defaults

		return BDDBUTreeAutCore::CheckInclusion(smaller, bigger, inclParam);
	}


	static bool CheckInclusion(
		const BDDBUTreeAutCore&     smaller,
		const BDDBUTreeAutCore&     bigger,
		const VATA::InclParam&      params);
	
	BDDBUTreeAutCore Complement() const;


	BDDTDTreeAutCore GetTopDownAut() const;

	StateDiscontBinaryRelation ComputeSimulation(
		const SimParam&                 /* params */) const;

	BDDBUTreeAutCore Reduce() const;

	StateBinaryRelation ComputeDownwardSimulation() const;

	StateBinaryRelation ComputeDownwardSimulation(
		size_t                    size) const;

	StateBinaryRelation ComputeDownwardSimulation(
		const SimParam&                    params) const;

	StateBinaryRelation ComputeUpwardSimulation() const;

	StateBinaryRelation ComputeUpwardSimulation(
		size_t                    size) const;

	StateBinaryRelation ComputeUpwardSimulation(
		const SimParam&                    params) const;
};

#endif
