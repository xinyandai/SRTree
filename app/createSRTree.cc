/*
 * createSRTree.cc
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
 * $Id: createSRTree.cc,v 1.3 1997/06/02 09:35:50 katayama Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "HnSRTreeFile.hh"
#include "record.hh"

static void printUsage(void);
static void createSRTree(const char *dataFileName, const char *treeFileName,
			 int dimension, int dataSize, int blockSize,
			 int splitFactor, int reinsertFactor,
			 int maxCount, HnBool debug);

int
main(int argc, char *argv[])
{
	int dimension, dataSize, blockSize;
	int splitFactor, reinsertFactor;
	char *dataFileName, *treeFileName;
	int maxCount;
	HnBool debug;

	int c, errflag;
	extern int optind;
	extern char *optarg;

	dimension = -1;
	dataSize = sizeof(RecordData);
	blockSize = 8192;

	splitFactor = 40;
	reinsertFactor = 30;

	maxCount = -1;
	debug = HnFALSE;

	errflag = 0;
	while((c = getopt(argc, argv, "b:c:dr:s:")) != EOF) {
		switch(c) {
		case 'b':
			blockSize = atoi(optarg);
			break;
		case 'c':
			maxCount = atoi(optarg);
			break;
		case 'd':
			debug = HnTRUE;
			break;
		case 'r':
			reinsertFactor = atoi(optarg);
			break;
		case 's':
			splitFactor = atoi(optarg);
			break;
		case '?':
			errflag = 1;
			break;
		}
	}
	if(errflag || optind != argc - 3) {
		printUsage();
		return 1;
	}

	dimension = atoi(argv[optind]);
	dataFileName = argv[optind + 1];
	treeFileName = argv[optind + 2];

	createSRTree(dataFileName, treeFileName,
		     dimension, dataSize, blockSize,
		     splitFactor, reinsertFactor, maxCount, debug);

	return 0;
}

static void
printUsage(void)
{
	fprintf(stderr, "\
Usage: createSRTree [options] dimension datafile treefile\n\
Options\n\
    -b blockSize      set the block size\n\
    -c count          set the count of data\n\
    -d                turn on the debug mode\n\
    -s splitFactor    set the split factor in percent (40 by default)\n\
    -r reinsertFactor set the reinsert factor in percent (30 by default)\n\
");
}

static void
createSRTree(const char *dataFileName, const char *treeFileName,
	     int dimension, int dataSize, int blockSize,
	     int splitFactor, int reinsertFactor,
	     int maxCount, HnBool debug)
{
	HnSRTreeFile treeFile;
	HnPoint point;
	HnData data;
	RecordPoint recordPoint;
	RecordData recordData;
	FILE *fp;
	int i, count;
	char buffer[BUFFER_SIZE];

	treeFile = new_HnSRTreeFile(treeFileName,
				    dimension, dataSize, blockSize,
				    splitFactor, reinsertFactor);

	if(treeFile == HnSRTreeFile::null) {
		perror(treeFileName);
		exit(1);
	}

	if(debug)
		treeFile.setDebug(HnTRUE);

	if((fp = fopen(dataFileName, "r")) == NULL) {
		perror(dataFileName);
		exit(1);
	}

	count = 0;

	while((maxCount < 0 || count < maxCount) &&
	      fgets(buffer, sizeof(buffer), fp) != NULL) {
		if(buffer[0] == '#' || buffer[0] == '\n')
			continue;

		getRecord(buffer, dimension, &recordPoint, &recordData);

		if(includesNANorINF(recordPoint))
			continue;

		point = new_HnPoint(dimension);
		for(i=0; i<dimension; i++) {
			point.setCoord(recordPoint.coords[i], i);
		}

		data = new_HnData(&recordData, sizeof(recordData));

		printf("Inserting %s\n", (char *)point.toString());

		treeFile.store(point, data);

		if(debug)
			treeFile.check();

		count ++;
	}

	fclose(fp);
	treeFile.close();
}

