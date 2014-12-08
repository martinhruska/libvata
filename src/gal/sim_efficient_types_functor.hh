#ifndef _VATA_SIM_EFFICIENT_TYPES_FUNCTOR_HH_
#define _VATA_SIM_EFFICIENT_TYPES_FUNCTOR_HH_

#include "sim_efficient_types.hh"

using namespace VATA::EfficientTypes;

namespace VATA
{
	namespace EfficientTypesFunctor
	{
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

		template<class SetType>
		bool areSetsEqual(const SetType& lhs, const SetType rhs)
		{
			if (lhs.size() != rhs.size())
			{
				return false;
			}

			for (const auto& litem : lhs)
			{
				if (!rhs.count(litem))
				{
					return false;
				}
			}

			return true;
		}

		template<class SetType>
		bool isSubsetEq(const SetType& lhs, const SetType rhs)
		{
			if (lhs.size() > rhs.size())
			{
				return false;
			}

			for (const auto& litem : lhs)
			{
				if (!rhs.count(litem))
				{
					return false;
				}
			}

			return true;
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


		void addStateToSym(
				StateToSyms& stateToSyms,
				const StateType& state,
				const RankType&  rank,
				const SymbolType& symbol)
		{
			addKeyToHash<RankToSymbols>(stateToSyms, state);
			addKeyToHash<PureSymbolSet>(stateToSyms[state], rank);
			stateToSyms[state][rank].insert(symbol);
		}

		void addSymbolToCounter(
				Counter& counter,
				const SymbolType& symbol)
		{
			addKeyToHash<RankToCounter>(counter, symbol);
		}

		void addMatrixToCounter(
				Counter& counter,
				const SymbolType& symbol,
				const RankType& rank,
				const size_t stateNumber)
		{
			counter[symbol][rank] = Matrix(stateNumber, RowType(stateNumber, 0));
		}
	}
}

#endif
