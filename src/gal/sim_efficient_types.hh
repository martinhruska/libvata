/*****************************************************************************
 *  VATA Tree Automata Library
 *
 *  Copyright (c) 2014 Martin Hruska, Petr Sebek <{xhrusk16, xsebek06}@fit.vutbr.cz>
 *
 *  Description:
 *  Data types used in BDDTopDownSimEfficient class
 *
 *  Nothe:
 *  Part of project for GAL lecture
 *
 *****************************************************************************/

#ifndef _VATA_SIM_EFFICIENT_TYPES_HH_
#define _VATA_SIM_EFFICIENT_TYPES_HH_

#include "../explicit_tree_aut_core.hh"

#include <unordered_map>
#include <unordered_set>

#include <deque>
#include <vector>

namespace VATA
{
	namespace Util
	{
		class DiscontBinaryRelation;
	}

	namespace EfficientTypes
	{

		// General types
		typedef VATA::ExplicitTreeAutCore::SymbolType SymbolType;
		typedef VATA::ExplicitTreeAutCore::StateType  StateType;
		typedef VATA::ExplicitTreeAutCore::StateTuple StateTuple;
		typedef VATA::ExplicitTreeAutCore::TuplePtr   TuplePtr;
		typedef Util::DiscontBinaryRelation StateDiscontBinaryRelation;
		typedef StateDiscontBinaryRelation Sim;

		typedef size_t RankType;

		typedef std::unordered_set<RankType> RankSet;
		typedef std::unordered_map<SymbolType, RankSet> SymbolSet;
		typedef std::unordered_set<StateType> StateSet;
		typedef std::vector<size_t> RowType;
		typedef std::vector<RowType> Matrix;

		// State positions
		typedef size_t Position;
		typedef std::unordered_set<Position> Positions;
		typedef std::unordered_map<StateType, Positions> ParentToPos;
		typedef std::unordered_map<SymbolType, ParentToPos> SymToPos;
		typedef std::unordered_map<StateType, SymToPos> StatePos;

		// State to symbols
		typedef std::unordered_set<SymbolType> PureSymbolSet;
		typedef std::unordered_map<RankType, PureSymbolSet> RankToSymbols;
		typedef std::unordered_map<StateType, RankToSymbols> StateToSyms;

		// Reverse trans
		typedef std::unordered_set<StateType> ParentsReverse;
		typedef std::unordered_map<SymbolType, ParentsReverse> SymToParents;
		typedef std::unordered_map<StateType, SymToParents> ReverseTrans;

		// Cardinality
		typedef std::unordered_map<RankType, size_t> RankToCard;
		typedef std::unordered_map<SymbolType, RankToCard> SymToCard;
		typedef std::unordered_map<StateType, SymToCard> Card;

		// Counter
		typedef std::unordered_map<RankType, Matrix> RankToCounter;
		typedef std::unordered_map<SymbolType, RankToCounter> Counter;

		// Queue
		typedef std::pair<StateType, StateType> QueueItem;
		typedef std::vector<QueueItem> Queue;
	}
}

#endif
