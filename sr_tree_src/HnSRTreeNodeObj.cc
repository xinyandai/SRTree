/*
 * HnSRTreeNodeObj.cc
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
 * 03/22/96 katayama@rd.nacsis.ac.jp
 * $Id: HnSRTreeNodeObj.cc,v 1.2 1997/06/02 09:35:42 katayama Exp $
 */

#include "HnSRTreeNode.hh"
#include "HnSRTreeNodeObj.hh"

void
HnSRTreeNodeObj::initialize(int dimension, int blockSize, off_t offset)
{
	this->dimension = dimension;
	this->blockSize = blockSize;
	this->maxCount = getMaxCount(dimension, blockSize);
	this->offset = offset;
	count = 0;
	cores = new_HnSRTreeCoreArray();
	rects = new_HnRectArray();
	offsets = new off_t[maxCount];
}

void
HnSRTreeNodeObj::dispose(void)
{
	delete offsets;
}
	
int
HnSRTreeNodeObj::getMaxCount(int dimension, int blockSize)
{
	size_t coreSize =
		sizeof(double) * dimension + sizeof(double) + sizeof(int);
	size_t rectSize = sizeof(double) * dimension * 2;

	return (blockSize - sizeof(HnSRTreeBlock::Type) - sizeof(int))
		/ (coreSize + rectSize + sizeof(off_t));
}

HnSRTreeNodeObj::HnSRTreeNodeObj(int dimension, int blockSize, off_t offset)
{
	initialize(dimension, blockSize, offset);
}

HnSRTreeNodeObj::HnSRTreeNodeObj(int dimension, const HnSRTreeBlock &block)
{
	const char *bp;
	HnSRTreeBlock::Type type;
	int i;

	initialize(dimension, block.getSize(), block.getOffset());

	bp = block.getBytes();

	memcpy(&type, bp, sizeof(HnSRTreeBlock::Type));
	bp += sizeof(HnSRTreeBlock::Type);

	if(type != HnSRTreeBlock::NODE)
		HnAbort("the given block is not of a node.");

	memcpy(&count, bp, sizeof(int));
	bp += sizeof(int);

	cores.clear();
	rects.clear();

	for(i=0; i<count; i++) {
		HnPoint center;
		double radius;
		int weight;
		HnRect rect;
		int axis;

		/* core */
		center = new_HnPoint(dimension);

		for(axis=0; axis<dimension; axis++) {
			double coord;

			memcpy(&coord, bp, sizeof(double));
			bp += sizeof(double);

			center.setCoord(coord, axis);
		}

		memcpy(&radius, bp, sizeof(double));
		bp += sizeof(double);

		memcpy(&weight, bp, sizeof(int));
		bp += sizeof(int);

		cores.append(new_HnSRTreeCore(center, radius, weight));

		/* rect */
		rect = new_HnRect(dimension);

		for(axis=0; axis<dimension; axis++) {
			double min, max;

			memcpy(&min, bp, sizeof(double));
			bp += sizeof(double);
			memcpy(&max, bp, sizeof(double));
			bp += sizeof(double);

			rect.setRange(min, HnRange::INCLUSIVE,
				      max, HnRange::INCLUSIVE,
				      axis);
		}
		rects.append(rect);

		/* offset */
		memcpy(&offsets[i], bp, sizeof(offsets[i]));
		bp += sizeof(offsets[i]);
	}
}

HnSRTreeNodeObj::~HnSRTreeNodeObj(void)
{
	dispose();
}

size_t
HnSRTreeNodeObj::getSize(void) const
{
	size_t coreSize =
		sizeof(double) * dimension + sizeof(double) + sizeof(int);
	size_t rectSize = sizeof(double) * dimension * 2;

	return sizeof(HnSRTreeBlock::Type) + sizeof(int)
		+ (coreSize + rectSize + sizeof(off_t)) * count;
}

HnSRTreeCore
HnSRTreeNodeObj::getCore(void) const
{
	HnPoint center;
	double radius;
	int weight;
	int i, axis;
	double sum, maxDistance;

	/* weight */

	weight = 0;
	for(i=0; i<count; i++)
		weight += cores[i].getWeight();

	/* center */

	center = new_HnPoint(dimension);

	for(axis=0; axis<dimension; axis++) {
		sum = 0;

		for(i=0; i<count; i++) {
			double coord = cores[i].getCenter().getCoord(axis);

			sum += coord * cores[i].getWeight();
		}

		center.setCoord(sum / weight, axis);
	}

	/* radius */

	maxDistance = -1;

	for(i=0; i<count; i++) {
		double sphereDistance, rectDistance, distance;

		sphereDistance = cores[i].getCenter().getDistance(center)
			+ cores[i].getRadius();
		rectDistance = rects[i].getMaxDistance(center);

		if(sphereDistance < rectDistance)
			distance = sphereDistance;
		else
			distance = rectDistance;

		if(maxDistance < 0 ||
		   distance > maxDistance)
			maxDistance = distance;
	}

	radius = maxDistance;

	return new_HnSRTreeCore(center, radius, weight);
}

HnSRTreeBlock
HnSRTreeNodeObj::toBlock(void) const
{
	HnSRTreeBlock block;
	char *buffer = new char[blockSize];
	char *bp;
	HnSRTreeBlock::Type type;
	int i;

	bp = buffer;
	memset(buffer, 0, blockSize);

	type = HnSRTreeBlock::NODE;
	memcpy(bp, &type, sizeof(type));
	bp += sizeof(type);

	memcpy(bp, &count, sizeof(count));
	bp += sizeof(count);

	for(i=0; i<count; i++) {
		int axis;
		double radius;
		int weight;

		/* core */
		for(axis=0; axis<dimension; axis++) {
			double coord = cores[i].getCenter().getCoord(axis);

			memcpy(bp, &coord, sizeof(coord));
			bp += sizeof(coord);
		}

		radius = cores[i].getRadius();
		memcpy(bp, &radius, sizeof(radius));
		bp += sizeof(radius);

		weight = cores[i].getWeight();
		memcpy(bp, &weight, sizeof(weight));
		bp += sizeof(weight);

		/* rect */
		for(axis=0; axis<dimension; axis++) {
			double min = rects[i].getRange(axis).getMin();
			double max = rects[i].getRange(axis).getMax();

			memcpy(bp, &min, sizeof(min));
			bp += sizeof(min);
			memcpy(bp, &max, sizeof(max));
			bp += sizeof(max);
		}

		/* offset */
		memcpy(bp, &offsets[i], sizeof(offsets[i]));
		bp += sizeof(offsets[i]);
	}

	if(bp > buffer + blockSize)
		HnAbort("error in block arrangement.");

	block = new_HnSRTreeBlock(offset, buffer, blockSize);

	delete buffer;

	return block;
}

HnString
HnSRTreeNodeObj::toString(void) const
{
	HnString string;
	char buffer[256];
	int i;

	string = "[ ";

	sprintf(buffer, "type=NODE, offset=0x%08X", (unsigned int)getOffset());
	string += buffer;

	for(i=0; i<getCount(); i++) {
		sprintf(buffer, ", cores[%d]=", i);
		string += buffer;
		string += (char *)getCoreAt(i).toString();

		sprintf(buffer, ", rects[%d]=", i);
		string += buffer;
		string += (char *)getRectAt(i).toString();

		sprintf(buffer, ", offsets[%d]=0x%08X",
			i, (unsigned int)getOffsetAt(i));
		string += buffer;
	}

	string += " ]";

	return string;
}
