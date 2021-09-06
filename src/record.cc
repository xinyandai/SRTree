/*
 * record.cc
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
 * 03/28/96 katayama@rd.nacsis.ac.jp
 * $Id: record.cc,v 1.4 1997/06/02 09:35:51 katayama Exp $
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "config.hh"
#include "record.hh"

#ifdef HAVE_NAN_H
#include <nan.h>
#endif

void
getRecord(const float *xb, int id, int dimension,
          RecordPoint *point_return, RecordData *data_return)
{

  RecordPoint &point = *point_return;
  /* initialize record data */
  memset(data_return, 0, sizeof(RecordData));

  std::string id_str = std::to_string(id);
  strncpy(data_return->fname, id_str.c_str(), id_str.size());
  data_return->fname[id_str.size()] = 0;

  /* point */
  point.dimension = dimension;

  for(int i = 0; i < dimension; i++) {
    point.coords[i] = xb[i];
  }

  /* data */
  strcpy(data_return->data, id_str.c_str());
}

void
getRecord(const char *line, int dimension,
	  RecordPoint *point_return, RecordData *data_return)
{
	const char *head, *tail;
	char buffer[BUFFER_SIZE];
	int i, length;
	RecordPoint &point = *point_return;

	/* initialize record data */
	memset(data_return, 0, sizeof(RecordData));

	head = line;

	/* fname */
	if((tail = strchr(head, ':')) == NULL) {
		fprintf(stderr, "`:' is expected.\n");
		exit(1);
	}
	length = tail - head;
	strncpy(data_return->fname, head, length);
	data_return->fname[length] = 0;
	head = tail + 1;

	/* point */
	point.dimension = dimension;

	for(i=0; i<dimension; i++) {
		if((tail = strchr(head, ':')) == NULL) {
			fprintf(stderr, "`:' is expected.\n");
			exit(1);
		}
		length = tail - head;
		strncpy(buffer, head, length);
		buffer[length] = 0;
		
		point.coords[i] = atof(buffer);

		head = tail + 1;
	}

	/* data */
	strcpy(data_return->data, head);
}

HnBool
includesNANorINF(const RecordPoint &point)
{
	int i;

	for(i=0; i<point.dimension; i++) {
#ifdef IsNANorINF
		if(IsNANorINF(point.coords[i]))
#else
		if(!finite(point.coords[i]))
#endif
			return HnTRUE;
	}

	return HnFALSE;
}

const char *
pointToString(const RecordPoint &point)
{
	static char buffer[BUFFER_SIZE];
	int i;
	char *bp;

	bp = buffer;

	sprintf(bp, "[ ");
	bp += strlen(bp);

	for(i=0; i<point.dimension; i++) {
		if(i != 0) {
			sprintf(bp, ", ");
			bp += strlen(bp);
		}

		sprintf(bp, "%g", point.coords[i]);
		bp += strlen(bp);
	}

	sprintf(bp, " ]");
	bp += strlen(bp);

	return buffer;
}
