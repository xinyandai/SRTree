/*
 * HnSRTreeNodeObj.hh
 * Copyright (C) 1997 Norio Katayama
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA
 *
 * 03/21/96 katayama@rd.nacsis.ac.jp
 * $Id: HnSRTreeNodeObj.hh,v 1.2 1997/06/02 09:35:42 katayama Exp $
 */

#ifndef _HnSRTreeNodeObj_hh
#define _HnSRTreeNodeObj_hh

#include "HnObject.hh"
/*{
#include "HnString.hh"
#include "HnRect.hh"
#include "HnSRTreeCore.hh"
#include "HnSRTreeBlock.hh"
#include <unistd.h>
}*/

class HnSRTreeNodeObj: public HnObject {
private:
	int dimension;
	int blockSize;
	int maxCount;
	off_t offset;
	int count;
	HnSRTreeCoreArray cores;
	HnRectArray rects;
	off_t *offsets;

private:
	void initialize(int dimension, int blockSize, off_t offset);
	void dispose(void);
public:
	HnSRTreeNodeObj(int dimension, int blockSize, off_t offset);
	HnSRTreeNodeObj(int dimension, const HnSRTreeBlock &block);
	~HnSRTreeNodeObj(void);

	void append(const HnSRTreeCore &core,
		    const HnRect &rect, off_t offset) {
		if(count == maxCount)
			HnAbort("HnSRTreeNodeObj: no more entry can be appended with max limit %d.", maxCount);
		if(core.getDimension() != dimension ||
		   rect.getDimension() != dimension)
			HnAbort("mismatch in dimensions.");

		cores.append(core);
		rects.append(rect);
		offsets[count] = offset;
		count ++;
	}
	void remove(int index) {
		int i;

		cores.remove(index);
		rects.remove(index);
		for(i=index + 1; i<count; i++)
			offsets[i-1] = offsets[i];
		count --;
	}
	void setElementAt(const HnSRTreeCore &core, const HnRect &rect,
			  off_t offset, int index) {
		cores.set(core, index);
		rects.set(rect, index);
		offsets[index] = offset;
	}
	void setCoreAt(const HnSRTreeCore &core, int index) {
		cores.set(core, index);
	}
	void setBoundingRectAt(const HnRect &rect, int index) {
		rects.set(rect, index);
	}

	int getDimension(void) const {
		return dimension;
	}
	int getBlockSize(void) const {
		return blockSize;
	}
	off_t getOffset(void) const {
		return offset;
	}
	int getCount(void) const {
		return count;
	}
	size_t getSize(void) const;
	HnBool isFull(void) const {
		return count == maxCount;
	}

	HnSRTreeCore getCore(void) const;
	HnRect getBoundingRect(void) const {
		return HnRect::getBoundingRect(rects);
	}

	HnSRTreeCore getCoreAt(int index) const {
		return cores[index];
	}
	HnRect getRectAt(int index) const {
		return rects[index];
	}
	off_t getOffsetAt(int index) const {
		return offsets[index];
	}

	HnSRTreeBlock toBlock(void) const;

	HnString toString(void) const;

	static int getMaxCount(int dimension, int blockSize);
};

#endif /* _HnSRTreeNodeObj_hh */
