/*
 * HnAbort.hh
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
 * $Id: HnAbort.hh,v 1.3 1997/06/02 09:35:17 katayama Exp $
 */

#ifndef _HnAbort_hh
#define _HnAbort_hh

extern void HnAbort(const char *format, ... )
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 6)
__attribute__ ((format (printf, 1, 2)))
__attribute__ ((noreturn))
#endif
;
extern void HnSysError(const char *format, ... )
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 6)
__attribute__ ((format (printf, 1, 2)))
__attribute__ ((noreturn))
#endif
;

extern void HnLog(const char *format, ... )
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 6)
__attribute__ ((format (printf, 1, 2)))
#endif
;

typedef void HnAbortProc(const char *errmsg);

extern void HnSetLogFile(const char *path);
extern void HnSetDefaultAbortProc(HnAbortProc *proc);
extern void HnAddAbortProc(HnAbortProc *proc);

#endif /* _HnAbort_hh */
