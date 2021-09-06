/*
 * HnRange.hh
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
 * $Id: HnRange.hh,v 1.2 1997/06/02 09:35:28 katayama Exp $
 */

#ifndef _HnRange_hh
#define _HnRange_hh

#include "HnAbort.hh"
#include "HnString.hh"

class HnRange {
public:
	enum EndType { INCLUSIVE, EXCLUSIVE };
	static HnRange null;

private:
	double minValue;
	EndType minType;
	double maxValue;
	EndType maxType;

public:
	HnRange(void) {
		minValue = 0;
		minType = EXCLUSIVE;
		maxValue = 0;
		maxType = EXCLUSIVE;
	}

	void set(double minValue, EndType minType,
		 double maxValue, EndType maxType) {
		if(minValue > maxValue)
			HnAbort("HnRange: "
				"min(%g) is greater than max(%g).",
				minValue, maxValue);

		this->minValue = minValue;
		this->minType = minType;
		this->maxValue = maxValue;
		this->maxType = maxType;
	}

	/* attributes */
	double getMin(void) const {
		return minValue;
	}
	double getMax(void) const {
		return maxValue;
	}

	EndType getMinType(void) const {
		return minType;
	}
	EndType getMaxType(void) const {
		return maxType;
	}

	/* utilities */
	double getWidth(void) const {
		return maxValue - minValue;
	}

	HnBool operator==(const HnRange &range) const {
		return equals(range);
	}
	HnBool equals(const HnRange &range) const;
	HnBool overlaps(const HnRange &range) const;
	HnBool includes(const HnRange &range) const;
	HnBool includes(double x) const;

	HnRange getOverlappingRange(const HnRange &range) const;
	HnRange getBoundingRange(const HnRange &range) const;

	HnString toString(void) const;
};

#endif /* _HnRange_hh */
