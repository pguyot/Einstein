/* -----------------------------------------------------------------------
   ffi_common.h - Copyright (c) 1996  Cygnus Solutions

   $Id: ffi_common.h,v 1.1 1999/08/08 22:58:30 green Exp $

   Common internal definitions and macros. Only necessary for building
   libffi.
   ----------------------------------------------------------------------- */

#ifndef FFI_COMMON_H
#define FFI_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/* Do not move this. Some versions of AIX are very picky about where
   this is positioned. */
#ifdef __GNUC__
# define alloca __builtin_alloca
#else
# if HAVE_ALLOCA_H
#  include <alloca.h>
# else
#  ifdef _AIX
 #pragma alloca
#  else
#   ifndef alloca /* predefined by HP cc +Olibcalls */
char *alloca ();
#   endif
#  endif
# endif
#endif

/* Check for the existence of memcpy. */
#if STDC_HEADERS
# include <string.h>
#else
# ifndef HAVE_MEMCPY
#  define memcpy(d, s, n) bcopy ((s), (d), (n))
# endif
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif

#ifndef __cplusplus
/* bool is a keyword in C++ */
/*@-cppnames@*/
typedef int bool;
/*@=cppnames@*/
#endif

#ifdef FFI_DEBUG

/* Debugging functions */
/*@exits@*/ int ffi_assert(/*@temp@*/ char *file, int line);
void ffi_stop_here(void);
bool ffi_type_test(/*@temp@*/ /*@out@*/ ffi_type *a);

#define FFI_ASSERT(x) ((x) ? 0 : ffi_assert(__FILE__,__LINE__))

#else

#define FFI_ASSERT(x) 

#endif

#define ALIGN(v, a)  (((((size_t) (v))-1) | ((a)-1))+1)

/* Perform machine dependent cif processing */
ffi_status ffi_prep_cif_machdep(ffi_cif *cif);

/* Extended cif, used in callback from assembly routine */
typedef struct
{
  /*@dependent@*/ ffi_cif *cif;
  /*@dependent@*/ void *rvalue;
  /*@dependent@*/ void **avalue;
} extended_cif;

/* Terse sized type definitions.  */
typedef unsigned int UINT8  __attribute__((__mode__(__QI__)));
typedef signed int   SINT8  __attribute__((__mode__(__QI__)));
typedef unsigned int UINT16 __attribute__((__mode__(__HI__)));
typedef signed int   SINT16 __attribute__((__mode__(__HI__)));
typedef unsigned int UINT32 __attribute__((__mode__(__SI__)));
typedef signed int   SINT32 __attribute__((__mode__(__SI__)));
typedef unsigned int UINT64 __attribute__((__mode__(__DI__)));
typedef signed int   SINT64 __attribute__((__mode__(__DI__)));

typedef float FLOAT32;

#ifdef __cplusplus
}
#endif

#endif


