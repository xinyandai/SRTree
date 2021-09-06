/*
 * HnPointObj.hh
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
 * 06/29/96 katayama@rd.nacsis.ac.jp
 * $Id: HnPointObj.hh,v 1.2 1997/06/02 09:35:27 katayama Exp $
 */

#ifndef _HnPointObj_hh
#define _HnPointObj_hh

#include "HnObject.hh"
#include "HnString.hh"

class HnPointObj: public HnObject {
private:
	int dimension;
	double *coords;

public:
	HnPointObj(int dimension) {
		int i;

		this->dimension = dimension;

		coords = new double[dimension];
		for(i=0; i<dimension; i++)
			coords[i] = 0;
	}
	~HnPointObj(void) {
		delete coords;
	}
	HnPointObj(const HnPoint &point) {
		int i;

		dimension = point.getDimension();

		coords = new double[dimension];
		for(i=0; i<dimension; i++)
			coords[i] = point.getCoord(i);
	}

	/* attributes */
	int getDimension(void) const {
		return dimension;
	}
	double getCoord(int index) const {
		if(index < 0 || index >= dimension)
			HnAbort("index is out of bounds.");
		return coords[index];
	}
	void setCoord(double coord, int index) {
		if(index < 0 || index >= dimension)
			HnAbort("index is out of bounds.");
		coords[index] = coord;
	}

	/* utilities */
	HnBool equals(const HnPoint &point) const;
	HnString toString(void) const;

	double getDistance(const HnPoint &point) const;
};

#endif /* _HnPointObj_hh */
