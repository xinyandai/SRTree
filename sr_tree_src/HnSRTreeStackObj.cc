/*
 * HnSRTreeStackObj.cc
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
 * 03/21/96 katayama@rd.nacsis.ac.jp
 * $Id: HnSRTreeStackObj.cc,v 1.2 1997/06/02 09:35:45 katayama Exp $
 */

#include "HnSRTreeStack.hh"
#include "HnSRTreeStackObj.hh"

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
HnSRTreeStackObj::push(const HnSRTreeNode &node, int cursor) {
	if(this->leaf.isValid())
		HnAbort("cannot push a node onto a leaf.");

	nodes.append(node);
	cursors = (int *)xrealloc(cursors, sizeof(int) * getDepth());
	cursors[getDepth() - 1] = cursor;
}

void
HnSRTreeStackObj::push(const HnSRTreeLeaf &leaf, int cursor) {
	if(this->leaf.isValid())
		HnAbort("cannot push a leaf onto a leaf.");

	this->leaf = leaf;
	cursors = (int *)xrealloc(cursors, sizeof(int) * getDepth());
	cursors[getDepth() - 1] = cursor;
}

void
HnSRTreeStackObj::pop(void) {
	if(leaf.isValid())
		leaf = HnSRTreeLeaf::null;
	else
		nodes.remove(nodes.length() - 1);
}

void
HnSRTreeStackObj::advance(void)
{
	if(leaf.isValid()) {
		if(cursors[getDepth() - 1] == leaf.getCount() - 1)
			HnAbort("cursor is on the tail");
		cursors[getDepth() - 1] ++;
	}
	else {
		HnSRTreeNode node = nodes[nodes.length() - 1];

		if(cursors[getDepth() - 1] == node.getCount() - 1)
			HnAbort("cursor is on the tail");
		cursors[getDepth() - 1] ++;
	}
}
			
int
HnSRTreeStackObj::getDepth(void) const {
	if(leaf.isValid())
		return nodes.length() + 1;
	else
		return nodes.length();
}

int
HnSRTreeStackObj::getCursor(void) const {
	return cursors[getDepth() - 1];
}

HnBool
HnSRTreeStackObj::hasMore(void) const {
	if(leaf.isValid()) {
		if(cursors[getDepth() - 1] == leaf.getCount() - 1)
			return HnFALSE;
		else
			return HnTRUE;
	}
	else {
		HnSRTreeNode node = nodes[nodes.length() - 1];

		if(cursors[getDepth() - 1] == node.getCount() - 1)
			return HnFALSE;
		else
			return HnTRUE;
	}
}

HnBool
HnSRTreeStackObj::isTopNode(void) const {
	return leaf.isInvalid();
}

HnBool
HnSRTreeStackObj::isTopLeaf(void) const {
	return leaf.isValid();
}

HnSRTreeNode
HnSRTreeStackObj::getTopNode(void) const {
	if(leaf.isValid())
		HnAbort("the top element is not a node.");

	return nodes[nodes.length() - 1];
}

HnSRTreeLeaf
HnSRTreeStackObj::getTopLeaf(void) const {
	if(leaf.isInvalid())
		HnAbort("the top element is not a leaf.");

	return leaf;
}
