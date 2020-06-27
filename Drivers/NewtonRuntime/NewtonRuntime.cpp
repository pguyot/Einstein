// =========== Header ===========
// File:                NewtonRuntime.cpp
// Project:             DCL / ToolchainUtils
// Written by:          Paul Guyot (pguyot@kallisys.net)
//
// Created on:          25/05/2020
// Internal version:    1
//
// Copyright:           © 2020 by Paul Guyot.
//                      All rights reserved worldwide.
// ===========

// =========== Change History ===========
// 25/05/2020   v1  [PG]    Creation of the file
// ===========

// Various runtime functions and declarations that are missing or rewritten here.

#include <new.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#include <Objects.h>
#include <NewtonMemory.h>
#include <NewtonErrors.h>
#include <NewtonExceptions.h>
#include <NewtonTime.h>
#include <NSandDDKIncludes.h>

void operator delete  ( void* ptr, unsigned int sz ) noexcept {
    ::operator delete ( ptr );
}

// Exceptions

DefineException(evRootEvent, evt);
DefineException(exRootException, evt.ex);
DefineException(exOutOfMemory, evt.ex.outofmem);
DefineException(exDivideByZero, evt.ex.div0);

DeclareException(exFrames, exRootException);
DefineException(exFrames, evt.ex.fr);
DeclareException(exStoreError, exFrames);
DefineException(exStoreError, evt.ex.fr.store);
DeclareException(exInterpreter, exFrames);
DefineException(exInterpreter, evt.ex.fr.intrp);
DeclareException(exInterpreterWithFrameData, exInterpreter);
DefineException(exInterpreterWithFrameData, evt.ex.fr.intrp;type.ref.frame);

// C functions

extern "C" {

time_t time(time_t * timer)
{
    time_t clock = RealClockSeconds();
    if (timer) {
        *timer = clock;
    }
    return clock;
}

struct tm *
gmtime(const time_t *clock)
{
    return NULL;
}

void
__div0(void)
{
    Throw(exDivideByZero, NULL, NULL);
}

void * calloc(size_t count, size_t size)
{
    size_t total_size = count * size;
    void * result = NewPtrClear(total_size);
    NewtonErr err = MemError();
    if (err == noErr) {
        return result;
    }
    return NULL;
}

};

// ========================================================================== //
// There is, in fact, no reason to believe that any given natural phenomenon, //
// however marvelous it may seem today, will remain forever inexplicable.     //
// Soon or late the laws governing the production of life itself will be      //
// discovered in the laboratory, and man may set up business as a creator     //
// on his own account.  The thing, indeed, is not only conceivable; it is     //
// even highly probable.                                                      //
//      -- H. L. Mencken, 1930                                                //
// ========================================================================== //
