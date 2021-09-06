/*
 * record.hh
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
 * $Id: record.hh,v 1.2 1997/06/02 09:35:51 katayama Exp $
 */

#ifndef _record_hh
#define _record_hh

#include "HnBool.hh"

#define RecordMaxDimension	256

typedef struct {
	int dimension;
	double coords[RecordMaxDimension];
} RecordPoint;

#define STRSIZ		256
#define BUFFER_SIZE	4096

typedef struct {
	char fname[STRSIZ];
	char data[STRSIZ];
} RecordData;

void
getRecord(const float *x, int id, int dimension,
          RecordPoint *point_return, RecordData *data_return);
void getRecord(const char *buffer, int dimension,
	       RecordPoint *point_return, RecordData *data_return);
HnBool includesNANorINF(const RecordPoint &point);
const char *pointToString(const RecordPoint &point);

#endif /* _record_hh */
