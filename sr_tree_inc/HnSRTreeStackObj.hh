/*
 * HnSRTreeStackObj.hh
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
 * $Id: HnSRTreeStackObj.hh,v 1.2 1997/06/02 09:35:46 katayama Exp $
 */

#ifndef _HnSRTreeStackObj_hh
#define _HnSRTreeStackObj_hh

#include "HnObject.hh"
/*{
#include "HnSRTreeNode.hh"
#include "HnSRTreeLeaf.hh"
}*/

#include <stdlib.h>

class HnSRTreeStackObj: public HnObject {
private:
	HnSRTreeNodeArray nodes;
	HnSRTreeLeaf leaf;
	int *cursors;

	void initialize(void) {
		nodes = new_HnSRTreeNodeArray();
		leaf = HnSRTreeLeaf::null;
		cursors = NULL;
	}
	void dispose(void) {
		free(cursors);
	}

public:
	HnSRTreeStackObj(void) {
		initialize();
	}
	~HnSRTreeStackObj(void) {
		dispose();
	}

	void push(const HnSRTreeNode &node, int cursor);
	void push(const HnSRTreeLeaf &leaf, int cursor);
	void push(const HnSRTreeNode &node) { push(node, 0); }
	void push(const HnSRTreeLeaf &leaf) { push(leaf, 0); }
	void pop(void);
	void advance(void);

	int getDepth(void) const;

	int getCursor(void) const;
	HnBool hasMore(void) const;

	HnBool isTopNode(void) const;
	HnBool isTopLeaf(void) const;

	HnSRTreeNode getTopNode(void) const;
	HnSRTreeLeaf getTopLeaf(void) const;
};

#endif /* _HnSRTreeStackObj_hh */
