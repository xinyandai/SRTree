/*
 * HnAbort.cc
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
 * 11/25/95 katayama@rd.nacsis.ac.jp
 * $Id: HnAbort.cc,v 1.4 1997/06/02 09:35:16 katayama Exp $
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "config.hh"
#include "HnAbort.hh"

#define BufferSize	4096

static HnAbortProc HnDefaultAbortProc;

/*
 * parameters
 */

static FILE *logfp = NULL;

static HnAbortProc *defaultAbortProc = HnDefaultAbortProc;

static HnAbortProc **procs = NULL;
static int num_procs = 0;

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
HnAbort(const char *format, ... )
{
	va_list ap;
	char buffer[BufferSize], *bp;
	int i;

	bp = buffer;
	sprintf(bp, "Error: ");
	bp += strlen(bp);

	va_start(ap, format);
	vsprintf(bp, format, ap);
	va_end(ap);
	bp += strlen(bp);

	if(logfp != NULL)
		fprintf(logfp, "%s\n", buffer);

	if(defaultAbortProc != NULL)
		defaultAbortProc(buffer);

	for(i=0; i<num_procs; i++)
		(procs[i])(buffer);

	abort();
}

void
HnSysError(const char *format, ... )
{
	va_list ap;
	char buffer[BufferSize], *bp;
	const char *errmsg;
	int i;

	bp = buffer;
	sprintf(bp, "Error: ");
	bp += strlen(bp);

	va_start(ap, format);
	vsprintf(bp, format, ap);
	va_end(ap);
	bp += strlen(bp);

	if((errmsg = strerror(errno)) != NULL)
		sprintf(bp, ": %s.", errmsg);

	if(logfp != NULL)
		fprintf(logfp, "%s\n", buffer);

	if(defaultAbortProc != NULL)
		defaultAbortProc(buffer);

	for(i=0; i<num_procs; i++)
		(procs[i])(buffer);

	abort();
}

void
HnLog(const char *format, ... )
{
	va_list ap;
	time_t t;
	static char when[256], buffer[1024];

	t = time(NULL);
	strftime(when, sizeof(when), "%D %H:%M:%S", localtime(&t));

	va_start(ap, format);
	vsprintf(buffer, format, ap);
	va_end(ap);

	if(logfp != NULL)
		fprintf(logfp, "(%s) %s\n", when, buffer);
	else
		fprintf(stderr, "(%s) %s\n", when, buffer);
}

static void
HnDefaultAbortProc(const char *errmsg)
{
	fprintf(stderr, "%s\n", errmsg);
}

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

void
HnSetLogFile(const char *path)
{
	FILE *fp;

	if(path == NULL) {
		if(logfp != NULL)
			fclose(logfp);

		logfp = NULL;
	}
	else {
		if((fp = fopen(path, "a")) == NULL)
			HnSysError("fopen(\"%s\", \"a\")", path);

		if(logfp != NULL)
			fclose(logfp);

		logfp = fp;
		setvbuf(logfp, NULL, _IOLBF, 0);
	}
}

void
HnSetDefaultAbortProc(HnAbortProc *proc)
{
	defaultAbortProc = proc;
}

void
HnAddAbortProc(HnAbortProc *proc)
{
	procs = (HnAbortProc **)
		xrealloc(procs, sizeof(HnAbortProc *) * (num_procs + 1));

	procs[num_procs] = proc;
	num_procs ++;
}
		
