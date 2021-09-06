/*
 * HnObject.hh
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
 * $Id: HnObject.hh,v 1.4 1997/06/02 09:35:20 katayama Exp $
 */

#ifndef _HnObject_hh
#define _HnObject_hh

#include <stdio.h>
#include <stdlib.h>
#include "config.hh"
#include "HnBool.hh"
#include "HnAbort.hh"

#ifndef HAVE_MEMMOVE
extern "C" {
	extern void bcopy(void *s1, void *s2, int len);
};
#define memmove(dst, src, n)	bcopy(src, dst, n)
#endif

class HnPointer;

class HnObject {
    friend HnPointer;
private:
	/* class variables */
	static HnObject *busyObject;
	static int numBusyObjects;

	static HnObject *freeObject;
	static int numFreeObjects;

	static int MAGIC;

private:
	/* reference */
	int magic;
	int refCount;
	HnObject *prev;
	HnObject *next;

	void setBusy(void);
	void unsetBusy(void);
	void setFree(void);
	void unsetFree(void);

	/* error */
	char *errmsg;

	void setErrorMessage(const char *message);

public:
	HnObject(void) {
		/* reference */
		magic = MAGIC;
		refCount = 0;

		setFree();

		/* error */
		errmsg = NULL;
	}
	virtual ~HnObject(void) {
		/* reference */
		if(refCount != 0)
			HnAbort("destructor is invoked for a busy object.");

		unsetFree();

		magic = -1;
		refCount = -1;

		/* error */
		if(errmsg != NULL)
			free(errmsg);
	}

	static int countObjects(void) {
		return numBusyObjects + numFreeObjects;
	}
	static int countBusyObjects(void) {
		return numBusyObjects;
	}
	static int countFreeObjects(void) {
		return numFreeObjects;
	}
	static void deleteFreeObjects(void) {
		HnObject *object, *next;

		object = freeObject;
		while(object != NULL) {
			next = object->next;
			if(object->refCount == 0)
				delete object;
			object = next;
		}
	}

    friend void HnRef(HnObject *object);
    friend void HnUnref(HnObject *object);

	/* error */
	void setError(const char *format, ... );
	void setSysError(const char *format, ... );
	char *getErrorMessage() {
		return errmsg;
	}
	HnBool hasFailed() {
		return errmsg != NULL;
	}
};

#endif /* HnObject_hh */
