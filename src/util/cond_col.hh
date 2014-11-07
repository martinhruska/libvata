#ifndef _VATA_COND_COL_APPLY_FUNCTOR_
#define _VATA_COND_COL_APPLY_FUNCTOR_

#include "../mtbdd/apply2func.hh"

#include <list>

GCC_DIAG_OFF(effc++)
template <
	class StateSet,
	class StateType,
	class AccumulatorItemType>
class CondColApplyFunctor :
	public VATA::MTBDDPkg::VoidApply2Functor<
		CondColApplyFunctor<StateSet, StateType, AccumulatorItemType>,  StateSet, bool>
{
GCC_DIAG_ON(effc++)

public:   // data types

	typedef std::list<AccumulatorItemType> AccumulatorType;

private:  // data members

	AccumulatorType accumulator_;

public:

	CondColApplyFunctor() :
		accumulator_()
	{ }

	inline const AccumulatorType& GetAccumulator() const
	{
		return accumulator_;
	}

	inline void Clear()
	{
		accumulator_.clear();
	}

	inline void ApplyOperation(const StateSet& lhs, const bool& rhs)
	{
		if (rhs)
		{
			accumulator_.insert(accumulator_.end(), lhs.begin(), lhs.end());
		}
	}
};

#endif
