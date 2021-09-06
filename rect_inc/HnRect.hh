/*
 * HnRect.hh
 *
 * DO NOT EDIT THIS FILE!
 *
 * This file is automatically generated by obj2ptr.
 * 1997/06/02 21:19:16
 */

#ifndef _HnRect_hh
#define _HnRect_hh

#include "HnPointer.hh"

#include "HnPoint.hh"
#include "HnRange.hh"
class HnRect;
class HnRectObj;
class HnRectArray;
class HnRectArrayObj;



HnRect
new_HnRect(int dimension);
/* constructors */
HnRectArray
new_HnRectArray(void);
HnRectArray
new_HnRectArray(const HnRectArray &ptr);
/*
 * HnRect
 */

class HnRect: public HnPointer {
private:
    friend class HnRectObj;
	HnRect(const HnRectObj *ptr) {
		HnPointer::assign((HnObject *)ptr);
	}
	HnRectObj *getObject(void) const {
		return (HnRectObj *)HnPointer::getObject();
	}
public:
	static const HnRect null;
	HnRect(void) {}
public:
    friend HnRect
	new_HnRect(int dimension);
    friend HnRect
	new_HnRect(const HnRect &rect);

	/* attributes */
	int getDimension(void) const;
	const HnRange &getRange(int index) const;
	void setRange(double min, HnRange::EndType minType,
		      double max, HnRange::EndType maxType,
		      int index);
	void setRange(const HnRange &range, int index);

	/* utilities */
	HnBool equals(const HnRect &rect) const;
	HnBool overlaps(const HnRect &rect) const;
	HnBool includes(const HnRect &rect) const;
	HnBool includes(const HnPoint &point) const;

	double getMaxWidth(void) const;
	double getMargin(void) const;
	HnPoint getCenterPoint(void) const;

	HnRect getOverlappingRect(const HnRect &rect) const;
	HnRect getBoundingRect(const HnRect &rect) const;

	double getDiagonal(void) const;
	double getVolume(void) const;

	double getMinDistance(const HnPoint &point) const;
	double getMaxDistance(const HnPoint &point) const;

	HnString toString(void) const;

	/* class methods */
	static HnRect getBoundingRect(const HnRectArray &rects);
};

/*
 * HnRectArray
 */

class HnRectArray: public HnPointer {
private:
    friend class HnRectArrayObj;
	HnRectArray(const HnRectArrayObj *ptr) {
		HnPointer::assign((HnObject *)ptr);
	}
	HnRectArrayObj *getObject(void) const {
		return (HnRectArrayObj *)HnPointer::getObject();
	}
public:
	static const HnRectArray null;
	HnRectArray(void) {}
public:
	/* constructors */
    friend HnRectArray
	new_HnRectArray(void);
    friend HnRectArray
	new_HnRectArray(const HnRectArray &ptr);

	/* clear, append, insert, and remove */
	void clear(void);
	void append(const HnRect &ptr);
	void append(const HnRectArray &ptr);
	void insert(const HnRect &ptr, int index);
	HnRect remove(int index);
	void set(const HnRect &ptr, int index);
	void swap(int i, int j);

	/* length and get */
	int length(void) const;
	HnRect &get(int i) const;
	HnRect &operator[](int i) const { return get(i); }

	/* utilities */
	void push(const HnRect &ptr) {
		append(ptr);
	}
	HnRect pop(void) {
		return remove(length() - 1);
	}
	HnBool equals(const HnRectArray &ptr) const;
	int indexOf(const HnRect &ptr, int fromIndex) const;
	int indexOf(const HnRect &ptr) const  {
	    return indexOf(ptr, 0);
	}
	HnString toString(void) const;
};

#endif /* _HnRect_hh */