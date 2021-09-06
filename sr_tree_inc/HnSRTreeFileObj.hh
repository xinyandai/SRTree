/*
 * HnSRTreeFileObj.hh
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
 * 03/20/96 katayama@rd.nacsis.ac.jp
 * $Id: HnSRTreeFileObj.hh,v 1.3 1997/06/02 09:35:39 katayama Exp $
 */

#ifndef _HnSRTreeFileObj_hh
#define _HnSRTreeFileObj_hh

#include <sys/types.h>
#include "HnObject.hh"
#include "HnSRTreeBlock.hh"
#include "HnSRTreeNode.hh"
#include "HnSRTreeLeaf.hh"
#include "HnSRTreeStack.hh"
#include "HnSRTreeReinsert.hh"
#include "HnSRTreeOffset.hh"
#include "HnSRTreeNeighbor.hh"

/*{
#include "HnPoint.hh"
#include "HnData.hh"
#include "HnRect.hh"
}*/

class HnSRTreeFileObj: public HnObject {
private:
	/* super block */
	struct SuperBlock {
		int dimension;
		int dataSize;
		int blockSize;
		int fileSize;

		off_t freeOffset;
		off_t rootOffset;
		int height;

		int splitFactor;
		int reinsertFactor;
	};

	/* split flag */
	enum SplitFlag { LEFT, RIGHT, SPLIT_NONE };

	/* reinsert flag */
	enum ReinsertFlag { REINSERT, STAY, REINSERT_NONE };

private:
	FILE *fp;

	int dimension;
	int dataSize;
	int blockSize;
	int fileSize;

	off_t freeOffset;
	off_t rootOffset;
	int height;

	int splitFactor;
	int reinsertFactor;

	struct {
		HnSRTreeStack stack;
		HnRect rect;
	} context;

	HnSRTreeReinsertArray reinsertList;
	HnSRTreeOffsetArray reinsertedBlocks;

	HnBool debug;

	/* initialize */
	void initialize(void) {
		debug = HnFALSE;
	}

	/* super block */
	void writeSuperBlock(void);
	void readSuperBlock(void);

	/* block */
	HnSRTreeBlock readBlock(off_t offset);
	void writeBlock(const HnSRTreeBlock &block);
	off_t allocateBlock(void);
	void releaseBlock(off_t offset);

	/* leaf */
	void writeLeaf(const HnSRTreeLeaf &leaf);
	HnSRTreeLeaf allocateLeaf(void);

	/* node */
	void writeNode(const HnSRTreeNode &node);
	HnSRTreeNode allocateNode(void);

	/* store */
	void insertPoint(const HnPoint &point, const HnData &data);
	void insertBlock(off_t offset, int level);
	HnSRTreeStack chooseLeaf(const HnPoint &point);
	int chooseSubtree(const HnSRTreeNode &node, const HnPoint &point);
	HnSRTreeStack chooseNode(const HnSRTreeCore &core, int level);
	void updateCoreAndRect(HnSRTreeStack stack);

	void reinsertLeaf(HnSRTreeStack &stack,
			  const HnPoint &newPoint, const HnData &newData);
	void selectPoints(const HnPointArray &points,
			  ReinsertFlag **flags_return);
	void reinsertNode(HnSRTreeStack &stack,
			  const HnSRTreeCore &newCore,
			  const HnRect &newRect, off_t newOffset);
	void selectCores(const HnSRTreeCoreArray &cores,
			 ReinsertFlag **flags_return);

	void splitLeaf(HnSRTreeStack &stack,
		       const HnPoint &newPoint, const HnData &newData);
	void splitNode(HnSRTreeStack &stack,
		       const HnSRTreeCore &newCore,
		       const HnRect &newRect, off_t newOffset);

	void updateNode(HnSRTreeStack &stack,
			const HnSRTreeCore &leftCore,
			const HnRect &leftRect, off_t leftOffset,
			const HnSRTreeCore &rightCore,
			const HnRect &rightRect, off_t rightOffset);
	void extendTree(const HnSRTreeCore &leftCore,
			const HnRect &leftRect, off_t leftOffset,
			const HnSRTreeCore &rightCore,
			const HnRect &rightRect, off_t rightOffset);

	/* split */
	void splitPoints(const HnPointArray &points, SplitFlag **flags_return);
	void splitCores(const HnSRTreeCoreArray &cores,
			SplitFlag **flags_return);

	/* construction */
	void getSplitFactors(const HnPointArray &points,
			     const HnDataArray &values,
			     int *numLevels_return, int *numSplits_return,
			     int *numRootSplits_return,
			     int *numNodeSplits_return);
	void constructTree(HnPointArray &points, HnDataArray &values,
			   int offset, int count,
			   int levelCount, int numLevels,
			   int splitCount, int numSplits,
			   int numRootSplits, int numNodeSplits,
			   HnSRTreeNode &node);
	int getConstructionAxis(HnPointArray &points, int offset, int count);
	void sortPoints(HnPointArray &points, HnDataArray &values,
			int offset, int count, int axis);

	/* remove */
	HnSRTreeStack searchPoint(const HnPoint &point, const HnData &data);

	/* neighbor search */
	double getMinDistance(const HnPoint &point,
			      const HnSRTreeCore &core, const HnRect &rect);
	double getMaxDistance(const HnPoint &point,
			      const HnSRTreeCore &core, const HnRect &rect);
	HnSRTreeNeighborArray
		chooseNeighbors(off_t offset,
				const HnPoint &point, int maxCount,
				const HnSRTreeNeighborArray &neighbors);
	HnSRTreeNeighborArray
		chooseNeighborsInNode(const HnSRTreeBlock &block,
				      const HnPoint &point, int maxCount,
				      HnSRTreeNeighborArray neighbors);
	HnSRTreeNeighborArray
		chooseNeighborsInLeaf(const HnSRTreeBlock &block,
				      const HnPoint &point, int maxCount,
				      const HnSRTreeNeighborArray &neighbors);

	/* check */
	int checkBlock(const HnSRTreeCore &core,
		       const HnRect &rect, off_t offset);
	HnPointArray checkInclusion(off_t offset);
	void describeExclusion(off_t offset, const HnPoint &center);

	/* print */
	void printNode(const HnSRTreeNode &node);
	void printLeaf(const HnSRTreeLeaf &leaf);

public:
	HnSRTreeFileObj(const char *path,
			int dimension, int dataSize, int blockSize,
			int splitFactor, int reinsertFactor);
	HnSRTreeFileObj(const char *path,
			int dimension, int dataSize, int blockSize,
			int splitFactor, int reinsertFactor,
			HnPointArray &points, HnDataArray &values,
			HnBool debug = HnFALSE);
	HnSRTreeFileObj(const char *path, const char *mode);
	~HnSRTreeFileObj(void);

	int getDimension(void) const {
		return dimension;
	}
	int getBlockSize(void) const {
		return blockSize;
	}
	int getDataSize(void) const {
		return dataSize;
	}
	int getHeight(void) const {
		return height;
	}

	void store(const HnPoint &point, const HnData &data);
	void remove(const HnPoint &point, const HnData &data);
	void close(void);

	void getFirst(HnPoint *point_return, HnData *data_return);
	void getFirst(const HnRect &rect,
		      HnPoint *point_return, HnData *data_return);
	void getNext(HnPoint *point_return, HnData *data_return);

	void getNeighbors(const HnPoint &point, int maxCount,
			  HnPointArray *points_return,
			  HnDataArray *values_return);

	void check(void);
	void print(void);
	void setDebug(HnBool flag);
};

#endif /* _HnSRTreeFileObj_hh */
