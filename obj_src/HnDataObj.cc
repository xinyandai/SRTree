/*
 * HnDataObj.cc
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
 * 03/27/96 katayama@rd.nacsis.ac.jp
 * $Id: HnDataObj.cc,v 1.2 1997/06/02 09:35:18 katayama Exp $
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HnDataObj.hh"
#include "HnAbort.hh"

static void *
xrealloc(void *ptr, size_t size)
{
	if(ptr == NULL) {
		if((ptr = malloc(size)) == NULL)
			HnSysError("malloc");
	}
	else {
		if((ptr = realloc(ptr, size)) == NULL)
			HnSysError("realloc");
	}

	return ptr;
}

HnDataObj::HnDataObj(void)
{
	initialize();

	buffer = NULL;
	len = 0;
}

HnDataObj::HnDataObj(const void *ptr, int n)
{
	initialize();

	buffer = (char *)xrealloc(buffer, n);
	memcpy(buffer, ptr, n);
	len = n;
}

HnDataObj::HnDataObj(const void *ptr1, int n1, const void *ptr2, int n2)
{
	initialize();

	buffer = (char *)xrealloc(buffer, n1 + n2);
	memcpy(buffer, ptr1, n1);
	memcpy(buffer + n1, ptr2, n2);
	len = n1 + n2;
}

HnDataObj::~HnDataObj(void)
{
	dispose();
}

HnBool
HnDataObj::equals(const HnData &data) const
{
	if(len == data.length() && memcmp(buffer, data.chars(), len) == 0)
		return HnTRUE;
	else
		return HnFALSE;
}
