/*
 * HnRangeTest.cc
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
 * $Id: HnRangeTest.cc,v 1.2 1997/06/02 09:35:29 katayama Exp $
 */

#include "HnString.hh"
#include "HnRange.hh"

HnRange
new_HnRange(double min, HnRange::EndType minType,
	    double max, HnRange::EndType maxType)
{
	HnRange range;

	range.set(min, minType, max, maxType);
	return range;
}

void
intersect(const HnRange &range1, const HnRange &range2)
{
	HnRange overlappingRange, boundingRange;

	overlappingRange = range1.getOverlappingRange(range2);
	boundingRange = range1.getBoundingRange(range2);

	printf("range1: %s\n", (char *)range1.toString());
	printf("range2: %s\n", (char *)range2.toString());
	printf("overlapping range: %s\n",
	       overlappingRange == HnRange::null ? 
	       "null" : (char *)overlappingRange.toString());
	printf("bounding range   : %s\n",
	       boundingRange == HnRange::null ?
	       "null" : (char *)boundingRange.toString());
	printf("range1.overlaps(range2) : %s\n",
	       (range1.overlaps(range2) ? "true" : "false"));
	printf("range2.overlaps(range1) : %s\n",
	       (range2.overlaps(range1) ? "true" : "false"));
	printf("range1.includes(range2) : %s\n",
	       (range1.includes(range2) ? "true" : "false"));
	printf("range2.includes(range1) : %s\n",
	       (range2.includes(range1) ? "true" : "false"));
	printf("\n");
}

int
main(int, char *[])
{
	HnRange range1, range2;

	/*
	 * (0, 0), (0, 0)
	 */
	range1 = new_HnRange(0, HnRange::EXCLUSIVE, 0, HnRange::EXCLUSIVE);
	range2 = new_HnRange(0, HnRange::EXCLUSIVE, 0, HnRange::EXCLUSIVE);
	intersect(range1, range2);

	/*
	 * [0, 0), (0, 0)
	 */
	range1 = new_HnRange(0, HnRange::INCLUSIVE, 0, HnRange::EXCLUSIVE);
	range2 = new_HnRange(0, HnRange::EXCLUSIVE, 0, HnRange::EXCLUSIVE);
	intersect(range1, range2);

	/*
	 * (0, 0], (0, 0)
	 */
	range1 = new_HnRange(0, HnRange::EXCLUSIVE, 0, HnRange::INCLUSIVE);
	range2 = new_HnRange(0, HnRange::EXCLUSIVE, 0, HnRange::EXCLUSIVE);
	intersect(range1, range2);

	/*
	 * [0, 0), (0, 0]
	 */
	range1 = new_HnRange(0, HnRange::INCLUSIVE, 0, HnRange::EXCLUSIVE);
	range2 = new_HnRange(0, HnRange::EXCLUSIVE, 0, HnRange::INCLUSIVE);
	intersect(range1, range2);

	/*
	 * [0, 0], [0, 1]
	 */
	range1 = new_HnRange(0, HnRange::INCLUSIVE, 0, HnRange::INCLUSIVE);
	range2 = new_HnRange(0, HnRange::INCLUSIVE, 1, HnRange::INCLUSIVE);
	intersect(range1, range2);

	/*
	 * [0, 0], (0, 1]
	 */
	range1 = new_HnRange(0, HnRange::INCLUSIVE, 0, HnRange::INCLUSIVE);
	range2 = new_HnRange(0, HnRange::EXCLUSIVE, 1, HnRange::INCLUSIVE);
	intersect(range1, range2);

	/*
	 * [0, 1], [1, 2]
	 */
	range1 = new_HnRange(0, HnRange::INCLUSIVE, 1, HnRange::INCLUSIVE);
	range2 = new_HnRange(1, HnRange::INCLUSIVE, 2, HnRange::INCLUSIVE);
	intersect(range1, range2);

	/*
	 * [0, 1], (1, 2]
	 */
	range1 = new_HnRange(0, HnRange::INCLUSIVE, 1, HnRange::INCLUSIVE);
	range2 = new_HnRange(1, HnRange::EXCLUSIVE, 2, HnRange::INCLUSIVE);
	intersect(range1, range2);

	/*
	 * [0, 1), (1, 2]
	 */
	range1 = new_HnRange(0, HnRange::INCLUSIVE, 1, HnRange::EXCLUSIVE);
	range2 = new_HnRange(1, HnRange::EXCLUSIVE, 2, HnRange::INCLUSIVE);
	intersect(range1, range2);

	/*
	 * [1, 1], [0, 2]
	 */
	range1 = new_HnRange(1, HnRange::INCLUSIVE, 1, HnRange::INCLUSIVE);
	range2 = new_HnRange(0, HnRange::INCLUSIVE, 2, HnRange::INCLUSIVE);
	intersect(range1, range2);

	/*
	 * [0, 2], [1, 1]
	 */
	range1 = new_HnRange(0, HnRange::INCLUSIVE, 2, HnRange::INCLUSIVE);
	range2 = new_HnRange(1, HnRange::INCLUSIVE, 1, HnRange::INCLUSIVE);
	intersect(range1, range2);

	/*
	 * [0, 1], (0, 1)
	 */
	range1 = new_HnRange(0, HnRange::INCLUSIVE, 1, HnRange::INCLUSIVE);
	range2 = new_HnRange(0, HnRange::EXCLUSIVE, 1, HnRange::EXCLUSIVE);
	intersect(range1, range2);

	/*
	 * [0, 1), (0, 1)
	 */
	range1 = new_HnRange(0, HnRange::INCLUSIVE, 1, HnRange::EXCLUSIVE);
	range2 = new_HnRange(0, HnRange::EXCLUSIVE, 1, HnRange::EXCLUSIVE);
	intersect(range1, range2);

	/*
	 * (0, 1], (0, 1)
	 */
	range1 = new_HnRange(0, HnRange::EXCLUSIVE, 1, HnRange::INCLUSIVE);
	range2 = new_HnRange(0, HnRange::EXCLUSIVE, 1, HnRange::EXCLUSIVE);
	intersect(range1, range2);

	return 0;
}

