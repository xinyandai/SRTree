/*
 * HnStringObj.hh
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
 * 11/20/95 katayama@rd.nacsis.ac.jp
 * $Id: HnStringObj.hh,v 1.2 1997/06/02 09:35:22 katayama Exp $
 */

#ifndef _HnStringObj_hh
#define _HnStringObj_hh

#include <stdlib.h>
#include "HnObject.hh"

/*{
#include <string.h>
class HnStringArray;
class HnString;
}*/

class HnStringObj: public HnObject {
private:
	char *buffer;
        int len;

	void initialize(void) {
		buffer = NULL;
		len = 0;
	}
	void dispose(void) {
		if(buffer != NULL)
			free(buffer);

		buffer = NULL;
	}

private:
	HnStringObj(const char *ptr1, int n1, const char *ptr2, int n2);

public:
/*{
	HnString(const HnString &string): HnPointer(string) {};
	HnString(const char *ptr) {
		if(ptr == NULL)
			assign(HnString::null);
		else
			assign(new_HnString(ptr, strlen(ptr)));
	}
	operator char *(void) const {
		if(isInvalid())
			return NULL;
		else	
			return (char *)chars();
	}
	HnString operator + (const char *ptr) const {
		return concat(ptr);
	}
	HnString &operator += (const char *ptr) {
		HnString string = concat(ptr);
		assign(string);
		return *this;
	}
	HnString &operator += (const HnString &value) {
		HnString string = concat(value.chars(), value.length());
		assign(string);
		return *this;
	}
	HnString &operator += (char c) {
		HnString string = concat(&c, 1);
		assign(string);
		return *this;
	}
        void append(const char *ptr, int n) {
		HnString string = concat(ptr, n);
		assign(string);
	}
        void append(const char *ptr) {
		append(ptr, ((ptr == NULL) ? 0 : strlen(ptr)));
	}
        void append(const HnString &string) {
                append(string.chars(), string.length());
	}

	HnBool equals(const char *ptr, int n) {
		if(getObject() == NULL) {
			if(ptr == NULL)
		       		return HnTRUE;
			else
				return HnFALSE;
		}
		else {
			if(ptr == NULL)
				return HnFALSE;
			else
				return (strncmp(chars(), ptr, n) == 0);
		}
	}
	HnBool equals(const char *ptr) {
		return equals(ptr, ((ptr == NULL) ? 0 : strlen(ptr)));
	}
}*/
public:
	/* constructor and destructor */
	HnStringObj(void);
	HnStringObj(const char *ptr, int n);
	HnStringObj(const HnString &string);
	~HnStringObj(void);

	/* chars and length */
	const char *chars(void) const { return buffer; };
	const char &get(int i) const { return buffer[i]; };
	const char &operator[](int i) const { return get(i); }
	int length(void) const { return len; };

	/* concat */
	/*
	 * NOTE:
	 *	The current implementation copies the entire string on concat.
	 */
	HnString concat(const char *ptr, int n) const;
	HnString concat(const char *ptr) const {
		return concat(ptr, ((ptr == NULL) ? 0 : strlen(ptr)));
	}
	HnString concat(const HnString &string) const {
		return concat(string.chars(), string.length());
	}

	/* utilities */
	HnBool equals(const HnString &string) const;

	HnBool startsWith(const char *prefix, int n) const;
	HnBool startsWith(const char *prefix) const {
		return startsWith(prefix, strlen(prefix));
	}
	HnBool startsWith(const HnString &string) const {
		return startsWith(string.chars(), string.length());
	}
	HnBool endsWith(const char *suffix, int n) const;
	HnBool endsWith(const char *suffix) const {
		return endsWith(suffix, strlen(suffix));
	}
	HnBool endsWith(const HnString &string) const {
		return endsWith(string.chars(), string.length());
	}

	int indexOf(int ch, int fromIndex) const;
	int indexOf(int ch) const {
		return indexOf(ch, 0);
	}
	int lastIndexOf(int ch, int fromIndex) const;
	int lastIndexOf(int ch) const {
		return lastIndexOf(ch, length() - 1);
	}

	HnString substring(int beginIndex, int endIndex) const;
	HnString substring(int beginIndex) const {
		return substring(beginIndex, length());
	}
	HnString trim(void) const;

public:
	static HnString getString(FILE *fp);
	static HnStringArray sort(const HnStringArray &strings);

	/*
	 * pack and unpack
	 */

	static void escape(const char *ptr, HnString *string_return);
	static void unescape(const char *ptr, HnString *string_return);
	static void pack(const HnStringArray &strings,
			 HnString *string_return);
	static void unpack(const char *ptr, HnStringArray *strings_return);

	/*
	 * variants of pack() and unpack()
	 */

	static void pack(const char *array[], int length,
			 HnString *string_return);

	static void pack(const char *ptr1, HnString *string_return);
	static void unpack(const char *ptr, HnString *string1_return);

	static void pack(const char *ptr1, const char *ptr2,
			 HnString *string_return);
	static void unpack(const char *ptr,
			   HnString *string1_return, HnString *string2_return);

	static void pack(const char *ptr1, const char *ptr2, const char *ptr3,
			 HnString *string_return);
	static void unpack(const char *ptr,
			   HnString *string1_return, HnString *string2_return,
			   HnString *string3_return);

	static void pack(const char *ptr1, const char *ptr2, const char *ptr3,
			 const char *ptr4, HnString *string_return);
	static void unpack(const char *ptr,
			   HnString *string1_return, HnString *string2_return,
			   HnString *string3_return, HnString *string4_return);
};

#endif /* _HnStringObj_hh */
