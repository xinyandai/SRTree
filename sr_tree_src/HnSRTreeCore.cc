/*
 * HnSRTreeCore.cc
 *
 * DO NOT EDIT THIS FILE!
 *
 * This file is automatically generated by obj2ptr.
 * 1997/06/02 21:19:32
 */

#include "HnSRTreeCore.hh"
#include "HnSRTreeCoreObj.hh"

/*
 * HnSRTreeCore
 */

const HnSRTreeCore HnSRTreeCore::null;

HnSRTreeCore
new_HnSRTreeCore(const HnPoint &center, double radius, int weight)
{
	HnSRTreeCoreObj *_obj;
	HnSRTreeCore _ptr;

	_obj = new HnSRTreeCoreObj(center, radius, weight);

	if(_obj->hasFailed())
		return HnSRTreeCore::null;

	_ptr.assign(_obj);
	return _ptr;
}

HnBool
HnSRTreeCore::equals(const HnSRTreeCore &ptr) const {
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

int
HnSRTreeCore::getDimension(void) const
{
	return getObject()->getDimension();
}

HnPoint
HnSRTreeCore::getCenter(void) const
{
	return getObject()->getCenter();
}

double
HnSRTreeCore::getRadius(void) const
{
	return getObject()->getRadius();
}

int
HnSRTreeCore::getWeight(void) const
{
	return getObject()->getWeight();
}

HnString
HnSRTreeCore::toString(void) const {
	if(isInvalid())
		return "HnSRTreeCore::null";
    	else
		return getObject()->toString();
}

/*
 * HnSRTreeCoreArrayObj
 */

#include <stdlib.h>
#include "HnString.hh"
class HnSRTreeCoreArrayObj: public HnObject {
private:
	HnSRTreeCore **array;
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
	HnSRTreeCoreArrayObj(void);
	HnSRTreeCoreArrayObj(const HnSRTreeCoreArray &ptr);
	~HnSRTreeCoreArrayObj(void);

	/* clear, append, insert, and remove */
	void clear(void);
	void append(const HnSRTreeCore &ptr);
	void append(const HnSRTreeCoreArray &ptr);
	void insert(const HnSRTreeCore &ptr, int index);
	HnSRTreeCore remove(int index);
	void set(const HnSRTreeCore &ptr, int index);
	void swap(int i, int j);

	/* length and get */
	int length(void) const { return len; }
	HnSRTreeCore &get(int i) const;

	/* utilities */
	HnBool equals(const HnSRTreeCoreArray &ptr) const;
	int indexOf(const HnSRTreeCore &ptr, int fromIndex) const;
	HnString toString(void) const;
};

const int HnSRTreeCoreArrayObj::initialSize = 4;

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
HnSRTreeCoreArrayObj::ensureSize(int requiredSize)
{
	int oldSize, newSize;

	if((oldSize = size) >= requiredSize)
		return;

	if((newSize = oldSize) == 0)
		newSize = initialSize;

	while(newSize < requiredSize)
		newSize *= 2;

	array = (HnSRTreeCore **)xrealloc(array, sizeof(HnSRTreeCore *) * newSize);

	size = newSize;
}

HnSRTreeCoreArrayObj::HnSRTreeCoreArrayObj(void)
{
	initialize();
}

HnSRTreeCoreArrayObj::HnSRTreeCoreArrayObj(const HnSRTreeCoreArray &ptr)
{
	initialize();

	clear();
	append(ptr);
}

HnSRTreeCoreArrayObj::~HnSRTreeCoreArrayObj(void)
{
	dispose();
}

void
HnSRTreeCoreArrayObj::clear(void)
{
	dispose();
}

void
HnSRTreeCoreArrayObj::append(const HnSRTreeCore &ptr)
{
	ensureSize(len + 1);

	array[len] = new HnSRTreeCore(ptr);
	len ++;
}

void
HnSRTreeCoreArrayObj::append(const HnSRTreeCoreArray &ptr)
{
	int i;

	ensureSize(len + ptr.length());

	for(i=0; i<ptr.length(); i++)
		append(ptr[i]);
}

void
HnSRTreeCoreArrayObj::insert(const HnSRTreeCore &ptr, int index)
{
	if(index < 0 || index > len)
		HnAbort("HnSRTreeCoreArrayObj::insert: "
			"index is out of bounds.");

	ensureSize(len + 1);

	memmove(array + index + 1, array + index,
		sizeof(HnSRTreeCore *) * (len - index));

	array[index] = new HnSRTreeCore(ptr);
	len ++;
}

HnSRTreeCore
HnSRTreeCoreArrayObj::remove(int index)
{
	HnSRTreeCore ptr = get(index);

	if(index < 0 || index >= len)
                HnAbort("HnSRTreeCoreArrayObj::remove: "
			"index is out of bounds.");

	delete array[index];

	memmove(array + index, array + index + 1,
		sizeof(HnSRTreeCore *) * (len - index - 1));
	len --;

	return ptr;
}

void
HnSRTreeCoreArrayObj::set(const HnSRTreeCore &ptr, int index)
{
	if(index < 0 || index >= len)
		HnAbort("HnSRTreeCoreArrayObj::set: "
			"index is out of bounds.");

	delete array[index];

	array[index] = new HnSRTreeCore(ptr);
}

void
HnSRTreeCoreArrayObj::swap(int i, int j)
{
	if(i < 0 || i >= len || j < 0 || j >= len)
		HnAbort("HnSRTreeCoreArrayObj::swap: index is out of bounds.");

	HnSRTreeCore *ptr = array[i];
	array[i] = array[j];
	array[j] = ptr;
}

HnSRTreeCore &
HnSRTreeCoreArrayObj::get(int i) const {
	if(i < 0 || i >= len)
		HnAbort("HnSRTreeCoreArrayObj::get: index is out of bounds.");

	return *array[i];
}

HnBool
HnSRTreeCoreArrayObj::equals(const HnSRTreeCoreArray &ptr) const {
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
HnSRTreeCoreArrayObj::indexOf(const HnSRTreeCore &ptr, int fromIndex) const
{
	int i;

	for(i=fromIndex; i<len; i++) {
		if(array[i]->equals(ptr))
			return i;
	}

	return -1;
}

HnString
HnSRTreeCoreArrayObj::toString(void) const
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
 * HnSRTreeCoreArray
 */

const HnSRTreeCoreArray HnSRTreeCoreArray::null;

HnSRTreeCoreArray
new_HnSRTreeCoreArray(void)
{
	HnSRTreeCoreArray ptr;

        ptr.assign(new HnSRTreeCoreArrayObj());
        return ptr;
}

HnSRTreeCoreArray
new_HnSRTreeCoreArray(const HnSRTreeCoreArray &value)
{
	HnSRTreeCoreArray ptr;

        if(value.isInvalid())
                ptr.assign(NULL);
        else
                ptr.assign(new HnSRTreeCoreArrayObj(value));

        return ptr;
}

void
HnSRTreeCoreArray::clear(void)
{
	getObject()->clear();
}

void
HnSRTreeCoreArray::append(const HnSRTreeCore &ptr)
{
	getObject()->append(ptr);
}

void
HnSRTreeCoreArray::append(const HnSRTreeCoreArray &ptr)
{
	getObject()->append(ptr);
}

void
HnSRTreeCoreArray::insert(const HnSRTreeCore &ptr, int index)
{
	getObject()->insert(ptr, index);
}

HnSRTreeCore
HnSRTreeCoreArray::remove(int index)
{
	return getObject()->remove(index);
}

void
HnSRTreeCoreArray::set(const HnSRTreeCore &ptr, int index)
{
	getObject()->set(ptr, index);
}

void
HnSRTreeCoreArray::swap(int i, int j)
{
	getObject()->swap(i, j);
}

int
HnSRTreeCoreArray::length(void) const
{
	return getObject()->length();
}

HnSRTreeCore &
HnSRTreeCoreArray::get(int i) const
{
	return getObject()->get(i);
}

HnBool
HnSRTreeCoreArray::equals(const HnSRTreeCoreArray &ptr) const
{
	return getObject()->equals(ptr);
}

int
HnSRTreeCoreArray::indexOf(const HnSRTreeCore &ptr, int fromIndex) const
{
	return getObject()->indexOf(ptr, fromIndex);
}

HnString
HnSRTreeCoreArray::toString(void) const
{
	return getObject()->toString();
}

