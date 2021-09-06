/*
 * HnRectObj.hh
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
 * 08/27/96 katayama@rd.nacsis.ac.jp
 * $Id: HnRectObj.hh,v 1.2 1997/06/02 09:35:30 katayama Exp $
 */

#ifndef _HnRectObj_hh
#define _HnRectObj_hh

#include "HnObject.hh"
#include "HnString.hh"

/*{
#include "HnPoint.hh"
#include "HnRange.hh"
class HnRectArray;
}*/

class HnRectObj: public HnObject {
private:
	int dimension;
	HnRange *ranges;
public:
	HnRectObj(int dimension) {
		this->dimension = dimension;
		this->ranges = new HnRange[dimension];
	}
	HnRectObj(const HnRect &rect) {
		int i;

		dimension = rect.getDimension();
		ranges = new HnRange[dimension];

		for(i=0; i<dimension; i++)
			ranges[i] = rect.getRange(i);
	}
	~HnRectObj(void) {
		delete ranges;
	}

	/* attributes */
	int getDimension(void) const {
		return dimension;
	}
	const HnRange &getRange(int index) const {
		return ranges[index];
	}
	void setRange(double min, HnRange::EndType minType,
		      double max, HnRange::EndType maxType,
		      int index) {
		ranges[index].set(min, minType, max, maxType);
	}
	void setRange(const HnRange &range, int index) {
		ranges[index] = range;
	}

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

#endif /* _HnRectObj_hh */
