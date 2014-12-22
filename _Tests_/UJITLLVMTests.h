// ==============================
// File:			UJITLLVMTests.h
// Project:			Einstein
//
// Copyright 2014 by Paul Guyot (pguyot@kallisys.net).
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

#ifndef _UJITLLVMTESTS_H
#define _UJITLLVMTESTS_H

class TLog;

///
/// Class to test various JITLLVM features.
///
class UJITLLVMTests
{
public:
	///
	/// Test translation to IR.
	///
	static void TranslateEntryPoint( const char* inHexWords, TLog* inLog );

	///
	/// Test translation to IR.
	///
	static void TranslateInstruction( const char* inHexWord, TLog* inLog );
};

#endif