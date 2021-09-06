/*
 * searchSRTree.cc
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
 * $Id: searchSRTree.cc,v 1.5 1997/06/02 09:35:52 katayama Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "HnSRTreeFile.hh"
#include "record.hh"

static void searchNeighbors(int dimension,
			    const char *dataFileName, const char *treeFileName,
			    int maxCount, HnBool debug, HnBool verify,
			    int neighbors);

static void searchRange(int dimemsion,
			const char *dataFileName, const char *treeFileName,
			int maxCount, HnBool debug, HnBool verify,
			double width);

int
main(int argc, char *argv[])
{
	int dimension;
	int maxCount, debug, verify;
	int neighbors;
	double width;
	char *treeFileName, *dataFileName;

	int c, errflag;
	extern int optind;
	extern char *optarg;

	maxCount = -1;
	debug = 0;
	neighbors = 1;
	width = 0;
	verify = 0;

	errflag = 0;
	while((c = getopt(argc, argv, "c:dn:vw:")) != EOF) {
		switch(c) {
		case 'c':
			maxCount = atoi(optarg);
			break;
		case 'd':
			debug = 1;
			break;
		case 'n':
			neighbors = atoi(optarg);
			break;
		case 'v':
			verify = 1;
			break;
		case 'w':
			width = atof(optarg);
			break;
		case '?':
			errflag = 1;
			break;
		}
	}
	if(errflag || optind != argc - 3) {
		fprintf(stderr,
			"Usage: %s [options] dimension datafile treefile\n"
			"Options\n"
			"    -c count     set the count of data\n"
			"    -d           turn on the debug mode\n"
			"    -n neighbors set the number of neighbors\n"
			"    -v verify    verify data existence\n"
			"    -w width     set the width of the search range\n",
			argv[0]);
		return 1;
	}
        if(neighbors > 1 && width > 0) {
                fprintf(stderr, "%s: cannot set both `-n' and `-w' options.\n",
                        argv[0]);
                return 1;
        }

	dimension = atoi(argv[optind]);
	dataFileName = argv[optind + 1];
	treeFileName = argv[optind + 2];

	if(width == 0)
		searchNeighbors(dimension, dataFileName, treeFileName,
				maxCount, debug, verify, neighbors);
	else
		searchRange(dimension, dataFileName, treeFileName,
			    maxCount, debug, verify, width);

	return 0;
}

static void
searchNeighbors(int dimension,
		const char *dataFileName, const char *treeFileName,
		int maxCount, HnBool debug, HnBool verify, int neighbors)
{
	HnSRTreeFile treeFile;
	HnPointArray points;
	HnDataArray values;
	HnPoint target;
	RecordPoint recordPoint;
	RecordData recordData;
	FILE *fp;
	int i, count, hitCount;
	char buffer[BUFFER_SIZE];

	/* for verify */
	HnPoint point;
	HnData data;
	HnBool found = HnFALSE;

	treeFile = new_HnSRTreeFile(treeFileName, "r");

	if(treeFile == HnSRTreeFile::null) {
		perror(treeFileName);
		exit(1);
	}

	if(debug)
		treeFile.setDebug(HnTRUE);

	if(treeFile.getDimension() != dimension) {
		fprintf(stderr, "Mismatch in dimensions. "
			"The dimension of the tree is %d.\n",
			treeFile.getDimension());
		exit(1);
	}

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

		/* make target */
		target = new_HnPoint(dimension);
		for(i=0; i<dimension; i++) {
			target.setCoord(recordPoint.coords[i], i);
		}

		if(verify) {
			point = new_HnPoint(dimension);
			for(i=0; i<dimension; i++)
				point.setCoord(recordPoint.coords[i], i);
			data = new_HnData(&recordData, sizeof(recordData));
			found = HnFALSE;
		}

		/* search data */
		printf("Searching %s\n", (char *)pointToString(recordPoint));

		treeFile.getNeighbors(target, neighbors, &points, &values);

		hitCount = points.length();

		for(i=0; i<hitCount; i++) {
			char *fname = ((RecordData *)values[i].chars())->fname;

			printf("%s: %g\n", fname,
			       points[i].getDistance(target));

			if(verify) {
				if(point.equals(points[i]) &&
				   data.equals(values[i]))
					found = HnTRUE;
			}
		}
		printf("\n");

		if(verify) {
			if(!found) {
				fprintf(stderr, "data is not found.\n");
				exit(1);
			}
		}

		count ++;
	}

	fclose(fp);
	treeFile.close();
}

static void
searchRange(int dimension, const char *dataFileName, const char *treeFileName,
	    int maxCount, HnBool debug, HnBool verify, double width)
{
	HnSRTreeFile treeFile;
	HnRect rect;
	HnPoint key;
	HnData value;
	RecordPoint recordPoint;
	RecordData recordData;
	FILE *fp;
	int i, count;
	char buffer[BUFFER_SIZE];

	/* for verify */
	HnPoint point;
	HnData data;
	HnBool found = HnFALSE;

	treeFile = new_HnSRTreeFile(treeFileName, "r");

	if(treeFile == HnSRTreeFile::null) {
		perror(treeFileName);
		exit(1);
	}

	if(debug)
		treeFile.setDebug(HnTRUE);

	if(treeFile.getDimension() != dimension) {
		fprintf(stderr, "Mismatch in dimensions. "
			"The dimension of the tree is %d.\n",
			treeFile.getDimension());
		exit(1);
	}

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

		/* extend the rectangle by the given width */
		rect = new_HnRect(dimension);
		for(i=0; i<dimension; i++) {
			double x = recordPoint.coords[i];
			rect.setRange(x - width / 2, HnRange::INCLUSIVE,
				      x + width / 2, HnRange::INCLUSIVE,
				      i);
		}

		if(verify) {
			point = new_HnPoint(dimension);
			for(i=0; i<dimension; i++)
				point.setCoord(recordPoint.coords[i], i);
			data = new_HnData(&recordData, sizeof(recordData));
			found = HnFALSE;
		}

		/* search data */
		printf("Searching %s\n", (char *)pointToString(recordPoint));

		treeFile.getFirst(rect, &key, &value);

		while(key.isValid()) {
			char *fname = ((RecordData *)value.chars())->fname;

			printf("%s\n", fname);

			if(verify) {
				if(point.equals(key) && data.equals(value))
					found = HnTRUE;
			}

			treeFile.getNext(&key, &value);
		}

		printf("\n");

		if(verify) {
			if(!found) {
				fprintf(stderr, "data is not found.\n");
				exit(1);
			}
		}

		count ++;
	}

	fclose(fp);
	treeFile.close();
}

