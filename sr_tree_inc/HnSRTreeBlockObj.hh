/*
 * HnSRTreeBlockObj.hh
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
 * 08/23/96 katayama@rd.nacsis.ac.jp
 * $Id: HnSRTreeBlockObj.hh,v 1.2 1997/06/02 09:35:37 katayama Exp $
 */

#ifndef _HnSRTreeBlockObj_hh
#define _HnSRTreeBlockObj_Hh

#include <string.h>
#include "HnObject.hh"

/*{
#include <unistd.h>
}*/

class HnSRTreeBlockObj: public HnObject {
private:
	off_t offset;
	char *bytes;
	size_t size;

public:
/*{
	typedef enum { NODE, LEAF, FREE } Type;
}*/
	HnSRTreeBlockObj(off_t offset, const char *bytes, size_t size) {
		this->offset = offset;
		this->bytes = new char[size];
		memcpy(this->bytes, bytes, size);
		this->size = size;
	}
	~HnSRTreeBlockObj(void) {
		delete bytes;
	}

	/* attributes */
	off_t getOffset(void) const {
		return offset;
	}
	const char *getBytes(void) const {
		return bytes;
	}
	size_t getSize(void) const {
		return size;
	}

	/* utilities */
	HnSRTreeBlock::Type getType(void) const {
		HnSRTreeBlock::Type type;
		memcpy(&type, bytes, sizeof(type));
		return type;
	}
	HnBool isNode(void) const {
		return (getType() == HnSRTreeBlock::NODE);
	}
	HnBool isLeaf(void) const {
		return (getType() == HnSRTreeBlock::LEAF);
	}
	HnBool isFree(void) const {
		return (getType() == HnSRTreeBlock::FREE);
	}
};

#endif /* _HnSRTreeBlockObj_hh */
