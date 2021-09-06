/*
 * HnObject.cc
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
 * $Id: HnObject.cc,v 1.4 1997/06/02 09:35:20 katayama Exp $
 */

#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include "config.hh"
#include "HnObject.hh"

HnObject *HnObject::busyObject = NULL;
int HnObject::numBusyObjects = 0;

HnObject *HnObject::freeObject = NULL;
int HnObject::numFreeObjects = 0;

int HnObject::MAGIC = 0x05121010;

void
HnObject::setBusy(void)
{
	prev = busyObject;
	next = NULL;
	busyObject = this;

	if(prev != NULL)
		prev->next = this;

	numBusyObjects ++;
}

void
HnObject::unsetBusy(void) {
	if(prev != NULL)
		prev->next = next;
	if(next != NULL)
		next->prev = prev;
	else
		busyObject = prev;

	numBusyObjects --;
}

void
HnObject::setFree(void) {
	prev = freeObject;
	next = NULL;
	freeObject = this;

	if(prev != NULL)
		prev->next = this;

	numFreeObjects ++;
}

void
HnObject::unsetFree(void) {
	if(prev != NULL)
		prev->next = next;
	if(next != NULL)
		next->prev = prev;
	else
		freeObject = prev;

	numFreeObjects --;
}

void
HnRef(HnObject *object)
{
	if(object->refCount == 0) {
		object->unsetFree();
		object->setBusy();
	}
	object->refCount ++;
}

void
HnUnref(HnObject *object)
{
	object->refCount --;
	if(object->refCount == 0) {
		object->unsetBusy();
		object->setFree();
		delete object;
	}
}

/*
 * error
 */

#define BufferSize	4096

#ifndef HAVE_STRERROR
#define strerror        mystrerror

static const char *
strerror(int errnum) {
        extern int sys_nerr;
        extern const char * const sys_errlist[];

        if(errnum >= 0 && errnum < sys_nerr)
                return sys_errlist[errnum];
        else
                return NULL;
}
#endif

void
HnObject::setErrorMessage(const char *message)
{
	if(errmsg != NULL)
		free(errmsg);

	if(message != NULL) {
		if((errmsg = (char *)malloc(strlen(message) + 1)) == NULL)
			HnSysError("malloc");
		strcpy(errmsg, message);
	}
	else
		errmsg = NULL;
}

void
HnObject::setError(const char *format, ... )
{
	va_list ap;
	char buffer[BufferSize];

	va_start(ap, format);
	vsprintf(buffer, format, ap);
	va_end(ap);

	setErrorMessage(buffer);
}

void
HnObject::setSysError(const char *format, ... )
{
	va_list ap;
	char buffer[BufferSize], *bp;
	const char *message;

	bp = buffer;

	va_start(ap, format);
	vsprintf(bp, format, ap);
	va_end(ap);
	bp += strlen(bp);

	if((message = strerror(errno)) != NULL)
		sprintf(bp, ": %s.", message);

	setErrorMessage(buffer);
}
