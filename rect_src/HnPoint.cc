/*
 * HnPoint.cc
 *
 * DO NOT EDIT THIS FILE!
 *
 * This file is automatically generated by obj2ptr.
 * 1997/06/02 21:19:16
 */

#include "HnPoint.hh"
#include "HnPointObj.hh"

/*
 * HnPoint
 */

const HnPoint HnPoint::null;

HnPoint
new_HnPoint(int dimension)
{
	HnPointObj *_obj;
	HnPoint _ptr;

	_obj = new HnPointObj(dimension);

	if(_obj->hasFailed())
		return HnPoint::null;

	_ptr.assign(_obj);
	return _ptr;
}

HnPoint
new_HnPoint(const HnPoint &point)
{
	HnPointObj *_obj;
	HnPoint _ptr;

	_obj = new HnPointObj(point);

	if(_obj->hasFailed())
		return HnPoint::null;

	_ptr.assign(_obj);
	return _ptr;
}

int
HnPoint::getDimension(void) const
{
	return getObject()->getDimension();
}

double
HnPoint::getCoord(int index) const
{
	return getObject()->getCoord(index);
}

void
HnPoint::setCoord(double coord, int index)
{
	getObject()->setCoord(coord, index);
}

HnBool
HnPoint::equals(const HnPoint &ptr) const {
	if(isInvalid()) {
		if(ptr.isInvalid())
			return HnTRUE;
		else
			return HnFALSE;
    	}
    	else {
		if(ptr.isInvalid())
			return HnFALSE;
		else
			return getObject()->equals(ptr);
	}
}

HnString
HnPoint::toString(void) const {
	if(isInvalid())
		return "HnPoint::null";
    	else
		return getObject()->toString();
}

double
HnPoint::getDistance(const HnPoint &point) const
{
	return getObject()->getDistance(point);
}

/*
 * HnPointArrayObj
 */

#include <stdlib.h>
#include "HnString.hh"
class HnPointArrayObj: public HnObject {
private:
	HnPoint **array;
	int len;
	int size;

	static const int initialSize;

	void initialize(void) {
		array = NULL;
		len = 0;
		size = 0;
	}
	void dispose(void) {
		if(array != NULL) {
			int i;
			for(i=0; i<len; i++)
				delete array[i];
			free(array);
		}
		array = NULL;
		len = 0;
		size = 0;
	}
	void ensureSize(int requiredSize);

public:
	/* constructor and destructor */
	HnPointArrayObj(void);
	HnPointArrayObj(const HnPointArray &ptr);
	~HnPointArrayObj(void);

	/* clear, append, insert, and remove */
	void clear(void);
	void append(const HnPoint &ptr);
	void append(const HnPointArray &ptr);
	void insert(const HnPoint &ptr, int index);
	HnPoint remove(int index);
	void set(const HnPoint &ptr, int index);
	void swap(int i, int j);

	/* length and get */
	int length(void) const { return len; }
	HnPoint &get(int i) const;

	/* utilities */
	HnBool equals(const HnPointArray &ptr) const;
	int indexOf(const HnPoint &ptr, int fromIndex) const;
	HnString toString(void) const;
};

const int HnPointArrayObj::initialSize = 4;

static void *
xrealloc(void *ptr, size_t size)
{
        if(ptr == NULL) {
                if((ptr = malloc(size)) == NULL)
                        HnSysError("malloc");
        }
        else {
                if((ptr = realloc(ptr, size)) == NULL)
                        HnSysError("realloc");
        }

        return ptr;
}

void
HnPointArrayObj::ensureSize(int requiredSize)
{
	int oldSize, newSize;

	if((oldSize = size) >= requiredSize)
		return;

	if((newSize = oldSize) == 0)
		newSize = initialSize;

	while(newSize < requiredSize)
		newSize *= 2;

	array = (HnPoint **)xrealloc(array, sizeof(HnPoint *) * newSize);

	size = newSize;
}

HnPointArrayObj::HnPointArrayObj(void)
{
	initialize();
}

HnPointArrayObj::HnPointArrayObj(const HnPointArray &ptr)
{
	initialize();

	clear();
	append(ptr);
}

HnPointArrayObj::~HnPointArrayObj(void)
{
	dispose();
}

void
HnPointArrayObj::clear(void)
{
	dispose();
}

void
HnPointArrayObj::append(const HnPoint &ptr)
{
	ensureSize(len + 1);

	array[len] = new HnPoint(ptr);
	len ++;
}

void
HnPointArrayObj::append(const HnPointArray &ptr)
{
	int i;

	ensureSize(len + ptr.length());

	for(i=0; i<ptr.length(); i++)
		append(ptr[i]);
}

void
HnPointArrayObj::insert(const HnPoint &ptr, int index)
{
	if(index < 0 || index > len)
		HnAbort("HnPointArrayObj::insert: "
			"index is out of bounds.");

	ensureSize(len + 1);

	memmove(array + index + 1, array + index,
		sizeof(HnPoint *) * (len - index));

	array[index] = new HnPoint(ptr);
	len ++;
}

HnPoint
HnPointArrayObj::remove(int index)
{
	HnPoint ptr = get(index);

	if(index < 0 || index >= len)
                HnAbort("HnPointArrayObj::remove: "
			"index is out of bounds.");

	delete array[index];

	memmove(array + index, array + index + 1,
		sizeof(HnPoint *) * (len - index - 1));
	len --;

	return ptr;
}

void
HnPointArrayObj::set(const HnPoint &ptr, int index)
{
	if(index < 0 || index >= len)
		HnAbort("HnPointArrayObj::set: "
			"index is out of bounds.");

	delete array[index];

	array[index] = new HnPoint(ptr);
}

void
HnPointArrayObj::swap(int i, int j)
{
	if(i < 0 || i >= len || j < 0 || j >= len)
		HnAbort("HnPointArrayObj::swap: index is out of bounds.");

	HnPoint *ptr = array[i];
	array[i] = array[j];
	array[j] = ptr;
}

HnPoint &
HnPointArrayObj::get(int i) const {
	if(i < 0 || i >= len)
		HnAbort("HnPointArrayObj::get: index is out of bounds.");

	return *array[i];
}

HnBool
HnPointArrayObj::equals(const HnPointArray &ptr) const {
	int i;

	if(len != ptr.length())
		return HnFALSE;

	for(i=0; i<len; i++) {
		if(!array[i]->equals(ptr[i]))
			return HnFALSE;
	}

	return HnTRUE;
}

int
HnPointArrayObj::indexOf(const HnPoint &ptr, int fromIndex) const
{
	int i;

	for(i=fromIndex; i<len; i++) {
		if(array[i]->equals(ptr))
			return i;
	}

	return -1;
}

HnString
HnPointArrayObj::toString(void) const
{
	HnString string;

	string = "[ ";
	for(int i=0; i<len; i++) {
		if(i != 0)
			string += ", ";
		string += array[i]->toString();
	}
	string += " ]";

	return string;
}

/*
 * HnPointArray
 */

const HnPointArray HnPointArray::null;

HnPointArray
new_HnPointArray(void)
{
	HnPointArray ptr;

        ptr.assign(new HnPointArrayObj());
        return ptr;
}

HnPointArray
new_HnPointArray(const HnPointArray &value)
{
	HnPointArray ptr;

        if(value.isInvalid())
                ptr.assign(NULL);
        else
                ptr.assign(new HnPointArrayObj(value));

        return ptr;
}

void
HnPointArray::clear(void)
{
	getObject()->clear();
}

void
HnPointArray::append(const HnPoint &ptr)
{
	getObject()->append(ptr);
}

void
HnPointArray::append(const HnPointArray &ptr)
{
	getObject()->append(ptr);
}

void
HnPointArray::insert(const HnPoint &ptr, int index)
{
	getObject()->insert(ptr, index);
}

HnPoint
HnPointArray::remove(int index)
{
	return getObject()->remove(index);
}

void
HnPointArray::set(const HnPoint &ptr, int index)
{
	getObject()->set(ptr, index);
}

void
HnPointArray::swap(int i, int j)
{
	getObject()->swap(i, j);
}

int
HnPointArray::length(void) const
{
	return getObject()->length();
}

HnPoint &
HnPointArray::get(int i) const
{
	return getObject()->get(i);
}

HnBool
HnPointArray::equals(const HnPointArray &ptr) const
{
	return getObject()->equals(ptr);
}

int
HnPointArray::indexOf(const HnPoint &ptr, int fromIndex) const
{
	return getObject()->indexOf(ptr, fromIndex);
}

HnString
HnPointArray::toString(void) const
{
	return getObject()->toString();
}

