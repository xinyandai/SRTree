/*
 * HnRectObj.cc
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
 * $Id: HnRectObj.cc,v 1.2 1997/06/02 09:35:29 katayama Exp $
 */

#include <math.h>
#include "HnRect.hh"
#include "HnRectObj.hh"

double
HnRectObj::getMaxWidth(void) const
{
	double max, width;
	int i;

	max = getRange(0).getWidth();

	for(i=1; i<dimension; i++) {
		width = getRange(i).getWidth();

		if(width > max)
			max = width;
	}

	return max;
}

double
HnRectObj::getMargin(void) const
{
	int i, numEdges;
	double sum;

	/* the number of edges for each axis */
	numEdges = 1 << (dimension - 1);

	sum = 0;
	for(i=0; i<dimension; i++)
		sum += getRange(i).getWidth();

	return sum * numEdges;
}

HnPoint
HnRectObj::getCenterPoint(void) const
{
	HnPoint point = new_HnPoint(dimension);
	int i;

	for(i=0; i<dimension; i++) {
		double min = ranges[i].getMin();
		double max = ranges[i].getMax();

		point.setCoord((min + max) / 2, i);
	}

	return point;
}

HnBool
HnRectObj::equals(const HnRect &rect) const
{
	int i;

	if(rect.getDimension() != dimension)
		HnAbort("mismatch in dimensions.");

	for(i=0; i<dimension; i++) {
		if(!getRange(i).equals(rect.getRange(i)))
			return HnFALSE;
	}

	return HnTRUE;
}

HnBool
HnRectObj::overlaps(const HnRect &rect) const
{
	int i;

	if(rect.getDimension() != dimension)
		HnAbort("mismatch in dimensions.");

	for(i=0; i<dimension; i++) {
		if(!getRange(i).overlaps(rect.getRange(i)))
			return HnFALSE;
	}

	return HnTRUE;
}

HnBool
HnRectObj::includes(const HnRect &rect) const
{
	int i;

	if(rect.getDimension() != dimension)
		HnAbort("mismatch in dimensions.");

	for(i=0; i<dimension; i++) {
		if(!getRange(i).includes(rect.getRange(i)))
			return HnFALSE;
	}

	return HnTRUE;
}

HnBool
HnRectObj::includes(const HnPoint &point) const
{
	int i;

	if(point.getDimension() != dimension)
		HnAbort("mismatch in dimensions.");

	for(i=0; i<dimension; i++) {
		if(!getRange(i).includes(point.getCoord(i)))
			return HnFALSE;
	}

	return HnTRUE;
}

HnRect
HnRectObj::getOverlappingRect(const HnRect &rect) const
{
	int i;
	HnRange overlappingRange;
	HnRect overlappingRect;

	if(rect.getDimension() != dimension)
		HnAbort("mismatch in dimensions.");

	overlappingRect = new_HnRect(dimension);

	for(i=0; i<dimension; i++) {
		overlappingRange = 
			getRange(i).getOverlappingRange(rect.getRange(i));

		if(overlappingRange == HnRange::null)
			return HnRect::null;

		overlappingRect.setRange(overlappingRange, i);
	}

	return overlappingRect;
}

HnRect
HnRectObj::getBoundingRect(const HnRect &rect) const
{
	int i;
	HnRange boundingRange;
	HnRect boundingRect;

	if(rect.getDimension() != dimension)
		HnAbort("mismatch in dimensions.");

	boundingRect = new_HnRect(dimension);

	for(i=0; i<dimension; i++) {
		boundingRange = getRange(i).getBoundingRange(rect.getRange(i));

		boundingRect.setRange(boundingRange, i);
	}

	return boundingRect;
}

double
HnRectObj::getDiagonal(void) const
{
	int i;
	double sum = 0;

	for(i=0; i<dimension; i++) {
		double width = getRange(i).getWidth();

		sum += width * width;
	}

	return sqrt(sum);
}

double
HnRectObj::getVolume(void) const
{
	int i;
	double volume = 1;

	for(i=0; i<dimension; i++)
		volume *= getRange(i).getWidth();

	return volume;
}

double
HnRectObj::getMinDistance(const HnPoint &point) const
{
	double *nearest = new double[dimension];
	int axis;
	double sum;

	if(point.getDimension() != dimension)
		HnAbort("mismatch in dimensions.");

	/* find nearest point */
	for(axis=0; axis<dimension; axis++) {
		double min = getRange(axis).getMin();
		double max = getRange(axis).getMax();
		double x = point.getCoord(axis);

		if(x < min)
			nearest[axis] = min;
		else if (x < max)
			nearest[axis] = x;
		else
			nearest[axis] = max;
	}

	/* calculate distance */
	sum = 0;
	for(axis=0; axis<dimension; axis++) {
		double dif = point.getCoord(axis) - nearest[axis];
		sum += dif * dif;
	}

	delete nearest;

	return sqrt(sum);
}

double
HnRectObj::getMaxDistance(const HnPoint &point) const
{
	double *farthest = new double[dimension];
	int axis;
	double sum;

	if(point.getDimension() != dimension)
		HnAbort("mismatch in dimensions.");

	/* find farthest point */
	for(axis=0; axis<dimension; axis++) {
		double min = getRange(axis).getMin();
		double max = getRange(axis).getMax();
		double x = point.getCoord(axis);

		if(x < (min + max) / 2)
			farthest[axis] = max;
		else
			farthest[axis] = min;
	}

	/* calculate distance */
	sum = 0;
	for(axis=0; axis<dimension; axis++) {
		double dif = point.getCoord(axis) - farthest[axis];
		sum += dif * dif;
	}

	delete farthest;

	return sqrt(sum);
}

HnString
HnRectObj::toString(void) const
{
	HnString string;
	int i;

	string = "[ ";

	for(i=0; i<dimension; i++) {
		if(i != 0)
			string += ", ";
		string += ranges[i].toString();
	}

	string += " ]";

	return string;
}

/*
 * Class Methods
 */

HnRect
HnRectObj::getBoundingRect(const HnRectArray &rects)
{
	HnRect rect;
	int i;

	rect = rects[0];

	for(i=1; i<rects.length(); i++)
		rect = rect.getBoundingRect(rects[i]);

	return rect;
}
