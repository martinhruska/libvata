/*****************************************************************************
 *  VATA Tree Automata Library
 *
 *  Copyright (c) 2014  Ondrej Lengal <ilengal@fit.vutbr.cz>
 *
 *  Description:
 *    Header file for the SimParam structure
 *
 *****************************************************************************/

#ifndef _SIM_PARAM_HH_
#define _SIM_PARAM_HH_

#include <cassert>
#include <string>

namespace VATA
{
	class SimParam
	{
	public:   // data types

		enum class e_sim_relation
		{
			TA_DOWNWARD,
			TA_UPWARD,
			FA_FORWARD,
			FA_BACKWARD
		};

		enum class e_sim_spec
		{
			NO,
			SPEC,
			SPEC1
		};

	private:  // data members

		/// the relation to be computed
		e_sim_relation relation_ = static_cast<e_sim_relation>(-1);

		/**
		 * @brief  Number of states of the automaton
		 *
		 * This field contains the number of states of the automaton on which the
		 * relation is to be computed. This is present so that it does not have to
		 * be recomputed. -1 denotes the @e unknown value.
		 */
		size_t numStates_ = static_cast<size_t>(-1);

		/**
		 * @brief Use of algorithm created in GAL lecture
		 */
		e_sim_spec bddAlg_ = static_cast<e_sim_spec>(-1);

	public:   // methods

		void SetRelation(e_sim_relation rel)
		{
			relation_ = rel;
		}

		e_sim_relation GetRelation() const
		{
			return relation_;
		}

		void SetNumStates(size_t numStates)
		{
			numStates_ = numStates;
		}

		size_t GetNumStates() const
		{
			return numStates_;
		}

		void SetBddAlg()
		{
			bddAlg_ = e_sim_spec::SPEC;
		}

		void SetBddAlgEfficient()
		{
			bddAlg_ = e_sim_spec::SPEC1;
		}

		bool IsBddAlg() const
		{
			return bddAlg_ == e_sim_spec::SPEC;
		}

		bool ISBddAlgEfficient() const
		{
			return bddAlg_ == e_sim_spec::SPEC1;
		}

		std::string toString() const
		{
			std::string result = "SimParam relation: ";
			switch (this->GetRelation())
			{
				case e_sim_relation::TA_DOWNWARD:
				{
					result += "TA_DOWNWARD";
					break;
				}
				case e_sim_relation::TA_UPWARD:
				{
					result += "TA_UPWARD";
					break;
				}
				case e_sim_relation::FA_FORWARD:
				{
					result += "FA_FORWARD";
					break;
				}
				case e_sim_relation::FA_BACKWARD:
				{
					result += "FA_BACKWARD";
					break;
				}
				default:
				{
					assert(false);     // fail gracefully
				}
			}
			
			result += ". Use GAL alg: ";
			if (IsBddAlg())
			{
				result += "BDD Alg";
			}
			else if (ISBddAlgEfficient())
			{
				result += "BDD Alg efficient";
			}
			else
			{
				result += "No";
			}

			return result;
		}
	};
}

#endif /* _SIM_PARAM_HH_ */
