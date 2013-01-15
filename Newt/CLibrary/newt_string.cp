// ==============================
// File:			Newt/CLibrary/newt_string.c
// Project:			Einstein
//
// Copyright 2012 by Newton Research Group and others
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#include <K/Defines/UByteSex.h>
#include "Newt/Common/Glue.h"
#include "newt_string.h"

void* newt_memcpy(void* s1, const void* s2, unsigned int n)
{
	char *a = (char *) s1;
	char *b = (char *) s2;
	char *buffer = (char *) malloc(n);
	char *x;
	unsigned int m = n;

	x = buffer;
	while (n > 0 && (unsigned int) b & 0x3) {
		*x = NewtReadByte(b);
		b++;
		x++;
		n--;
	}
	while (n > 3) {
		*(KUInt32 *) x = UByteSex::Swap(NewtReadWord(b));
		b += 4;
		x += 4;
		n -= 4;
	}
	while (n > 0) {
		*x = NewtReadByte(b);
		b++;
		x++;
		n--;
	}

	n = m;
	x = buffer;
	while (n > 0 && (unsigned int) a & 0x3) {
		NewtWriteByte(a, *x);
		a++;
		x++;
		n--;
	}
	while (n > 3) {
		NewtWriteWord(a, UByteSex::Swap(*(KUInt32 *) x));
		a += 4;
		x += 4;
		n -= 4;
	}
	while (n > 0) {
		NewtWriteByte(a, *x);
		a++;
		x++;
		n--;
	}
	free (buffer);

/*
	if (a >= b && a < b + n) {
		a += n - 1;
		b += n - 1;
		while (n > 0) {
			NewtWriteByte(a, NewtReadByte(b));
			a--;
			b--;
			n--;
		}
	} else {
		while (n > 0) {
			NewtWriteByte(a, NewtReadByte(b));
			a++;
			b++;
			n--;
		}
	}
*/
	return s1;
}

NEWT_INJECTION(0x00382440, "memcpy(void* s1, const void* s2, unsigned int n)") {
	NEWT_RETVAL newt_memcpy(NEWT_ARG0(void *), NEWT_ARG1(const void *), NEWT_ARG2(unsigned int));
	NEWT_RETURN;
}


