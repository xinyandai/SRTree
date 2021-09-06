/*
 * HnSRTreeOffsetObj.hh
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
 * 08/26/96 katayama@rd.nacsis.ac.jp
 * $Id: HnSRTreeOffsetObj.hh,v 1.2 1997/06/02 09:35:43 katayama Exp $
 */

#ifndef _HnSRTreeOffsetObj_hh
#define _HnSRTreeOffsetObj_hh

#include "HnObject.hh"

/*{
#include <sys/types.h>
#include "HnString.hh"
}*/

class HnSRTreeOffsetObj: public HnObject {
private:
	off_t value;
public:
/*{
	HnSRTreeOffset(off_t value) {
		assign(new_HnSRTreeOffset(value));
	}
}*/
	HnSRTreeOffsetObj(off_t value) {
		this->value = value;
	}
	off_t getValue(void) const {
		return value;
	}
	HnBool equals(const HnSRTreeOffset &ptr) const {
		if(value == ptr.getValue())
			return HnTRUE;
		else
			return HnFALSE;
	}
	HnString toString(void) const {
		char buffer[256];

		sprintf(buffer, "0x%08X", (int)value);
		return buffer;
	}
	operator off_t(void) const {
		return getValue();
	}
};

#endif /* _HnSRTreeOffsetObj_hh */
