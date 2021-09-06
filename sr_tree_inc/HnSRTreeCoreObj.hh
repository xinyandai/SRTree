/*
 * HnSRTreeCoreObj.hh
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
 * 08/24/96 katayama@rd.nacsis.ac.jp
 * $Id: HnSRTreeCoreObj.hh,v 1.2 1997/06/02 09:35:38 katayama Exp $
 */

#ifndef _HnSRTreeCoreObj_hh
#define _HnSRTreeCoreObj_hh

#include "HnObject.hh"

/*{
#include "HnString.hh"
#include "HnPoint.hh"
}*/

class HnSRTreeCoreObj: public HnObject {
private:
	HnPoint center;
	double radius;
	int weight;
public:
	HnSRTreeCoreObj(const HnPoint &center, double radius, int weight) {
		this->center = center;
		this->radius = radius;
		this->weight = weight;
	}

	HnBool equals(const HnSRTreeCore &core) const {
		return (center.equals(core.getCenter()) &&
			radius == core.getRadius() &&
			weight == core.getWeight());
	}

	int getDimension(void) const {
		return center.getDimension();
	}

	HnPoint getCenter(void) const {
		return center;
	}
	double getRadius(void) const {
		return radius;
	}
	int getWeight(void) const {
		return weight;
	}

	HnString toString(void) const;
};

#endif /* _HnSRTreeCoreObj_hh */
