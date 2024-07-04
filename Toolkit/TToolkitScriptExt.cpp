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
#include <FL/Fl_Shared_Image.H>
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
 "    mov    r0, [r1] \n"	// dereference the first argument
 "    mov    r0, [r0] \n"	// and return it
 "    mov    pc, lr   \n"	// return an NS value
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
	char binfilename[FL_PATH_MAX + 1];
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

newtRef
NewtMakeIconFromFile(const char* srcfilename, bool /*literal*/)
{
	Fl_Shared_Image* img = Fl_Shared_Image::get(srcfilename);
	if ((!img) || (img->d() < 1) || (img->fail()) || (img->count() != 1))
	{
		if (img)
			img->release();
		return NewtThrow(kNErrFileNotFound, NewtMakeString(srcfilename, false));
	}
	auto icon = NewtMakeFrame(kNewtRefNIL, 0);
	newtObjRef icon_obj = (newtObjRef) NewtRefToPointer(icon);
	auto bounds = NewtMakeFrame(kNewtRefNIL, 0);
	newtObjRef bounds_obj = (newtObjRef) NewtRefToPointer(bounds);
	NewtObjSetSlot(icon_obj, NSSYM(bounds), bounds);
	NewtObjSetSlot(bounds_obj, NSSYM(top), NewtMakeInteger(0));
	NewtObjSetSlot(bounds_obj, NSSYM(left), NewtMakeInteger(0));
	NewtObjSetSlot(bounds_obj, NSSYM(bottom), NewtMakeInteger(img->h()));
	NewtObjSetSlot(bounds_obj, NSSYM(right), NewtMakeInteger(img->w()));

	int x, y, d = img->d(), w = img->w(), h = img->h(), ld, bits_size;
	ld = img->ld();
	if (ld == 0)
		ld = w * d;
	int dld = ((w + 31) & ~31) / 4; // align to 32 bits
	bits_size = h * dld + 16;
	uint8_t* bits_data = (uint8_t*) ::calloc(1, bits_size);
	uint16_t* bits_desc = (uint16_t*) bits_data;
	bits_desc[2] = htons(dld);
	bits_desc[6] = htons(h);
	bits_desc[7] = htons(w);
	const uint8_t* data = (const uint8_t*) img->data()[0];
	if ((d == 2) || (d == 4))
	{
		for (y = 0; y < h; y++)
		{
			for (x = 0; x < w; x++)
			{
				const uint8_t* src = data + x * d + d - 1 + y * ld;
				// const uint8_t *src = data + x*d + y*ld;
				if (*src >= 128)
				{
					uint8_t* dst = bits_data + 16 + x / 8 + y * dld;
					*dst |= 128 >> (x & 7);
				}
			}
		}
		auto mask = NewtMakeBinary(NSSYM(mask), bits_data, bits_size, false);
		NewtObjSetSlot(icon_obj, NSSYM(mask), mask);
		memset(bits_data + 16, 0, bits_size - 16);
	}
	for (y = 0; y < h; y++)
	{
		for (x = 0; x < w; x++)
		{
			const uint8_t* src = data + x * d + y * ld;
			const uint8_t* srca = data + x * d + d - 1 + y * ld;
			if (*src < 128 && *srca >= 128)
			{
				uint8_t* dst = bits_data + 16 + x / 8 + y * dld;
				*dst |= 128 >> (x & 7);
			}
		}
	}
	auto bits = NewtMakeBinary(NSSYM(bits), bits_data, bits_size, false);
	NewtObjSetSlot(icon_obj, NSSYM(bits), bits);
	::free(bits_data);

	img->release();
	return icon;
}

/**
 NewtonScript interface to `NewtMakeIconFromFile`.
 \param rcvr ignored
 \param text filename or path to an image file
 \return a frame containing the b&w icon with optional mask, or throws an exception
 */
static newtRef
NsMakeIconFromFile(newtRefArg rcvr, newtRefArg filename)
{
	(void) rcvr;
	if (!NewtRefIsString(filename))
		return NewtThrow(kNErrNotAString, filename);
	return NewtMakeIconFromFile(NewtRefToString(filename), false);
}

/**
 Patch a ROM file using the given ARM instructions.

 This function will assemble the ARM instructions in the first string
 into an object file. Then we read the object file and patch the segment
 into the binary file.

 \code
 PatchFileFromARM("
 .org 0x001412f8
 ldmdb   r11, {r11, sp, pc} @ return early from screen calibration
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
		cPosition = (int) NewtRefToInteger(position);
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

	int cMaxBytes = (int) NewtRefToInteger(maxBytes);
	if (cMaxBytes <= 0)
		return NewtThrow(kNErrOutOfRange, maxBytes);

	uint8_t* buffer = (uint8_t*) ::malloc(cMaxBytes);
	int n = (int) ::fread(buffer, 1, cMaxBytes, f);
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

	int ret = (int) fwrite(cData, 1, cSize, f);
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
	NewtDefGlobalFunc0(NSSYM(MakeIconFromFile), (void*) NsMakeIconFromFile, 1, false, (char*) "MakeIconFromFile(imagefile)");
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

	// the FILE namespace for reading and writing files on the host machine
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

 Create an app icon from one or more GIF/PNG images.

 App icons can be 1 bit or 4 bit deep, have an alternative image when clicked
 (highlighted), and have one 1-bit maks for normal and highlighted each.
 There is an option to use no mask, a bitmap mask, or a calculated mask.
 Iconsize is usually 26x25 pixels.

 (icon is also used in "viewclass: 76")

 We also want to create image and sound data for scripts.

 V1: icon { mask*, bits, bounds { left, top, right bottom } }
 V2: icon, iconPro { unhilited { bounds, bits, mask, colorData [* { cbits, bitDepth } ]* } hilited { } }
 V2c: icon { mask*, bits, bounds { left, top, right bottom } colorData { cbits, bitDepth } }



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

 '|CalcButton:JoBS|,
 {
 bits: MakeBinaryFromHex(
 "000000000004000000000000000B0008FF000000FF00000000000000DB000000DB00000000000000"
 "DB000000DB00000000000000DB000000DB000000",
 'bits),
 bounds: {
 left: 0,
 top: 0,
 right: 8,
 bottom: 11
 }
 },

 '|ProMedexButton:STARMER|,
 {
 bits: MakeBinaryFromHex(
 "000000000004012A012A01140135011F1F00000060C000004E4000008E200000BFA00000BFA00000"
 "BFA000008E2000004E40000060C000001F000000",
 'bits),
 bounds: {
 left: 0,
 top: 0,
 right: 11,
 bottom: 11
 }
 },


 iconPro: {
 unhilited: {
 bounds: refc445bfe1 := {
 left: 0,
 top: 0,
 right: 20,
 bottom: 19
 },
 bits: MakeBinaryFromHex(
 "0000000000040000000000000013001400000000040000000C0000001F8000003FE000003FF80000"
 "1FFC00000C7E0000041E0000001E0000000F0000000F0000000F0000003FC000001F8000000F0000"
 "000600000000000000000000",
 'bits),
 colorData: {
 cbits: MakeBinaryFromHex(
 "0000000000040000000000000013001400000000040000000C0000001F8000003FE000003FF80000"
 "1FFC00000C7E0000041E0000001E0000000F0000000F0000000F0000003FC000001F8000000F0000"
 "000600000000000000000000",
 'bits),
 bitDepth: 1
 },
 mask: MakeBinaryFromHex(
 "0000000000040000000000000013001400000000040000000C0000001F8000003FE000003FF80000"
 "1FFC00000C7E0000041E0000001E0000000F0000000F0000000F0000003FC000001F8000000F0000"
 "000600000000000000000000",
 'mask)
 },
 hilited: {
 bounds: {
 left: 0,
 top: 0,
 right: 20,
 bottom: 19
 },
 bits: MakeBinaryFromHex(
 "0000000000040000000000000013001400000000040000000C0000001F8000003FE000003FF80000"
 "1FFC00000C7E0000041E0000001E0000000F0000000F0000000F0000003FC000001F8000000F0000"
 "000600000000000000000000",
 'bits),
 colorData: {
 cbits: MakeBinaryFromHex(
 "0000000000040000000000000013001400000000040000000C0000001F8000003FE000003FF80000"
 "1FFC00000C7E0000041E0000001E0000000F0000000F0000000F0000003FC000001F8000000F0000"
 "000600000000000000000000",
 'bits),
 bitDepth: 1
 },
 mask: MakeBinaryFromHex(
 "0000000000040000000000000013001400000000040000000C0000001F8000003FE000003FF80000"
 "1FFC00000C7E0000041E0000001E0000000F0000000F0000000F0000003FC000001F8000000F0000"
 "000600000000000000000000",
 'mask)
 }
 },
 icon: {
 bits: MakeBinaryFromHex(
 "0000000000040000000000000013001400000000040000000C0000001F8000003FE000003FF80000"
 "1FFC00000C7E0000041E0000001E0000000F0000000F0000000F0000003FC000001F8000000F0000"
 "000600000000000000000000",
 'bits),
 bounds: refc445bfe1,
 mask: MakeBinaryFromHex(
 "00000000000400000000000000130014000000000000000000000000000000000000000000000000"
 "00000000000000000000000000000000000000000000000000000000000000000000000000000000"
 "000000000000000000000000",
 'bits)
 },


 icon: {
 mask: MakeBinaryFromHex(
 "000000000004000000000000001B0018000000000000000000001C0000003F0000003F001FFF7F00"
 "3FFF7E003FFFFE003FFFFC003FFFFC003FFFF8003FFFF8003FFFF0003FFFF0003FFFE0003FFFE000"
 "3FFFC0003FFFC0003FFF80003FFF00003FFF80003FFF80003FFF80001FFF00001FFF00000FFE0000"
 "00000000",
 'mask),
 bits: MakeBinaryFromHex(
 "000000000004000000000000001B0018000000000000000000001C0000003F00000033001FFF7B00"
 "3FFF6E003000CE0037FCCC0037FD9C0034059800355338003403300035567000340660003554E000"
 "340CC000354FC000340B8000360F000037FE800037FD8000300B8000180300001FFF00000FFE0000"
 "00000000",
 'bits),
 bounds: {
 left: 0,
 top: 0,
 right: 24,
 bottom: 27
 }
 },

 reffb87bfe1 := {
 bits: MakeBinaryFromHex(
 "000000000004000300030002001B001B00FFFC0003FFFF0003FFFF00073FFF80073FFF80065CFF80"
 "06D8DF801DDB00807FC77F80F7E7FF80C3FFFF00E3FFFF0061FFFC007070000031E0C000398CE000"
 "183C60001CF0E0000CC3E0000E0F8000063E000006F8000003E0000003800000",
 'bits),
 bounds: {
 left: 0,
 top: 0,
 right: 25,
 bottom: 24
 }
 },

 reffb91bfe1 := {
 bits: MakeBinaryFromHex(
 "000000000004000400040003001C001C00FFFC0003FFFF0003FFFF0007FFFF8007FFCF8007E79780"
 "07C3B7801E1971807FFD7F80F7FC7F80C3FEFF00E3FFFF0061FFFC007070000031E0C000398CE000"
 "183C60001CF0E0000CC3E0000E0F8000063E000006F8000003E0000003800000",
 'bits),
 bounds: {
 left: 0,
 top: 0,
 right: 25,
 bottom: 24
 }
 },

 icon: {
 mask: MakeBinaryFromHex(
 "000000000004000500050000001B001F07FFFFC00FFFFFE01FFFFFF03FFFFFF87FFFFFFCFFFFFFFE"
 "FFFFFFFEFFFFFFFEFFFFFFFEFFFFFFFEFFFFFFFEFFFFFFFEFFFFFFFEFFFFFFFEFFFFFFFEFFFFFFFE"
 "FFFFFFFEFFFFFFFEFFFFFFFEFFFFFFFEFFFFFFFE00000000",
 'mask),
 bits: MakeBinaryFromHex(
 "000000000004000500050000001B001F07FFFFC00800002010000010200000084000000480000002"
 "80000002800000028000000280000002800000028000000280000002800000028000000280000002"
 "80000002800000028000000280000002FFFFFFFE00000000",
 'bits),
 bounds: {
 left: 0,
 top: 0,
 right: 31,
 bottom: 22
 }
 },


 icon: {
 mask: MakeBinaryFromHex(
 "000000000004000A000A0001001C001F0000000C000001FC00003FFC0007FFFC00FFFFFC1FFFFFFC"
 "FFFFFFFCFFFFFFFCFFFFFFFCFFFFFFFCFFFFFFFCFFFFFFFCFFFFFFFCFFFFFFFCFFFFFFFCFFFFFFFC"
 "FFFFFFFC00000000",
 'mask),
 bits: MakeBinaryFromHex(
 "000000000004000A000A0001001C001F0000000C000001F400003E040007C00400F800041F000004"
 "E0000004800000048000000480000004800000048000000480000004800000048000000480000004"
 "FFFFFFFC00000000",
 'bits),
 bounds: {
 left: 0,
 top: 0,
 right: 30,
 bottom: 18
 }
 },


 icon: {
 bounds: {
 left: 0,
 top: 0,
 right: 173,
 bottom: 17
 },
 bits: MakeBinaryFromHex(
 "000000000018000000000000001100AD00000000000000000000000000000000001F000000000000"
 "0C7E00000000000000000000000000001F9E0000000000007E6C0000000000000000000000000000"
 "199E00000000000000CC000000000000000000000000000019DEC00000000000FFD8000000000000"
 "000000000000000019FFC0000000000001FF060C1C00000000036C00001800181FFF030000000000"
 "7FD8039E0C000E030001B00000CC060C063E01800DF0000000D8033C6F01C601BF018000007C03CF"
 "061E019C076000007EFE1F983E00FF036C1FFC00006C067E1F9F07F600C0000000D80ED81BE06D83"
 "180D80C0606C060C1E3F83E30180000000D806CC18C0F8C60007B07FF06CC60C1EFEC1C303F00000"
 "7EFE0CFC1981F8C780079800006D878C1EDEC3830618000066D80CCC3001B0C7800D8C00006F07BC"
 "1E36079F0C18000066D81EC03601F987601F8C0000CE076E1FB6C7B703D8000066D81B806600C303"
 "3F038000018C036F1C66C1B7067000007EFF018063F00E03000180000300033800C3E19D83E00000"
 "60C000000000000000000000000000000000000000000000",
 'bits),
 colorData: {
 cbits: MakeBinaryFromHex(
 "000000000018000000000000001100AD00000000000000000000000000000000001F000000000000"
 "0C7E00000000000000000000000000001F9E0000000000007E6C0000000000000000000000000000"
 "199E00000000000000CC000000000000000000000000000019DEC00000000000FFD8000000000000"
 "000000000000000019FFC0000000000001FF060C1C00000000036C00001800181FFF030000000000"
 "7FD8039E0C000E030001B00000CC060C063E01800DF0000000D8033C6F01C601BF018000007C03CF"
 "061E019C076000007EFE1F983E00FF036C1FFC00006C067E1F9F07F600C0000000D80ED81BE06D83"
 "180D80C0606C060C1E3F83E30180000000D806CC18C0F8C60007B07FF06CC60C1EFEC1C303F00000"
 "7EFE0CFC1981F8C780079800006D878C1EDEC3830618000066D80CCC3001B0C7800D8C00006F07BC"
 "1E36079F0C18000066D81EC03601F987601F8C0000CE076E1FB6C7B703D8000066D81B806600C303"
 "3F038000018C036F1C66C1B7067000007EFF018063F00E03000180000300033800C3E19D83E00000"
 "60C000000000000000000000000000000000000000000000",
 'bits),
 bitDepth: 1
 }
 },

 ref2d66bfe1 := {
 data: MakeBinaryFromHex(
 "368800000000015E00FD001102FF0C00FFFE0000004800000048000000000000015E00FD00000000"
 "0001000A00000000015E00FD0098808000000000015E00FD00000000000000000048000000480000"
 "00000004000100040000000001B5214000000000000000248000000F0000FFFFFFFFFFFF0001EEEE"
 "EEEEEEEE0002DDDDDDDDDDDD0003CCCCCCCCCCCC0004BBBBBBBBBBBB0005AAAAAAAAAAAA00069999"
...
 "000DE7DD00DBB300010CDC0DEC0000BDE8DD00C0B300010DDC0EEC00010BBCEADD00DBB200010CDD"
 "18E70002B9BB9BF9BB05B0BBBDDDCDDBFEBB00B0B200010CFC058300010344058300010CDC0000FF",
 'picture)
 }

 icon: {
 bits: MakeBinaryFromHex(
 "000000000004001400140014002000200F0000003FC0000070E0000066600000C0300000CE300000"
 "C6300000C63000006F60000070E000003FC000000F000000",
 'bits),
 bounds: {
 left: 0,
 top: 0,
 right: 12,
 bottom: 12
 }
 },




 icon: {
 bounds: {
 left: 0,
 top: 0,
 right: 5,
 bottom: 9
 },
 bits: MakeBinaryFromHex(
 "00000000000400000000000000090005700000007000000000000000F00000007000000070000000"
 "7000000070000000F8000000",
 'bits),
 colorData: [
 {
 cbits: MakeBinaryFromHex(
 "0000000000080000000000000009000500FFFFFF0000000000FFFFFF000000000000000000000000"
 "FFFFFFFF0000000000FFFFFF0000000000FFFFFF0000000000FFFFFF0000000000FFFFFF00000000"
 "FFFFFFFFFF000000",
 'cbits),
 bitDepth: 8
 },
 {
 cbits: MakeBinaryFromHex(
 "000000000004000000000000000900050FFF00000FFF000000000000FFFF00000FFF00000FFF0000"
 "0FFF00000FFF0000FFFFF000",
 'cbits),
 bitDepth: 4
 },
 {
 cbits: MakeBinaryFromHex(
 "000000000004000000000000000900053F0000003F00000000000000FF0000003F0000003F000000"
 "3F0000003F000000FFC00000",
 'cbits),
 bitDepth: 2
 },
 {
 cbits: MakeBinaryFromHex(
 "0000 0000 0004 0000 0000 0000 0009 0005 70000000 70000000 00000000 F0000000 70000000 70000000"
 "70000000 70000000 F8000000",
 'bits),
 bitDepth: 1
 }
 ],
 mask: MakeBinaryFromHex(
 "00000000000400000000000000090005700000007000000000000000F00000007000000070000000"
 "7000000070000000F8000000",
 'mask)
 },


icon: {
  bounds: {
	left: 0,
	top: 0,
	right: 19,    // our icon is 19 bits wide
	bottom: 18    // and 18 bits high
  },
  bits: MakeBinaryFromHex(
	"0000 0000 0004 0000 0000 0000 0012 0013" // 8 shorts, ? ? bytes_per_row ? x, y, bottom, right
	"03F800000C06000018030000200080004000400040004000"
	"8000200080E0200081102000811020008110200080E0200040004000400040002000800018030000"
	"0C06000003F80000",
	// 176 characters = 88 bytes = 72 bytes data = 72/18 rows = 4 bytes per row
	// pad bytes per row to 32 bit?
	'bits),
  mask: MakeBinaryFromHex(
 "00000000000400000000000000120013 03F800000C06000018030000200080004000400040004000"
 "8000200080E0200081102000811020008110200080E0200040004000400040002000800018030000"
 "0C06000003F80000",
 'mask),
 colorData: [
 {
 cbits: MakeBinaryFromHex(
 "0000 0000 0014 0000 0000 0000 0012 0013"
 "000000000000FFFFFFFFFFFFFF00000000000000"
 "00000000FFFF44444444444444FFFF0000000000"
 "000000FFFF443344333344334433FFFF00000000"
 "0000FF33333333223333333322333333FF000000"
 "00FF222222222222222222222222222233FF0000"
 "00FF221122112211221122112222112211FF0000"
 "FF1111110011111100111111001111111111FF00"
 "FF00000011000000FFFFFF00110000000000FF00"
 "FF333333223333FF000000FF333333333322FF00"
 "FF555555665566FF000000FF555555555566FF00"
 "FF332233223322FF000000FF333333223322FF00"
 "FF00000000000000FFFFFF00000000000000FF00"
 "00FF111111111111111111111111111111FF0000"
 "00FF112211221122112211221111221122FF0000"
 "0000FF22332222223322222233222222FF000000"
 "000000FFFF333333333333333333FFFF00000000"
 "00000000FFFF33443333443344FFFF0000000000"
 "000000000000FFFFFFFFFFFFFF00000000000000", // 376 bytes
 'cbits),
 bitDepth: 8
 },
 {
 cbits: MakeBinaryFromHex(
 "0000 0000 000C 0000 0000 0000 0012 0013"
 "000000FFFFFFF000000000000000FF4444444FF000000000"
 "000FF434334343FF0000000000F3333233332333F00000000F222222222222223F0000000F212121"
 "212122121F000000F11101110111011111F00000F0001000FFF0100000F00000F333233F000F3333"
 "32F00000F555656F000F555556F00000F323232F000F333232F00000F0000000FFF0000000F00000"
 "0F111111111111111F0000000F121212121211212F00000000F2322232223222F0000000000FF333"
 "333333FF000000000000FF3433434FF000000000000000FFFFFFF00000000000",
 'cbits),
 bitDepth: 4
 },
 {
 cbits: MakeBinaryFromHex(
 "0000 0000 0008 0000 0000 0000 0012 0013"
 "000FFFC00000000000F5557C0000000003D5555F00000000"
 "0D545515C000000030000000700000003000000030000000C00000000C000000C000FC000C000000"
 "D51703554C000000D55703555C000000D11303544C000000C000FC000C0000003000000030000000"
 "30000000300000000C404040C000000003D5555F0000000000F5557C00000000000FFFC000000000",
 'cbits),
 bitDepth: 2
 },
 {
 cbits: MakeBinaryFromHex(
 "0000000000040000000000000012001303F800000C06000018030000200080004000400040004000"
 "8000200080E0200081102000811020008110200080E0200040004000400040002000800018030000"
 "0C06000003F80000",
 'bits),
 bitDepth: 1
 }
 ]
 },


 gray =    (.30 * red value) +
 (.59 * green value) +
 (.11 blue value)

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
