/*
 * HnPointObj.cc
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
 * $Id: HnPointObj.cc,v 1.2 1997/06/02 09:35:26 katayama Exp $
 */

#include "HnPoint.hh"
#include "HnPointObj.hh"
#include <math.h>

HnBool
HnPointObj::equals(const HnPoint &point) const
{
	int i;

	if(point.getDimension() != dimension)
		HnAbort("mismatch in dimensions.");

	for(i=0; i<dimension; i++) {
		if(getCoord(i) != point.getCoord(i))
			return HnFALSE;
	}

	return HnTRUE;
}

HnString
HnPointObj::toString(void) const
{
	HnString string;
	char buffer[256];
	int i;

	string = "[ ";

	for(i=0; i<dimension; i++) {
		if(i != 0)
			string += ", ";
		sprintf(buffer, "%g", getCoord(i));
		string += buffer;
	}

	string += " ]";

	return string;
}

double
HnPointObj::getDistance(const HnPoint &point) const
{
	int i;
	double dif, sum;

	if(point.getDimension() != dimension)
		HnAbort("mismatch in dimensions.");

	sum = 0;
	for(i=0; i<dimension; i++) {
		dif = getCoord(i) - point.getCoord(i);
		sum += dif * dif;
	}

	return sqrt(sum);
}
	
