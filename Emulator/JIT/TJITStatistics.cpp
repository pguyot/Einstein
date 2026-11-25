// ==============================
// File:			JITStatistics.cpp
// Project:			Einstein
//
// Copyright 2003-2026 by Paul Guyot (pguyot@kallisys.net).
//                     and Matthias Melcher (m.melcher@robowerk.com)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
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

#include "TJITStatistics.h"

#ifdef JIT_ENABLE_STATISTICS

#include "Emulator/TEmulator.h"
#include "Emulator/TInterruptManager.h"
#include "Monitor/TSymbolList.h"

#include <stdarg.h>
#include <chrono>
#include <thread>

TJITStatistics gJITStatistics;

void
TJITStatistics::write_all()
{
#ifdef JIT_STATISTIC_EXEC_COUNT
	write_exec_count();
#endif
#ifdef JIT_STATISTIC_EXEC_ORDER
	write_exec_order();
#endif
}

void
TJITStatistics::hit(KUInt32 at)
{
	(void) at;
	KUInt32 index = at >> 2;
	(void) index;
#ifdef JIT_STATISTIC_EXEC_COUNT
	if (index < k8MBWords)
	{
		uint64_t& count = mExecCount[index];
		if (count < 0xffffffffffffffffULL)
			count++;
	}
#endif
#ifdef JIT_STATISTIC_EXEC_ORDER
	if (index < k8MBWords)
	{
		uint32_t& order = mExecOrder[index];
		if (order == 0)
			order = mExecOrderIndex++;
	}
#endif
}

#ifdef JIT_STATISTIC_EXEC_COUNT
void
TJITStatistics::write_exec_count()
{
	FILE* out = fopen(JIT_STATISTICS_PATH "exec_count.bin", "wb");
	if (!out)
		return;
	fwrite(mExecCount, sizeof(uint64_t), k8MBWords, out);
	fclose(out);
}
#endif // JIT_STATISTIC_EXEC_COUNT

#ifdef JIT_STATISTIC_EXEC_ORDER
void
TJITStatistics::write_exec_order()
{
	FILE* out = fopen(JIT_STATISTICS_PATH "exec_order.bin", "wb");
	if (!out)
		return;
	fwrite(mExecOrder, sizeof(uint32_t), k8MBWords, out);
	fclose(out);
}
#endif // JIT_STATISTIC_EXEC_ORDER

#endif // JIT_ENABLE_STATISTICS
