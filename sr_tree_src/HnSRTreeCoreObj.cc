/*
 * HnSRTreeCoreObj.cc
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
 * 08/24/96 katayama@rd.nacsis.ac.jp
 * $Id: HnSRTreeCoreObj.cc,v 1.2 1997/06/02 09:35:37 katayama Exp $
 */

#include "HnSRTreeCore.hh"
#include "HnSRTreeCoreObj.hh"

HnString
HnSRTreeCoreObj::toString(void) const
{
	HnString string;
	char buffer[256];

	string = "[ center = ";
	string += center.toString();

	sprintf(buffer, ", radius = %g", radius);
	string += buffer;

	sprintf(buffer, ", weight = %d", weight);
	string += buffer;

	string += " ]";

	return string;
}
