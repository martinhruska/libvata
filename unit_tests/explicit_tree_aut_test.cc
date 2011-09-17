/*****************************************************************************
 *  Vojnar's Army Tree Automata Library
 *
 *  Copyright (c) 2011  Ondra Lengal <ilengal@fit.vutbr.cz>
 *
 *  Description:
 *    Test suite for explicit tree automaton
 *
 *****************************************************************************/

// VATA headers
#include <vata/vata.hh>
#include <vata/explicit_tree_aut.hh>
#include <vata/explicit_tree_aut_op.hh>

class AutTypeFixture
{
protected:// data types

	typedef VATA::ExplicitTreeAut<size_t> AutType;

protected:// data members

	AutType::SymbolType nextSymbol_;
	AutType::StateType nextState_;

protected:// methods

	AutTypeFixture() :
		nextSymbol_(0),
		nextState_(0)
	{
		AutType::SetNextStatePtr(&nextState_);
	}

	virtual ~AutTypeFixture() { }
};

#include "tree_aut_test.hh"