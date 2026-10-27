// ==============================
// File:            LanternDriverAPI.cpp
// Project:         Einstein
//
// Copyright 2026 by Matthias Melcher.
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

// Calls into a protocol implementation, as ProtocolGen generated them for the
// MPW tools: the object at this+4 is the implementation, and its branch table
// at +8 has an entry per method, after the reserved word, ClassInfo, New and
// Delete. The offsets are the same as in the glue of the NE2K.pkg of 1997.

#include "LanternDriver.h"

#define LANTERN_API_CALL(offset)       \
	asm volatile(                      \
		"ldr r0, [r0, #4]\n"           \
		"ldr r12, [r0, #8]\n"          \
		"add pc, r12, #" #offset "\n")

__attribute__((naked)) NewtonErr
TLanternDriverAPI::Init(TLanternTask*)
{
	LANTERN_API_CALL(16);
}

__attribute__((naked)) NewtonErr
TLanternDriverAPI::PowerOn(Boolean*, Boolean)
{
	LANTERN_API_CALL(20);
}

__attribute__((naked)) NewtonErr
TLanternDriverAPI::PowerOff(ULong, Boolean)
{
	LANTERN_API_CALL(24);
}

__attribute__((naked)) Ptr
TLanternDriverAPI::NewPacketPtr(Size)
{
	LANTERN_API_CALL(28);
}

__attribute__((naked)) NewtonErr
TLanternDriverAPI::RxReady(Ptr, Size)
{
	LANTERN_API_CALL(32);
}

__attribute__((naked)) NewtonErr
TLanternDriverAPI::InterruptDone(void)
{
	LANTERN_API_CALL(36);
}

__attribute__((naked)) NewtonErr
TLanternDriverAPI::CardHandlerSpecific(ULong, ...)
{
	LANTERN_API_CALL(40);
}

__attribute__((naked)) NewtonErr
TLanternDriverAPI::AddEventHandler(kLanternEventType, DriverProcPtr)
{
	LANTERN_API_CALL(44);
}

__attribute__((naked)) NewtonErr
TLanternDriverAPI::PostEvent(kLanternEventType, ...)
{
	LANTERN_API_CALL(48);
}

__attribute__((naked)) NewtonErr
TLanternDriverAPI::PostLocalEvent(kLanternEventType, ...)
{
	LANTERN_API_CALL(52);
}

__attribute__((naked)) NewtonErr
TLanternDriverAPI::PostLocalEvent(TTime*, kLanternEventType, ...)
{
	LANTERN_API_CALL(56);
}

__attribute__((naked)) NewtonErr
TLanternDriverAPI::PostReply(NewtonErr)
{
	LANTERN_API_CALL(60);
}

__attribute__((naked)) NewtonErr
TLanternDriverAPI::PostReply(NewtonErr, ULong, ...)
{
	LANTERN_API_CALL(64);
}
