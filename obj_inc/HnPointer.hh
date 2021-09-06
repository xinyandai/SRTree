/*
 * HnPointer.hh
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
 * 11/23/95 katayama@rd.nacsis.ac.jp
 * $Id: HnPointer.hh,v 1.2 1997/06/02 09:35:21 katayama Exp $
 */

#ifndef _HnPointer_hh
#define _HnPointer_hh

#include <stdio.h>
#include "HnBool.hh"

class HnObject;
class HnString;

void HnRef(HnObject *object);
void HnUnref(HnObject *object);

class HnPointer {
public:
	static const HnPointer null;
	enum HnReference { None, External, Internal };

private:
	HnReference reference;
	HnObject *object;

private:
	void initialize(void) {
		reference = None;
		object = NULL;
	}
	void dispose(void) {
		if(reference == External && object != NULL)
			HnUnref(object);
		reference = None;
		object = NULL;
	}

protected:
	HnPointer &assign(HnObject *ptr, HnReference type = External);
	HnPointer &assign(const HnPointer &pointer,
			  HnReference type = External) {
		assign(pointer.object, type);
		return *this;
	}
	HnObject *getObject(void) const;

public:
	HnPointer(void) {
		initialize();
	}
	virtual ~HnPointer(void) {
		dispose();
	}

	int isValid(void) const { return (object != NULL); }
	int isInvalid(void) const { return (object == NULL); }
	HnPointer &invalidate(void) { return assign(NULL, External); }

	HnPointer &operator=(const HnPointer &pointer) {
		assign(pointer.object, pointer.reference);
		return *this;
	}
	/* compares the equivalence of pointers */
	HnBool operator==(const HnPointer &pointer) const {
		if(isInvalid()) {
			if(pointer.isInvalid())
				return HnTRUE;
			else
				return HnFALSE;
		}
		else {
			if(pointer.isInvalid())
				return HnFALSE;
			else
				return (getObject() == pointer.getObject());
		}
	}
	HnBool operator!=(const HnPointer &pointer) const {
		return !operator==(pointer);
	}
	HnPointer(const HnPointer &pointer) {
		initialize();
		assign(pointer.object, pointer.reference);
	}

	/* compares the equivalence of objects */
	HnBool equals(const HnPointer &ptr) const {
		if(isInvalid()) {
			if(ptr.isInvalid())
				return HnTRUE;
			else
				return HnFALSE;
		}
		else {
			if(ptr.isInvalid())
				return HnFALSE;
			else
				return (getObject() == ptr.getObject());
		}
	}
	virtual HnString toString(void) const;
};

#endif /* _HnPointer_hh */
