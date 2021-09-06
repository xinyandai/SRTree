/*
 * HnPointer.cc
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
 * $Id: HnPointer.cc,v 1.2 1997/06/02 09:35:21 katayama Exp $
 */

#include "HnObject.hh"
#include "HnPointer.hh"
#include "HnString.hh"
#include <stdint.h>

const HnPointer HnPointer::null;

HnPointer &
HnPointer::assign(HnObject *ptr, HnReference type)
{
	if(ptr != NULL) {
		if(ptr->magic != HnObject::MAGIC || ptr->refCount < 0)
			HnAbort("invalid object is assigned to the pointer.");
	}
	  
	if(reference == External && object != NULL)
		HnUnref(object);

	reference = type;
	object = ptr;

	if(reference == External && object != NULL)
		HnRef(object);

	return *this;
}

HnObject *
HnPointer::getObject(void) const
{
	if(object == NULL)
		HnAbort("object is not assigned.");

	if(object->magic != HnObject::MAGIC || object->refCount <= 0)
		HnAbort("assigned object is invalid.");

	return object;
}

HnString
HnPointer::toString(void) const
{
	char buffer[64];

	if(isInvalid())
		sprintf(buffer, "HnPointer::null");
	else
		sprintf(buffer, "HnPointer@%08lX", (uintptr_t)getObject());

	return buffer;
}
