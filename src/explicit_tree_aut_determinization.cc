#include "explicit_tree_aut_core.hh"

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <vector>

using namespace VATA;

using StateType = ExplicitTreeAutCore::StateType;
using SymbolType = ExplicitTreeAutCore::StateType;

namespace
{
	template <class Item>
	class SuperSetManager
	{
	private: // data types
		using ItemSet = std::unordered_set<Item>;
		struct ItemToSet
		{
			ItemSet set;
			Item item;

			ItemToSet(const ItemSet& s, const Item& i) : set(s), item(i)
			{}

			operator const ItemSet&() const {return set;}
			operator Item() const {return item;}
		};

		using SetList = std::vector<struct ItemToSet>;
		using ItemToSetMap = std::unordered_map<Item, SetList>;

		struct Id
		{
			const Item key;
			const Item listPos;

			Id(const Item& k, const Item& l) : key(k), listPos(l)
			{}


			Id(const Id&& id)
			: key(id.key), listPos(id.listPos)
			{}
		};
		
		using ItemToId = std::unordered_map<Item, Id>;
	
	private: // data members
		ItemToSetMap map_;
		ItemToId cache_;
		Item items_;

	public: // constructors
		SuperSetManager() :
			map_(),
			cache_(),
			items_(1)
		{}

	public: // methods
		Item Insert(const ItemSet& itemSet)
		{
			const size_t key = sum(itemSet);

			if (!map_.count(key))
			{
				map_[key] = SetList();
			}

			Item res = items_;
			const Item foundItem = findInSetList(itemSet, map_.at(key));
			if (!foundItem)
			{
				map_.at(key).push_back(ItemToSet(itemSet, items_));
				cache_.insert(std::pair<Item, Id>(items_,Id(key, map_.at(key).size()-1)));
				++items_;
			}
			else
			{
				res = foundItem;
			}

			return res;
		}

		const ItemSet& Get(const Item& item)
		{
			const Id& id = cache_.at(item);

			return map_.at(id.key).at(id.listPos);
		}



	private: // methods

		// Item has to have associative addition
		Item sum(const ItemSet& itemSet)
		{
			Item acc(0);

			for (const auto& item : itemSet)
			{
				acc += item;
			}

			return acc;
		}

		Item findInSetList(const ItemSet& itemSet, const SetList& setList)
		{
			for (const auto& rhs : setList)
			{
				if (areEqual(itemSet, rhs))
				{
					return rhs;
				}
			}

			return Item(0);
		}

		bool areEqual(const ItemSet& lhs, const ItemSet& rhs)
		{
			for (const auto& li : lhs)
			{
				if (!rhs.count(li))
				{
					return false;
				}
			}

			return lhs.size() == rhs.size();
		}
	};

	template <class SymbolType, class StateType>
	class WordAutomataRepre
	{
	private:
		using StateSet = std::unordered_set<StateType>;
		using SymbolToStateSet = std::unordered_map<SymbolType, StateSet>;
		using WordRepre = std::unordered_map<StateType, SymbolToStateSet>;

	private: 
		WordRepre wordRepre_;

	public:
		WordAutomataRepre() : wordRepre_()
		{}

	public:
		void Insert(StateType lhs, SymbolType symbol, StateType rhs)
		{
			if (!wordRepre_.count(lhs))
			{
				wordRepre_[lhs] = SymbolToStateSet();
			}

			if (!wordRepre_.at(lhs).count(symbol))
			{
				wordRepre_.at(lhs)[symbol] = StateSet();
			}

			
			wordRepre_.at(lhs).at(symbol).insert(rhs);
		}

		const StateSet& Get(const StateType& state, const SymbolType& symbol)
		{
			return wordRepre_.at(state).at(symbol);
		}

		const SymbolToStateSet& GetSymToState(const StateType& state)
		{
			return wordRepre_.at(state);
		}

		size_t count(const StateType& state) const
		{
			return wordRepre_.count(state);
		}
	};

	void copyIfFinal(const ExplicitTreeAutCore& nfa, ExplicitTreeAutCore& dfa,
			const std::unordered_set<StateType>& stateSet, StateType newState)
	{
		for (const StateType s : stateSet)
		{
			if (nfa.IsStateFinal(s))
			{
				dfa.SetStateFinal(newState);
				return;
			}
		}
	}
}

ExplicitTreeAutCore ExplicitTreeAutCore::Determinization() const
{
	ExplicitTreeAutCore explAut;

	SuperSetManager<StateType> superSetManager;
	WordAutomataRepre<StateType, SymbolType> wordRepre_;


	std::unordered_set<StateType> initSet;
	SymbolType sym;
	for (const auto& trans : *this)
	{
		if (trans.GetChildren().size() == 0)
		{
			initSet.insert(trans.GetParent());
			sym = trans.GetSymbol();
		}
		else
		{
			assert(trans.GetChildren().size() == 1);  // just for word automata
			wordRepre_.Insert(trans.GetChildren().at(0), trans.GetSymbol(), trans.GetParent());
		}
	}

	if (initSet.size() == 0)
	{
		return ExplicitTreeAutCore();
	}

	const StateType newState = superSetManager.Insert(initSet);
	copyIfFinal(*this, explAut, initSet, newState);

	std::vector<StateType> todo;
	std::unordered_set<StateType> todoSet;
	std::unordered_set<StateType> processed;

	explAut.AddTransition(StateTuple(), sym, newState);

	todo.push_back(newState);

	while(todo.size())
	{
		//	take from Q super state S
		const StateType actState = todo.back();
		todo.pop_back();
		std::unordered_map<SymbolType, std::unordered_set<StateType>> symbolToSet;

		//	for s \in S
		for (const auto& s : superSetManager.Get(actState))
		{
			if (!wordRepre_.count(s))
			{
				continue;
			}
			for (const auto& symToStates : wordRepre_.GetSymToState(s))
			{
				//	foreach a in \Sigma
				//		map[a] <- p, s.t. (s,a,p) \in delta
				const SymbolType& sym = symToStates.first;
				if (!symbolToSet.count(sym))
				{
					symbolToSet[sym] = std::unordered_set<StateType>();
				}

				symbolToSet[sym].insert(wordRepre_.Get(s,sym).begin(), wordRepre_.Get(s,sym).end());
			}
		}
		
		//	foreach State S in map
		//	   create superstate S in cache
		//	   add S to Q in not in processed P
		for (const auto& symToSetItem : symbolToSet)
		{
			if (symToSetItem.second.empty())
			{
				continue;
			}

			const StateType newState = superSetManager.Insert(symToSetItem.second);
			if (!todoSet.count(newState) && !processed.count(newState))
			{
				todo.push_back(newState);
				todoSet.insert(newState);

				copyIfFinal(*this, explAut, symToSetItem.second, newState);
			}

			explAut.AddTransition(StateTuple(1, actState), symToSetItem.first, newState);
		}

		todoSet.erase(actState);
		processed.insert(actState);
	}

	return explAut;
}
