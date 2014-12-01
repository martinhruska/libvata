/*****************************************************************************
 *  VATA Tree Automata Library
 *
 *  Copyright (c) 2011  Jiri Simacek <isimacek@fit.vutbr.cz>
 *
 *  Description:
 *    Binary relation class.
 *
 *****************************************************************************/

#ifndef _VATA_BINARY_RELATION_HH_
#define _VATA_BINARY_RELATION_HH_

// VATA headers
#include <vata/vata.hh>
#include <vata/util/convert.hh>
#include <vata/util/transl_weak.hh>
#include <vata/util/two_way_dict.hh>

// Standard library headers
#include <vector>
#include <algorithm>
#include <unordered_map>

namespace VATA
{
	namespace Util
	{
		class BinaryRelation;
		class Identity;
		class DiscontBinaryRelation;
	}
}


/**
 * @brief  A binary relation address continuously
 *
 * A binary relation addressed from indices from 0 to @p size_ - 1
 */
class VATA::Util::BinaryRelation
{
	std::vector<bool> data_;
	size_t rowSize_;
	size_t size_;

protected:

	void realloc(size_t newRowSize, bool defVal)
	{
		// check for sane parameters
		assert(0 < newRowSize);

		std::vector<bool> tmp(newRowSize*newRowSize, defVal);
		std::vector<bool>::const_iterator src = data_.begin();
		std::vector<bool>::iterator dst = tmp.begin();
		for (size_t i = 0; i < size_; ++i) {
			std::copy(src, src + size_, dst);
			src += rowSize_;
			dst += newRowSize;
		}
		std::swap(data_, tmp);
		rowSize_ = newRowSize;
	}

	void grow(size_t newSize, bool defVal = false)
	{
		assert(rowSize_ <= newSize);

		size_t newRowSize = rowSize_;
		while (newRowSize <= newSize)
			newRowSize <<= 1;
		assert(newSize <= newRowSize);
		this->realloc(newRowSize, defVal);

	}
/*
	void shrinkToFit() {

		assert(this->rowSize_ > this->size_);
		size_t newRowSize = this->rowSize_;
		while (this->size_ < (newRowSize >> 1))
			newRowSize >>= 1;
		assert(this->size_ <= newRowSize);
		this->realloc(newRowSize);

	}
*/
public:

	void reset(bool defVal)
	{
		std::fill(data_.begin(), data_.end(), defVal);
	}

	void resize(size_t size, bool defVal = false)
	{

		if (rowSize_ < size)
		{
			this->grow(size, defVal);
		}

		size_ = size;
	}

	size_t alloc()
	{
		if (size_ >= rowSize_)
		{
			this->grow(size_ + 1);
		}

		return size_++;
	}

	size_t split(size_t i, bool reflexive = true)
	{
		assert(i < size_);

		if (size_ >= rowSize_)
		{
			this->grow(size_ + 1);
		}

		assert((size_ + 1)*rowSize_ <= data_.size());

		// fill collumns
		auto src = data_.begin() + i;
		auto end = src + size_*rowSize_;
		auto dst = data_.begin() + size_;

		for ( ; src != end; src += rowSize_, dst += rowSize_)
		{
			*dst = *src;
		}

		// fill rows
		src = data_.begin() + i*rowSize_;
		dst = data_.begin() + size_*rowSize_;

		std::copy(src, src + size_, dst);

		// set the reflexive bit
		*(dst + size_) = reflexive;

		return size_++;
	}

	bool get(size_t r, size_t c) const
	{
		assert(r < size_ && c < size_);
		assert(r*rowSize_ + c < data_.size());

		return data_[r*rowSize_ + c];
	}

	void set(size_t r, size_t c, bool v)
	{
		assert(r < size_ && c < size_);
		assert(r*rowSize_ + c < data_.size());

		data_[r*rowSize_ + c] = v;
	}

	size_t size() const
	{
		return size_;
	}

public:

	using IndexType    = std::vector<std::vector<size_t>>;

	BinaryRelation(
		size_t         size = 0,
		bool           defVal = false,
		size_t         rowSize = 16) :
		data_(rowSize*rowSize, defVal),
		rowSize_(rowSize),
		size_(0)
	{
		this->resize(size, defVal);
	}

/*
	BinaryRelation(const BinaryRelation& rel)
		: data_(rel.data_), rowSize_(rel.rowSize_), size_(rel.size_) {}
*/

	BinaryRelation(const std::vector<std::vector<bool> >& rel) :
		data_(16*16, false),
		rowSize_(16),
		size_(0)
	{
		this->resize(rel.size(), false);
		for (size_t i = 0; i < rel.size(); ++i)
		{
			assert(rel[i].size() == rel.size());
			for (size_t j = 0; j < rel.size(); ++j)
			{
				this->set(i, j, rel[i][j]);
			}
		}
	}

	// TODO: does this do what is expected? What is expected this should do???
	// WHAT ABOUT SOME FREAKING DOCUMENTATION??????
	bool sym(size_t row, size_t column) const
	{
		return this->get(row, column) && this->get(column, row);
	}

	// build equivalence classes
	void buildClasses(std::vector<size_t>& headIndex) const
	{
		headIndex.resize(size_);
		std::vector<size_t> head;
		for (size_t i = 0; i < size_; ++i)
		{
			size_t j = 0;
			while ((j < head.size()) && !this->sym(i, head[j]))
			{
				++j;
			}

			if (j < head.size())
			{
				headIndex[i] = head[j];
			}
			else
			{
				headIndex[i] = i;
				head.push_back(i);
			}
		}
	}

	// build equivalence classes
	void buildClasses(
		std::vector<size_t>&       index,
		std::vector<size_t>&       head) const
	{
		index.resize(size_);
		head.clear();

		for (size_t i = 0; i < size_; ++i)
		{
			size_t j = 0;
			while ((j < head.size()) && !this->sym(i, head[j]))
			{
				++j;
			}

			if (j < head.size())
			{
				index[i] = j;
			}
			else
			{
				index[i] = head.size();
				head.push_back(i);
			}
		}
	}

	// and composition
	BinaryRelation& operator&=(const BinaryRelation& rhs)
	{
		assert(size_ == rhs.size_);

		for (size_t i = 0; i < size_; ++i)
		{
			for (size_t j = 0; j < size_; ++j)
			{
				this->set(i, j, this->get(i, j) && rhs.get(i,j));
			}
		}

		return *this;
	}

	// transposition
	BinaryRelation& transposed(BinaryRelation& dst) const
	{
		dst.resize(size_);
		for (size_t i = 0; i < size_; ++i)
		{
			for (size_t j = 0; j < size_; ++j)
			{
				dst.set(j, i, this->get(i, j));
			}
		}

		return dst;
	}


	/**
	 * @brief  Creates a mapping from elements to their images
	 *
	 * This method maps elements to their images. Formally, it creates for each
	 * element 'x' the mapping 'x -> Y' where 'Y = {y | xRy}'
	 *
	 * @param[out]  dst  The result mapping every element to its images
	 *
	 * @see  buildInvIndex
	 */
	void buildIndex(IndexType& dst) const
	{
		dst.resize(size_);

		auto rowStart = data_.begin();
		auto src = rowStart;
		auto end = src + size_*rowSize_;
		auto dstIter = dst.begin();

		for (; src != end; src = rowStart, ++dstIter)
		{
			auto rowEnd = src + size_;
			rowStart = src + rowSize_;

			for (size_t i = 0; src != rowEnd; ++src, ++i)
			{
				if (*src)
				{
					dstIter->push_back(i);
				}
			}
		}
	}

	/**
	 * @brief  Creates a mapping from elements to their co-images
	 *
	 * This method maps elements to their co-images. Formally, it creates for each
	 * element 'x' the mapping 'x -> Y' where 'Y = {y | yRx}'
	 *
	 * @param[out]  dst  The result mapping every element to its co-images
	 *
	 * @see buildIndex
	 */
	void buildInvIndex(IndexType& dst) const
	{
		dst.resize(size_);

		for (size_t i = 0; i < size_; ++i)
		{
			for (size_t j = 0; j < size_; ++j)
			{
				if (this->get(i, j))
				{
					dst[j].push_back(i);
				}
			}
		}
	}

	/**
	 * @brief  Creates a mapping from elements to their images and co-images
	 *
	 * This method the call to buildIndex and buildInvIndex, i.e. it maps
	 * elements to their images (in @p ind) and their co-images (in @p inv).
	 *
	 * @param[out]  ind  The result mapping every element to its images
	 * @param[out]  ind  The result mapping every element to its co-images
	 *
	 * @see buildIndex buildInvIndex
	 */
	void buildIndex(IndexType& ind, IndexType& inv) const
	{
		ind.resize(size_);
		inv.resize(size_);

		for (size_t i = 0; i < size_; ++i)
		{
			for (size_t j = 0; j < size_; ++j)
			{
				if (this->get(i, j))
				{
					ind[i].push_back(j);
					inv[j].push_back(i);
				}
			}
		}
	}

	/**
	 * @brief  Restricts the relation to its symmetric fragment
	 */
	void RestrictToSymmetric()
	{
		for (size_t row = 0; row < this->size(); ++row)
		{
			for (size_t col = row + 1; col < this->size(); ++col)
			{	// traverse the matrix (we do just the part above the diagonal)
				bool res = this->get(row, col) && this->get(col, row);
				this->set(row, col, res);
				this->set(col, row, res);
			}
		}
	}


	/**
	 * @brief  Gets the projection of elements to their representatives
	 *
	 * This method relies on the fact that @p *this represents an equivalance
	 * relation (the result is undefined otherwise). The method creates the
	 * projection of elements to their representatives in the quotient set
	 * induced by the equivalence relation represented by @p *this.
	 *
	 * @param[out]  quotProj  The vector mapping elements to their
	 *                        representatives in the quotient set, quotProj[i] is
	 *                        the number that the i-th guy maps to
	 */
	void GetQuotientProjection(
		std::vector<size_t>&             quotProj)
	{
		const size_t UNDEF_PROJ = static_cast<size_t>(-1);

		quotProj.resize(this->size());
		for (size_t& elem : quotProj)
		{
			elem = UNDEF_PROJ;
		}

		for (size_t row = 0; row < this->size(); ++row)
		{	// traverse the matrix above the diagonal, only
			assert(row < quotProj.size());
			if (UNDEF_PROJ != quotProj[row])
			{	// in the case 'row' is already in some equivalence class
				continue;
			}

			quotProj[row] = row;
			for (size_t col = row + 1; col < this->size(); ++col)
			{
				if (this->get(row, col))
				{	// if 'col' is equivalent (w.r.t. the relation) to 'row'
					assert(UNDEF_PROJ == quotProj[col]);

					quotProj[col] = row;
				}
			}
		}
	}


	friend std::ostream& operator<<(
		std::ostream&             os,
		const BinaryRelation&     v)
	{
		for (size_t i = 0; i < v.size_; ++i)
		{
			for (size_t j = 0; j < v.size_; ++j)
			{
				os << v.get(i, j);
			}

			os << std::endl;
		}

		return os;
	}
};

class VATA::Util::Identity
{
private:  // data members

	size_t size_;

public:   // methods

	bool get(size_t r, size_t c) const
	{
		assert(r < size_);
		assert(c < size_);

		return r == c;
	}

	size_t size() const
	{
		return size_;
	}

public:

	using IndexType    = std::unordered_map<size_t, std::vector<size_t>>;

	explicit Identity(size_t size) :
		size_(size)
	{ }

	bool sym(size_t r, size_t c) const
	{
		assert(r < size_);
		assert(c < size_);

		return r == c;
	}

	// build equivalence classes
	void buildClasses(std::vector<size_t>& headIndex) const
	{
		headIndex.resize(size_);
		std::vector<size_t> head;

		for (size_t i = 0; i < size_; ++i)
		{
			size_t j = 0;
			while ((j < head.size()) && !this->sym(i, head[j]))
			{
				++j;
			}

			if (j < head.size())
			{
				headIndex[i] = head[j];
			}
			else
			{
				headIndex[i] = i;
				head.push_back(i);
			}
		}
	}

	// build equivalence classes
	void buildClasses(
		std::vector<size_t>& index,
		std::vector<size_t>& head) const
	{
		index.resize(size_);
		head.clear();
		for (size_t i = 0; i < size_; ++i)
		{
			size_t j = 0;
			while ((j < head.size()) && !this->sym(i, head[j]))
			{
				++j;
			}

			if (j < head.size())
			{
				index[i] = j;
			}
			else
			{
				index[i] = head.size();
				head.push_back(i);
			}
		}
	}

	// relation index
	void buildIndex(IndexType& dst) const
	{
		assert(dst.empty());

		for (size_t i = 0; i < size_; ++i)
		{
			bool inserted = dst.insert(std::make_pair(i, std::vector<size_t>({i}))).second;
			if (!inserted)  assert(false);
		}
	}

	// inverted relation index
	void buildInvIndex(IndexType& dst) const
	{
		assert(dst.empty());
		this->buildIndex(dst);
	}

	// relation index
	void buildIndex(IndexType& ind, IndexType& inv) const
	{
		assert(ind.empty());
		assert(inv.empty());

		this->buildIndex(ind);
		inv = ind;
	}

	friend std::ostream& operator<<(
		std::ostream&        os,
		const Identity&      v)
	{
		for (size_t i = 0; i < v.size(); ++i)
		{
			for (size_t j = 0; j < v.size(); ++j)
			{
				os << v.get(i, j);
			}
			os << std::endl;
		}

		return os;
	}
};

/**
 * @brief  A binary relation with discontinuous indexing
 */
class VATA::Util::DiscontBinaryRelation
{
public:   // data types

	using IndexType    = std::unordered_map<size_t, std::vector<size_t>>;
	// using DictType     = std::unordered_map<size_t, size_t>;
	using DictType     = VATA::Util::TwoWayDict<size_t, size_t,
		std::unordered_map<size_t, size_t>, std::unordered_map<size_t, size_t>>;
	using TranslType   = VATA::Util::TranslatorWeak<DictType>;

private:  // data members

	/// The underlying binary relation, indexed from 0
	BinaryRelation rel_;

	/// The counter of allocated indices
	size_t indexCnt_;

	/// The mapping of inputs to the range 0..size-1
	DictType dict_;

	/// The translator for indices
	TranslType transl_;

private:  // methods

	/**
	 * @brief  Translates index of internal relation to the discontinuous
	 *
	 * @param[in,out]  innerIndex  The index for the internal continuous relation
	 *
	 * @returns  The index in the discontinuous relation
	 *
	 * @note  Destroys @p innerIndex
	 *
	 * @note  Is this optimal?
	 */
	IndexType translateIndexToDiscont(
		BinaryRelation::IndexType&      innerIndex) const
	{
		IndexType result;

		for (size_t x = 0; x < innerIndex.size(); ++x)
		{
			std::vector<size_t>& images = innerIndex[x];

			// 'std::transform' is C++ for 'map' in Haskell
			std::transform(
				/* where to start */ images.cbegin(),
				/* where to finish */ images.cend(),
				/* where to insert the results */ images.begin(),
				/* what to compute */ [this](size_t img) { return dict_.TranslateBwd(img);});

			bool inserted = result.insert(
				std::make_pair(dict_.TranslateBwd(x), std::move(images))).second;
			if (!inserted)  assert(false);
		}

		return result;
	}

public:   // methods

	/**
	 * @brief  The constructor
	 */
	DiscontBinaryRelation(
		size_t         size = 0,
		bool           defVal = false,
		size_t         rowSize = 16) :
		rel_(size, defVal, rowSize),
		indexCnt_(0),
		dict_(),
		transl_(dict_, [this](const size_t&){return indexCnt_++;})
	{ }


	/**
	 * @brief  Constructor from a binary relation and a dictionary with copy semantics
	 *
	 * @note This allows a conversion from TDict to DictType
	 */
	template <
		class TDict>
	DiscontBinaryRelation(
		const BinaryRelation&      rel,
		const TDict&               dict) :
		rel_(rel),
		indexCnt_(0),
		dict_(dict),
		transl_(dict_, [this](const size_t&){return indexCnt_++;})
	{ }


	/**
	 * @brief  Constructor from a binary relation and a dictionary with copy semantics
	 */
	DiscontBinaryRelation(
		const BinaryRelation&      rel,
		const DictType&            dict) :
		rel_(rel),
		indexCnt_(0),
		dict_(dict),
		transl_(dict_, [this](const size_t&){return indexCnt_++;})
	{ }


	/**
	 * @brief  Constructor from a binary relation and a dictionary with move semantics
	 *
	 * @note This allows a conversion from TDict to DictType
	 */
	template <
		class TDict>
	DiscontBinaryRelation(
		BinaryRelation&&      rel,
		TDict&&               dict) :
		rel_(rel),
		indexCnt_(0),
		dict_(dict),
		transl_(dict_, [this](const size_t&){return indexCnt_++;})
	{ }

	/**
	 * @brief  Constructor from a binary relation and a dictionary with move semantics
	 */
	DiscontBinaryRelation(
		BinaryRelation&&      rel,
		DictType&&            dict) :
		rel_(rel),
		indexCnt_(0),
		dict_(dict),
		transl_(dict_, [this](const size_t&){return indexCnt_++;})
	{ }


	/**
	 * @brief  Copy constructor
	 */
	DiscontBinaryRelation(const DiscontBinaryRelation& rhs) :
		rel_(rhs.rel_),
		indexCnt_(rhs.indexCnt_),
		dict_(rhs.dict_),
		transl_(dict_, [this](const size_t&){return indexCnt_++;})
	{ }


	/**
	 * @brief  Move constructor
	 */
	DiscontBinaryRelation(DiscontBinaryRelation&& rhs) :
		rel_(rhs.rel_),
		indexCnt_(rhs.indexCnt_),
		dict_(rhs.dict_),
		transl_(dict_, [this](const size_t&){return indexCnt_++;})
	{ }


	/**
	 * @brief  Assignment operator
	 */
	DiscontBinaryRelation& operator=(const DiscontBinaryRelation& rhs)
	{
		if (this != &rhs)
		{
			rel_ = rhs.rel_;
			dict_ = rhs.dict_;
			indexCnt_ = rhs.indexCnt_;

			// the following (or a similar thing because this does not work) should
			// not be necessary because dict_ is used as a reference
			//
			// transl_ = TranslType(dict_, [this](const size_t&){return indexCnt_++;});
		}

		return *this;
	}


	/**
	 * @brief  Move assignment operator
	 */
	DiscontBinaryRelation& operator=(DiscontBinaryRelation&& rhs)
	{
		assert(this != &rhs);

		rel_ = std::move(rhs.rel_);
		dict_ = std::move(rhs.dict_);
		indexCnt_ = std::move(rhs.indexCnt_);

		// the following (or a similar thing because this does not work) should
		// not be necessary because dict_ is used as a reference
		//
		// transl_ = TranslType(dict_, [this](const size_t&){return indexCnt_++;});

		return *this;
	}


	/**
	 * @brief  Output stream operator
	 */
	friend std::ostream& operator<<(
		std::ostream&                     os,
		const DiscontBinaryRelation&      rel)
	{
		for (size_t i = 0; i < rel.size(); ++i)
		{
			for (size_t j = 0; j < rel.size(); ++j)
			{
				os << rel.get(i, j);
			}
			os << std::endl;
		}

		return os;
	}

    bool operator==(const DiscontBinaryRelation& rel) 
    {
        for (size_t i = 0; i < rel.size(); ++i)
        {
            for(size_t j = 0; j < rel.size(); ++j)
            {
                if(this->get(i, j) != rel.get(i, j))
                    return false;
            }
        }
        return true;
    }

	/**
	 * @brief  Creates a mapping from elements to their images
	 *
	 * This method maps elements to their images. Formally, it creates for each
	 * element 'x' the mapping 'x -> Y' where 'Y = {y | xRy}'
	 *
	 * @param[out]  dst  The result mapping every element to its images
	 */
	void buildIndex(IndexType& dst) const
	{
		assert(dst.empty());

		std::vector<std::vector<size_t>> innerIndex;

		rel_.buildIndex(innerIndex);
		dst = this->translateIndexToDiscont(innerIndex);
	}

	// relation index
	void buildIndex(
		IndexType&           ind,
		IndexType&           inv) const
	{
		assert(ind.empty());
		assert(inv.empty());

		std::vector<std::vector<size_t>> innerInd;
		std::vector<std::vector<size_t>> innerInv;

		rel_.buildIndex(innerInd, innerInv);
		assert(innerInd.size() == innerInv.size());
		ind = translateIndexToDiscont(innerInd);
		inv = translateIndexToDiscont(innerInv);
	}

	bool get(size_t row, size_t column) const
	{
		return rel_.get(transl_.at(row), transl_.at(column));
	}

	void set(size_t row, size_t column, bool value)
	{
		rel_.set(transl_[row], transl_[column], value);
	}

	size_t size() const
	{
		return rel_.size();
	}

	/**
	 * @brief  Restricts the relation to its symmetric fragment
	 */
	void RestrictToSymmetric()
	{
		rel_.RestrictToSymmetric();
	}


	/**
	 * @brief  Gets the projection of elements to their representatives
	 *
	 * This method relies on the fact that @p *this represents an equivalance
	 * relation (the result is undefined otherwise). The method creates the
	 * projection of elements to their representatives in the quotient set
	 * induced by the equivalence relation represented by @p *this.
	 *
	 * @param[out]  quotProj  The projection of elements to their representatives
	 *                        in the quotient set
	 */
	template <
		class MapType>
	void GetQuotientProjection(
		MapType&       quotProj)
	{
		assert(quotProj.empty());

		std::vector<size_t> innerProj;

		// get the vector for continuous values
		rel_.GetQuotientProjection(innerProj);

		for (size_t i = 0; i < innerProj.size(); ++i)
		{	// go over all elements in the vector
			bool inserted = quotProj.insert(
				std::make_pair(dict_.TranslateBwd(i), dict_.TranslateBwd(innerProj[i]))).second;
			if (!inserted)  assert(false);
		}
	}
};

#endif
