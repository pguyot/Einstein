// ==============================
// File:			TToolkitScriptExt.cp
// Project:			Einstein
//
// Copyright 2022 by Paul Guyot and Matthias Melcher.
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
// ==============================
// $Id$
// ==============================

//
// This file contains extensions to the Newt/64 NewtonScript implementation.
//

#include "TToolkitScriptExt.h"

#include "TToolkit.h"
#include "app/FLTK/TFLSettingsUI.h"

extern "C" {
#include "NewtBC.h"
#include "NewtCore.h"
#include "NewtEnv.h"
#include "NewtFile.h"
#include "NewtNSOF.h"
#include "NewtParser.h"
#include "NewtPkg.h"
#include "NewtPrint.h"
#include "NewtVM.h"
}

#include <FL/Fl_File_Chooser.H>
#include <FL/fl_ask.H>

#if TARGET_OS_WIN32
#include <Winsock2.h>
#include <direct.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#else
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <cstdlib>
#include <errno.h>
#include <fcntl.h>
#include <string>

extern newtRef NewtMakeBinaryFromARMFile(const char* srcfilename, bool /*literal*/);

/**
 The 'info' element in the Package header wants a binary object containing ASCII without a trailing 0.

 This is a simple function because newt/64 stores strings internally as UTF8 already.

 \param klass provide a class symbol for the binary
 \param text in UTF8
 \param literal see NewtMakeBinary
 \return a binary object, or `kNewtRefUnbind`
 */
static newtRef
NewtMakeBinaryFromString(newtRefArg klass, const char* text, bool literal)
{
	uint32_t size = (uint32_t) strlen(text);
	newtRef obj = NewtMakeBinary(klass, 0, size, literal);
	if (obj)
	{
		uint8_t* dst = NewtRefToBinary(obj);
		memcpy(dst, text, size);
		return obj;
	}
	return kNewtRefUnbind;
}

/**
 NewtonScript interface to `NewtMakeBinaryFromString`.
 \param rcvr ignored
 \param text UTF8 text ref
 \param klass ref to class symbol
 \return a binary object, or throws an exception
 */
static newtRef
NsMakeBinaryFromString(newtRefArg rcvr, newtRefArg text, newtRefArg klass)
{
	(void) rcvr;
	if (!NewtRefIsString(text))
		return NewtThrow(kNErrNotAString, text);
	return NewtMakeBinaryFromString(klass, NewtRefToString(text), false);
}

/**
 Compile the text field of the function into a binary object.

 This allows users to inline ARM code directly into a NewtonScript app.
 \code
 peek: {
 class : 'BinCFunction,
 code: MakeBinaryFromARM(
 "	mov		r0, [r1]	\n"	// unreference the first argument
 "	mov		r0, [r0]	\n"	// and return it
 "  	mov		pc, lr 		\n"	// return an NS value
 ),
 numArgs: 1,
 offset: 0
 }
 \endcode
 \param text utf8 formatted source code
 \return a binary object
 */
newtRef
NewtMakeBinaryFromARM(const char* text, bool /*literal*/)
{
	auto text_len = strlen(text);
	// write the string to a temporary file
	Fl_Preferences prefs(Fl_Preferences::USER, "robowerk.com", "einstein");
	char basename[FL_PATH_MAX + 1];
	prefs.get_userdata_path(basename, FL_PATH_MAX);
	char srcfilename[FL_PATH_MAX + 1];
	strncpy(srcfilename, basename, FL_PATH_MAX);
	strncat(srcfilename, "inline.s", FL_PATH_MAX);

	FILE* f = fl_fopen(srcfilename, "wb");
	fwrite(text, text_len, 1, f);
	fclose(f);
	return NewtMakeBinaryFromARMFile(srcfilename, false);
}

/**
 NewtonScript interface to `NewtMakeBinaryFromARM`.
 \param rcvr ignored
 \param text UTF8 arm assembly code (note indents and newline formatting)
 \return a binary object, or throws an exception
 */
static newtRef
NsMakeBinaryFromARM(newtRefArg rcvr, newtRefArg text)
{
	(void) rcvr;
	if (!NewtRefIsString(text))
		return NewtThrow(kNErrNotAString, text);
	return NewtMakeBinaryFromARM(NewtRefToString(text), false);
}

/**
 Compile a text file containing ARM instructions into a binary object.
 \param filename NS String, filename relative to current source file
 \return a binary object
 */
newtRef
NewtMakeBinaryFromARMFile(const char* srcfilename, bool /*literal*/)
{
	Fl_Preferences prefs(Fl_Preferences::USER, "robowerk.com", "einstein");
	char basename[FL_PATH_MAX + 1];
	prefs.get_userdata_path(basename, FL_PATH_MAX);
	char objfilename[FL_PATH_MAX + 1];
	strncpy(objfilename, basename, FL_PATH_MAX);
	strncat(objfilename, "inline.o", FL_PATH_MAX);
	char binfilename[FL_PATH_MAX];
	strncpy(binfilename, basename, FL_PATH_MAX);
	strncat(binfilename, "inline", FL_PATH_MAX);
	char errfilename[FL_PATH_MAX + 1];
	strncpy(errfilename, basename, FL_PATH_MAX);
	strncat(errfilename, "inline.err", FL_PATH_MAX);
	// run `arm-none-eabi-as -march=armv4 -mbig-endian test.s -o test.o`

	char cmd[4 * FL_PATH_MAX];
	snprintf(cmd, sizeof(cmd),
		"\"%s\" -march=armv4 -mbig-endian \"%s\" -o \"%s\" >\"%s\" 2>&1",
		gApp->GetSettings()->mDevAsmPath,
		srcfilename, objfilename, errfilename);
	fl_system(cmd);
	gToolkit->PrintErrFile(errfilename);

	// run `arm-none-eabi-objcopy -O binary -j .text test.o test`
	snprintf(cmd, sizeof(cmd),
		"\"%s\" -O binary -j .text \"%s\" \"%s\" >\"%s\" 2>&1",
		gApp->GetSettings()->mDevObjCopyPath,
		objfilename, binfilename, errfilename);
	fl_system(cmd);
	gToolkit->PrintErrFile(errfilename);

	newtRef filename_ref = NewtMakeString(binfilename, false);
	return NsLoadBinary(kNewtRefUnbind, filename_ref);
}

/**
 NewtonScript interface to `NewtMakeBinaryFromARMFile`.
 \param rcvr ignored
 \param text filename or path to an assembler source file
 \return a binary object, or throws an exception
 */
static newtRef
NsMakeBinaryFromARMFile(newtRefArg rcvr, newtRefArg filename)
{
	(void) rcvr;
	if (!NewtRefIsString(filename))
		return NewtThrow(kNErrNotAString, filename);
	return NewtMakeBinaryFromARMFile(NewtRefToString(filename), false);
}

/**
 Patch a ROM file using the given ARM instructions.

 This function will assemble the ARM instructions in the first string
 into an object file. Then we read the object file and patch the segment
 into the binary file.

 \code
 PatchFileFromARM("
 .org 0x001412f8
 ldmdb   r11, {r11, sp, pc} @ return early from screen calibrartion
 ", "myROMFile")
 \endcode
 \param text utf8 formatted source code
 \param filename using the path to the current ROM, or a path plus filename
 \return a binary object
 \note This patch function is not perfect. Four or more consecutine 0 bytes
 may not actually be written.
 \todo Update ROM and REx checksums to avoid deleting internal flash.
 \todo Fix overall checksum to give Einstein a chance to recognize pre-patched ROMs.
 */
newtRef
NewtPatchFileFromARM(const char* text, const char* filename, bool /*literal*/)
{
	auto text_len = strlen(text);
	// write the string to a temporary file
	Fl_Preferences prefs(Fl_Preferences::USER, "robowerk.com", "einstein");
	char basename[FL_PATH_MAX + 1];
	prefs.get_userdata_path(basename, FL_PATH_MAX);
	char srcfilename[FL_PATH_MAX + 1];
	strncpy(srcfilename, basename, FL_PATH_MAX);
	strncat(srcfilename, "inline.s", FL_PATH_MAX);
	char objfilename[FL_PATH_MAX + 1];
	strncpy(objfilename, basename, FL_PATH_MAX);
	strncat(objfilename, "inline.o", FL_PATH_MAX);
	char disfilename[FL_PATH_MAX + 1];
	strncpy(disfilename, basename, FL_PATH_MAX);
	strncat(disfilename, "inline.dis", FL_PATH_MAX);
	char errfilename[FL_PATH_MAX + 1];
	strncpy(errfilename, basename, FL_PATH_MAX);
	strncat(errfilename, "inline.err", FL_PATH_MAX);

	// run `arm-none-eabi-as -march=armv4 -mbig-endian test.s -o test.o`
	FILE* f = fl_fopen(srcfilename, "wb");
	fwrite(text, text_len, 1, f);
	fclose(f);
	char cmd[4 * FL_PATH_MAX];
	snprintf(cmd, sizeof(cmd),
		"\"%s\" -march=armv4 -mbig-endian \"%s\" -o \"%s\" >\"%s\" 2>&1",
		gApp->GetSettings()->mDevAsmPath,
		srcfilename, objfilename, errfilename);
	fl_system(cmd);
	gToolkit->PrintErrFile(errfilename);

	// run `arm-none-eabi-objdump -d test.o >test.dis`
	snprintf(cmd, sizeof(cmd),
		"\"%s\" -d --show-raw-insn \"%s\" >\"%s\" 2>\"%s\"",
		gApp->GetSettings()->mDevObjDumpPath,
		objfilename, disfilename, errfilename);
	fl_system(cmd);
	gToolkit->PrintErrFile(errfilename);

	char appDir[FL_PATH_MAX + 1], romDir[FL_PATH_MAX + 1];
	getcwd(appDir, FL_PATH_MAX);
	fl_filename_absolute(romDir, FL_PATH_MAX, gApp->GetSettings()->ROMPath);
	char* x = (char*) fl_filename_name(romDir);
	if (x)
		x[0] = 0;
	chdir(romDir);
	int bin = fl_open(filename, O_WRONLY | O_CREAT);
	chdir(appDir);
	if (bin == -1)
	{
		gToolkit->PrintErr("Can't open ");
		gToolkit->PrintErr(filename);
		gToolkit->PrintErr(": ");
		gToolkit->PrintErr(strerror(errno));
		gToolkit->PrintErr("\n");
		return NewtMakeInteger(errno);
	}
	f = fl_fopen(disfilename, "rb");
	if (!f)
	{
		close(bin);
		gToolkit->PrintErr("Can't open disassembled file\n");
		return NewtMakeInteger(-1);
	}
	for (;;)
	{
		char buf[FL_PATH_MAX + 1];
		if (fgets(buf, FL_PATH_MAX, f) == nullptr)
			break;
		char* sep = strstr(buf, ":\t");
		if (sep == nullptr)
			continue;
		KUInt32 addr;
		union {
			KUInt32 data;
			KUInt8 d[4];
		};
		int n = sscanf(buf, "%8x:\t%x", &addr, &data);
		if (n != 2)
			continue;
		lseek(bin, addr, SEEK_SET);
		data = htonl(data);
		if (sep[4] == ' ')
			write(bin, d + 3, 1);
		else if (sep[6] == ' ')
			write(bin, d + 2, 2);
		else if (sep[8] == ' ')
			write(bin, d + 1, 3);
		else
			write(bin, d, 4);
		if (n == 2)
		{
			printf("0x%08x: 0x%08x\n", addr, data);
		}
	}
	close(bin);
	fclose(f);
	return kNewtRefNIL;
}

/**
 NewtonScript interface to `NewtMakeBinaryFromARM`.
 \param rcvr ignored
 \param text UTF8 arm assembly code (note indents and newline formatting)
 \return a binary object, or throws an exception
 */
static newtRef
NsPatchFileFromARM(newtRefArg rcvr, newtRefArg text, newtRefArg filename)
{
	(void) rcvr;
	if (!NewtRefIsString(text))
		return NewtThrow(kNErrNotAString, text);
	return NewtPatchFileFromARM(NewtRefToString(text), NewtRefToString(filename), false);
}

/**
 Implements the NTK function `AddStepForm`.
 \param rcvr
 \param form add to this form
 \param child add this child
 \return `kNewtRefNIL` or throws an exception.
 */
static newtRef
NSAddStepForm(newtRefArg rcvr, newtRefArg form, newtRefArg child)
{
	(void) rcvr;
	if (!NewtRefIsFrame(form))
	{
		return NewtThrow(kNErrNotAFrame, NewtMakeString("First argument should be a view form", true));
	}
	if (!NewtRefIsFrame(child))
	{
		return NewtThrow(kNErrNotAFrame, NewtMakeString("Second argument should be a view form", true));
	}
	newtRef stepChildren = NcGetSlot(form, NSSYM(StepChildren));
	if (!NewtRefIsArray(stepChildren))
	{
		stepChildren = NewtMakeArray(NSSYM(array), 0);
		NcSetSlot(form, NSSYM(StepChildren), stepChildren);
	}
	NcAddArraySlot(stepChildren, child);
	return kNewtRefNIL;
}

/**
 Implements the NTK function `StepDeclare`.
 \param rcvr
 \param form add to this form
 \param ref reference to child
 \param sym symbol to access child
 \return `kNewtRefNIL` or throws an exception.
 */
static newtRef
NSStepDeclare(newtRefArg rcvr, newtRefArg form, newtRefArg ref, newtRefArg sym)
{
	(void) rcvr;

	if (!NewtRefIsFrame(form))
	{
		return NewtThrow(kNErrNotAFrame, NewtMakeString("First argument should be a view form", true));
	}
	if (!NewtRefIsFrame(ref))
	{
		return NewtThrow(kNErrNotAFrame, NewtMakeString("Second argument should be a view form", true));
	}
	if (!NewtRefIsSymbol(sym))
	{
		return NewtThrow(kNErrNotASymbol, NewtMakeString("Third argument should be a symbol", true));
	}

	// add an array named StepAllocateContext in form
	newtRef stepAllocateContext = NcGetSlot(form, NSSYM(StepAllocateContext));
	if (!NewtRefIsArray(stepAllocateContext))
	{
		stepAllocateContext = NewtMakeArray(NSSYM(array), 0);
		NcSetSlot(form, NSSYM(StepAllocateContext), stepAllocateContext);
	}
	// add a slot to the array of value sym
	NcAddArraySlot(stepAllocateContext, sym);
	// add a slot to the array of value ref
	NcAddArraySlot(stepAllocateContext, ref);
	// add slot named sym to form with a value of NIL
	NsSetSlot(rcvr, form, sym, kNewtRefNIL);
	// add a slot to ref named preallocatedContext with sym as its value
	NsSetSlot(rcvr, ref, NSSYM(PreallocatedContext), sym);

	return kNewtRefNIL;
}

/**
 Pop open an FLTK standard dialog box with a message text.
 \param rcvr
 \param text must an NS String
 \return NIL
 */
static newtRef
NsFltkMessage(newtRefArg rcvr, newtRefArg text)
{
	(void) rcvr;
	if (!NewtRefIsString(text))
		return NewtThrow(kNErrNotAString, text);
	fl_message("%s", NewtRefToString(text));
	return kNewtRefNIL;
}

/**
 Pop open an FLTK choice dialog and return the user's choice
 \param rcvr
 \param text an NS String message text
 \param b0 String for the rightmost button - this is also selected if the
	dialog fails or cancels
 \param b1 String for the second button, or NIL for no button - this is
	also the default button when the user presses Return
 \param b2 String for the leftmost button, or NIL for no button
 \return 0, 1, or 2, depending on the user's choice
 */
static newtRef
NsFltkChoice(newtRefArg rcvr, newtRefArg text, newtRefArg b0, newtRefArg b1, newtRefArg b2)
{
	(void) rcvr;
	if (!NewtRefIsString(text))
		return NewtThrow(kNErrNotAString, text);
	char* b0text = nullptr;
	if (!NewtRefIsNIL(b0))
	{
		if (NewtRefIsString(b0))
			b0text = NewtRefToString(b0);
		else
			return NewtThrow(kNErrNotAString, b0);
	}
	char* b1text = nullptr;
	if (!NewtRefIsNIL(b1))
	{
		if (NewtRefIsString(b1))
			b1text = NewtRefToString(b1);
		else
			return NewtThrow(kNErrNotAString, b1);
	}
	char* b2text = nullptr;
	if (!NewtRefIsNIL(b2))
	{
		if (NewtRefIsString(b2))
			b2text = NewtRefToString(b2);
		else
			return NewtThrow(kNErrNotAString, b2);
	}
	int ret = fl_choice("%s", b0text, b1text, b2text, NewtRefToString(text));
	return NewtMakeInteger(ret);
}

/**
 Pop open a FLTK file chooser dialog.
 \param rcvr
 \param message String to tell the user what to expect
 \param pattern String, for example "HTML Files (*.html)\tImage Files (*.{bmp,gif,jpg,png})"
 \param filename String or NIL, preset a directory or filename
 \param relative NIL or TRUE, if TRUE, this returns a filename relative to the current directory
 \return NIL if the user canceled, or a String containing the file name and path
 */
static newtRef
NsFltkFileChooser(newtRefArg rcvr, newtRefArg message, newtRefArg pattern, newtRefArg filename, newtRefArg relative)
{
	(void) rcvr;
	if (!NewtRefIsString(message))
		return NewtThrow(kNErrNotAString, message);
	char* cPattern = nullptr;
	if (!NewtRefIsNIL(pattern))
	{
		if (NewtRefIsString(pattern))
			cPattern = NewtRefToString(pattern);
		else
			return NewtThrow(kNErrNotAString, pattern);
	}
	char* cFilename = nullptr;
	if (!NewtRefIsNIL(filename))
	{
		if (NewtRefIsString(filename))
			cFilename = NewtRefToString(filename);
		else
			return NewtThrow(kNErrNotAString, filename);
	}
	int cRelative = -1;
	if (NewtRefIsNIL(relative))
		cRelative = 0;
	else if (relative == kNewtRefTRUE)
		cRelative = 1;
	else
		return NewtThrow(kNErrNotTrueOrNil, relative);
	const char* ret = fl_file_chooser(NewtRefToString(message), cPattern, cFilename, cRelative);
	if (ret == nullptr)
		return kNewtRefNIL;
	return NewtMakeString(ret, false);
}

static newtRef
NsFileNew(newtRefArg rcvr)
{
	(void) rcvr;
	// return NewtMakeString("Test", false);
	//  newtRef file = NewtMakeFrame(kNewtRefNIL, 0);
	//  newtObjRef file_obj = (newtObjRef) NewtRefToPointer(file);
	//  NewtObjSetSlot(file_obj, NSSYM(_proto), NSSYM(file));
	//  NewtObjSetSlot(file_obj, NSSYM(_cFile), NewtMakeInt64(0));

	newtRefVar r = NcMakeFrame();
	NcSetSlot(r, NSSYM(_proto), NsGetGlobalVar(rcvr, NSSYM(file)));
	NcSetSlot(r, NSSYM(_cFile), NewtMakeInt64(0));

	return r;
}

static newtRef
NsFileOpen(newtRefArg rcvr, newtRefArg filename, newtRefArg mode)
{
	newtObjRef self = (newtObjRef) NewtRefToPointer(rcvr);
	newtRefArg cFile = NewtObjGetSlot(self, NSSYM(_cFile));
	FILE* f = (FILE*) NewtRefToInteger(cFile);
	if (f)
		return NewtThrow(kNErrNotNil, NSSYM(_cFile));

	char* cFilename = nullptr;
	if (NewtRefIsString(filename))
		cFilename = NewtRefToString(filename);
	else
		return NewtThrow(kNErrNotAString, filename);

	const char* cMode = nullptr;
	if (NewtRefIsSymbol(mode))
	{
		if (NewtSymbolEqual(mode, NSSYM(read)))
			cMode = "rb";
		else if (NewtSymbolEqual(mode, NSSYM(write)))
			cMode = "wb";
		else if (NewtSymbolEqual(mode, NSSYM(readwrite)))
			cMode = "rwb";
		else if (NewtSymbolEqual(mode, NSSYM(append)))
			cMode = "ab";
		else
			return NewtThrow(kNErrBadArgs, mode);
	} else
	{
		return NewtThrow(kNErrNotASymbol, mode);
	}

	f = fl_fopen(cFilename, cMode);
	if (!f)
		return NewtThrow(kNErrFileNotOpen, filename);
	NewtObjSetSlot(self, NSSYM(_cFile), NewtMakeInt64((int64_t) f));

	return kNewtRefTRUE;
}

static newtRef
NsFileIsOpen(newtRefArg rcvr)
{
	newtObjRef self = (newtObjRef) NewtRefToPointer(rcvr);
	newtRefArg cFile = NewtObjGetSlot(self, NSSYM(_cFile));
	FILE* f = (FILE*) NewtRefToInteger(cFile);
	if (f)
		return kNewtRefTRUE;
	else
		return kNewtRefNIL;
}

static newtRef
NsFileSize(newtRefArg rcvr)
{
	newtObjRef self = (newtObjRef) NewtRefToPointer(rcvr);
	newtRefArg cFile = NewtObjGetSlot(self, NSSYM(_cFile));
	FILE* f = (FILE*) NewtRefToInteger(cFile);
	if (!f)
		return NewtThrow(kNErrFileNotOpen, rcvr);
	int fd = fileno(f);
	struct stat buf;
	fstat(fd, &buf);
	off_t size = buf.st_size;
	return NewtMakeInteger(size);
}

static newtRef
NsFileSeek(newtRefArg rcvr, newtRefArg position, newtRefArg mode)
{
	newtObjRef self = (newtObjRef) NewtRefToPointer(rcvr);
	newtRefArg file = NewtObjGetSlot(self, NSSYM(_cFile));
	FILE* f = (FILE*) NewtRefToInteger(file);
	if (!f)
		return NewtThrow(kNErrNotNil, NSSYM(_cFile));

	int cPosition = 0;
	if (NewtRefIsInteger(position))
		cPosition = NewtRefToInteger(position);
	else
		return NewtThrow(kNErrNotAnInteger, position);

	int cMode = SEEK_SET;
	if (NewtRefIsSymbol(mode))
	{
		if (NewtSymbolEqual(mode, NSSYM(set)))
			cMode = SEEK_SET;
		else if (NewtSymbolEqual(mode, NSSYM(cur)))
			cMode = SEEK_CUR;
		else if (NewtSymbolEqual(mode, NSSYM(end)))
			cMode = SEEK_END;
		else
			return NewtThrow(kNErrBadArgs, mode);
	} else
	{
		return NewtThrow(kNErrNotASymbol, mode);
	}

	int ret = fseek(f, cPosition, cMode);
	return NewtMakeInteger(ret);
}

static newtRef
NsFileTell(newtRefArg rcvr)
{
	newtObjRef self = (newtObjRef) NewtRefToPointer(rcvr);
	newtRefArg cFile = NewtObjGetSlot(self, NSSYM(_cFile));
	FILE* f = (FILE*) NewtRefToInteger(cFile);
	if (!f)
		return NewtThrow(kNErrFileNotOpen, rcvr);
	return NewtMakeInteger(ftell(f));
}

static newtRef
NsFileRead(newtRefArg rcvr, newtRefArg maxBytes)
{
	newtObjRef self = (newtObjRef) NewtRefToPointer(rcvr);
	newtRefArg cFile = NewtObjGetSlot(self, NSSYM(_cFile));
	FILE* f = (FILE*) NewtRefToInteger(cFile);
	if (!f)
		return NewtThrow(kNErrFileNotOpen, rcvr);

	int cMaxBytes = NewtRefToInteger(maxBytes);
	if (cMaxBytes <= 0)
		return NewtThrow(kNErrOutOfRange, maxBytes);

	uint8_t* buffer = (uint8_t*) ::malloc(cMaxBytes);
	int n = ::fread(buffer, 1, cMaxBytes, f);
	if (n <= 0)
		return kNewtRefNIL;

	return NewtMakeBinary(NSSYM(data), buffer, n, false);
}

static newtRef
NsFileWrite(newtRefArg rcvr, newtRefArg data)
{
	// TODO: throw an error if this a read-only file?
	newtObjRef self = (newtObjRef) NewtRefToPointer(rcvr);
	newtRefArg cFile = NewtObjGetSlot(self, NSSYM(_cFile));
	FILE* f = (FILE*) NewtRefToInteger(cFile);
	if (!f)
		return NewtThrow(kNErrFileNotOpen, rcvr);

	void* cData = nullptr;
	int cSize = 0;
	if (NewtRefIsBinary(data))
	{
		cData = NewtObjData((newtObjRef) NewtRefToPointer(data));
		cSize = NewtBinaryLength(data);
		// TODO: if the binary is a string, remove the trailing 0 (cSize--)
	} else
	{
		return NewtThrow(kNErrNotABinaryObject, data);
	}

	int ret = fwrite(cData, 1, cSize, f);
	return NewtMakeInteger(ret);
}

static newtRef
NsFileClose(newtRefArg rcvr)
{
	newtObjRef self = (newtObjRef) NewtRefToPointer(rcvr);
	newtRefArg cFile = NewtObjGetSlot(self, NSSYM(_cFile));
	FILE* f = (FILE*) NewtRefToInteger(cFile);
	if (!f)
		return NewtThrow(kNErrFileNotOpen, rcvr);
	fclose(f);
	f = nullptr;
	return kNewtRefTRUE;
}

void
RegisterToolkitScriptExtensions()
{
	// a list of additional global functions
	NewtDefGlobalFunc0(NSSYM(MakeBinaryFromString), (void*) NsMakeBinaryFromString, 2, false, (char*) "MakeBinaryFromString(str, sym)");
	NewtDefGlobalFunc0(NSSYM(MakeBinaryFromARM), (void*) NsMakeBinaryFromARM, 1, false, (char*) "MakeBinaryFromARM(ARM_Instructions)");
	NewtDefGlobalFunc0(NSSYM(MakeBinaryFromARMFile), (void*) NsMakeBinaryFromARMFile, 1, false, (char*) "MakeBinaryFromARMFile(ARM_Assembler_Filename)");
	NewtDefGlobalFunc0(NSSYM(PatchFileFromARM), (void*) NsPatchFileFromARM, 2, false, (char*) "PatchFileFromARM(ARM_Instructions, filename)");
	NewtDefGlobalFunc0(NSSYM(AddStepForm), (void*) NSAddStepForm, 2, false, (char*) "AddStepForm(mainView, scrollClipper);");
	NewtDefGlobalFunc0(NSSYM(StepDeclare), (void*) NSStepDeclare, 3, false, (char*) "StepDeclare(mainView, scrollClipper, 'scrollClipper);");

	// redirect error output into a string field
	NcDefGlobalVar(NSSYM0(_STDERR_), NewtMakeString("", false));
	NcDefGlobalVar(NSSYM0(_STDOUT_), NewtMakeString("", false));

	// the FLTK namespace with a few GUI related calls
	auto fltk = NewtMakeFrame(kNewtRefNIL, 0);
	newtObjRef fltk_obj = (newtObjRef) NewtRefToPointer(fltk);
	NewtObjSetSlot(fltk_obj, NSSYM(message), NewtMakeNativeFunc0((void*) NsFltkMessage, 1, false, (char*) "Open a message dialog box (message)"));
	NewtObjSetSlot(fltk_obj, NSSYM(choice), NewtMakeNativeFunc0((void*) NsFltkChoice, 4, false, (char*) "Open a user choice dialog box (message, cancel buttem, ok button, opt button) => selection (int)"));
	NewtObjSetSlot(fltk_obj, NSSYM(filechooser), NewtMakeNativeFunc0((void*) NsFltkFileChooser, 4, false, (char*) "Open a file chooser dialog (message, pattern, filename, relative) => filename or NIL"));
	NcDefGlobalVar(NSSYM(fltk), fltk);

	// the FILE namespace for reading and writing files on the hos machine
	auto file = NewtMakeFrame(kNewtRefNIL, 0);
	newtObjRef file_obj = (newtObjRef) NewtRefToPointer(file);
	NewtObjSetSlot(file_obj, NSSYM(new), NewtMakeNativeFunc0((void*) NsFileNew, 0, false, (char*) "Create a frame to access files on the host machine."));
	NewtObjSetSlot(file_obj, NSSYM(open), NewtMakeNativeFunc0((void*) NsFileOpen, 2, false, (char*) "Open(filename, 'read|'write|'readwrite|'append)"));
	NewtObjSetSlot(file_obj, NSSYM(isOpen), NewtMakeNativeFunc0((void*) NsFileIsOpen, 0, false, (char*) "IsOpen() -> nil|true"));
	// string FILE:error(); // return the strerror text for all platforms, we may also want a unified error number
	NewtObjSetSlot(file_obj, NSSYM(size), NewtMakeNativeFunc0((void*) NsFileSize, 0, false, (char*) "Size() -> int size in bytes"));
	NewtObjSetSlot(file_obj, NSSYM(read), NewtMakeNativeFunc0((void*) NsFileRead, 1, false, (char*) "Read(maxSize) -> binary object"));
	NewtObjSetSlot(file_obj, NSSYM(write), NewtMakeNativeFunc0((void*) NsFileWrite, 1, false, (char*) "Write(binary object) -> bytes written"));
	NewtObjSetSlot(file_obj, NSSYM(seek), NewtMakeNativeFunc0((void*) NsFileSeek, 2, false, (char*) "seek(int pos, 'set|'cur|'end) -> int 0=ok, -1=error"));
	NewtObjSetSlot(file_obj, NSSYM(tell), NewtMakeNativeFunc0((void*) NsFileTell, 0, false, (char*) "Tell() -> int position in file"));
	NewtObjSetSlot(file_obj, NSSYM(close), NewtMakeNativeFunc0((void*) NsFileClose, 0, false, (char*) "Close() -> ex or TRUE"));
	NcDefGlobalVar(NSSYM(file), file);
}

/* MakeIcon/ImagFormPNG:

 B/W image format:
 newt.app.parts[0].data.icon: {
 mask: MakeBinaryFromHex(
 "0000000000040000000000000015001501FC000007FF00000FFF80001FFFC0003FFFE0007FFFF000"
 "7FFFF0007FFFF000FFFFF800FFFFF800FFFFF800FFFFF800FFFFF800FFFFF8007FFFF0007FFFF000"
 "3FFFE0001FFFC0000FFF800007FF000001FC0000",
 'mask),
 bits: MakeBinaryFromHex(
 "0000000000040000000000000015001501FC000007DF00000FFF80001E03C0003DFDE0007BFEF000"
 "77FF70006F8FB000EF07B800AE23A800AE23A800EF07B800EF8FB800F7FF78007BFEF0007DFDF000"
 "3E03E0001FFFC0000FFF800007DF000001FC0000",
 'bits),
 bounds: {
 left: 0,
 top: 0,
 right: 21,
 bottom: 21
 }

 iconPro: {
	unhilited: {
		bounds: ref00b1eb81 := {
			left: 0,
			top: 0,
			right: 24,
			bottom: 24
		},
		bits: MakeBinaryFromHex(
			"00000000000400000000000000180018001E0000903F1400503F1D00702D0E00307F0C00F87FCC00"
			"3FFFEC000FFFFC0007FFF800007F900000FFC00018FFC0003FFFFF003FFFFF003FFFFF001FFFDE00"
			"0E7F1C00471E380037807800FF8078003F001F0040000E000000020000000000",
			'bits),
		colorData: {
			cbits: MakeBinaryFromHex(
				"00000000000400000000000000180018001E0000903F1400503F1D00702D0E00307F0C00F87FCC00"
				"3FFFEC000FFFFC0007FFF800007F900000FFC00018FFC0003FFFFF003FFFFF003FFFFF001FFFDE00"
				"0E7F1C00471E380037807800FF8078003F001F0040000E000000020000000000",
				'bits),
			bitDepth: 1
		},
		mask: MakeBinaryFromHex(
			"00000000000400000000000000180018001E0000903F1400503F1D00703F0E00307F0C00F87FCC00"
			"3FFFEC000FFFFC0007FFF800007F900000FFC00018FFC0003FFFFF003FFFFF003FFFFF001FFFDE00"
			"0E7F1C00471E380037807800FF8078003F001F0040000E000000020000000000",
			'mask)
		},
	hilited: {
		bits: MakeBinaryFromHex(
		"000000000004000000000000001800180000008C0000000000000001001200000000005000000026"
		"00000000000000A00000002600000001000000010000000100000001000000010000000100000001"
		"0000000100000001000000010000000100000001000000000000000000000000",
		'bits),
		bounds: {
			left: 0,
			top: 0,
			right: 24,
			bottom: 24
		}
	}

 image:
	bounds
	bits
	mask
	colorData
		[
			{
				cbits
				bitDepth: 1
			}
			{
				cbits
				bitDepth: 4
			}
		]

 */

// ======================================================================= //
// Newton Toolkit
//
// A development environment created by Apple for writing NewtonScript programs
// for Newton OS. A prototype for Newton Toolkit called EtchASketch was written
// using Macintosh Common Lisp. Also referred to as NTK.
//
//   -- Newton Glossary
// ======================================================================= //
