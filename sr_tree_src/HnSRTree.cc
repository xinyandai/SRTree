/*
 * HnSRTree.cc
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
 * 10/14/96 katayama@rd.nacsis.ac.jp
 * $Id: HnSRTree.cc,v 1.4 1997/06/02 09:35:35 katayama Exp $
 */

#include <stdlib.h>
#include "HnSRTree.h"
#include "HnSRTreeFile.hh"

static HnSRTreeRecord *allocateRecords(int dimension, int dataSize, int count);
static void freeRecords(HnSRTreeRecord *records, int count);
static void setRecord(HnSRTreeRecord *record,
		      const HnPoint &point, const HnData &data);

HnSRTree *
HnSRTreeCreate(const char *path, int dimension, int dataSize, int blockSize,
	       int splitFactor, int reinsertFactor)
{
	HnSRTreeFile *file;

	file = new HnSRTreeFile();

	*file = new_HnSRTreeFile(path, dimension, dataSize, blockSize,
				 splitFactor, reinsertFactor);

	if(*file == HnSRTreeFile::null) {
		delete file;
		return NULL;
	}

	return (void *)file;
}

HnSRTree *
HnSRTreeUpdate(const char *path)
{
	HnSRTreeFile *file;

	file = new HnSRTreeFile();

	*file = new_HnSRTreeFile(path, "rw");

	if(*file == HnSRTreeFile::null) {
		delete file;
		return NULL;
	}

	return (void *)file;
}

HnSRTree *
HnSRTreeOpen(const char *path)
{
	HnSRTreeFile *file;

	file = new HnSRTreeFile();

	*file = new_HnSRTreeFile(path, "r");

	if(*file == HnSRTreeFile::null) {
		delete file;
		return NULL;
	}

	return (void *)file;
}

void
HnSRTreeClose(HnSRTree *srtree)
{
	HnSRTreeFile *file = (HnSRTreeFile *)srtree;

	file->close();
	delete file;
}

void
HnSRTreeStore(HnSRTree *srtree,
	      const double coords[], const void *data, int size)
{
	HnSRTreeFile *file = (HnSRTreeFile *)srtree;
	int dimension = file->getDimension();
	HnPoint point;
	HnData value;
	int i;

	point = new_HnPoint(dimension);
	for(i=0; i<dimension; i++)
		point.setCoord(coords[i], i);

	value = new_HnData(data, size);

	file->store(point, value);
}

void
HnSRTreeRemove(HnSRTree *srtree,
	       const double coords[], const void *data, int size)
{
	HnSRTreeFile *file = (HnSRTreeFile *)srtree;
	int dimension = file->getDimension();
	HnPoint point;
	HnData value;
	int i;

	point = new_HnPoint(dimension);
	for(i=0; i<dimension; i++)
		point.setCoord(coords[i], i);

	value = new_HnData(data, size);

	file->remove(point, value);
}

void
HnSRTreeGetFirst(HnSRTree *srtree, const HnSRTreeRange ranges[],
		 HnSRTreeRecord **record_return)
{
	HnSRTreeFile *file = (HnSRTreeFile *)srtree;
	int dimension = file->getDimension();
	int dataSize = file->getDataSize();
	HnPoint point;
	HnData data;
	static HnSRTreeRecord *records = NULL;

	if(ranges == NULL)
		file->getFirst(&point, &data);
	else {
		HnRect rect = new_HnRect(dimension);
		int i;

		for(i=0; i<dimension; i++) {
			rect.setRange(ranges[i].min, HnRange::INCLUSIVE,
				      ranges[i].max, HnRange::INCLUSIVE,
				      i);
		}

		file->getFirst(rect, &point, &data);
	}

	if(point == HnPoint::null)
		*record_return = NULL;
	else {
		if(records != NULL)
			freeRecords(records, 1);
		records = allocateRecords(dimension, dataSize, 1);
		setRecord(&records[0], point, data);
		*record_return = &records[0];
	}
}

void
HnSRTreeGetNext(HnSRTree *srtree, HnSRTreeRecord **record_return)
{
	HnSRTreeFile *file = (HnSRTreeFile *)srtree;
	int dimension = file->getDimension();
	int dataSize = file->getDataSize();
	HnPoint point;
	HnData data;
	static HnSRTreeRecord *records = NULL;

	file->getNext(&point, &data);

	if(point == HnPoint::null)
		*record_return = NULL;
	else {
		if(records != NULL)
			freeRecords(records, 1);
		records = allocateRecords(dimension, dataSize, 1);
		setRecord(&records[0], point, data);
		*record_return = &records[0];
	}
}

void
HnSRTreeGetNeighbors(HnSRTree *srtree, const double coords[], int maxCount,
		     HnSRTreeRecord **records_return, int *numRecords_return)
{
	HnSRTreeFile *file = (HnSRTreeFile *)srtree;
	int dimension = file->getDimension();
	int dataSize = file->getDataSize();
	HnPoint target;
	HnPointArray points;
	HnDataArray values;
	int i;
	static HnSRTreeRecord *records = NULL;
	static int numRecords = -1;

	target = new_HnPoint(dimension);
	for(i=0; i<dimension; i++)
		target.setCoord(coords[i], i);

	file->getNeighbors(target, maxCount, &points, &values);

	if(records != NULL)
		freeRecords(records, numRecords);

	numRecords = points.length();
	records = allocateRecords(dimension, dataSize, numRecords);

	for(i=0; i<numRecords; i++)
		setRecord(&records[i], points[i], values[i]);

	*records_return = records;
	*numRecords_return = numRecords;
}

int
HnSRTreeGetDimension(HnSRTree *srtree)
{
	HnSRTreeFile *file = (HnSRTreeFile *)srtree;

	return file->getDimension();
}

void
HnSRTreeCheck(HnSRTree *srtree)
{
	HnSRTreeFile *file = (HnSRTreeFile *)srtree;

	file->check();
}

void
HnSRTreePrint(HnSRTree *srtree)
{
	HnSRTreeFile *file = (HnSRTreeFile *)srtree;

	file->print();
}

void
HnSRTreeSetDebug(HnSRTree *srtree, int debug)
{
	HnSRTreeFile *file = (HnSRTreeFile *)srtree;

	file->setDebug(debug);
}

static HnSRTreeRecord *
allocateRecords(int dimension, int dataSize, int count)
{
	HnSRTreeRecord *records;
	int i;

	if((records = (HnSRTreeRecord *)
	    malloc(sizeof(HnSRTreeRecord) * count)) == NULL)
		HnSysError("malloc");

	for(i=0; i<count; i++) {
		char *ptr;

		if((ptr = (char *)
		    malloc(sizeof(double) * dimension + dataSize)) == NULL)
			HnSysError("malloc");

		records[i].coords = (double *)ptr;
		records[i].data = ptr + sizeof(double) * dimension;
	}

	return records;
}

static void
freeRecords(HnSRTreeRecord *records, int count)
{
	int i;

	for(i=0; i<count; i++)
		free(records[i].coords);
	free(records);
}

static void
setRecord(HnSRTreeRecord *record, const HnPoint &point, const HnData &data)
{
	int i;

	for(i=0; i<point.getDimension(); i++)
		record->coords[i] = point.getCoord(i);

	memcpy(record->data, data.chars(), data.length());
	record->size = data.length();
}

