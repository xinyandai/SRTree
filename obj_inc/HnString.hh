/*
 * HnString.hh
 *
 * DO NOT EDIT THIS FILE!
 *
 * This file is automatically generated by obj2ptr.
 * 1997/06/02 21:18:59
 */

#ifndef _HnString_hh
#define _HnString_hh

#include "HnPointer.hh"

#include <string.h>
class HnStringArray;
class HnString;

class HnStringObj;
class HnStringArrayObj;

HnString
new_HnString(void);
HnString
new_HnString(const HnString &string);
HnString
new_HnString(const char *ptr, int n);
HnStringArray
new_HnStringArray(void);
HnStringArray
new_HnStringArray(const HnStringArray &ptr);
/*
 * HnString
 */


class HnString: public HnPointer {
private:
    friend class HnStringObj;
	HnString(const HnStringObj *ptr) {
		HnPointer::assign((HnObject *)ptr);
	}
	HnStringObj *getObject(void) const {
		return (HnStringObj *)HnPointer::getObject();
	}
public:
	static const HnString null;
	HnString(void) {}
public:
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

	/* constructor and destructor */
    friend HnString
	new_HnString(void);
    friend HnString
	new_HnString(const char *ptr, int n);
    friend HnString
	new_HnString(const HnString &string);

	/* chars and length */
	const char *chars(void) const;
	const char &get(int i) const;
	const char &operator[](int i) const;
	int length(void) const;

	/* concat */
	/*
	 * NOTE:
	 *	The current implementation copies the entire string on concat.
	 */
	HnString concat(const char *ptr, int n) const;
	HnString concat(const char *ptr) const;
	HnString concat(const HnString &string) const;

	/* utilities */
	HnBool equals(const HnString &string) const;

	HnBool startsWith(const char *prefix, int n) const;
	HnBool startsWith(const char *prefix) const;
	HnBool startsWith(const HnString &string) const;
	HnBool endsWith(const char *suffix, int n) const;
	HnBool endsWith(const char *suffix) const;
	HnBool endsWith(const HnString &string) const;

	int indexOf(int ch, int fromIndex) const;
	int indexOf(int ch) const;
	int lastIndexOf(int ch, int fromIndex) const;
	int lastIndexOf(int ch) const;

	HnString substring(int beginIndex, int endIndex) const;
	HnString substring(int beginIndex) const;
	HnString trim(void) const;

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

	HnString toString(void) const {
		if(isInvalid())
			return "HnString::null";
    		else
			return *this;
	}
};

/*
 * HnStringArray
 */

class HnStringArray: public HnPointer {
private:
    friend class HnStringArrayObj;
	HnStringArray(const HnStringArrayObj *ptr) {
		HnPointer::assign((HnObject *)ptr);
	}
	HnStringArrayObj *getObject(void) const {
		return (HnStringArrayObj *)HnPointer::getObject();
	}
public:
	static const HnStringArray null;
	HnStringArray(void) {}
public:
	/* constructors */
    friend HnStringArray
	new_HnStringArray(void);
    friend HnStringArray
	new_HnStringArray(const HnStringArray &ptr);

	/* clear, append, insert, and remove */
	void clear(void);
	void append(const HnString &ptr);
	void append(const HnStringArray &ptr);
	void insert(const HnString &ptr, int index);
	HnString remove(int index);
	void set(const HnString &ptr, int index);
	void swap(int i, int j);

	/* length and get */
	int length(void) const;
	HnString &get(int i) const;
	HnString &operator[](int i) const { return get(i); }

	/* utilities */
	void push(const HnString &ptr) {
		append(ptr);
	}
	HnString pop(void) {
		return remove(length() - 1);
	}
	HnBool equals(const HnStringArray &ptr) const;
	int indexOf(const HnString &ptr, int fromIndex) const;
	int indexOf(const HnString &ptr) const  {
	    return indexOf(ptr, 0);
	}
	HnString toString(void) const;
};

#endif /* _HnString_hh */