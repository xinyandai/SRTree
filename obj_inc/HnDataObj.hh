/*
 * HnDataObj.hh
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
 * $Id: HnDataObj.hh,v 1.2 1997/06/02 09:35:18 katayama Exp $
 */

#ifndef _HnDataObj_hh
#define _HnDataObj_hh

#include <stdlib.h>
#include "HnObject.hh"
#include "HnData.hh"

class HnDataObj: public HnObject {
private:
	char *buffer;
        int len;

	void initialize(void) {
		buffer = NULL;
		len = 0;
	}
	void dispose(void) {
		if(buffer != NULL)
			free(buffer);

		buffer = NULL;
	}

public:
	/* constructor and destructor */
	HnDataObj(void);
	HnDataObj(const void *ptr, int n);
	HnDataObj(const void *ptr1, int n1, const void *ptr2, int n2);
	~HnDataObj(void);

	/* chars and length */
	const char *chars(void) const { return buffer; };
	const char &get(int i) const { return buffer[i]; };
	int length(void) const { return len; };

	/* utilities */
	HnBool equals(const HnData &data) const;
};

#endif /* _HnDataObj_hh */
