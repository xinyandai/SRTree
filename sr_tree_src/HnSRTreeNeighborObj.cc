/*
 * HnSRTreeNeighborObj.cc
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
 * $Id: HnSRTreeNeighborObj.cc,v 1.2 1997/06/02 09:35:41 katayama Exp $
 */

#include "HnSRTreeNeighbor.hh"
#include "HnSRTreeNeighborObj.hh"

static int
compare(const HnSRTreeNeighbor &neighbor1, const HnSRTreeNeighbor &neighbor2)
{
	int n;
	double w;

	/* distance */
	w = neighbor1.getDistance() - neighbor2.getDistance();
	if(w < 0)
		return -1;
	if(w > 0)
		return 1;

	/* data */
	n = neighbor1.getData().length() - neighbor2.getData().length();
	if(n < 0)
		return -1;
	if(n > 0)
		return 1;

	return memcmp(neighbor1.getData().chars(), neighbor2.getData().chars(),
		      neighbor1.getData().length());
}

HnSRTreeNeighborArray
HnSRTreeNeighborObj::sort(const HnSRTreeNeighborArray &neighbors)
{
	HnSRTreeNeighborArray array = new_HnSRTreeNeighborArray(neighbors);
	int len = array.length();
	int i, j;

	for(i=0; i<len; i++) {
		for(j=i+1; j<len; j++) {
			if(compare(array[i], array[j]) > 0)
				array.swap(i, j);
		}
	}

	return array;
}

HnString
HnSRTreeNeighborObj::toString(void) const
{
	HnString string;
	char buffer[256];

	string = "[ point = ";
	string += point.toString();
	sprintf(buffer, ", distance = %g", distance);
	string += buffer;
	string += " ]";

	return string;
}
