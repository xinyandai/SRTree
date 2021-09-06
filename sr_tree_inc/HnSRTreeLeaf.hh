/*
 * HnSRTreeLeaf.hh
 *
 * DO NOT EDIT THIS FILE!
 *
 * This file is automatically generated by obj2ptr.
 * 1997/06/02 21:19:33
 */

#ifndef _HnSRTreeLeaf_hh
#define _HnSRTreeLeaf_hh

#include "HnPointer.hh"

#include "HnData.hh"
#include "HnPoint.hh"
#include "HnRect.hh"
#include "HnSRTreeCore.hh"
#include "HnSRTreeBlock.hh"
#include <unistd.h>

class HnSRTreeLeaf;
class HnSRTreeLeafObj;
class HnSRTreeLeafArrayObj;

 HnSRTreeLeaf
new_HnSRTreeLeaf(int dimension, int dataSize,
                 int blockSize, off_t offset);
 HnSRTreeLeaf
new_HnSRTreeLeaf(int dimension, int dataSize,
                 const HnSRTreeBlock &block);
/*
 * HnSRTreeLeaf
 */

class HnSRTreeLeaf: public HnPointer {
private:
    friend class HnSRTreeLeafObj;
	HnSRTreeLeaf(const HnSRTreeLeafObj *ptr) {
		HnPointer::assign((HnObject *)ptr);
	}
	HnSRTreeLeafObj *getObject(void) const {
		return (HnSRTreeLeafObj *)HnPointer::getObject();
	}
public:
	static const HnSRTreeLeaf null;
	HnSRTreeLeaf(void) {}
public:
    friend HnSRTreeLeaf
	new_HnSRTreeLeaf(int dimension, int dataSize,
			int blockSize, off_t offset);
    friend HnSRTreeLeaf
	new_HnSRTreeLeaf(int dimension, int dataSize,
			const HnSRTreeBlock &block);

	void append(const HnPoint &point, const HnData &data);

	void remove(int index);

	int getDimension(void) const;
	int getDataSize(void) const;
	int getBlockSize(void) const;
	off_t getOffset(void) const;
	int getCount(void) const;
	size_t getSize(void) const;
	HnBool isFull(void) const;

	HnSRTreeCore getCore(void) const;
	HnRect getBoundingRect(void) const;

	HnPoint getPointAt(int index) const;
	HnData getDataAt(int index) const;

	HnSRTreeBlock toBlock(void) const;

	HnString toString(void) const;

	static int getMaxCount(int dimension, int dataSize, int blockSize);
};

/*
 * HnSRTreeLeafArray
 */

class HnSRTreeLeafArray: public HnPointer {
private:
    friend class HnSRTreeLeafArrayObj;
	HnSRTreeLeafArray(const HnSRTreeLeafArrayObj *ptr) {
		HnPointer::assign((HnObject *)ptr);
	}
	HnSRTreeLeafArrayObj *getObject(void) const {
		return (HnSRTreeLeafArrayObj *)HnPointer::getObject();
	}
public:
	static const HnSRTreeLeafArray null;
	HnSRTreeLeafArray(void) {}
public:
	/* constructors */
    friend HnSRTreeLeafArray
	new_HnSRTreeLeafArray(void);
    friend HnSRTreeLeafArray
	new_HnSRTreeLeafArray(const HnSRTreeLeafArray &ptr);

	/* clear, append, insert, and remove */
	void clear(void);
	void append(const HnSRTreeLeaf &ptr);
	void append(const HnSRTreeLeafArray &ptr);
	void insert(const HnSRTreeLeaf &ptr, int index);
	HnSRTreeLeaf remove(int index);
	void set(const HnSRTreeLeaf &ptr, int index);
	void swap(int i, int j);

	/* length and get */
	int length(void) const;
	HnSRTreeLeaf &get(int i) const;
	HnSRTreeLeaf &operator[](int i) const { return get(i); }

	/* utilities */
	void push(const HnSRTreeLeaf &ptr) {
		append(ptr);
	}
	HnSRTreeLeaf pop(void) {
		return remove(length() - 1);
	}
	HnBool equals(const HnSRTreeLeafArray &ptr) const;
	int indexOf(const HnSRTreeLeaf &ptr, int fromIndex) const;
	int indexOf(const HnSRTreeLeaf &ptr) const  {
	    return indexOf(ptr, 0);
	}
	HnString toString(void) const;
};

#endif /* _HnSRTreeLeaf_hh */