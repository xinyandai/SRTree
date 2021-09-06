/*
 * HnSRTreeLeafObj.cc
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
 * 03/21/96 katayama@rd.nacsis.ac.jp
 * $Id: HnSRTreeLeafObj.cc,v 1.2 1997/06/02 09:35:39 katayama Exp $
 */
#include "HnSRTreeCore.hh"
#include "HnSRTreeBlock.hh"


#include "HnSRTreeLeaf.hh"
#include "HnSRTreeLeafObj.hh"

void
HnSRTreeLeafObj::initialize(int dimension, int dataSize,
			    int blockSize, off_t offset)
{
	this->dimension = dimension;
	this->dataSize = dataSize;
	this->blockSize = blockSize;
	this->maxCount = getMaxCount(dimension, dataSize, blockSize);
	this->offset = offset;
	count = 0;
	points = new_HnPointArray();
	values = new_HnDataArray();
}

void
HnSRTreeLeafObj::dispose(void)
{
}

HnSRTreeLeafObj::HnSRTreeLeafObj(int dimension, int dataSize, 
				 int blockSize, off_t offset)
{
	initialize(dimension, dataSize, blockSize, offset);
}

HnSRTreeLeafObj::HnSRTreeLeafObj(int dimension, int dataSize,
				 const HnSRTreeBlock &block)
{
	const char *bp;
	int i;
	size_t size;

	initialize(dimension, dataSize, block.getSize(), block.getOffset());

	bp = block.getBytes();

	bp += sizeof(HnSRTreeBlock::Type);

	memcpy(&count, bp, sizeof(int));
	bp += sizeof(int);

	points.clear();

	for(i=0; i<count; i++) {
		HnPoint point = new_HnPoint(dimension);
		int axis;

		for(axis=0; axis<dimension; axis++) {
			double coord;

			memcpy(&coord, bp, sizeof(double));
			bp += sizeof(double);

			point.setCoord(coord, axis);
		}
		points.append(point);

		memcpy(&size, bp, sizeof(size));
		bp += sizeof(size);

		values.append(new_HnData(bp, size));
		bp += size;
	}
}

HnSRTreeLeafObj::~HnSRTreeLeafObj(void)
{
	dispose();
}

size_t
HnSRTreeLeafObj::getSize(void) const
{
	size_t size;
	int i;

	size = sizeof(HnSRTreeBlock::Type) + sizeof(int);

	for(i=0; i<count; i++) {
		size += sizeof(double) * dimension;
		size += sizeof(size_t) + values[i].length();
	}

	return size;
}

HnSRTreeBlock
HnSRTreeLeafObj::toBlock(void) const
{
	HnSRTreeBlock block;
	char *buffer = new char[blockSize];
	char *bp;
	HnSRTreeBlock::Type type;
	int i;
	size_t size;

	bp = buffer;
	memset(buffer, 0, blockSize);

	type = HnSRTreeBlock::LEAF;
	memcpy(bp, &type, sizeof(type));
	bp += sizeof(type);

	memcpy(bp, &count, sizeof(count));
	bp += sizeof(count);

	for(i=0; i<count; i++) {
		int axis;

		for(axis=0; axis<dimension; axis++) {
			double coord = points[i].getCoord(axis);

			memcpy(bp, &coord, sizeof(coord));
			bp += sizeof(coord);
		}

		size = values[i].length();
		memcpy(bp, &size, sizeof(size));
		bp += sizeof(size);

		memcpy(bp, values[i].chars(), size);
		bp += size;
	}

	if(bp > buffer + blockSize)
		HnAbort("error in block arrangement.");

	block = new_HnSRTreeBlock(offset, buffer, blockSize);

	delete buffer;

	return block;
}

HnSRTreeCore
HnSRTreeLeafObj::getCore(void) const {
	int i, axis;
	double sum, maxDistance;
	HnPoint center;
	double radius;
	int weight;

	/* center */

	center = new_HnPoint(dimension);

	for(axis=0; axis<dimension; axis++) {
		sum = 0;
		for(i=0; i<count; i++)
			sum += points[i].getCoord(axis);

		center.setCoord(sum / count, axis);
	}

	/* radius */

	maxDistance = -1;
	for(i=0; i<count; i++) {
		double distance = points[i].getDistance(center);

		if(maxDistance < 0 ||
		   distance > maxDistance)
			maxDistance = distance;
	}
	radius = maxDistance;

	/* weight */

	weight = count;

	return new_HnSRTreeCore(center, radius, weight);
}

HnRect
HnSRTreeLeafObj::getBoundingRect(void) const
{
	int i, axis;
	double min, max;
	HnRect boundingRect;

	if(count == 0)
		return HnRect::null;

	boundingRect = new_HnRect(dimension);

	for(axis=0; axis<dimension; axis++) {
		min = max = points[0].getCoord(axis);

		for(i=1; i<count; i++) {
			double coord = points[i].getCoord(axis);

			if(coord < min)
				min = coord;
			if(coord > max)
				max = coord;
		}

		boundingRect.setRange(min, HnRange::INCLUSIVE,
				      max, HnRange::INCLUSIVE,
				      axis);
	}

	return boundingRect;
}

HnString
HnSRTreeLeafObj::toString(void) const
{
	HnString string;
	char buffer[256];
	int i;

	string = "[ ";

	sprintf(buffer, "type=LEAF, offset=0x%08X", (unsigned int)getOffset());
	string += buffer;

	for(i=0; i<getCount(); i++) {
		sprintf(buffer, ", points[%d]=", i);
		string += buffer;
		string += (char *)getPointAt(i).toString();

		sprintf(buffer, ", values[%d].length=%d",
			i, getDataAt(i).length());
		string += buffer;
	}

	string += " ]";

	return string;
}

int
HnSRTreeLeafObj::getMaxCount(int dimension, int dataSize, int blockSize)
{
	size_t pointSize = sizeof(double) * dimension;

	return (blockSize - sizeof(HnSRTreeBlock::Type) - sizeof(int))
		/ (pointSize + sizeof(size_t) + dataSize);
}
