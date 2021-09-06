/*
 * HnRange.cc
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
 * $Id: HnRange.cc,v 1.2 1997/06/02 09:35:28 katayama Exp $
 */

#include "HnRange.hh"

HnRange HnRange::null;

HnBool
HnRange::equals(const HnRange &range) const
{
	if(getMin() == range.getMin() && getMinType() == range.getMinType() &&
	   getMax() == range.getMax() && getMaxType() == range.getMaxType())
		return HnTRUE;
	else
		return HnFALSE;
}

HnBool
HnRange::overlaps(const HnRange &range) const
{
	/* separated */
	if(range.getMax() < getMin())
		return HnFALSE;
	if(range.getMin() > getMax())
		return HnFALSE;

	/* touching */
	if(range.getMax() == getMin()) {
		if((getMinType() == HnRange::INCLUSIVE ||
		    (getMax() == getMin() &&
		     getMaxType() == HnRange::INCLUSIVE)) &&
		   (range.getMaxType() == HnRange::INCLUSIVE ||
		    (range.getMin() == range.getMax() &&
		     range.getMinType() == HnRange::INCLUSIVE)))
			return HnTRUE;
		else
			return HnFALSE;
	}
	if(range.getMin() == getMax()) {
		if((getMaxType() == HnRange::INCLUSIVE ||
		    (getMin() == getMax() &&
		     getMinType() == HnRange::INCLUSIVE)) &&
		   (range.getMinType() == HnRange::INCLUSIVE ||
		    (range.getMax() == range.getMin() &&
		     range.getMaxType() == HnRange::INCLUSIVE)))
			return HnTRUE;
		else
			return HnFALSE;
	}
		   
	/* overlapped */
	return HnTRUE;
}

HnBool
HnRange::includes(const HnRange &range) const
{
	/* separated or overlapped */
	if(range.getMin() < getMin())
		return HnFALSE;
	if(range.getMax() > getMax())
		return HnFALSE;

	/* coincides */
	if(range.getMin() == getMin()) {
		if(!(getMinType() == HnRange::INCLUSIVE ||
		     (getMin() == getMax() &&
		      getMaxType() == HnRange::INCLUSIVE)) &&
		   (range.getMinType() == HnRange::INCLUSIVE ||
		    (range.getMin() == range.getMax() &&
		     range.getMaxType() == HnRange::INCLUSIVE)))
			return HnFALSE;
	}
	if(range.getMax() == getMax()) {
		if(!(getMaxType() == HnRange::INCLUSIVE ||
		     (getMin() == getMax() &&
		      getMinType() == HnRange::INCLUSIVE)) &&
		   (range.getMaxType() == HnRange::INCLUSIVE ||
		    (range.getMin() == range.getMax() &&
		     range.getMinType() == HnRange::INCLUSIVE)))
			return HnFALSE;
	}
		   
	/* null point */
	if(range.getMin() == range.getMax() &&
	   range.getMinType() == HnRange::EXCLUSIVE &&
	   range.getMaxType() == HnRange::EXCLUSIVE)
		return HnFALSE;

	/* includes */
	return HnTRUE;
}

HnBool
HnRange::includes(double x) const
{
	/* outside */
	if(x < getMin() || x > getMax())
		return HnFALSE;

	if(x == getMin()) {
		if(getMinType() == HnRange::INCLUSIVE ||
		   (getMax() == getMin() &&
		    getMaxType() == HnRange::INCLUSIVE))
			return HnTRUE;
		else
			return HnFALSE;
	}
	if(x == getMax()) {
		if(getMaxType() == HnRange::INCLUSIVE ||
		   (getMin() == getMax() &&
		    getMinType() == HnRange::INCLUSIVE))
			return HnTRUE;
		else
			return HnFALSE;
	}

	/* inside */
	return HnTRUE;
}

HnRange
HnRange::getOverlappingRange(const HnRange &range) const
{
	double minValue, maxValue;
	EndType minType, maxType;
	HnRange overlappingRange;

	/* separated */
	if(range.getMax() < getMin())
		return HnRange::null;
	if(range.getMin() > getMax())
		return HnRange::null;

	/* touching */
	if(range.getMax() == getMin()) {
		if((getMinType() == HnRange::INCLUSIVE ||
		    (getMax() == getMin() &&
		     getMaxType() == HnRange::INCLUSIVE)) &&
		   (range.getMaxType() == HnRange::INCLUSIVE ||
		    (range.getMin() == range.getMax() &&
		     range.getMinType() == HnRange::INCLUSIVE))) {
			overlappingRange.set(getMin(), HnRange::INCLUSIVE,
					     getMin(), HnRange::INCLUSIVE);
			return overlappingRange;
		}
		else
			return HnRange::null;
	}
	if(range.getMin() == getMax()) {
		if((getMaxType() == HnRange::INCLUSIVE ||
		    (getMin() == getMax() &&
		     getMinType() == HnRange::INCLUSIVE)) &&
		   (range.getMinType() == HnRange::INCLUSIVE ||
		    (range.getMax() == range.getMin() &&
		     range.getMaxType() == HnRange::INCLUSIVE))) {
			overlappingRange.set(getMax(), HnRange::INCLUSIVE,
					     getMax(), HnRange::INCLUSIVE);
			return overlappingRange;
		}
		else
			return HnRange::null;
	}
		   
	/* overlapped */
	if(getMin() > range.getMin()) {
		minValue = getMin();
		minType = getMinType();
	}
	else if(getMin() < range.getMin()) {
		minValue = range.getMin();
		minType = range.getMinType();
	}
	else {
		minValue = getMin();
		if(getMinType() == HnRange::INCLUSIVE &&
		   range.getMinType() == HnRange::INCLUSIVE)
			minType = HnRange::INCLUSIVE;
		else
			minType = HnRange::EXCLUSIVE;
	}

	if(getMax() < range.getMax()) {
		maxValue = getMax();
		maxType = getMaxType();
	}
	else if(getMax() > range.getMax()) {
		maxValue = range.getMax();
		maxType = range.getMaxType();
	}
	else {
		maxValue = getMax();
		if(getMaxType() == HnRange::INCLUSIVE &&
		   range.getMaxType() == HnRange::INCLUSIVE)
			maxType = HnRange::INCLUSIVE;
		else
			maxType = HnRange::EXCLUSIVE;
	}

	overlappingRange.set(minValue, minType, maxValue, maxType);
	return overlappingRange;
}

HnRange
HnRange::getBoundingRange(const HnRange &range) const
{
	double minValue, maxValue;
	EndType minType, maxType;
	HnRange boundingRange;

	/* get the minimum value and the minimum type */

	minValue = getMin();
	minType = getMinType();

	if(getMax() == minValue) {
		if(getMaxType() == HnRange::INCLUSIVE)
			minType = HnRange::INCLUSIVE;
	}
	if(range.getMin() <= minValue) {
		minValue = range.getMin();
		if(range.getMinType() == HnRange::INCLUSIVE)
			minType = HnRange::INCLUSIVE;
	}
	if(range.getMax() == minValue) {
		if(range.getMaxType() == HnRange::INCLUSIVE)
			minType = HnRange::INCLUSIVE;
	}

	/* get the maximum value and the maximum type */

	maxValue = getMax();
	maxType = getMaxType();

	if(getMin() == maxValue) {
		if(getMinType() == HnRange::INCLUSIVE)
			maxType = HnRange::INCLUSIVE;
	}
	if(range.getMax() >= maxValue) {
		maxValue = range.getMax();
		if(range.getMaxType() == HnRange::INCLUSIVE)
			maxType = HnRange::INCLUSIVE;
	}
	if(range.getMin() == maxValue) {
		if(range.getMinType() == HnRange::INCLUSIVE)
			maxType = HnRange::INCLUSIVE;
	}

	boundingRange.set(minValue, minType, maxValue, maxType);
	return boundingRange;
}

HnString
HnRange::toString(void) const {
	char buffer[256];

	sprintf(buffer, "%c%g, %g%c",
		(getMinType() == HnRange::INCLUSIVE ? '[' : '('), getMin(),
		getMax(), (getMaxType() == HnRange::INCLUSIVE ? ']' : ')'));

	return buffer;
}
