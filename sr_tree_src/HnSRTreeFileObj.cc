/*
 * HnSRTreeFileObj.cc
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
 * $Id: HnSRTreeFileObj.cc,v 1.5 1997/12/04 03:32:30 katayama Exp $
 */

#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include "HnSRTreeFile.hh"
#include "HnSRTreeFileObj.hh"
#include "HnAbort.hh"

/*
 * Constructor and Destructor
 */

HnSRTreeFileObj::HnSRTreeFileObj(const char *path,
				 int dimension, int dataSize, int blockSize,
				 int splitFactor, int reinsertFactor)
{
	HnSRTreeLeaf leaf;

	if(splitFactor < 0 || splitFactor > 50)
		HnAbort("split factor %d is out of range.", splitFactor);

	if(reinsertFactor < 0 || reinsertFactor > 50)
		HnAbort("reinsert factor %d is out of range.", reinsertFactor);

	initialize();

	if((fp = fopen(path, "w+")) == NULL) {
		setSysError("fopen(%s, \"w+\")", path);
		return;
	}

	this->dimension = dimension;
	this->dataSize = dataSize;
	this->blockSize = blockSize;
	this->fileSize = blockSize;

	this->splitFactor = splitFactor;
	this->reinsertFactor = reinsertFactor;

	freeOffset = -1;
	rootOffset = 0;
	height = 0;

	leaf = allocateLeaf();
	rootOffset = leaf.getOffset();
	height ++;

	writeLeaf(leaf);
	writeSuperBlock();
}

HnSRTreeFileObj::HnSRTreeFileObj(const char *path,
				 int dimension, int dataSize, int blockSize,
				 int splitFactor, int reinsertFactor,
				 HnPointArray &points, HnDataArray &values,
				 HnBool debug)
{
	HnSRTreeLeaf leaf;
	int numLevels, numSplits, numRootSplits, numNodeSplits;

	if(splitFactor < 0 || splitFactor > 50)
		HnAbort("split factor %d is out of range.", splitFactor);

	if(reinsertFactor < 0 || reinsertFactor > 50)
		HnAbort("reinsert factor %d is out of range.", reinsertFactor);

	initialize();

	if((fp = fopen(path, "w+")) == NULL) {
		setSysError("fopen(%s, \"w+\")", path);
		return;
	}

	this->dimension = dimension;
	this->dataSize = dataSize;
	this->blockSize = blockSize;
	this->fileSize = blockSize;

	this->splitFactor = splitFactor;
	this->reinsertFactor = reinsertFactor;

	freeOffset = -1;
	rootOffset = 0;
	height = 0;

	this->debug = debug;

	getSplitFactors(points, values, &numLevels, 
			&numSplits, &numRootSplits, &numNodeSplits);

	if(numLevels == 1) {
		HnSRTreeLeaf leaf = allocateLeaf();
		int i;

		for(i=0; i<points.length(); i++)
			leaf.append(points[i], values[i]);

		rootOffset = leaf.getOffset();
		height = numLevels;

		writeLeaf(leaf);
		writeSuperBlock();
	}
	else {
		HnSRTreeNode node = allocateNode();

		constructTree(points, values, 0, points.length(),
			      0, numLevels, 0, numSplits,
			      numRootSplits, numNodeSplits,
			      node);

		rootOffset = node.getOffset();
		height = numLevels;

		writeNode(node);
		writeSuperBlock();
	}
}

HnSRTreeFileObj::HnSRTreeFileObj(const char *path, const char *mode)
{
	initialize();

	if(strcmp(mode, "rw") == 0) {
		if((fp = fopen(path, "r+")) == NULL) {
			setSysError("fopen(\"%s\", \"r+\")", path);
			return;
		}
	}
	else if(strcmp(mode, "r") == 0) {
		if((fp = fopen(path, "r")) == NULL) {
			setSysError("fopen(\"%s\", \"r\")", path);
			return;
		}
	}
	else
		HnAbort("mode must be `r' or `rw'");

	readSuperBlock();
}

HnSRTreeFileObj::~HnSRTreeFileObj(void)
{
	if(fp != NULL)
		fclose(fp);
}

void
HnSRTreeFileObj::close(void)
{
	if(fp != NULL)
		fclose(fp);
	fp = NULL;
}

/*
 * Super Block
 */

void
HnSRTreeFileObj::writeSuperBlock(void)
{
	SuperBlock superBlock;

	superBlock.dimension = dimension;
	superBlock.dataSize = dataSize;
	superBlock.blockSize = blockSize;
	superBlock.fileSize = fileSize;

	superBlock.freeOffset = freeOffset;
	superBlock.rootOffset = rootOffset;
	superBlock.height = height;

	superBlock.splitFactor = splitFactor;
	superBlock.reinsertFactor = reinsertFactor;

	if(fseek(fp, 0, SEEK_SET) < 0)
		HnSysError("fseek");
	if(fwrite(&superBlock, sizeof(superBlock), 1, fp) != 1)
		HnSysError("fwrite");
}

void
HnSRTreeFileObj::readSuperBlock(void)
{
	SuperBlock superBlock;

	if(fseek(fp, 0, SEEK_SET) < 0)
		HnSysError("fseek");
	if(fread(&superBlock, sizeof(superBlock), 1, fp) != 1)
		HnSysError("fread");

	dimension = superBlock.dimension;
	dataSize = superBlock.dataSize;
	blockSize = superBlock.blockSize;
	fileSize = superBlock.fileSize;

	freeOffset = superBlock.freeOffset;
	rootOffset = superBlock.rootOffset;
	height = superBlock.height;

	splitFactor = superBlock.splitFactor;
	reinsertFactor = superBlock.reinsertFactor;
}

/*
 * Block
 */

void
HnSRTreeFileObj::writeBlock(const HnSRTreeBlock &block)
{
	if(fseek(fp, block.getOffset(), SEEK_SET) < 0)
		HnSysError("fseek");
	if(fwrite(block.getBytes(), block.getSize(), 1, fp) != 1)
		HnSysError("fwrite");
}

HnSRTreeBlock
HnSRTreeFileObj::readBlock(off_t offset)
{
	char *bytes = new char[blockSize];
	HnSRTreeBlock block;

	if(fseek(fp, offset, SEEK_SET) < 0)
		HnSysError("fseek");
	if(fread(bytes, blockSize, 1, fp) != 1)
		HnSysError("fread");

	block = new_HnSRTreeBlock(offset, bytes, blockSize);
	delete bytes;

	return block;
}

off_t
HnSRTreeFileObj::allocateBlock(void)
{
	off_t offset;

	if(freeOffset < 0) {
		offset = fileSize;
		fileSize += blockSize;

		writeSuperBlock();
	}
	else {
		HnSRTreeBlock block;
		const char *bp;
		HnSRTreeBlock::Type type;

		offset = freeOffset;

		block = readBlock(freeOffset);
		bp = block.getBytes();

		memcpy(&type, bp, sizeof(type));
		bp += sizeof(type);

		if(type != HnSRTreeBlock::FREE)
			HnAbort("busy block is included in the free block "
				"chain.");

                memcpy(&freeOffset, bp, sizeof(off_t));

                writeSuperBlock();
        }

        return offset;
}

void
HnSRTreeFileObj::releaseBlock(off_t offset)
{
	HnSRTreeBlock block;
	char *buffer = new char[blockSize];
	char *bp;
	HnSRTreeBlock::Type type;

        memset(buffer, 0, blockSize);

	bp = buffer;

	type = HnSRTreeBlock::FREE;
	memcpy(bp, &type, sizeof(type));
	bp += sizeof(type);

	memcpy(bp, &freeOffset, sizeof(off_t));

        block = new_HnSRTreeBlock(offset, buffer, blockSize);

	delete buffer;

        writeBlock(block);

        freeOffset = offset;
        writeSuperBlock();
}

/*
 * Leaf
 */

void
HnSRTreeFileObj::writeLeaf(const HnSRTreeLeaf &leaf)
{
	HnSRTreeBlock block;

	if(debug) {
		fprintf(stderr, "HnSRTreeFileObj::writeLeaf: %s\n",
			(char *)leaf.toString());
	}

	block = leaf.toBlock();
	writeBlock(block);
}

HnSRTreeLeaf
HnSRTreeFileObj::allocateLeaf(void)
{
	off_t offset;

	offset = allocateBlock();
	return new_HnSRTreeLeaf(dimension, dataSize, blockSize, offset);
}

/*
 * Node
 */

void
HnSRTreeFileObj::writeNode(const HnSRTreeNode &node)
{
	HnSRTreeBlock block;

	block = node.toBlock();
	writeBlock(block);
}

HnSRTreeNode
HnSRTreeFileObj::allocateNode(void)
{
	off_t offset;

	offset = allocateBlock();
	return new_HnSRTreeNode(dimension, blockSize, offset);
}

/*
 * Store
 */

void
HnSRTreeFileObj::store(const HnPoint &point, const HnData &data)
{
	if(point.getDimension() != getDimension())
		HnAbort("mismatch in the dimensions (point: %d, file: %d)",
			point.getDimension(), getDimension());

	if(data.length() > dataSize)
		HnAbort("The size of data (%d) exceeds the limit (%d).",
			data.length(), dataSize);

	reinsertList = new_HnSRTreeReinsertArray();
	reinsertedBlocks = new_HnSRTreeOffsetArray();

	reinsertList.append(new_HnSRTreeReinsert(point, data));

	while(reinsertList.length() != 0) {
		if(reinsertList[0].getType() == HnSRTreeReinsert::POINT) {
			HnPoint point = reinsertList[0].getPoint();
			HnData data = reinsertList[0].getData();

			reinsertList.remove(0);
			insertPoint(point, data);
		}
		else {
			off_t offset = reinsertList[0].getOffset();
			int level = reinsertList[0].getLevel();

			reinsertList.remove(0);
			insertBlock(offset, level);
		}
	}

	if(debug)
		check();
}

void
HnSRTreeFileObj::insertPoint(const HnPoint &point, const HnData &data)
{
	HnSRTreeStack stack;
	HnSRTreeLeaf leaf;

	stack = chooseLeaf(point);

	leaf = stack.getTopLeaf();

	if(!leaf.isFull()) {
		leaf.append(point, data);
		writeLeaf(leaf);
		updateCoreAndRect(stack);
	}
	else {
		int index = -1;

		if(stack.getDepth() != 1 &&
		   (index = reinsertedBlocks.indexOf(leaf.getOffset())) < 0) {
			reinsertedBlocks.append(leaf.getOffset());
			reinsertLeaf(stack, point, data);
		}
		else {
			if(index >= 0)
				reinsertedBlocks.remove(index);
			splitLeaf(stack, point, data);
		}
	}
}

void
HnSRTreeFileObj::insertBlock(off_t offset, int level)
{
	HnSRTreeBlock block = readBlock(offset);
	HnSRTreeCore core;
	HnRect rect;
	HnSRTreeStack stack;
	HnSRTreeNode node;

	if(block.isNode()) {
		HnSRTreeNode node = new_HnSRTreeNode(dimension, block);
		core = node.getCore();
		rect = node.getBoundingRect();
	}
	else if(block.isLeaf()) {
		HnSRTreeLeaf leaf =
			new_HnSRTreeLeaf(dimension, dataSize, block);
		core = leaf.getCore();
		rect = leaf.getBoundingRect();
	}
	else
		HnAbort("unexpected block type.");

	stack = chooseNode(core, level);

	node = stack.getTopNode();

	if(!node.isFull()) {
		node.append(core, rect, offset);
		writeNode(node);
		updateCoreAndRect(stack);
	}
	else {
		int index = -1;

		if(stack.getDepth() != 1 &&
		   (index = reinsertedBlocks.indexOf(node.getOffset())) < 0) {
			reinsertedBlocks.append(node.getOffset());
			reinsertNode(stack, core, rect, offset);
		}
		else {
			if(index >= 0)
				reinsertedBlocks.remove(index);
			splitNode(stack, core, rect, offset);
		}
	}
}

HnSRTreeStack
HnSRTreeFileObj::chooseLeaf(const HnPoint &point)
{
	HnSRTreeStack stack = new_HnSRTreeStack();
	HnSRTreeBlock block;
	HnSRTreeNode node;
	HnSRTreeLeaf leaf;
	int index;

	/*
	 * NOTE:
	 *	The stack is used for keeping the trace of the access path.
	 *	Cursors indicates which entry is used at each node.
	 */

	block = readBlock(rootOffset);

	while(block.isNode()) {
		node = new_HnSRTreeNode(dimension, block);
		index = chooseSubtree(node, point);

		stack.push(node, index);
		block = readBlock(node.getOffsetAt(index));
	}

	leaf = new_HnSRTreeLeaf(dimension, dataSize, block);
	stack.push(leaf, -1);

	return stack;
}

int
HnSRTreeFileObj::chooseSubtree(const HnSRTreeNode &node, const HnPoint &point)
{
	struct {
		int index;
		double distance;
	} min, cur;

	min.index = -1;
	min.distance = -1;

	for(cur.index=0; cur.index<node.getCount(); cur.index++) {
		HnSRTreeCore core = node.getCoreAt(cur.index);

		cur.distance = point.getDistance(core.getCenter());

		if(min.index == -1)
			min = cur;
		else {
			if(cur.distance < min.distance)
				min = cur;
		}
	}

	return min.index;
}

HnSRTreeStack
HnSRTreeFileObj::chooseNode(const HnSRTreeCore &core, int level)
{
	HnSRTreeStack stack = new_HnSRTreeStack();
	HnSRTreeBlock block;
	HnSRTreeNode node;
	int index;

	/*
	 * NOTE:
	 *	The stack is used for keeping the trace of the access path.
	 *	Cursors indicates which entry is used at each node.
	 */

	block = readBlock(rootOffset);

	while(stack.getDepth() != height - level) {
		node = new_HnSRTreeNode(dimension, block);
		index = chooseSubtree(node, core.getCenter());

		stack.push(node, index);
		block = readBlock(node.getOffsetAt(index));
	}

	return stack;
}

void
HnSRTreeFileObj::updateCoreAndRect(HnSRTreeStack stack)
{
	HnSRTreeCore core;
	HnRect rect;
	HnSRTreeLeaf leaf;
	HnSRTreeNode node;
	int cursor;

	if(stack.isTopNode()) {
		node = stack.getTopNode();
		core = node.getCore();
		rect = node.getBoundingRect();
		stack.pop();
	}
	else {
		leaf = stack.getTopLeaf();
		core = leaf.getCore();
		rect = leaf.getBoundingRect();
		stack.pop();
	}

	while(stack.getDepth() != 0) {
		node = stack.getTopNode();
		cursor = stack.getCursor();

		node.setCoreAt(core, cursor);
		node.setBoundingRectAt(rect, cursor);

		core = node.getCore();
		rect = node.getBoundingRect();

		writeNode(node);
		stack.pop();
	}
}

void
HnSRTreeFileObj::reinsertLeaf(HnSRTreeStack &stack,
			      const HnPoint &newPoint, const HnData &newData)
{
	HnSRTreeLeaf leaf, newLeaf;
	HnPointArray points;
	int i;
	ReinsertFlag *flags;

	leaf = stack.getTopLeaf();

	points = new_HnPointArray();
	for(i=0; i<leaf.getCount(); i++)
		points.append(leaf.getPointAt(i));
	points.append(newPoint);

	selectPoints(points, &flags);

	newLeaf = new_HnSRTreeLeaf(dimension, dataSize, 
				   blockSize, leaf.getOffset());

	for(i=0; i<leaf.getCount(); i++) {
		HnPoint point = leaf.getPointAt(i);
		HnData data = leaf.getDataAt(i);

		switch(flags[i]) {
		case STAY:
			newLeaf.append(point, data);
			break;
		case REINSERT:
			reinsertList.append(new_HnSRTreeReinsert(point, data));
			break;
		default:
			HnAbort("reinsert flag is incorrectly assigned.");
		}
	}
	switch(flags[i]) {
	case STAY:
		newLeaf.append(newPoint, newData);
		break;
	case REINSERT:
		reinsertList.append(new_HnSRTreeReinsert(newPoint, newData));
		break;
	default:
		HnAbort("reinsert flag is incorrectly assigned.");
	}

	writeLeaf(newLeaf);

	/* replace leaf with newLeaf in the stack */
	stack.pop();
	stack.push(newLeaf);

	updateCoreAndRect(stack);
}

void
HnSRTreeFileObj::selectPoints(const HnPointArray &points,
			      ReinsertFlag **flags_return)
{
	static ReinsertFlag *flags = NULL;
	int numPoints = points.length();
	int reinsertCount = numPoints * reinsertFactor / 100;
	HnPoint center;
	int i, j, axis;
	double *distances = new double[numPoints];
	int *array = new int[numPoints];

	/* initialize flags */
	if(flags != NULL)
		delete flags;
	flags = new ReinsertFlag[numPoints];

	for(i=0; i<numPoints; i++)
		flags[i] = REINSERT_NONE;

	/* calculate center */
	center = new_HnPoint(dimension);

	for(axis=0; axis<dimension; axis++) {
		double sum = 0;

		for(i=0; i<numPoints; i++)
			sum += points[i].getCoord(axis);

		center.setCoord(sum / numPoints, axis);
	}

	/* initialize array */
	for(i=0; i<numPoints; i++) {
		distances[i] = points[i].getDistance(center);
		array[i] = i;
	}

	/* sort points by distance in descent order */
	for(i=0; i<numPoints; i++) {
		for(j=i+1; j<numPoints; j++) {
			if(distances[array[i]] < distances[array[j]]) {
				int temp = array[i];
				array[i] = array[j];
				array[j] = temp;
			}
		}
	}

	/* make reinsert flags */
	i=0;
	while(i<reinsertCount) {
		flags[array[i]] = REINSERT;
		i++;
	}
	while(i<numPoints) {
		flags[array[i]] = STAY;
		i++;
	}

	if(debug) {
		fprintf(stderr, "HnSRTreeFileObj::selectPoints:\n");
		fprintf(stderr, "    REINSERT\n");
		for(i=0; i<numPoints; i++) {
			if(flags[i] == REINSERT)
				fprintf(stderr,
					"    points[%d].getDistance(center) "
					"= %g\n",
					i, points[i].getDistance(center));
		}
		fprintf(stderr, "    STAY\n");
		for(i=0; i<numPoints; i++) {
			if(flags[i] == STAY)
				fprintf(stderr,
					"    points[%d].getDistance(center) "
					"= %g\n",
					i, points[i].getDistance(center));
		}
	}

	*flags_return = flags;

	delete distances;
	delete array;
}

void
HnSRTreeFileObj::reinsertNode(HnSRTreeStack &stack,
			      const HnSRTreeCore &newCore,
			      const HnRect &newRect, off_t newOffset)
{
	HnSRTreeNode node, newNode;
	HnSRTreeCoreArray cores;
	int i, level;
	ReinsertFlag *flags;

	node = stack.getTopNode();
	level = height - stack.getDepth();

	cores = new_HnSRTreeCoreArray();
	for(i=0; i<node.getCount(); i++)
		cores.append(node.getCoreAt(i));
	cores.append(newCore);

	selectCores(cores, &flags);

	newNode = new_HnSRTreeNode(dimension, blockSize, node.getOffset());

	for(i=0; i<node.getCount(); i++) {
		HnSRTreeCore core = node.getCoreAt(i);
		HnRect rect = node.getRectAt(i);
		off_t offset = node.getOffsetAt(i);

		switch(flags[i]) {
		case STAY:
			newNode.append(core, rect, offset);
			break;
		case REINSERT:
			reinsertList.append(new_HnSRTreeReinsert(offset,
								 level));
			break;
		default:
			HnAbort("reinsert flag is incorrectly assigned.");
		}
	}
	switch(flags[i]) {
	case STAY:
		newNode.append(newCore, newRect, newOffset);
		break;
	case REINSERT:
		reinsertList.append(new_HnSRTreeReinsert(newOffset, level));
		break;
	default:
		HnAbort("reinsert flag is incorrectly assigned.");
	}

	writeNode(newNode);

	/* replace node with newNode in the stack */
	stack.pop();
	stack.push(newNode);

	updateCoreAndRect(stack);
}

void
HnSRTreeFileObj::selectCores(const HnSRTreeCoreArray &cores,
			     ReinsertFlag **flags_return)
{
	static ReinsertFlag *flags = NULL;
	int numCores = cores.length();
	int reinsertCount = numCores * reinsertFactor / 100;
	HnPoint center;
	int i, j, axis;
	double *distances = new double[numCores];
	int *array = new int[numCores];

	/* initialize flags */
	if(flags != NULL)
		delete flags;
	flags = new ReinsertFlag[numCores];

	for(i=0; i<numCores; i++)
		flags[i] = REINSERT_NONE;

	/* calculate center */
	center = new_HnPoint(dimension);

	for(axis=0; axis<dimension; axis++) {
		double sum = 0;
		int total = 0;

		for(i=0; i<numCores; i++) {
			double coord = cores[i].getCenter().getCoord(axis);
			int weight = cores[i].getWeight();

			sum += coord * weight;
			total += weight;
		}

		center.setCoord(sum / total, axis);
	}

	/* initialize array */
	for(i=0; i<numCores; i++) {
		distances[i] = cores[i].getCenter().getDistance(center);
		array[i] = i;
	}

	/* sort cores by distance in descent order */
	for(i=0; i<numCores; i++) {
		for(j=i+1; j<numCores; j++) {
			if(distances[array[i]] < distances[array[j]]) {
				int temp = array[i];
				array[i] = array[j];
				array[j] = temp;
			}
		}
	}

	/* make reinsert flags */
	i=0;
	while(i<reinsertCount) {
		flags[array[i]] = REINSERT;
		i++;
	}
	while(i<numCores) {
		flags[array[i]] = STAY;
		i++;
	}

	if(debug) {
		fprintf(stderr, "HnSRTreeFileObj::selectCores:\n");
		fprintf(stderr, "    REINSERT\n");
		for(i=0; i<numCores; i++) {
			if(flags[i] == REINSERT)
				fprintf(stderr,
					"    cores[%d].getCenter()."
					"getDistance(center) = %g\n",
					i, cores[i].getCenter().
					getDistance(center));
		}
		fprintf(stderr, "    STAY\n");
		for(i=0; i<numCores; i++) {
			if(flags[i] == STAY)
				fprintf(stderr,
					"    cores[%d].getCenter()."
					"getDistance(center) = %g\n",
					i, cores[i].getCenter().
					getDistance(center));
		}
	}

	*flags_return = flags;

	delete distances;
	delete array;
}

void
HnSRTreeFileObj::splitLeaf(HnSRTreeStack &stack,
			   const HnPoint &newPoint, const HnData &newData)
{
	HnSRTreeLeaf leaf, left, right;
	int i;
	HnPointArray points;
	SplitFlag *flags;

	leaf = stack.getTopLeaf();

	if(debug) {
		fprintf(stderr, "HnSRTreeFileObj::splitLeaf: leaf = %s\n",
			(char *)leaf.toString());
	}

	/* put points into an array */
	points = new_HnPointArray();
	for(i=0; i<leaf.getCount(); i++)
		points.append(leaf.getPointAt(i));
	points.append(newPoint);

	/* split points */
	splitPoints(points, &flags);

	/* put points into the left and right leaves */
	left = new_HnSRTreeLeaf(dimension, dataSize,
				blockSize, leaf.getOffset());
	right = allocateLeaf();

	for(i=0; i<leaf.getCount(); i++) {
		switch(flags[i]) {
		case LEFT:
			left.append(leaf.getPointAt(i),	leaf.getDataAt(i));
			break;
		case RIGHT:
			right.append(leaf.getPointAt(i), leaf.getDataAt(i));
			break;
		default:
			HnAbort("split flag is incorrectly assigned.");
		}
	}
	switch(flags[i]) {
	case LEFT:
		left.append(newPoint, newData);
		break;
	case RIGHT:
		right.append(newPoint, newData);
		break;
	default:
		HnAbort("split flag is incorrectly assigned.");
	}

	if(debug) {
		fprintf(stderr, "HnSRTreeFileObj::splitLeaf: left  = %s\n",
			(char *)left.toString());
		fprintf(stderr, "HnSRTreeFileObj::splitLeaf: right = %s\n",
			(char *)right.toString());
	}

	writeLeaf(left);
	writeLeaf(right);

	stack.pop();

	updateNode(stack,
		   left.getCore(),
		   left.getBoundingRect(), left.getOffset(),
		   right.getCore(), 
		   right.getBoundingRect(), right.getOffset());
}

void
HnSRTreeFileObj::splitNode(HnSRTreeStack &stack,
			   const HnSRTreeCore &newCore, 
			   const HnRect &newRect, off_t newOffset)
{
	HnSRTreeNode node, left, right;
	int i;
	HnSRTreeCoreArray cores;
	SplitFlag *flags;

	node = stack.getTopNode();

	if(debug) {
		fprintf(stderr, "HnSRTreeFileObj::splitNode: node = %s\n",
			(char *)node.toString());
	}

	/* put cores into an array */
	cores = new_HnSRTreeCoreArray();
	for(i=0; i<node.getCount(); i++)
		cores.append(node.getCoreAt(i));
	cores.append(newCore);

	/* split cores */
	splitCores(cores, &flags);

	/* put cores and rects into the left and right nodes */
	left = new_HnSRTreeNode(dimension, blockSize, node.getOffset());
	right = allocateNode();

	for(i=0; i<node.getCount(); i++) {
		switch(flags[i]) {
		case LEFT:
			left.append(node.getCoreAt(i),
				    node.getRectAt(i), node.getOffsetAt(i));
			break;
		case RIGHT:
			right.append(node.getCoreAt(i),
				     node.getRectAt(i), node.getOffsetAt(i));
			break;
		default:
			HnAbort("split flag is incorrectly assigned.");
		}
	}
	switch(flags[i]) {
	case LEFT:
		left.append(newCore, newRect, newOffset);
		break;
	case RIGHT:
		right.append(newCore, newRect, newOffset);
		break;
	default:
		HnAbort("split flag is incorrectly assigned.");
	}

	if(debug) {
		fprintf(stderr, "HnSRTreeFileObj::splitNode: left  = %s\n",
			(char *)left.toString());
		fprintf(stderr, "HnSRTreeFileObj::splitNode: right = %s\n",
			(char *)right.toString());
	}

	writeNode(left);
	writeNode(right);

	stack.pop();

	updateNode(stack,
		   left.getCore(),
		   left.getBoundingRect(), left.getOffset(),
		   right.getCore(),
		   right.getBoundingRect(), right.getOffset());
}

void
HnSRTreeFileObj::updateNode(HnSRTreeStack &stack,
			    const HnSRTreeCore &leftCore,
			    const HnRect &leftRect, off_t leftOffset,
			    const HnSRTreeCore &rightCore,
			    const HnRect &rightRect, off_t rightOffset)
{
	HnSRTreeNode node;
	int cursor;

	if(stack.getDepth() == 0)
		extendTree(leftCore, leftRect, leftOffset,
			   rightCore, rightRect, rightOffset);
	else {
		node = stack.getTopNode();
		cursor = stack.getCursor();

		node.setElementAt(leftCore, leftRect, leftOffset, cursor);

		if(node.isFull()) {
			int index = -1;

			if(stack.getDepth() != 1 &&
			   (index =
                            reinsertedBlocks.indexOf(node.getOffset())) < 0) {
				reinsertedBlocks.append(node.getOffset());
				reinsertNode(stack,
					     rightCore,
					     rightRect, rightOffset);
			}
			else {
				if(index >= 0)
					reinsertedBlocks.remove(index);
				splitNode(stack,
					  rightCore, rightRect, rightOffset);
			}
		}
		else {
			node.append(rightCore, rightRect, rightOffset);

			writeNode(node);
			updateCoreAndRect(stack);
		}
	}
}

void
HnSRTreeFileObj::extendTree(const HnSRTreeCore &leftCore,
			    const HnRect &leftRect, off_t leftOffset,
			    const HnSRTreeCore &rightCore,
			    const HnRect &rightRect, off_t rightOffset)
{
	HnSRTreeNode node;

	node = allocateNode();
	node.append(leftCore, leftRect, leftOffset);
	node.append(rightCore, rightRect, rightOffset);
	writeNode(node);

	rootOffset = node.getOffset();
	height ++;
	writeSuperBlock();
}

/*
 * Split
 */

void
HnSRTreeFileObj::splitPoints(const HnPointArray &points,
			     SplitFlag **flags_return)
{
	static SplitFlag *flags = NULL;
	int numPoints = points.length();
	int minCount = numPoints * splitFactor / 100;
	int maxCount = numPoints - minCount;
	struct {
		int axis;
		double var;
	} max;
	struct {
		int count;
		double leftVar, rightVar;
	} min;
	int axis, count;
	int i, j;
	int *array = new int[numPoints];

	/* initialize flags */
	if(flags != NULL)
		delete flags;
	flags = new SplitFlag[numPoints];

	for(i=0; i<numPoints; i++)
		flags[i] = SPLIT_NONE;

	max.axis = -1;
	max.var = -1;

	/* calculate variance */
	for(axis=0; axis<dimension; axis++) {
		double avg, var;
		double sum = 0;
		double sum2 = 0;

		for(i=0; i<numPoints; i++) {
			double coord = points[i].getCoord(axis);
			sum += coord;
			sum2 += coord * coord;
		}

		avg = sum / numPoints;
		var = sum2 / numPoints - avg * avg;

		if(max.axis == -1) {
			max.axis = axis;
			max.var = var;
		}
		else {
			if(var > max.var) {
				max.axis = axis;
				max.var = var;
			}
		}
	}

	/* sort points */
	for(i=0; i<numPoints; i++)
		array[i] = i;

	for(i=0; i<numPoints; i++) {
		for(j=i+1; j<numPoints; j++) {
			double coord1 = points[array[i]].getCoord(max.axis);
			double coord2 = points[array[j]].getCoord(max.axis);

			if(coord1 > coord2) {
				int temp = array[i];
				array[i] = array[j];
				array[j] = temp;
			}
		}
	}

	/* choose split point */
	min.count = -1;
	min.leftVar = 0;
	min.rightVar = 0;

	for(count=minCount; count<=maxCount; count++) {
		double leftVar, rightVar;

		/* calculate the sum of variances on the left side */
		leftVar = 0;

		for(axis=0; axis<dimension; axis++) {
			double sum, sum2, avg, var;

			sum = 0;
			sum2 = 0;

			for(i=0; i<count; i++) {
				double coord = points[array[i]].getCoord(axis);
				sum += coord;
				sum2 += coord * coord;
			}

			avg = sum / count;
			var = sum2 / count - avg * avg;

			leftVar += var;
		}

		/* calculate the sum of variances on the right side */
		rightVar = 0;

		for(axis=0; axis<dimension; axis++) {
			double sum, sum2, avg, var;

			sum = 0;
			sum2 = 0;

			for(i=count; i<numPoints; i++) {
				double coord = points[array[i]].getCoord(axis);
				sum += coord;
				sum2 += coord * coord;
			}
			avg = sum / (numPoints - count);
			var = sum2 / (numPoints - count) - avg * avg;

			rightVar += var;
		}

		if(min.count < 0) {
			min.count = count;
			min.leftVar = leftVar;
			min.rightVar = rightVar;
		}
		else {
			if(leftVar + rightVar < min.leftVar + min.rightVar) {
				min.count = count;
				min.leftVar = leftVar;
				min.rightVar = rightVar;
			}
		}
	}

	/* make split flags */
	i=0;
	while(i<min.count) {
		flags[array[i]] = LEFT;
		i++;
	}
	while(i<numPoints) {
		flags[array[i]] = RIGHT;
		i++;
	}

	if(debug) {
		fprintf(stderr, "HnSRTreeFileObj::splitPoints\n");
		fprintf(stderr, "    LEFT\n");
		for(i=0; i<numPoints; i++) {
			if(flags[i] == LEFT)
				fprintf(stderr,
					"    points[%d].getCoord(%d) = %g\n",
					i, max.axis,
					points[i].getCoord(max.axis));
		}
		fprintf(stderr, "    RIGHT\n");
		for(i=0; i<numPoints; i++) {
			if(flags[i] == RIGHT)
				fprintf(stderr,
					"    points[%d].getCoord(%d) = %g\n",
					i, max.axis,
					points[i].getCoord(max.axis));
		}
	}

	*flags_return = flags;

	delete array;
}

void
HnSRTreeFileObj::splitCores(const HnSRTreeCoreArray &cores,
			    SplitFlag **flags_return)
{
	static SplitFlag *flags = NULL;
	int numCores = cores.length();
	int minCount = numCores * splitFactor / 100;
	int maxCount = numCores - minCount;
	struct {
		int axis;
		double var;
	} max;
	struct {
		int count;
		double leftVar, rightVar;
	} min;
	int axis, count;
	int i, j;
	int *array = new int[numCores];

	/* initialize flags */
	if(flags != NULL)
		delete flags;
	flags = new SplitFlag[numCores];

	for(i=0; i<numCores; i++)
		flags[i] = SPLIT_NONE;

	max.axis = -1;
	max.var = -1;

	/* calculate variance */
	for(axis=0; axis<dimension; axis++) {
		double avg, var;
		double sum = 0;
		double sum2 = 0;
		int total = 0;

		for(i=0; i<numCores; i++) {
			HnPoint center = cores[i].getCenter();
			double coord = center.getCoord(axis);

			/*
			 * NOTE:
			 *	In the calculation of the variance,
			 *	it is supposed that all points are located
			 *	at the center of the sphere.
			 */

			sum += coord * cores[i].getWeight();
			sum2 += coord * coord * cores[i].getWeight();
			total += cores[i].getWeight();
		}

		avg = sum / total;
		var = sum2 / total - avg * avg;

		if(max.axis == -1 || var > max.var) {
			max.axis = axis;
			max.var = var;
		}
	}

	/* sort points */
	for(i=0; i<numCores; i++)
		array[i] = i;

	for(i=0; i<numCores; i++) {
		for(j=i+1; j<numCores; j++) {
			HnPoint center1 = cores[array[i]].getCenter();
			HnPoint center2 = cores[array[j]].getCenter();
			double coord1 =	center1.getCoord(max.axis);
			double coord2 = center2.getCoord(max.axis);

			if(coord1 > coord2) {
				int temp = array[i];
				array[i] = array[j];
				array[j] = temp;
			}
		}
	}

	/* choose split point */
	min.count = -1;
	min.leftVar = 0;
	min.rightVar = 0;

	for(count=minCount; count<=maxCount; count++) {
		double leftVar, rightVar;

		/* calculate the sum of variances on the left side */
		leftVar = 0;

		for(axis=0; axis<dimension; axis++) {
			double sum, sum2, avg, var;
			int total;

			/*
			 * NOTE:
			 *	In the calculation of the variance,
			 *	it is supposed that all points are located
			 *	at the center of the sphere.
			 */

			sum = 0;
			sum2 = 0;
			total = 0;

			for(i=0; i<count; i++) {
				HnPoint center = cores[array[i]].getCenter();
				double coord = center.getCoord(axis);

				sum += coord * cores[array[i]].getWeight();
				sum2 += coord * coord
					* cores[array[i]].getWeight();
				total += cores[array[i]].getWeight();
			}

			avg = sum / total;
			var = sum2 / total - avg * avg;

			leftVar += var;
		}

		/* calculate the sum of variances on the right side */
		rightVar = 0;

		for(axis=0; axis<dimension; axis++) {
			double sum, sum2, avg, var;
			int total;

			/*
			 * NOTE:
			 *	In the calculation of the variance,
			 *	it is supposed that all points are located
			 *	at the center of the sphere.
			 */

			sum = 0;
			sum2 = 0;
			total = 0;

			for(i=count; i<numCores; i++) {
				HnPoint center = cores[array[i]].getCenter();
				double coord = center.getCoord(axis);

				sum += coord * cores[array[i]].getWeight();
				sum2 += coord * coord
					* cores[array[i]].getWeight();
				total += cores[array[i]].getWeight();
			}

			avg = sum / total;
			var = sum2 / total - avg * avg;

			rightVar += var;
		}

		if(min.count < 0) {
			min.count = count;
			min.leftVar = leftVar;
			min.rightVar = rightVar;
		}
		else {
			if(leftVar + rightVar < min.leftVar + min.rightVar) {
				min.count = count;
				min.leftVar = leftVar;
				min.rightVar = rightVar;
			}
		}
	}

	/* make split flags */
	i=0;
	while(i<min.count) {
		flags[array[i]] = LEFT;
		i++;
	}
	while(i<numCores) {
		flags[array[i]] = RIGHT;
		i++;
	}

	if(debug) {
		fprintf(stderr, "HnSRTreeFileObj::selectSpheres:\n");
		fprintf(stderr, "    LEFT\n");
		for(i=0; i<numCores; i++) {
			if(flags[i] == LEFT)
				fprintf(stderr,
					"    cores[%d].getCenter()."
					"getCoord(%d) = %g\n",
					i, max.axis,
					cores[i].getCenter().
					getCoord(max.axis));
		}
		fprintf(stderr, "    RIGHT\n");
		for(i=0; i<numCores; i++) {
			if(flags[i] == RIGHT)
				fprintf(stderr,
					"    cores[%d].getCenter()."
					"getCoord(%d) = %g\n",
					i, max.axis,
					cores[i].getCenter().
					getCoord(max.axis));
		}
	}

	*flags_return = flags;

	delete array;
}

/*
 * Construction
 */

void
HnSRTreeFileObj::getSplitFactors(const HnPointArray &points,
				 const HnDataArray &values,
				 int *numLevels_return, int *numSplits_return,
				 int *numRootSplits_return,
				 int *numNodeSplits_return)
{
	int i;
	int maxLeafCount, maxNodeCount, maxSplitsPerNode;
	int numLeaves, numSplits, numLevels;
	int numRootSplits, numNodeSplits;

	for(i=0; i<values.length(); i++) {
		if(values[i].length() > dataSize) {
			HnAbort("The size of data[%d] (%d) "
				"exceeds the limit (%d).",
				i, values[i].length(), dataSize);
		}
	}

	maxLeafCount =
		HnSRTreeLeaf::getMaxCount(dimension, dataSize, blockSize);
	maxNodeCount = HnSRTreeNode::getMaxCount(dimension, blockSize);

	numLeaves = (points.length() - 1) / maxLeafCount + 1;

	maxSplitsPerNode = (int)(log(maxNodeCount) / log(2));
	numSplits = (int)ceil(log(numLeaves) / log(2));

	numLevels = ((numSplits - 1) / maxSplitsPerNode + 1) + 1;

	if((numRootSplits = numSplits % maxSplitsPerNode) == 0)
		numRootSplits = maxSplitsPerNode;
	numNodeSplits = maxSplitsPerNode;

	*numLevels_return = numLevels;
	*numSplits_return = numSplits;
	*numRootSplits_return = numRootSplits;
	*numNodeSplits_return = numNodeSplits;

	if(debug) {
		fprintf(stderr, "HnSRTreeFileObj::getSplitFactors: "
			"maxLeafCount = %d, maxNodeCount = %d, "
			"maxSplitsPerNode = %d, numLeaves = %d, "
			"numSplits = %d, numLevels = %d, "
			"numRootSplits = %d, numNodeSplits = %d\n",
			maxLeafCount, maxNodeCount, maxSplitsPerNode,
			numLeaves, numSplits, numLevels,
			numRootSplits, numNodeSplits);
	}
}

void
HnSRTreeFileObj::constructTree(HnPointArray &points, HnDataArray &values,
			       int offset, int count,
			       int levelCount, int numLevels,
			       int splitCount, int numSplits,
			       int numRootSplits, int numNodeSplits,
			       HnSRTreeNode &node)
{
	int axis;

	axis = getConstructionAxis(points, offset, count);

	sortPoints(points, values, offset, count, axis);

	if(debug) {
		int i;

		for(i=offset; i<offset + count - 1; i++) {
			if(points[i].getCoord(axis) >
			   points[i+1].getCoord(axis))
				HnAbort("sorting failed.");
		}
	}

	if(splitCount + 1 == numSplits) {
		HnSRTreeLeaf leftLeaf = allocateLeaf();
		HnSRTreeLeaf rightLeaf = allocateLeaf();
		int index;

		index = offset;
		while(index < offset + count / 2) {
			leftLeaf.append(points[index], values[index]);
			index ++;
		}
		while(index < offset + count) {
			rightLeaf.append(points[index], values[index]);
			index ++;
		}

		node.append(leftLeaf.getCore(),
			    leftLeaf.getBoundingRect(),
			    leftLeaf.getOffset());
		node.append(rightLeaf.getCore(),
			    rightLeaf.getBoundingRect(),
			    rightLeaf.getOffset());

		writeLeaf(leftLeaf);
		writeLeaf(rightLeaf);
	}
	else if(splitCount + 1 == numRootSplits + levelCount * numNodeSplits) {
		HnSRTreeNode leftNode = allocateNode();
		HnSRTreeNode rightNode = allocateNode();

		constructTree(points, values,
			      offset, count / 2,
			      levelCount + 1, numLevels,
			      splitCount + 1, numSplits,
			      numRootSplits, numNodeSplits,
			      leftNode);
		constructTree(points, values,
			      offset + count / 2, count - count / 2,
			      levelCount + 1, numLevels, 
			      splitCount + 1, numSplits,
			      numRootSplits, numNodeSplits,
			      rightNode);

		node.append(leftNode.getCore(),
			    leftNode.getBoundingRect(),
			    leftNode.getOffset());
		node.append(rightNode.getCore(),
			    rightNode.getBoundingRect(),
			    rightNode.getOffset());

		writeNode(leftNode);
		writeNode(rightNode);
	}
	else {
		constructTree(points, values,
			      offset, count / 2,
			      levelCount, numLevels,
			      splitCount + 1, numSplits,
			      numRootSplits, numNodeSplits,
			      node);
		constructTree(points, values,
			      offset + count / 2, count - count / 2,
			      levelCount, numLevels,
			      splitCount + 1, numSplits,
			      numRootSplits, numNodeSplits,
			      node);
	}
}

int
HnSRTreeFileObj::getConstructionAxis(HnPointArray &points,
				     int offset, int count)
{
	int axis;
	double *sum = new double[dimension];
	double *sum2 = new double[dimension];
	int i;
	struct {
		int axis;
		double var;
	} max;

	for(axis=0; axis<dimension; axis++) {
		sum[axis] = 0;
		sum2[axis] = 0;
	}

	for(i=offset; i<offset + count; i++) {
		for(axis=0; axis<dimension; axis++) {
			double x = points[i].getCoord(axis);

			sum[axis] += x;
			sum2[axis] += x * x;
		}
	}

	max.axis = -1;
	max.var = 0;

	for(axis=0; axis<dimension; axis++) {
		double mean = sum[axis] / count;
		double var = sum2[axis] / count - mean * mean;

		if(axis == 0) {
			max.axis = 0;
			max.var = var;
		}
		else {
			if(var > max.var) {
				max.axis = axis;
				max.var = var;
			}
		}
	}

	delete sum;
	delete sum2;

	return max.axis;
}

void
HnSRTreeFileObj::sortPoints(HnPointArray &points, HnDataArray &values,
			    int offset, int count, int axis)
{
	if(count < 16) {
		int i, j;

		for(i=offset; i<offset + count; i++) {
			for(j=i+1; j<offset + count; j++) {
				if(points[i].getCoord(axis) >
				   points[j].getCoord(axis)) {
					points.swap(i, j);
					values.swap(i, j);
				}
			}
		}
	}
	else {
		double pivot =
			points[offset + count / 2].getCoord(axis);
		int leftIndex, rightIndex;

		points.swap(offset, offset + count / 2);
		values.swap(offset, offset + count / 2);

		leftIndex = offset;
		rightIndex = offset + count;

		for(;;) {
			do {
				leftIndex ++;
			} while(leftIndex < offset + count &&
				points[leftIndex].getCoord(axis)
				<= pivot);
			do {
				rightIndex --;
			} while(rightIndex >= offset && 
				points[rightIndex].getCoord(axis)
				> pivot);

			if(leftIndex > rightIndex)
				break;

			if(leftIndex == rightIndex)
				HnAbort("impossible situation.");

			points.swap(leftIndex, rightIndex);
			values.swap(leftIndex, rightIndex);
		}

		if(leftIndex != rightIndex + 1)
			HnAbort("impossible situation.");

		if(offset != leftIndex - 1) {
			points.swap(offset, leftIndex - 1);
			values.swap(offset, leftIndex - 1);
		}

		if(debug) {
			int i;

			for(i=offset; i<leftIndex - 1; i++) {
				if(points[i].getCoord(axis) > pivot)
					HnAbort("left pivot is failed.");
			}
			if(points[leftIndex-1].getCoord(axis) != pivot)
				HnAbort("center pivot is failed.");
			for(i=leftIndex; i<offset + count; i++) {
				if(points[i].getCoord(axis) <= pivot)
					HnAbort("right pivot is failed.");
			}
		}

		sortPoints(points, values,
			   offset, leftIndex - 1 - offset, axis);
		sortPoints(points, values,
			   leftIndex, offset + count - leftIndex, axis);
	}
}

/*
 * Remove
 */

void
HnSRTreeFileObj::remove(const HnPoint &point, const HnData &data)
{
	HnSRTreeStack stack;
	HnSRTreeLeaf leaf;
	HnSRTreeNode node;
	int cursor, maxCount, minCount;
	int level;
	HnBool underflow;
	HnSRTreeCore core;
	HnRect rect;
	int i;

	/* search point */
	if((stack = searchPoint(point, data)) == HnSRTreeStack::null)
		HnAbort("the given point/data pair is not found in the tree.");

	if(debug) {
		fprintf(stderr, "HnSRTreeFile::remove: point is found.\n");
	}

	/* remove point */
	leaf = stack.getTopLeaf();
	cursor = stack.getCursor();
	stack.pop();

	leaf.remove(cursor);
	
	if(stack.getDepth() == 0) {
		writeLeaf(leaf);

		if(debug) {
			fprintf(stderr,	"HnSRTreeFile::remove: "
				"removal is finished.\n");
		}

		return;
	}

	/* condense tree */
	reinsertList = new_HnSRTreeReinsertArray();
	reinsertedBlocks = new_HnSRTreeOffsetArray();

	maxCount = HnSRTreeLeaf::getMaxCount(dimension, dataSize, blockSize);
	minCount = maxCount * splitFactor / 100;
	underflow = (leaf.getCount() < minCount);

	if(underflow) {
		if(debug) {
			fprintf(stderr, "HnSRTreeFile::remove: "
				"underflow (count: %d, minCount: %d) "
				"occurred in the leaf (offset: 0x%08X).\n",
				leaf.getCount(), minCount,
				(int)leaf.getOffset());
		}

		for(i=0; i<leaf.getCount(); i++) {
			HnPoint leafPoint = leaf.getPointAt(i);
			HnData leafData = leaf.getDataAt(i);

			reinsertList.append(new_HnSRTreeReinsert(leafPoint,
								 leafData));
		}
		releaseBlock(leaf.getOffset());
	}
	else {
		writeLeaf(leaf);
		core = leaf.getCore();
		rect = leaf.getBoundingRect();
	}

	/* intermediate nodes */
	while(stack.getDepth() > 1) {
		node = stack.getTopNode();
		cursor = stack.getCursor();
		level = height - stack.getDepth();
		stack.pop();

		if(underflow) {
			node.remove(cursor);

			maxCount = HnSRTreeNode::getMaxCount(dimension,
							     blockSize);
			minCount = maxCount * splitFactor / 100;
			underflow = (node.getCount() < minCount);

			if(underflow) {
				if(debug) {
					fprintf(stderr,
						"HnSRTreeFile::remove: "
						"underflow (count: %d, "
						"minCount: %d) "
						"occurred in the node "
						"(offset: 0x%08X, "
						"level: %d).\n",
						node.getCount(), minCount,
						(int)node.getOffset(), level);
				}

				for(i=0; i<node.getCount(); i++) {
					off_t offset = node.getOffsetAt(i);
					HnSRTreeReinsert reinsert;

					reinsert =
						new_HnSRTreeReinsert(offset,
								     level);
					reinsertList.append(reinsert);
				}
				releaseBlock(node.getOffset());
			}
			else {
				writeNode(node);
				core = node.getCore();
				rect = node.getBoundingRect();
			}
		}
		else {
			node.setCoreAt(core, cursor);
			node.setBoundingRectAt(rect, cursor);

			writeNode(node);
			core = node.getCore();
			rect = node.getBoundingRect();
		}
	}
		
	/* root node */
	node = stack.getTopNode();
	cursor = stack.getCursor();
	level = height - stack.getDepth();
	stack.pop();

	if(underflow) {
		node.remove(cursor);

		if(node.getCount() == 1) {
			if(debug) {
				fprintf(stderr, "HnSRTreeFile::remove: "
					"tree is shrunken.\n");
			}

			releaseBlock(node.getOffset());
			rootOffset = node.getOffsetAt(0);
			height --;

			writeSuperBlock();
		}
		else
			writeNode(node);
	}
	else {
		node.setCoreAt(core, cursor);
		node.setBoundingRectAt(rect, cursor);

		writeNode(node);
	}

	/* reinsert orphaned entries */
	while(reinsertList.length() != 0) {
		if(reinsertList[0].getType() == HnSRTreeReinsert::POINT) {
			HnPoint point = reinsertList[0].getPoint();
			HnData data = reinsertList[0].getData();

			if(debug) {
				fprintf(stderr, "HnSRTreeFile::remove: "
					"reinserting point %s.\n",
					(char *)point.toString());
			}

			reinsertList.remove(0);
			insertPoint(point, data);
		}
		else {
			off_t offset = reinsertList[0].getOffset();
			int level = reinsertList[0].getLevel();

			if(debug) {
				fprintf(stderr, "HnSRTreeFile::remove: "
					"reinserting block 0x%08X "
					"at the level %d.\n",
					(int)offset, level);
			}

			reinsertList.remove(0);
			insertBlock(offset, level);
		}
	}

	if(debug) {
		fprintf(stderr,
			"HnSRTreeFile::remove: removal is finished.\n");
	}

	if(debug)
		check();
}

HnSRTreeStack
HnSRTreeFileObj::searchPoint(const HnPoint &point, const HnData &data)
{
	HnSRTreeStack stack;
	HnSRTreeBlock block;
	HnSRTreeNode node;
	HnSRTreeLeaf leaf;
	HnBool found;

	stack = new_HnSRTreeStack();

	block = readBlock(rootOffset);
	if(block.isNode()) {
		node = new_HnSRTreeNode(dimension, block);
		stack.push(node);
	}
	else if(block.isLeaf()) {
		leaf = new_HnSRTreeLeaf(dimension, dataSize, block);
		stack.push(leaf);
	}
	else
		HnAbort("unexpected block type.");

	for(;;) {
		if(stack.isTopNode()) {
			node = stack.getTopNode();
		
			/*
			 * search for overlapping entries
			 */

			for(;;) {
				int cursor = stack.getCursor();
				HnRect rect = node.getRectAt(cursor);

				if(debug) {
					fprintf(stderr,
						"comparing with a rect"
						" #%d at a node %08X "
						"on the level %d.\n",
						cursor,
						(int)node.getOffset(),
						stack.getDepth() - 1);
				}

				if(rect.includes(point)) {
					found = HnTRUE;
					break;
				}

				if(!stack.hasMore()) {
					found = HnFALSE;
					break;
				}

				stack.advance();
			}

			if(found) {
				int cursor = stack.getCursor();
				block = readBlock(node.getOffsetAt(cursor));

				if(block.isNode()) {
					node = new_HnSRTreeNode(dimension,
								block);
					stack.push(node);
				}
				else if(block.isLeaf()) {
					leaf = new_HnSRTreeLeaf(dimension,
								dataSize,
								block);
					stack.push(leaf);
				}
				else
					HnAbort("unexpected block type.");
			}
			else {
				/*
				 * go up until the top node has some remaining
				 * entries or the stack is empty.
				 */

				for(;;) {
					stack.pop();

					if(stack.getDepth() == 0)
						return HnSRTreeStack::null;

					if(stack.hasMore())
						break;
				}

				stack.advance();
			}
		}
		else {
			leaf = stack.getTopLeaf();

			/*
			 * search for overlapping entries
			 */

			for(;;) {
				int cursor = stack.getCursor();
				HnPoint leafPoint = leaf.getPointAt(cursor);
				HnData leafData = leaf.getDataAt(cursor);

				if(debug) {
					fprintf(stderr,
						"comparing with "
						"a point #%d at "
						"a leaf %08X "
						"on the level %d.\n",
						cursor,
						(int)leaf.getOffset(),
						stack.getDepth() - 1);
				}

				if(leafPoint.equals(point) &&
				   leafData.equals(data)) {
					found = HnTRUE;
					break;
				}

				if(!stack.hasMore()) {
					found = HnFALSE;
					break;
				}

				stack.advance();
			}

			if(found)
				return stack;
			else {
				/*
				 * go up until the top node has some remaining
				 * entries or the stack is empty.
				 */

				for(;;) {
					stack.pop();

					if(stack.getDepth() == 0)
						return HnSRTreeStack::null;

					if(stack.hasMore())
						break;
				}

				stack.advance();
			}
		}
	}
}

/*
 * Sequential Access
 */

void
HnSRTreeFileObj::getFirst(HnPoint *point_return, HnData *data_return)
{
	getFirst(HnRect::null, point_return, data_return);
}

void
HnSRTreeFileObj::getFirst(const HnRect &rect,
			  HnPoint *point_return, HnData *data_return)
{
	HnSRTreeStack stack;

	context.stack = stack = new_HnSRTreeStack();
	context.rect = rect;

	getNext(point_return, data_return);
}

void
HnSRTreeFileObj::getNext(HnPoint *point_return, HnData *data_return)
{
	HnSRTreeStack &stack = context.stack;
	HnSRTreeBlock block;
	HnSRTreeNode node;
	HnSRTreeLeaf leaf;
	HnBool found;

	if(stack.getDepth() == 0) {
		/* initialize */
		block = readBlock(rootOffset);
		if(block.isNode()) {
			node = new_HnSRTreeNode(dimension, block);
			stack.push(node);
		}
		else if(block.isLeaf()) {
			leaf = new_HnSRTreeLeaf(dimension, dataSize, block);
			stack.push(leaf);
		}
		else
			HnAbort("unexpected block type.");
	}
	else {
		/* proceed */
		if(stack.hasMore())
			stack.advance();
		else {
			for(;;) {
				stack.pop();

				if(stack.getDepth() == 0) {
					*point_return = HnPoint::null;
					*data_return = HnData::null;
					return;
				}

				if(stack.hasMore())
					break;
			}

			stack.advance();
		}
	}

	for(;;) {
		if(stack.isTopNode()) {
			node = stack.getTopNode();
		
			/*
			 * search for overlapping entries
			 */

			if(context.rect.isInvalid())
				found = HnTRUE;
			else {
				for(;;) {
					int cursor = stack.getCursor();
					HnRect rect = node.getRectAt(cursor);

					if(debug) {
						fprintf(stderr,
							"comparing with a rect"
							" #%d at a node %08X "
							"on the level %d.\n",
							cursor,
							(int)node.getOffset(),
							stack.getDepth() - 1);
					}

					if(context.rect.overlaps(rect)) {
						found = HnTRUE;
						break;
					}

					if(!stack.hasMore()) {
						found = HnFALSE;
						break;
					}

					stack.advance();
				}
			}

			if(found) {
				int cursor = stack.getCursor();
				block = readBlock(node.getOffsetAt(cursor));

				if(block.isNode()) {
					node = new_HnSRTreeNode(dimension,
								block);
					stack.push(node);
				}
				else if(block.isLeaf()) {
					leaf = new_HnSRTreeLeaf(dimension,
								dataSize,
								block);
					stack.push(leaf);
				}
				else
					HnAbort("unexpected block type.");
			}
			else {
				/*
				 * go up until the top node has some remaining
				 * entries or the stack is empty.
				 */

				for(;;) {
					stack.pop();

					if(stack.getDepth() == 0) {
						*point_return = HnPoint::null;
						*data_return = HnData::null;
						return;
					}

					if(stack.hasMore())
						break;
				}

				stack.advance();
			}
		}
		else {
			leaf = stack.getTopLeaf();

			/*
			 * search for overlapping entries
			 */

			if(context.rect.isInvalid())
				found = HnTRUE;
			else {
				for(;;) {
					int cursor = stack.getCursor();
					HnPoint point =
						leaf.getPointAt(cursor);

					if(debug) {
						fprintf(stderr,
							"comparing with "
							"a point #%d at "
							"a leaf %08X "
							"on the level %d.\n",
							cursor,
							(int)leaf.getOffset(),
							stack.getDepth() - 1);
					}

					if(context.rect.includes(point)) {
						found = HnTRUE;
						break;
					}

					if(!stack.hasMore()) {
						found = HnFALSE;
						break;
					}

					stack.advance();
				}
			}

			if(found) {
				int cursor = stack.getCursor();
				*point_return = leaf.getPointAt(cursor);
				*data_return = leaf.getDataAt(cursor);
				return;
			}
			else {
				/*
				 * go up until the top node has some remaining
				 * entries or the stack is empty.
				 */

				for(;;) {
					stack.pop();

					if(stack.getDepth() == 0) {
						*point_return = HnPoint::null;
						*data_return = HnData::null;
						return;
					}

					if(stack.hasMore())
						break;
				}

				stack.advance();
			}
		}
	}
}

/*
 * Neighbor Search
 */

void
HnSRTreeFileObj::getNeighbors(const HnPoint &point, int maxCount,
			      HnPointArray *points_return,
			      HnDataArray *values_return)
{
	HnSRTreeNeighborArray neighbors = new_HnSRTreeNeighborArray();
	HnPointArray points;
	HnDataArray values;
	int i;

	if(point.getDimension() != getDimension())
		HnAbort("mismatch in the dimensions (point: %d, file: %d)",
			point.getDimension(), getDimension());

        if(maxCount <= 0)
		HnAbort("invalid max count %d.", maxCount);

	neighbors = chooseNeighbors(rootOffset, point, maxCount, neighbors);

	/* make output */
	points = new_HnPointArray();
	values = new_HnDataArray();

	for(i=0; i<neighbors.length(); i++) {
		points.append(neighbors[i].getPoint());
		values.append(neighbors[i].getData());
	}

	*points_return = points;
	*values_return = values;
}

double
HnSRTreeFileObj::getMinDistance(const HnPoint &point,
				const HnSRTreeCore &core, const HnRect &rect)
{
	double sphereDistance, rectDistance, minDistance;
	double distance, radius;

	/* sphere distance */
	distance = point.getDistance(core.getCenter());
	radius = core.getRadius();

	if(distance < radius)
		sphereDistance = 0;
	else
		sphereDistance = distance - radius;

	/* rect distance */
	rectDistance = rect.getMinDistance(point);

	/* compare distances */
	if(sphereDistance > rectDistance)
		minDistance = sphereDistance;
	else
		minDistance = rectDistance;

	return minDistance;
}

double
HnSRTreeFileObj::getMaxDistance(const HnPoint &point,
				const HnSRTreeCore &core, const HnRect &rect)
{
	double sphereDistance, rectDistance, maxDistance;

	sphereDistance =
		point.getDistance(core.getCenter()) + core.getRadius();

	rectDistance = rect.getMaxDistance(point);

	if(sphereDistance < rectDistance)
		maxDistance = sphereDistance;
	else
		maxDistance = rectDistance;

	return maxDistance;
}

HnSRTreeNeighborArray
HnSRTreeFileObj::chooseNeighbors(off_t offset,
				 const HnPoint &point, int maxCount,
				 const HnSRTreeNeighborArray &neighbors)
{
	HnSRTreeBlock block = readBlock(offset);

	if(block.isNode()) {
		return chooseNeighborsInNode(block, point, maxCount,
					     neighbors);
	}
	else if(block.isLeaf()) {
		return chooseNeighborsInLeaf(block, point, maxCount,
					     neighbors);
	}
	else
		HnAbort("unexpected block type.");
}

HnSRTreeNeighborArray
HnSRTreeFileObj::chooseNeighborsInNode(const HnSRTreeBlock &block,
				       const HnPoint &point, int maxCount,
				       HnSRTreeNeighborArray neighbors)
{
	HnSRTreeNode node = new_HnSRTreeNode(dimension, block);
	int numRects = node.getCount();
	int i, j;
	struct Entry {
		int index;
		double minDistance;
		double maxDistance;
	} *array = new Entry[numRects], temp;
		
	/* initialize array */
	for(i=0; i<numRects; i++) {
		array[i].index = i;
		array[i].minDistance =
			getMinDistance(point,
				       node.getCoreAt(i), node.getRectAt(i));
		array[i].maxDistance =
			getMaxDistance(point,
				       node.getCoreAt(i), node.getRectAt(i));
	}

	/* sort array */
	for(i=0; i<numRects; i++) {
		for(j=i+1; j<numRects; j++) {
			if(array[i].minDistance > array[j].minDistance ||
			   ((array[i].minDistance == array[j].minDistance) &&
			    (array[i].maxDistance > array[j].maxDistance))) {
				temp = array[i];
				array[i] = array[j];
				array[j] = temp;
			}
		}
	}

	if(debug) {
		fprintf(stderr, "HnSRTreeFileObj::chooseNeighborsInNode: "
			"sorted rectangles\n");
		for(i=0; i<numRects; i++) {
			HnRect rect = node.getRectAt(array[i].index);
			fprintf(stderr,
				"    %2d: cnt = %7.6f, "
				"min = %7.6f, max = %7.6f, inc = %s\n",
				i, point.getDistance(rect.getCenterPoint()),
				array[i].minDistance, array[i].maxDistance,
				(rect.includes(point) ? "yes" : "no"));
		}
	}

	for(i=0; i<numRects; i++) {
		off_t offset = node.getOffsetAt(array[i].index);
		double minDistance = array[i].minDistance;

		if(neighbors.length() < maxCount) {
			/*
			 * If the number of neighbors does not reach the max,
			 * try every children to collect neighbors.
			 */
			neighbors = chooseNeighbors(offset, point, maxCount,
						    neighbors);
		}
		else {
			/*
			 * If the number of neighbors reaches the max,
			 * try rectangles which overlap with the current
			 * searching rectangle.
			 *
			 * The searching rectangle is defined as follows:
			 *	(1) It is a cube.
			 *	(2) Its center point is a target point.
			 *	(3) The width of sides is twice of the
			 *	    distance of the farthest neighbor.
			 */

			HnSRTreeNeighbor farthest;
			double farthestDistance;

			/*
			 * NOTE:
			 *	`neighbors' are assumed to be sorted.
			 */

			farthest = neighbors[neighbors.length() - 1];
			farthestDistance = farthest.getDistance();

			if(minDistance <= farthestDistance) {
				neighbors = chooseNeighbors(offset,
							    point, maxCount,
							    neighbors);
			}
		}
	}

	delete array;

	return neighbors;
}

HnSRTreeNeighborArray
HnSRTreeFileObj::chooseNeighborsInLeaf(const HnSRTreeBlock &block,
				       const HnPoint &point, int maxCount,
				       const HnSRTreeNeighborArray &neighbors)
{
	HnSRTreeLeaf leaf = new_HnSRTreeLeaf(dimension, dataSize, block);
	int numRects = leaf.getCount();
	HnSRTreeNeighborArray array, sorted;
	int i, count;

	/* add rectangles in the leaf */
	array = new_HnSRTreeNeighborArray(neighbors);

	for(i=0; i<numRects; i++) {
		HnPoint key = leaf.getPointAt(i);
		HnData value = leaf.getDataAt(i);
		double distance = point.getDistance(key);

		array.append(new_HnSRTreeNeighbor(key, value, distance));
	}

	sorted = HnSRTreeNeighbor::sort(array);

	/* select closer rectangles up to the max count */
	array = new_HnSRTreeNeighborArray();

	for(count = 0; count < sorted.length(); count ++) {
		if(count >= maxCount) {
			/*
			 * NOTE:
			 *	Even if the count is greater than the max,
			 *	rectangles that have the same distance
			 *	need to be appended together.
			 */

			double lastDistance = sorted[count - 1].getDistance();
			double nextDistance = sorted[count    ].getDistance();

			if(lastDistance != nextDistance)
				break;
		}

		array.append(sorted[count]);
	}

	if(debug) {
		fprintf(stderr, "HnSRTreeFileObj::getNeighborsInLeaf: \n");
		for(i=0; i<neighbors.length(); i++) {
			fprintf(stderr, "    neighbors[%d] = %s\n",
				i, (char *)array[i].toString());
		}
	}

	return array;
}

/*
 * Check
 */

void
HnSRTreeFileObj::check(void)
{
	checkBlock(HnSRTreeCore::null, HnRect::null, rootOffset);
	checkInclusion(rootOffset);
}

int
HnSRTreeFileObj::checkBlock(const HnSRTreeCore &core, 
			    const HnRect &rect, off_t offset)
{
	HnSRTreeBlock block;
	HnSRTreeNode node;
	HnSRTreeLeaf leaf;
	int i, code, level = -1;

	block = readBlock(offset);

	if(block.isNode()) {
		node = new_HnSRTreeNode(dimension, block);

		if(core != HnSRTreeCore::null) {
			if(!node.getCore().equals(core))
				HnAbort("mismatch in core %s and %s.",
					(char *)
					node.getCore().toString(),
					(char *)core.toString());
		}

		if(rect.isValid()) {
			if(!node.getBoundingRect().equals(rect))
				HnAbort("mismatch in bounding rect %s and %s.",
					(char *)
					node.getBoundingRect().toString(),
					(char *)rect.toString());
		}

		for(i=0; i<node.getCount(); i++) {
			code = checkBlock(node.getCoreAt(i),
					  node.getRectAt(i),
					  node.getOffsetAt(i));
			if(i == 0)
				level = code;
			else {
				if(code != level)
					HnAbort("the tree is not balanced at "
						"a node %08X.",	(int)offset);
			}
		}
	}
	else if(block.isLeaf()) {
		leaf = new_HnSRTreeLeaf(dimension, dataSize, block);

		if(core.isValid()) {
			if(!leaf.getCore().equals(core))
				HnAbort("mismatch in core %s and %s.",
					(char *)
					leaf.getCore().toString(),
					(char *)core.toString());
		}

		if(rect.isValid()) {
			if(!leaf.getBoundingRect().equals(rect))
				HnAbort("mismatch in bounding rect %s and %s.",
					(char *)
					leaf.getBoundingRect().toString(),
					(char *)rect.toString());
		}
	}
	else
		HnAbort("unexpected block type.");
		

	/*
	 * The `level' variable is used to confirm that the tree is balanced.
	 */

	return level + 1;
}

HnPointArray
HnSRTreeFileObj::checkInclusion(off_t offset)
{
	HnSRTreeBlock block;
	HnPointArray sum = new_HnPointArray();

	block = readBlock(offset);

	if(block.isNode()) {
		HnSRTreeNode node = new_HnSRTreeNode(dimension, block);
		int i, j;

		for(i=0; i<node.getCount(); i++) {
			HnPointArray points =
				checkInclusion(node.getOffsetAt(i));
			HnPoint center = node.getCoreAt(i).getCenter();
			double radius = node.getCoreAt(i).getRadius();

			for(j=0; j<points.length(); j++) {
				double distance;

				distance = points[j].getDistance(center);
				
				if(distance > radius) {
					describeExclusion(node.getOffsetAt(i),
							  center);
					HnAbort("point (%s) is not included "
						"in the core (center = %s, "
						"raidus = %g) of the node %08X"
						" when the distance is %g.",
						(char *)points[j].toString(),
						(char *)center.toString(),
						radius, (int)offset, distance);
				}
			}

			sum.append(points);
		}
	}
	else if(block.isLeaf()) {
		HnSRTreeLeaf leaf =
			new_HnSRTreeLeaf(dimension, dataSize, block);
		int i;

		for(i=0; i<leaf.getCount(); i++)
			sum.append(leaf.getPointAt(i));
	}
	else
		HnAbort("unexpected block type.");

	return sum;
}

void
HnSRTreeFileObj::describeExclusion(off_t offset, const HnPoint &center)
{
	HnSRTreeBlock block = readBlock(offset);

	if(block.isNode()) {
		HnSRTreeNode node = new_HnSRTreeNode(dimension, block);
		int i;

		for(i=0; i<node.getCount(); i++) {
			HnSRTreeCore core = node.getCoreAt(i);
			HnRect rect = node.getRectAt(i);
			double sphereDistance, rectDistance;

			sphereDistance = 
				core.getCenter().getDistance(center)
					+ core.getRadius();

			rectDistance = rect.getMaxDistance(center);
			
			printf("    %2d: core = %s, rect = %s, "
			       "sphereDistance = %g, rectDistance = %g\n",
			       i, (char *)core.toString(),
			       (char *)rect.toString(),
			       sphereDistance, rectDistance);
		}
	}
	else if(block.isLeaf()) {
		HnSRTreeLeaf leaf =
			new_HnSRTreeLeaf(dimension, dataSize, block);
		int i;

		for(i=0; i<leaf.getCount(); i++) {
			HnPoint point = leaf.getPointAt(i);
			double distance = point.getDistance(center);

			printf("    %2d: point = %s, distance = %g\n",
			       i, (char *)point.toString(), distance);
		}
	}
	else
		HnAbort("unexpected block type.");
}

/*
 * Print
 */

void
HnSRTreeFileObj::print(void)
{
	class Statistics {
	private:
		double min;
		double max;
		double sum;
		int count;

	public:
		Statistics(void) {
			min = 0;
			max = 0;
			sum = 0;
			count = 0;
		}
		void add(double utility) {
			if(count == 0) {
				min = utility;
				max = utility;
			}
			else {
				if(utility < min)
					min = utility;
				if(utility > max)
					max = utility;
			}
			sum += utility;
			count ++;
		}
		double getMin(void) const {
			return min;
		}
		double getMax(void) const {
			return max;
		}
		double getAverage(void) const {
			return (double)sum / count;
		}
	};

	Statistics blockUtility, rootUtility, nodeUtility, leafUtility;

	off_t offset, size;
	double utility;
	int numNodes, numLeaves, numPoints, numFreeBlocks;

	readSuperBlock();

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);

	numNodes = 0;
	numLeaves = 0;
	numPoints = 0;
	numFreeBlocks = 0;

	for(offset = blockSize; offset < size; offset += blockSize) {
		HnSRTreeBlock block = readBlock(offset);

		if(block.isNode()) {
			HnSRTreeNode node = new_HnSRTreeNode(dimension, block);

			printNode(node);

			utility = (double)node.getSize() / blockSize;

			blockUtility.add(utility);
			if(offset == rootOffset)
				rootUtility.add(utility);
			else
				nodeUtility.add(utility);

			numNodes ++;
		}
		else if(block.isLeaf()) {
			HnSRTreeLeaf leaf =
				new_HnSRTreeLeaf(dimension, dataSize, block);

			printLeaf(leaf);

			utility = (double)leaf.getSize() / blockSize;

			blockUtility.add(utility);
			leafUtility.add(utility);

			numLeaves ++;
			numPoints += leaf.getCount();
		}
		else if(block.isFree())
			numFreeBlocks ++;
		else
			HnAbort("unexpected block type.");
	}

	printf("SuperBlock\n");
	printf("    dimension     : %d\n", dimension);
	printf("    dataSize      : %d\n", dataSize);
	printf("    blockSize     : %d\n", blockSize);
	printf("    fileSize      : %d\n", fileSize);
	printf("    freeOffset    : 0x%08X\n", (unsigned int)freeOffset);
	printf("    rootOffset    : 0x%08X\n", (unsigned int)rootOffset);
	printf("    height        : %d\n", height);
	printf("    splitFactor   : %d\n", splitFactor);
	printf("    reinsertFactor: %d\n", reinsertFactor);

	printf("File Size is 0x%08X\n", (unsigned int)size);

	printf("Block utility      : "
	       "avg %8.5f %%, min %8.5f %%, max %8.5f %%\n",
	       blockUtility.getAverage() * 100,
	       blockUtility.getMin() * 100,
	       blockUtility.getMax() * 100);

	printf("Root block utility : "
	       "    %8.5f %%\n",
	       rootUtility.getAverage() * 100);

	printf("Node block utility : "
	       "avg %8.5f %%, min %8.5f %%, max %8.5f %%\n",
	       nodeUtility.getAverage() * 100,
	       nodeUtility.getMin() * 100,
	       nodeUtility.getMax() * 100);

	printf("Leaf block utility : "
	       "avg %8.5f %%, min %8.5f %%, max %8.5f %%\n",
	       leafUtility.getAverage() * 100,
	       leafUtility.getMin() * 100,
	       leafUtility.getMax() * 100);

	printf("Number of nodes : %d\n", numNodes);
	printf("Number of leaves: %d\n", numLeaves);
	printf("Number of free blocks: %d\n", numFreeBlocks);
	printf("Number of leaf points: %d\n", numPoints);
}

void
HnSRTreeFileObj::printNode(const HnSRTreeNode &node)
{
	int i;
	HnRectArray rects;

	printf("Block (0x%08X)\n",
	       (unsigned int)node.getOffset());
	printf("    type       : NODE\n");
	printf("    count      : %d\n", node.getCount());
	printf("    utility    : %d (%g %%)\n",
	       (int)node.getSize(), 
	       (double)node.getSize() / blockSize * 100);

	/* children */
	for(i=0; i<node.getCount(); i++) {
		printf("    %5d: core = %s, rect = %s, offset = 0x%08X\n",
		       i,
		       (char *)node.getCoreAt(i).toString(),
		       (char *)node.getRectAt(i).toString(),
		       (unsigned int)node.getOffsetAt(i));
	}
}

void
HnSRTreeFileObj::printLeaf(const HnSRTreeLeaf &leaf)
{
	int i;

	printf("Block (0x%08X)\n",
	       (unsigned int)leaf.getOffset());
	printf("    type    : LEAF\n");
	printf("    count   : %d\n", leaf.getCount());
	printf("    utility : %d (%g %%)\n",
	       (int)leaf.getSize(),
	       (double)leaf.getSize() / blockSize * 100);

	for(i=0; i<leaf.getCount(); i++) {
		printf("    %5d: %s\n", i,
		       (char *)leaf.getPointAt(i).toString());
	}
}

void
HnSRTreeFileObj::setDebug(HnBool flag)
{
	debug = flag;
}

