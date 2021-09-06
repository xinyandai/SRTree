/*
 * HnSRTreeLeafObj.hh
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
 * $Id: HnSRTreeLeafObj.hh,v 1.2 1997/06/02 09:35:40 katayama Exp $
 */

#ifndef _HnSRTreeLeafObj_hh
#define _HnSRTreeLeafObj_hh

#include "HnObject.hh"
#include "HnString.hh"
/*{
#include "HnData.hh"
#include "HnPoint.hh"
#include "HnRect.hh"
#include "HnSRTreeCore.hh"
#include "HnSRTreeBlock.hh"
#include <unistd.h>
}*/

class HnSRTreeLeafObj: public HnObject {
private:
	int dimension;
	int dataSize;
	int blockSize;
	int maxCount;
	off_t offset;
	int count;
	HnPointArray points;
	HnDataArray values;

private:
	void initialize(int dimension, int dataSize,
			int blockSize, off_t offset);
	void dispose(void);
public:
	HnSRTreeLeafObj(int dimension, int dataSize,
			int blockSize, off_t offset);
	HnSRTreeLeafObj(int dimension, int dataSize,
			const HnSRTreeBlock &block);
	~HnSRTreeLeafObj(void);

	void append(const HnPoint &point, const HnData &data) {
		if(count == maxCount)
			HnAbort("HnSRTreeLeafObj: no more entry can be appended with max count limit %d .", maxCount);
		if(point.getDimension() != dimension)
			HnAbort("mismatch in dimensions.");
		if(data.length() > dataSize)
			HnAbort("data size (%d) exceeds the limit (%d).",
				data.length(), dataSize);

		points.append(point);
		values.append(data);
		count ++;
	}

	void remove(int index) {
		points.remove(index);
		values.remove(index);
		count --;
	}

	int getDimension(void) const {
		return dimension;
	}
	int getDataSize(void) const {
		return dataSize;
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
	HnRect getBoundingRect(void) const;

	HnPoint getPointAt(int index) const {
		return points[index];
	}
	HnData getDataAt(int index) const {
		return values[index];
	}

	HnSRTreeBlock toBlock(void) const;

	HnString toString(void) const;

	static int getMaxCount(int dimension, int dataSize, int blockSize);
};

#endif /* _HnSRTreeLeafObj_hh */
