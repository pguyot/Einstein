// ==============================
// File:            Newt/CLibrary/newt_string.c
// Project:         Einstein
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

void* newt_memset(void* s, int c, unsigned int n)
{	
    char *a = (char *)s;
    
    while (n > 0 && (unsigned int) a & 0x3) {
        NewtWriteByte(a, c & 0xff);
        a++;
        n--;
    }
    while (n > 3) {
		KUInt32 lc = (c << 24) | (c << 16) | (c << 8) | c;
        NewtWriteWord(a, UByteSex::Swap(lc));
        a += 4;
        n -= 4;
    }
    while (n > 0) {
        NewtWriteByte(a, c & 0xff);
        a++;
        n--;
    }
	
	return s;
}

void newt_FillBytes(void* s, KSInt32 len, unsigned int pattern)
{
	newt_memset(s, pattern, len);
}

void newt_ZeroBytes(void* s, KSInt32 len)
{
	newt_memset(s, 0x00, len);
}

void newt_BlockMove(void* src, void* dst, KSInt32 len)
{
	newt_memcpy(dst, src, len);
}

KUInt32 newt_memcmp(void* ptr1, void* ptr2, KSInt32 byteCount)
{
	KUInt32 n = byteCount;
	char* a = (char*)ptr1;
	char* b = (char*)ptr2;
	KUInt32 retVal = 0;
	
	while ( n > 0 )
	{
		char aval = NewtReadByte(a);
		char bval = NewtReadByte(b);

		if ( aval != bval )
		{
			retVal = 1;
			break;
		}
			
		--n;
		++a;
		++b;
	}
	
	return retVal;
}


NEWT_INJECTION(0x00358074, "memcmp(const void* ptr1, const void* ptr2, Size byteCount)") {
    NEWT_RETVAL newt_memcmp(NEWT_ARG0(void *), NEWT_ARG1(void *), NEWT_ARG2(KUInt32));
    NEWT_RETURN;
}

NEWT_INJECTION(0x00382440, "memcpy(void* dst, const void* src, unsigned int n)") {
    NEWT_RETVAL newt_memcpy(NEWT_ARG0(void *), NEWT_ARG1(const void *), NEWT_ARG2(unsigned int));
    NEWT_RETURN;
}

NEWT_INJECTION(0x003828C8, "memset(void *b, int c, size_t len)") {
    NEWT_RETVAL newt_memset(NEWT_ARG0(void *), NEWT_ARG1(int), NEWT_ARG2(unsigned int));
    NEWT_RETURN;
}

NEWT_INJECTION(0x00311104, "BlockMove(const void *src, void *dst, Size bytes)") {
    newt_BlockMove(NEWT_ARG0(void *), NEWT_ARG1(void *), NEWT_ARG2(KUInt32));
    NEWT_RETURN;
}

NEWT_INJECTION(0x00311488, "FillBytes(void *b, Size len, UChar pattern)") {
    newt_FillBytes(NEWT_ARG0(void *), NEWT_ARG1(KUInt32), NEWT_ARG2(unsigned int));
    NEWT_RETURN;
}

NEWT_INJECTION(0x00018CA4, "LoadFromPhysAddress(PAddr)") {
    NEWT_RETVAL NewtReadWordPhysical(NEWT_ARG0(KUInt32));
    NEWT_RETURN;
}

NEWT_INJECTION(0x00018CE0, "StoreToPhysAddress(PAddr, value)") {
   	NewtWriteWordPhysical(NEWT_ARG0(KUInt32), NEWT_ARG1(KUInt32));
    NEWT_RETURN;
}

NEWT_INJECTION(0x0031139C, "ZeroBytes(void *b, Size len)") {
    newt_ZeroBytes(NEWT_ARG0(void *), NEWT_ARG1(KUInt32));
    NEWT_RETURN;
}
