/*
 * HnBool.hh
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
 * 10/15/96 katayama@rd.nacsis.ac.jp
 * $Id: HnBool.hh,v 1.3 1997/06/02 09:35:17 katayama Exp $
 */

#ifndef _HnBool_hh
#define _HnBool_hh

#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)
#define	HnTRUE	true
#define HnFALSE	false

typedef bool HnBool;
#else
#define HnTRUE	1
#define HnFALSE	0

typedef int HnBool;
#endif

#endif /* _HnBool_hh */
