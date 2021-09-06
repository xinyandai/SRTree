/*
 * HnSRTreeNeighborObj.hh
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
 * 07/04/96 katayama@rd.nacsis.ac.jp
 * $Id: HnSRTreeNeighborObj.hh,v 1.2 1997/06/02 09:35:41 katayama Exp $
 */

#ifndef _HnSRTreeNeighborObj_hh
#define _HnSRTreeNeighborObj_hh

#include "HnObject.hh"

/*{
#include "HnPoint.hh"
#include "HnData.hh"
#include "HnString.hh"
class HnSRTreeNeighborArray;
}*/

class HnSRTreeNeighborObj: public HnObject {
private:
	/*
	 * NOTE:
	 *	The metrics for the distance may be min distance,
	 *	max distance, center point distance, or others,
	 *	as long as it is less than or equal to the min distance
	 *	and greater than or equal to the max distance.
	 */

	HnPoint point;
	HnData data;
	double distance;

public:
	HnSRTreeNeighborObj(const HnPoint &point, const HnData &data,
			   double distance) {
		this->point = point;
		this->data = data;
		this->distance = distance;
	}

	HnPoint getPoint(void) const {
		return point;
	}
	HnData getData(void) const {
		return data;
	}
	double getDistance(void) const {
		return distance;
	}
	HnString toString(void) const;

	static HnSRTreeNeighborArray sort(const HnSRTreeNeighborArray &ptr);
};

#endif /* _HnSRTreeNeighborObj_hh */
