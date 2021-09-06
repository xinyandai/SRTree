/*
 * HnSRTree.h
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
 * $Id: HnSRTree.h,v 1.4 1997/06/02 09:35:36 katayama Exp $
 */

#ifndef _HnSRTree_h
#define _HnSRTree_h

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	double min;
	double max;
} HnSRTreeRange;

typedef struct {
	double *coords;
	void *data;
	int size;
} HnSRTreeRecord;

/*
 * Functions
 */

typedef void HnSRTree;

HnSRTree *HnSRTreeCreate(const char *path,
			 int dimension, int dataSize, int blockSize,
			 int splitFactor, int reinsertFactor);
HnSRTree *HnSRTreeUpdate(const char *path);
HnSRTree *HnSRTreeOpen(const char *path);
void HnSRTreeClose(HnSRTree *srtree);

void HnSRTreeStore(HnSRTree *srtree,
		   const double coords[], const void *data, int size);
void HnSRTreeRemove(HnSRTree *srtree,
		    const double coords[], const void *data, int size);

void HnSRTreeGetFirst(HnSRTree *srtree, const HnSRTreeRange ranges[],
		      HnSRTreeRecord **record_return);
void HnSRTreeGetNext(HnSRTree *srtree,
		     HnSRTreeRecord **record_return);

void HnSRTreeGetNeighbors(HnSRTree *srtee, const double coords[], int maxCount,
			  HnSRTreeRecord **records_return,
			  int *numRecords_return);

int HnSRTreeGetDimension(HnSRTree *srtree);

void HnSRTreeCheck(HnSRTree *srtree);
void HnSRTreePrint(HnSRTree *srtree);
void HnSRTreeSetDebug(HnSRTree *srtree, int debug);

#ifdef __cplusplus
};
#endif

#endif /* _HnSRTree_h */
