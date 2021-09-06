/*
 * HnStringObj.cc
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
 * $Id: HnStringObj.cc,v 1.2 1997/06/02 09:35:22 katayama Exp $
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HnString.hh"
#include "HnStringObj.hh"
#include "HnAbort.hh"

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

HnStringObj::HnStringObj(void)
{
	initialize();

	buffer = (char *)xrealloc(buffer, 1);
	len = 0;
	buffer[len] = 0;
}

HnStringObj::HnStringObj(const char *ptr, int n)
{
	initialize();

	buffer = (char *)xrealloc(buffer, n + 1);
	memcpy(buffer, ptr, n);
	len = n;
	buffer[len] = 0;
}

HnStringObj::HnStringObj(const HnString &string)
{
	const char *ptr = string.chars();
	int n = string.length();

	initialize();

	buffer = (char *)xrealloc(buffer, n + 1);
	memcpy(buffer, ptr, n);
	len = n;
	buffer[len] = 0;
}

HnStringObj::HnStringObj(const char *ptr1, int n1, const char *ptr2, int n2)
{
	initialize();

	buffer = (char *)xrealloc(buffer, n1 + n2 + 1);
	memcpy(buffer, ptr1, n1);
	memcpy(buffer + n1, ptr2, n2);
	len = n1 + n2;
	buffer[len] = 0;
}

HnStringObj::~HnStringObj(void)
{
	dispose();
}

HnString
HnStringObj::concat(const char *ptr, int n) const
{
	if(n == 0)
		return this;
	else
		return new HnStringObj(chars(), length(), ptr, n);
}

HnBool
HnStringObj::equals(const HnString &string) const
{
	if(len == string.length() && strcmp(buffer, string.chars()) == 0)
		return HnTRUE;
	else
		return HnFALSE;
}

HnBool
HnStringObj::startsWith(const char *prefix, int n) const
{
	if(n > len)
		return HnFALSE;
	else
		return (strncmp(chars(), prefix, n) == 0);
}

HnBool
HnStringObj::endsWith(const char *suffix, int n) const
{
	if(n > len)
		return HnFALSE;
	else
		return (strncmp(chars() + len - n, suffix, n) == 0);
}

int
HnStringObj::indexOf(int ch, int fromIndex) const
{
	for(int i=fromIndex; i<len; i++) {
		if(buffer[i] == ch)
			return i;
	}

	return -1;
}

int
HnStringObj::lastIndexOf(int ch, int fromIndex) const
{
	for(int i=fromIndex; i>=0; i--) {
		if(buffer[i] == ch)
			return i;
	}

	return -1;
}

HnString
HnStringObj::substring(int beginIndex, int endIndex) const
{
	/*
	 * NOTE:
	 *	`beginIndex' is inclusive, while `endIndex' is exclusive.
	 */

	if(beginIndex < 0 || endIndex > len)
		HnAbort("HnStringObj::substring: index is out of bounds.");

	return new_HnString(buffer + beginIndex, endIndex - beginIndex);
}

HnString
HnStringObj::trim(void) const
{
	int beginIndex, endIndex;

	beginIndex = 0;
	while(beginIndex < len && isspace(buffer[beginIndex]))
		beginIndex ++;

	endIndex = len;
	while(endIndex > 0 && isspace(buffer[endIndex - 1]))
		endIndex --;

	return substring(beginIndex, endIndex);
}

/*
 * Class Methods
 */

HnString
HnStringObj::getString(FILE *fp)
{
	HnString string = new_HnString();
	char buffer[4096];
	int length;

	while(fgets(buffer, sizeof(buffer), fp) != NULL) {
		length = strlen(buffer);
		if(buffer[length - 1] == '\n') {
			buffer[length - 1] = 0;
			string.append(buffer, length - 1);
			return string;
		}
		else
			string.append(buffer, length);
	}

	if(feof(fp) && string.length() == 0)
		return HnString::null;
	else
		return string;
}

HnStringArray
HnStringObj::sort(const HnStringArray &strings)
{
	HnStringArray array = new_HnStringArray(strings);
	int i, j, len = strings.length();

	for(i=0; i<len; i++) {
		for(j=i+1; j<len; j++) {
			if(strcmp(array[i], array[j]) > 0)
				array.swap(i, j);
		}
	}

	return array;
}

/*
 * pack and unpack
 */

/* 
 * NOTE:
 *	packString() converts an arbitrary character string into an
 *	escaped unique form. The conversion syntax is as follows:
 *
 *		invalid string	   -> 	%UNDEF
 *		empty string	   -> 	%EMPTY
 *
 *		`%' character	   ->	%%
 *		` ' character	   ->	+
 *		`{' character	   ->	%(
 *		`}' character	   ->	%(
 *
 *		control	characters ->	%[0-9A-F][0-9A-F]
 *		`+' character	   ->	%[0-9A-F][0-9A-F]
 *		`/', `=', `,' 	   ->	%[0-9A-F][0-9A-F]
 *		`[', `]', `~', `:' ->	%[0-9A-F][0-9A-F]
 */

static void
escapeString(const char *ptr, HnString *string_return)
{
#ifdef DEBUG
	struct timeval s, e;
	gettimeofday(&s, NULL);
#endif
	/*
	 * NOTE:
	 *	The variable `string_return' is only appended not cleared.
	 */

	int maxLength;
	char *bp;
	static char *buffer = NULL;

	/*
	 * special treatment for `%UNDEF' and '%EMPTY'
	 */

	if(ptr == NULL) {
		string_return->append("%UNDEF");
		return;
	}

	if(ptr[0] == 0) {
		string_return->append("%EMPTY");
		return;
	}

	/*
	 * allocate buffer
	 *
	 * NOTE:
	 *	The maxium length of the result is the three time length of
	 *	the given string.
	 */

	maxLength = strlen(ptr) * 3;

	if(buffer == NULL) {
		if((buffer = (char *)malloc(maxLength + 1)) == NULL)
			HnSysError("malloc");
	}
	else {
		if((buffer = (char *)realloc(buffer, maxLength + 1)) == NULL)
			HnSysError("realloc");
	}

	bp = buffer;

	while(*ptr != 0) {
		if(iscntrl((unsigned char)*ptr) ||
		   strchr("% {}+/=,[]~:", *ptr) != NULL) {
			switch(*ptr) {
			case '%':
				*bp++ = '%';
				*bp++ = '%';
				break;
			case ' ':
				*bp++ = '+';
				break;
			case '{':
				*bp++ = '%';
				*bp++ = '(';
				break;
			case '}':
				*bp++ = '%';
				*bp++ = ')';
				break;
			default:
				sprintf(bp, "%%%02X", *ptr);
				bp += 3;
				break;
			}
			ptr ++;
		}
		else
			*bp++ = *ptr++;
	}

	string_return->append(buffer, bp - buffer);
#ifdef DEBUG
	gettimeofday(&e, NULL);
	sumEscapeString += e.tv_sec * 1000 + e.tv_usec / 1000
		- s.tv_sec * 1000 - s.tv_usec / 1000;
#endif
}

static void
unescapeString(const char *ptr, int n, HnString *string_return)
{
#ifdef DEBUG
	struct timeval s, e;
	gettimeofday(&s, NULL);
#endif
	HnString &string = *string_return;
	const char *start = ptr;
	const char *end = ptr + n;
	char *bp;
	static char *buffer = NULL;

	/*
	 * special treatment for `%UNDEF' and `%EMPTY'
	 */

	if(strncmp(ptr, "%UNDEF", n) == 0) {
		string = HnString::null;
		return;
	}
	if(strncmp(ptr, "%EMPTY", n) == 0) {
		string = "";
		return;
	}

	/*
	 * allocate buffer
	 */

	if(buffer == NULL) {
		if((buffer = (char *)malloc(n + 1)) == NULL)
			HnSysError("malloc");
	}
	else {
		if((buffer = (char *)realloc(buffer, n + 1)) == NULL)
			HnSysError("realloc");
	}

	bp = buffer;
	
	while(ptr < end) {
		switch(*ptr) {
		case '+':
			*bp++ = ' ';
			ptr ++;
			break;
		case '%':
			ptr ++;
			if(*ptr == '%') {
				*bp++ = '%';
				ptr ++;
			}
			else if(*ptr == '(') {
				*bp++ = '{';
				ptr ++;
			}
			else if(*ptr == ')') {
				*bp++ = '}';
				ptr ++;
			}
			else if(isxdigit(ptr[0]) && isxdigit(ptr[1])) {
				int c;

				sscanf(ptr, "%2X", &c);
				*bp++ = c;
				ptr += 2;
			}
			else {
				HnString string =
					new_HnString(start, end - start);
				HnAbort("unescapeString: an unexpected "
					"character follows after `%%' "
					"at `%s'[%ld].",
					(char *)string, ptr - start);
			}
			break;
		default:
			*bp ++ = *ptr;
			ptr ++;
			break;
		}
	}

	string = new_HnString(buffer, bp - buffer);
#ifdef DEBUG
	gettimeofday(&e, NULL);
	sumUnescapeString += e.tv_sec * 1000 + e.tv_usec / 1000
		- s.tv_sec * 1000 - s.tv_usec / 1000;
#endif
}

void
HnStringObj::escape(const char *ptr, HnString *string_return)
{
	HnString &string = *string_return;

	string = new_HnString();
	escapeString(ptr, &string);
}

void
HnStringObj::unescape(const char *ptr, HnString *string_return)
{
	unescapeString(ptr, (ptr == NULL ? 0 : strlen(ptr)), string_return);
}

/*
 * pack and unpack character strings
 *
 * NOTE:
 *	The partial match for truncated array should be permitted, because
 *	HnEntityFile, HnConnectionFile, HnLinkFile, HnPropertyFile, and 
 *	HnReferenceFile depend on that property.
 *
 *	The conversion syntax is as follows:
 *		invalid array	->	%UNDEF
 *		empty array	->	%EMPTY
 *		normal array	->	{array[0]}{array[1]}...{array[n-1]}
 */

void
HnStringObj::pack(const HnStringArray &strings, HnString *string_return)
{
	HnString &string = *string_return;
	int i;

	/*
	 * special treatment for `%UNDEF' and '%EMPTY'
	 */

	if(strings.isInvalid()) {
		string = "%UNDEF";
		return;
	}

	if(strings.length() == 0) {
		string = "%EMPTY";
		return;
	}

	string = new_HnString();

	for(i=0; i<strings.length(); i++) {
		string += '{';
		escapeString(strings[i], &string);
		string += '}';
	}
}

void
HnStringObj::unpack(const char *chars, HnStringArray *strings_return)
{
	HnStringArray &strings = *strings_return;
	const char *head, *ptr;
	HnString string;

	/*
	 * special treatment for `%UNDEF' and `%EMPTY'
	 */

	if(strcmp(chars, "%UNDEF") == 0) {
		strings.invalidate();
		return;
	}
	if(strcmp(chars, "%EMPTY") == 0) {
		strings = new_HnStringArray();
		strings.clear();
		return;
	}

	strings = new_HnStringArray();
	strings.clear();
	ptr = chars;

	while(*ptr != 0) {
		if(*ptr != '{')
			HnAbort("HnStringObj::unpack: "
				"`{' is not found at `%s'[%ld].",
				chars, ptr - chars);
		head = ptr + 1;

		if((ptr = strchr(head, '}')) == NULL)
			HnAbort("HnStringObj::unpack: "
				"`}' is not found at `%s'[%ld].",
				chars, head - chars);

		unescapeString(head, ptr - head, &string);
		strings.append(string);

		ptr++;
	}
}

/*
 * variants of pack() and unpack()
 */

void
HnStringObj::pack(const char *array[], int length, HnString *string_return)
{
	HnStringArray strings;
	int i;

	if(array == NULL)
		strings.invalidate();
	else {
		strings = new_HnStringArray();
		strings.clear();
		for(i=0; i<length; i++)
			strings.append(array[i]);
	}

	HnStringObj::pack(strings, string_return);
}

void
HnStringObj::pack(const char *ptr1, HnString *string_return)
{
	const char *array[1];

	array[0] = ptr1;

	HnStringObj::pack(array, 1, string_return);
}

void
HnStringObj::unpack(const char *chars, HnString *string1_return)
{
	HnStringArray strings;

	HnStringObj::unpack(chars, &strings);

	if(string1_return != NULL)
		*string1_return = strings[0];
}

void
HnStringObj::pack(const char *ptr1, const char *ptr2, HnString *string_return)
{
	const char *array[2];

	array[0] = ptr1;
	array[1] = ptr2;

	HnStringObj::pack(array, 2, string_return);
}

void
HnStringObj::unpack(const char *chars,
		    HnString *string1_return, HnString *string2_return)
{
	HnStringArray strings;

	HnStringObj::unpack(chars, &strings);

	if(string1_return != NULL)
		*string1_return = strings[0];
	if(string2_return != NULL)
		*string2_return = strings[1];
}

void
HnStringObj::pack(const char *ptr1, const char *ptr2, const char *ptr3,
		  HnString *string_return)
{
	const char *array[3];

	array[0] = ptr1;
	array[1] = ptr2;
	array[2] = ptr3;

	HnStringObj::pack(array, 3, string_return);
}

void
HnStringObj::unpack(const char *chars,
		    HnString *string1_return, HnString *string2_return,
		    HnString *string3_return)
{
	HnStringArray strings;

	HnStringObj::unpack(chars, &strings);

	if(string1_return != NULL)
		*string1_return = strings[0];
	if(string2_return != NULL)
		*string2_return = strings[1];
	if(string3_return != NULL)
		*string3_return = strings[2];
}

void
HnStringObj::pack(const char *ptr1, const char *ptr2,
		  const char *ptr3, const char *ptr4, HnString *string_return)
{
	const char *array[4];

	array[0] = ptr1;
	array[1] = ptr2;
	array[2] = ptr3;
	array[3] = ptr4;

	HnStringObj::pack(array, 4, string_return);
}

void
HnStringObj::unpack(const char *chars,
		    HnString *string1_return, HnString *string2_return,
		    HnString *string3_return, HnString *string4_return)
{
	HnStringArray strings;

	HnStringObj::unpack(chars, &strings);

	if(string1_return != NULL)
		*string1_return = strings[0];
	if(string2_return != NULL)
		*string2_return = strings[1];
	if(string3_return != NULL)
		*string3_return = strings[2];
	if(string4_return != NULL)
		*string4_return = strings[3];
}
