/*
 * HnSRTreeFile.hh
 *
 * DO NOT EDIT THIS FILE!
 *
 * This file is automatically generated by obj2ptr.
 * 1997/06/02 21:19:34
 */

#ifndef _HnSRTreeFile_hh
#define _HnSRTreeFile_hh

#include "HnPointer.hh"

#include "HnPoint.hh"
#include "HnData.hh"
#include "HnRect.hh"

class HnSRTreeFile;
class HnSRTreeFileObj;
class HnSRTreeFileArrayObj;

HnSRTreeFile
new_HnSRTreeFile(const char *path, const char *mode);
HnSRTreeFile
new_HnSRTreeFile(const char *path, int dimension, int dataSize, int blockSize, int splitFactor, int reinsertFactor);

/*
 * HnSRTreeFile
 */

class HnSRTreeFile: public HnPointer {
private:
    friend class HnSRTreeFileObj;
	HnSRTreeFile(const HnSRTreeFileObj *ptr) {
		HnPointer::assign((HnObject *)ptr);
	}
	HnSRTreeFileObj *getObject(void) const {
		return (HnSRTreeFileObj *)HnPointer::getObject();
	}
public:
	static const HnSRTreeFile null;
	HnSRTreeFile(void) {}
public:
    friend HnSRTreeFile
	new_HnSRTreeFile(const char *path,
			int dimension, int dataSize, int blockSize,
			int splitFactor, int reinsertFactor);
    friend HnSRTreeFile
	new_HnSRTreeFile(const char *path,
			int dimension, int dataSize, int blockSize,
			int splitFactor, int reinsertFactor,
			HnPointArray &points, HnDataArray &values,
			HnBool debug);
    friend HnSRTreeFile
	new_HnSRTreeFile(const char *path, const char *mode);

	int getDimension(void) const;
	int getBlockSize(void) const;
	int getDataSize(void) const;
	int getHeight(void) const;

	void store(const HnPoint &point, const HnData &data);
	void remove(const HnPoint &point, const HnData &data);
	void close(void);

	void getFirst(HnPoint *point_return, HnData *data_return);
	void getFirst(const HnRect &rect,
		      HnPoint *point_return, HnData *data_return);
	void getNext(HnPoint *point_return, HnData *data_return);

	void getNeighbors(const HnPoint &point, int maxCount,
			  HnPointArray *points_return,
			  HnDataArray *values_return);

	void check(void);
	void print(void);
	void setDebug(HnBool flag);
};

/*
 * HnSRTreeFileArray
 */

class HnSRTreeFileArray: public HnPointer {
private:
    friend class HnSRTreeFileArrayObj;
	HnSRTreeFileArray(const HnSRTreeFileArrayObj *ptr) {
		HnPointer::assign((HnObject *)ptr);
	}
	HnSRTreeFileArrayObj *getObject(void) const {
		return (HnSRTreeFileArrayObj *)HnPointer::getObject();
	}
public:
	static const HnSRTreeFileArray null;
	HnSRTreeFileArray(void) {}
public:
	/* constructors */
    friend HnSRTreeFileArray
	new_HnSRTreeFileArray(void);
    friend HnSRTreeFileArray
	new_HnSRTreeFileArray(const HnSRTreeFileArray &ptr);

	/* clear, append, insert, and remove */
	void clear(void);
	void append(const HnSRTreeFile &ptr);
	void append(const HnSRTreeFileArray &ptr);
	void insert(const HnSRTreeFile &ptr, int index);
	HnSRTreeFile remove(int index);
	void set(const HnSRTreeFile &ptr, int index);
	void swap(int i, int j);

	/* length and get */
	int length(void) const;
	HnSRTreeFile &get(int i) const;
	HnSRTreeFile &operator[](int i) const { return get(i); }

	/* utilities */
	void push(const HnSRTreeFile &ptr) {
		append(ptr);
	}
	HnSRTreeFile pop(void) {
		return remove(length() - 1);
	}
	HnBool equals(const HnSRTreeFileArray &ptr) const;
	int indexOf(const HnSRTreeFile &ptr, int fromIndex) const;
	int indexOf(const HnSRTreeFile &ptr) const  {
	    return indexOf(ptr, 0);
	}
	HnString toString(void) const;
};

#endif /* _HnSRTreeFile_hh */