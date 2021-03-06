/*
 * HnSRTreeBlock.hh
 *
 * DO NOT EDIT THIS FILE!
 *
 * This file is automatically generated by obj2ptr.
 * 1997/06/02 21:19:32
 */

#ifndef _HnSRTreeBlock_hh
#define _HnSRTreeBlock_hh

#include "HnPointer.hh"

#include <unistd.h>

class HnSRTreeBlock;
class HnSRTreeBlockObj;
class HnSRTreeBlockArray;
class HnSRTreeBlockArrayObj;

HnSRTreeBlockArray
new_HnSRTreeBlockArray(void);
HnSRTreeBlockArray
new_HnSRTreeBlockArray(const HnSRTreeBlockArray &ptr);
HnSRTreeBlock
new_HnSRTreeBlock(off_t offset, const char *bytes, size_t size);

/*
 * HnSRTreeBlock
 */

class HnSRTreeBlock: public HnPointer {
private:
    friend class HnSRTreeBlockObj;
	HnSRTreeBlock(const HnSRTreeBlockObj *ptr) {
		HnPointer::assign((HnObject *)ptr);
	}
	HnSRTreeBlockObj *getObject(void) const {
		return (HnSRTreeBlockObj *)HnPointer::getObject();
	}
public:
	static const HnSRTreeBlock null;
	HnSRTreeBlock(void) {}
public:
	typedef enum { NODE, LEAF, FREE } Type;

    friend HnSRTreeBlock
	new_HnSRTreeBlock(off_t offset, const char *bytes, size_t size);

	/* attributes */
	off_t getOffset(void) const;
	const char *getBytes(void) const;
	size_t getSize(void) const;

	/* utilities */
	HnSRTreeBlock::Type getType(void) const;
	HnBool isNode(void) const;
	HnBool isLeaf(void) const;
	HnBool isFree(void) const;
};

/*
 * HnSRTreeBlockArray
 */

class HnSRTreeBlockArray: public HnPointer {
private:
    friend class HnSRTreeBlockArrayObj;
	HnSRTreeBlockArray(const HnSRTreeBlockArrayObj *ptr) {
		HnPointer::assign((HnObject *)ptr);
	}
	HnSRTreeBlockArrayObj *getObject(void) const {
		return (HnSRTreeBlockArrayObj *)HnPointer::getObject();
	}
public:
	static const HnSRTreeBlockArray null;
	HnSRTreeBlockArray(void) {}
public:
	/* constructors */
    friend HnSRTreeBlockArray
	new_HnSRTreeBlockArray(void);
    friend HnSRTreeBlockArray
	new_HnSRTreeBlockArray(const HnSRTreeBlockArray &ptr);

	/* clear, append, insert, and remove */
	void clear(void);
	void append(const HnSRTreeBlock &ptr);
	void append(const HnSRTreeBlockArray &ptr);
	void insert(const HnSRTreeBlock &ptr, int index);
	HnSRTreeBlock remove(int index);
	void set(const HnSRTreeBlock &ptr, int index);
	void swap(int i, int j);

	/* length and get */
	int length(void) const;
	HnSRTreeBlock &get(int i) const;
	HnSRTreeBlock &operator[](int i) const { return get(i); }

	/* utilities */
	void push(const HnSRTreeBlock &ptr) {
		append(ptr);
	}
	HnSRTreeBlock pop(void) {
		return remove(length() - 1);
	}
	HnBool equals(const HnSRTreeBlockArray &ptr) const;
	int indexOf(const HnSRTreeBlock &ptr, int fromIndex) const;
	int indexOf(const HnSRTreeBlock &ptr) const  {
	    return indexOf(ptr, 0);
	}
	HnString toString(void) const;
};

#endif /* _HnSRTreeBlock_hh */
