// ==============================
// File:                        sys/error.h
// Project:                     Albert
//
// Copyright 2003-2008 by Matthias Melcher (albert@matthiasm.com).
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

#ifndef ALBERT_SYS_ERROR_H
#define ALBERT_SYS_ERROR_H

#define ERRBASE_OS -10000

#define noErr 0

// ---------------  Result codes…  -------------

#define kCallInProgress                 (1)
#define kEnterMonitor                   (2)
#define kExitFromFaultMonitor						(3)				// fault monitor exit code
#define kSuspendTaskWaitForMemory				(4)				// suspend this task on way out of monitor until memory becomes available
#define kSuspendTaskOnMemoryQ						(5)				// suspend task on memory queue for fault monitors

// ---------------  OS errors…  -------------

#define kOSErrBadDomainObjectId					(ERRBASE_OS)
#define kOSErrBadPhysicalPageObjectId			(ERRBASE_OS - 1)
#define kOSErrUnexpectedObjectType				(ERRBASE_OS - 2)
#define kOSErrNoPageTable							(ERRBASE_OS - 3)
#define kOSErrAllocOnUninitalizedHeap			(ERRBASE_OS - 4)
#define kOSErrCallNotImplemented					(ERRBASE_OS - 5)
#define kOSErrBadParameters						(ERRBASE_OS - 6)
#define kOSErrNoMemory								(ERRBASE_OS - 7)
#define kOSErrItemNotFound							(ERRBASE_OS - 8)
#define kOSErrCouldNotCreateObject				(ERRBASE_OS - 9)
#define kOSErrMustUseRPC							(ERRBASE_OS - 10)
#define kOSErrBadObject								(ERRBASE_OS - 11)
#define kOSErrNotUserCall							(ERRBASE_OS - 12)
#define kOSErrTaskDoesNotExist					(ERRBASE_OS - 13)
#define kOSErrMsgDoneNotExpected					(ERRBASE_OS - 14)
#define kOSErrBadObjectId							(ERRBASE_OS - 15)
#define kOSErrBadMessageObjectId					(ERRBASE_OS - 16)
#define kOSErrMessageAlreadyPosted				(ERRBASE_OS - 17)
#define kOSErrCantCashToken						(ERRBASE_OS - 18)
#define kOSErrPortNoLongerExists					(ERRBASE_OS - 19)
#define kOSErrNoMessageWaiting					(ERRBASE_OS - 20)
#define kOSErrMessageTimedOut						(ERRBASE_OS - 21)
#define kOSErrBadSemaphoreGroupId				(ERRBASE_OS - 22)
#define kOSErrBadSemaphoreOpListId				(ERRBASE_OS - 23)
#define kOSErrSemaphoreGroupNoLongerExists	(ERRBASE_OS - 24)
#define kOSErrSemaphoreWouldCauseBlock			(ERRBASE_OS - 25)
#define kOSErrTaskNoLongerExists					(ERRBASE_OS - 26)
#define kOSErrTaskAborted							(ERRBASE_OS - 27)
#define kOSErrCannotSuspendBlockedTask			(ERRBASE_OS - 28)	// *** this will go away
#define kOSErrBadRegisterNumber					(ERRBASE_OS - 29)
#define kOSErrBadMonitorFunction					(ERRBASE_OS - 30)
#define kOSErrNoSuchMonitor						(ERRBASE_OS - 31)
#define kOSErrNotAMonitor							(ERRBASE_OS - 32)
#define kOSErrSizeTooLargeCopyTruncated		(ERRBASE_OS - 33) // size was too large in shared memory call (copy truncated)
#define kOSErrSMemModeViolation					(ERRBASE_OS - 34)
#define kOSErrObjectNotOwnedByTask				(ERRBASE_OS - 35)
#define kOSErrObjectNotAssignedToTask			(ERRBASE_OS - 36)
#define kOSErrImTotallyConfused					(ERRBASE_OS - 37)
#define kOSErrAnotherTaskAlreadyBlocking		(ERRBASE_OS - 38)
#define kOSErrCallAborted							(ERRBASE_OS - 39)
#define kOSErrObjectAlreadyInitialized			(ERRBASE_OS - 40)
#define kOSErrNestedCollection					(ERRBASE_OS - 41)
#define kOSErrSharedMemMsgNoLongerExists		(ERRBASE_OS - 42)
#define kOSErrReceiverDidNotDoRPC				(ERRBASE_OS - 43)
#define kOSErrCopyAborted							(ERRBASE_OS - 44)
#define kOSErrBadSignature							(ERRBASE_OS - 45)
#define kOSErrCallNotInProgress					(ERRBASE_OS - 46)
#define kOSErrTokenExpected						(ERRBASE_OS - 47)
#define kOSErrReceiverObjectNoLongerExists	(ERRBASE_OS - 48)
#define kOSErrMonitorNotSuspsended				(ERRBASE_OS - 49)
#define kOSErrNotAFaultMonitor					(ERRBASE_OS - 50)
#define kOSErrNoAvailablePage						(ERRBASE_OS - 51)
#define kOSErrInterruptNotEnabled 				(ERRBASE_OS - 52)
#define kOSErrInterruptNotImplemented 			(ERRBASE_OS - 53)
#define kOSErrTricInterruptNotEnabled 			(ERRBASE_OS - 54)
#define kOSErrTricInterruptNotImplemented 	(ERRBASE_OS - 55)
#define kOSErrUnResolvedFault				 		(ERRBASE_OS - 56)
#define kOSErrCallAlreadyInProgress		 		(ERRBASE_OS - 57)
#define kOSErrOffsetBeyondData			 		(ERRBASE_OS - 58)
#define kOSErrBusAccess								(ERRBASE_OS - 59)
#define kOSErrAccessPermission					(ERRBASE_OS - 60)
#define kOSErrPermissionViolation			 	(ERRBASE_OS - 61)	// the dreaded -10061 !!
#define kOSErrDuplicateObject						(ERRBASE_OS - 62)
#define kOSErrIllFormedDomain						(ERRBASE_OS - 63)
#define kOSErrOutOfDomains							(ERRBASE_OS - 64)
#define kOSErrWriteProtected						(ERRBASE_OS - 65)
#define kOSErrTimerExpired							(ERRBASE_OS - 66)
#define kOSErrNotRegistered						(ERRBASE_OS - 67)
#define kOSErrAlreadyRegistered					(ERRBASE_OS - 68)
#define kOSErrRebootPowerFault					(ERRBASE_OS - 69)
#define kOSErrRebootBatteryFault					(ERRBASE_OS - 70)
#define kOSErrRebootNoCard							(ERRBASE_OS - 72)
#define kOSErrRAMTableFull							(ERRBASE_OS - 73)
#define kOSErrUnableToSatisfyRequest			(ERRBASE_OS - 74)
#define kOSErrSorrySystemError					(ERRBASE_OS - 75)
#define kOSErrSorrySystemFailure					(ERRBASE_OS - 76)
#define kOSErrNewSystemSoftware					(ERRBASE_OS - 77)
#define kOSErrResourceClaimed						(ERRBASE_OS - 78)
#define kOSErrResourceUnclaimed					(ERRBASE_OS - 79)
#define kOSErrRebootCalibrationMissing			(ERRBASE_OS - 80)
#define kOSErrObsoleteHardware					(ERRBASE_OS - 81)
#define kOSErrRebootFromResetSwitch				(ERRBASE_OS - 82)
#define kOSErrRebootAllPowerRemoved				(ERRBASE_OS - 83)
#define kOSErrNewROMInstalled						(ERRBASE_OS - 84)
#define kOSErrRebootBatteryChanged				(ERRBASE_OS - 85)

// ---------------  Stack errors…  -------------

#define kStackErrStackTooSmall					(ERRBASE_OS - 200)
#define kStackErrNoRoomForHeap					(ERRBASE_OS - 201)
#define kStackErrBogusStack						(ERRBASE_OS - 202)
#define kStackErrStackOverflow					(ERRBASE_OS - 203)
#define kStackErrStackUnderflow					(ERRBASE_OS - 204)
#define kStackErrAddressOutOfRange				(ERRBASE_OS - 205)
#define kStackErrBadDomain							(ERRBASE_OS - 206)

// ---------------  RDM errors…  -------------

#define kOSErrUnsupportedRequest					(ERRBASE_OS - 301)

// ---------------  Package errors…  -------------

#define kOSErrBadPackage							(ERRBASE_OS - 401)
#define kOSErrPackageAlreadyExists			   (ERRBASE_OS - 402)
#define kOSErrBadPackageVersion					(ERRBASE_OS - 403)
#define kOSErrUnexpectedEndOfPackage			(ERRBASE_OS - 404)
#define kOSErrUnexpectedEndOfPkgPart			(ERRBASE_OS - 405)
#define kOSErrPartTypeAlreadyRegistered		(ERRBASE_OS - 406)
#define kOSErrPartTypeNotRegistered				(ERRBASE_OS - 407)
#define kOSErrNoSuchPackage						(ERRBASE_OS - 408)
#define kOSErrNewerPackageAlreadyExists		(ERRBASE_OS - 409)
#define kOSErrOlderPackageAlreadyExists		(ERRBASE_OS - 410)
#define kOSErrPackageBusy							(ERRBASE_OS - 411)
#define kOSErrXIPNotPossible						(ERRBASE_OS - 412)

// ---------------  Card errors…  -------------

#define kCardErrUnrecognized 						(ERRBASE_OS - 501)
#define kCardErrNotReady	 						(ERRBASE_OS - 502)
#define kCardErrBadPower	 						(ERRBASE_OS - 503)
#define kCardErrUnspecified						(ERRBASE_OS - 504)
#define kCardErrReset								(ERRBASE_OS - 505)
#define kCardErrNotInitialized					(ERRBASE_OS - 506)
#define kCardErrServiceNotInstalled				(ERRBASE_OS - 507)
#define kCardErrServiceNotSuspend				(ERRBASE_OS - 508)
#define kCardErrServiceNotResume					(ERRBASE_OS - 509)
#define kCardErrNoUsableConfigurations			(ERRBASE_OS - 510)
#define kCardErrFormatFailed	 					(ERRBASE_OS - 511)
#define kCardErrUnformattedWriteProtected		(ERRBASE_OS - 512)

#define kCardErrParserProcPtr						(ERRBASE_OS - 520)
#define kCardErrTupleUnknown						(ERRBASE_OS - 521)
#define kCardErrSubTupleUnknown					(ERRBASE_OS - 522)
#define kCardErrTupleOrder							(ERRBASE_OS - 523)
#define kCardErrTupleSize							(ERRBASE_OS - 524)
#define kCardErrTupleNoLinkAndLink				(ERRBASE_OS - 525)
#define kCardErrTupleLinkAndNoLink				(ERRBASE_OS - 526)
#define kCardErrTupleLinkTarget					(ERRBASE_OS - 527)
#define kCardErrTupleVersion1						(ERRBASE_OS - 528)
#define kCardErrTupleVersion2						(ERRBASE_OS - 529)
#define kCardErrTupleJedec							(ERRBASE_OS - 530)
#define kCardErrChecksum							(ERRBASE_OS - 531)
#define kCardErrNoCIS								(ERRBASE_OS - 532)		// First tuple is 0xFF
#define kCardErrBlankCIS							(ERRBASE_OS - 533)		// All 0's for 16 bytes
#define kCardErrBadCIS								(ERRBASE_OS - 534)		// Bad CIS
#define kCardErrLinkTargetC						(ERRBASE_OS - 535)

// ---------------  Flash errors…  -------------

#define kFlashErrBusy								(ERRBASE_OS - 551)
#define kFlashErrNotErasing						(ERRBASE_OS - 552)
#define kFlashErrNotEraseSuspend					(ERRBASE_OS - 553)
#define kFlashErrSuspendErase						(ERRBASE_OS - 554)
#define kFlashErrEraseFailed						(ERRBASE_OS - 555)
#define kFlashErrWriteFailed						(ERRBASE_OS - 556)
#define kFlashErrVppLow								(ERRBASE_OS - 557)
#define kFlashErrInSleep							(ERRBASE_OS - 558)
#define kFlashErrNotEnoughPower					(ERRBASE_OS - 559)
#define kFlashErrTimeOut							(ERRBASE_OS - 560)
#define kFlashErrUnsupportedConfiguration		(ERRBASE_OS - 561)
#define kFlashErrAddressOutOfRange				(ERRBASE_OS - 562)
#define kFlashErrNoFlashInMotherBoard			(ERRBASE_OS - 563)

// ---------------  ATA errors…  -------------

#define kATAErrAddressMarkNotFound				(ERRBASE_OS - 570)
#define kATAErrTrack0NotFound						(ERRBASE_OS - 571)
#define kATAErrAbortedCommand						(ERRBASE_OS - 572)
#define kATAErrMediaChangedRequested			(ERRBASE_OS - 573)
#define kATAErrSectorIdNotFound					(ERRBASE_OS - 574)
#define kATAErrMediaChanged						(ERRBASE_OS - 575)
#define kATAErrUncorrectedError					(ERRBASE_OS - 576)
#define kATAErrBadBlockDetected					(ERRBASE_OS - 577)
#define kATAErrUnknownError						(ERRBASE_OS - 578)
#define kATAErrBusy									(ERRBASE_OS - 579)
#define kATAErrNotReady								(ERRBASE_OS - 580)
#define kATAErrNoDRQ									(ERRBASE_OS - 581)
#define kATAErrNoPartition							(ERRBASE_OS - 582)
#define kATAErrBadPartition						(ERRBASE_OS - 583)
#define kATAErrPartitionChecksum					(ERRBASE_OS - 584)

// ---------------  Store errors…  -------------

#define kStoreErrObjectOverRun					(ERRBASE_OS - 600)	/* attempt to read or write outside object bounds */
#define kStoreErrBadBufferPtr						(ERRBASE_OS - 601)	/* was "bad object id" in heap-object-store world */
#define kStoreErrBadCardAccess					(ERRBASE_OS - 602)
#define kStoreErrBadStorageType					(ERRBASE_OS - 603)
#define kStoreErrStoreNotFound					(ERRBASE_OS - 604)
#define kStoreErrWriteProtected					(ERRBASE_OS - 605)	/* user has protected the store */
#define kStoreErrObjectNotFound					(ERRBASE_OS - 606)	/* non-existent object */
#define kStoreErrBlockFull							(ERRBASE_OS - 607)	/* flash internal condition (means "try somewhere else") */
#define kStoreErrNotVirgin							(ERRBASE_OS - 608)	/* (internal condition) retry write somewhere else */
#define kStoreErrWriteError						(ERRBASE_OS - 609)	/* one or more bits failed to assert */
#define kStoreErrNoMoreObjects					(ERRBASE_OS - 610)	/* flash internal condition (TFlashBlock::NextObject ran out of objects) */
#define kStoreErrEraseInProgress					(ERRBASE_OS - 611)	/* flash internal condition: erase in progress */
#define kStoreErrCardFull							(ERRBASE_OS - 612)	/* flash internal condition: no more blocks left on the card */
#define kStoreErrNoMoreBlocks						(ERRBASE_OS - 613)	/* flash internal condition: no more blocks left in search */
#define kStoreErrLogFull							(ERRBASE_OS - 614)	/* flash log is full ("cannot happen") */
#define kStoreErrNeedsFormat						(ERRBASE_OS - 615)	/* card needs to be formatted */
#define kStoreErrBadPSSID							(ERRBASE_OS - 616)	/* totally martian PSSID */
#define kStoreErrStoreFull							(ERRBASE_OS - 617)	/* we're tapped-out */
#define kStoreErrBadBattery						(ERRBASE_OS - 618)	/* missing or low battery on SRAM card */
#define kStoreErrNotInTransaction				(ERRBASE_OS - 619)	/* attempt to modify store w/o a transaction in effect */
#define kStoreErrTransactionAborted				(ERRBASE_OS - 620)	/* transaction was aborted */
#define kStoreErrWPButNeedsRepair				(ERRBASE_OS - 621)	/* needs recovery, but card is write protected */
#define kStoreErrObjectTooBig						(ERRBASE_OS - 622)	/* object too big for store */
#define kOSErrReservedBlockInvalidObject		(ERRBASE_OS - 700)	/* ~valid flag for object */

// ---------------  DMA errors…  -------------

#define kDMAErrMode									(ERRBASE_OS - 800)
#define kDMAErrBusAccess							(ERRBASE_OS - 801)
#define kDMAErrBufferNotExist						(ERRBASE_OS - 802)
#define kDMAErrAddrWordAligned					(ERRBASE_OS - 803)
#define kDMAErrCountWordAligned					(ERRBASE_OS - 804)
#define kDMAErrCountSize							(ERRBASE_OS - 805)
#define kDMAErrOffsetSize							(ERRBASE_OS - 806)
#define kDMAErrPCMCIAReady							(ERRBASE_OS - 820)
#define kDMAErrPCMCIAInputAck						(ERRBASE_OS - 821)
#define kDMAErrPCMCIAWriteProtect				(ERRBASE_OS - 822)
#define kDMAErrPCMCIATimeOut						(ERRBASE_OS - 823)

/*	----------------------------------------------------------------
 **
 **	Results of CheckSafeHeap() and SSafeHeapPage::Check()
 **	 (various ways that heaps go wrong that we want to check)
 **
 */
#define kSafeHeapErrOddBlockSize					(ERRBASE_OS - 900)
#define kSafeHeapErrBlockOutOfRange				(ERRBASE_OS - 901)
#define kSafeHeapErrPreferredFreeNotFound 	(ERRBASE_OS - 902)
#define kSafeHeapErrFreeAccountingError 		(ERRBASE_OS - 903)
#define kSafeHeapErrAccountingError 			(ERRBASE_OS - 904)
#define kSafeHeapErrBlockTooBig					(ERRBASE_OS - 905)
#define kSafeHeapErrBadPriorPtr					(ERRBASE_OS - 906)
#define kSafeHeapErrBadLastPtrInPrior			(ERRBASE_OS - 907)
#define kSafeHeapErrBadLastPtrInLast			(ERRBASE_OS - 908)

// ---------------  Interrupt errors…  -------------

#define kIntErrCouldNotRemoveInterrupt			(ERRBASE_INTERRUPT - 4)
#define kIntErrCouldNotCreateInterrupt			(ERRBASE_INTERRUPT - 5)
#define kIntErrInterruptAlreadyExists			(ERRBASE_INTERRUPT - 6)

#define kNameAlreadyRegistered	kError_Already_Registered
#define kNoSuchNameRegistered		kError_Not_Registered

#endif
// ALBERT_SYS_ERROR_H

