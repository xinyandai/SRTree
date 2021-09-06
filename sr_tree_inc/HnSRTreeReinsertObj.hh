/*
 * HnSRTreeReinsertObj.hh
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
 * $Id: HnSRTreeReinsertObj.hh,v 1.2 1997/06/02 09:35:44 katayama Exp $
 */

#ifndef _HnSRTreeReinsertObj_hh
#define _HnSRTreeReinsertObj_hh

#include "HnObject.hh"

/*{
#include <unistd.h>
#include "HnPoint.hh"
#include "HnData.hh"
}*/

class HnSRTreeReinsertObj: public HnObject {
private:
	HnSRTreeReinsert::Type type;

	/* for points */
	HnPoint point;
	HnData data;

	/* for blocks */
	off_t offset;
	int level;
public:
/*{
	enum Type { POINT, BLOCK };
}*/
	HnSRTreeReinsertObj(const HnPoint &point, const HnData &data) {
		this->type = HnSRTreeReinsert::POINT;
		this->point = point;
		this->data = data;
	}
	HnSRTreeReinsertObj(off_t offset, int level) {
		this->type = HnSRTreeReinsert::BLOCK;
		this->offset = offset;
		this->level = level;
	}
	HnSRTreeReinsert::Type getType(void) const {
		return type;
	}
	HnPoint getPoint(void) const {
		if(type != HnSRTreeReinsert::POINT)
			HnAbort("not a point.");
		return point;
	}
	HnData getData(void) const {
		if(type != HnSRTreeReinsert::POINT)
			HnAbort("not a point.");
		return data;
	}
	off_t getOffset(void) const {
		if(type != HnSRTreeReinsert::BLOCK)
			HnAbort("not a block.");
		return offset;
	}
	int getLevel(void) const {
		if(type != HnSRTreeReinsert::BLOCK)
			HnAbort("not a block.");
		return level;
	}
};

#endif /* _HnSRTreeReinsertObj_hh */
