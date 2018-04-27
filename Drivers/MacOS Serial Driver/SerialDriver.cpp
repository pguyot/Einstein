
#include "SerialDriver.h"

#include <IOKit/IOLib.h>
#include <IOKit/IOTypes.h>
#include <IOKit/IOMessage.h>
#include <IOKit/IOWorkLoop.h>

#include <IOKit/serial/IOSerialKeys.h>


// This required macro defines the class's constructors, destructors,
// and several other methods I/O Kit requires.
OSDefineMetaClassAndStructors(com_matthiasm_elektriktrick_SerialDriver, IOSerialDriverSync);

// Define the driver's superclass.
#define super IOSerialDriverSync

#define STATE_ALL (PD_RS232_S_MASK | PD_S_MASK)
#define STATE_EXTERNAL (PD_S_MASK | (PD_RS232_S_MASK & ~PD_RS232_S_LOOP))


bool com_matthiasm_elektriktrick_SerialDriver::init(OSDictionary *dict)
{
	bool result = super::init(dict);
	IOLog("matthiasm: Initializing\n");

	pNub = nullptr;
	pWorkLoop = nullptr;
	pState = 0;
	pWatchState = 0;
	pLock = IOLockAlloc();

	return result;
}

void com_matthiasm_elektriktrick_SerialDriver::free(void)
{
	IOLog("matthiasm: Freeing\n");
	IOLockFree(pLock);
	super::free();
}

IOService *com_matthiasm_elektriktrick_SerialDriver::probe(IOService *provider,
														   SInt32 *score)
{
	IOService *result = super::probe(provider, score);
	IOLog("matthiasm: Probing (%p)\n", result);
	return result;
}

bool com_matthiasm_elektriktrick_SerialDriver::start(IOService *provider)
{
	IOLog("matthiasm: Starting...\n");

	// start the super class
	bool ret = super::start(provider);
	if (ret==false) {
		IOLog("matthiasm: ERROR: super won't start\n");
		return false;
	}

	// create and initialise the link to the BSD file system
	if (!pNub) {
		pNub = new IORS232SerialStreamSync;
		if (!pNub) {
			IOLog("matthiasm: ERROR: IORS232SerialStreamSync ctor failed\n");
			return false;
		}
	}

	ret = pNub->init(nullptr, nullptr);  // ?? optional dictionary and user pointer
	if (ret==false) {
		IOLog("matthiasm: ERROR: won't init nub\n");
		return false;
	}

	ret = pNub->attach(this);
	if (ret==false) {
		IOLog("matthiasm: ERROR: won't attach nub to driver\n");
		return false;
	}

	pNub->setProperty(kIOTTYBaseNameKey, "Einstein");
	pNub->setProperty(kIOTTYSuffixKey, "");

	pNub->registerService();

	// create and initialize our event loop
	if (!pWorkLoop) {
		pWorkLoop = IOWorkLoop::workLoop();
		if (!pWorkLoop) {
			IOLog("matthiasm: ERROR: can't find Work Loop\n");
			return false;
		}
		pWorkLoop->retain();
	}


	IOLog("matthiasm: Starting: success\n");
	return true;
}


void com_matthiasm_elektriktrick_SerialDriver::stop(IOService *provider)
{
	IOLog("matthiasm: Stopping\n");

	if (pWorkLoop) {
		pWorkLoop->release();
		pWorkLoop = nullptr;
	}

	if (pNub) {
		//		pNub->getResourceService()->release();
		pNub->removeProperty(kIOTTYSuffixKey);
		pNub->removeProperty(kIOTTYBaseNameKey);
		pNub->detach(this);
		pNub->release();
		pNub = nullptr;
	}

	super::stop(provider);
	IOLog("matthiasm: Stopping: ok\n");
}


/* acquire tests and sets the state of the port object.  If the port was
 * available, then the state is set to busy, and kIOReturnSuccess is returned.
 * If the port was already busy and sleep is YES, then the thread will sleep
 * until the port is freed, then re-attempts the acquire.  If the port was
 * already busy and sleep in NO, then kIOReturnExclusiveAccess is returned.
 */
IOReturn com_matthiasm_elektriktrick_SerialDriver::acquirePort(bool sleep, void *refCon) {
	IOLog("matthiasm: acquirePort\n");

	IOReturn ret = kIOReturnExclusiveAccess;
	IOLockLock(pLock);

	if (sleep && (pState&PD_S_ACQUIRED)) {
		IOLog("matthiasm: NOT SUPPORTED: sleep=true\n");
		/*
		 while (sleep) {
		 // FIXME:
		 UInt32 reqState = 0;
		 IOReturn rtn = watchState(&reqState, PD_S_ACQUIRED, refCon, true);
		 if (rtn != kIOReturnSuccess) {
		 IOLockUnlock(_lock);
		 return rtn;
		 }
		 }
		 */
	}
	if ((pState&PD_S_ACQUIRED)==0) {
		setState(PD_S_ACQUIRED, STATE_ALL, refCon, true);
		// FIXME: startReceive(nullptr);
		ret = kIOReturnSuccess;
	}

	IOLockUnlock(pLock);

	if (ret==kIOReturnExclusiveAccess) {
		IOLog("matthiasm: acquirePort: busy\n");
	}
	if (ret==kIOReturnSuccess) {
		IOLog("matthiasm: acquirePort: ok\n");
	}
	return ret;
}


/* release sets the state of the port object to available and wakes up any
 *threads sleeping for access to this port.  It will return kIOReturnSuccess
 *if the port was in a busy state, and kIOReturnNotOpen if it was available.
 */
IOReturn com_matthiasm_elektriktrick_SerialDriver::releasePort(void *refCon) {
	IOLog("matthiasm: releasePort\n");
	return kIOReturnIOError;
}


/*
 *Set the state for the port device.
 */
IOReturn com_matthiasm_elektriktrick_SerialDriver::setState(UInt32 state, UInt32 mask, void *refCon) {
	return setState(state, mask, refCon, false);
}


IOReturn com_matthiasm_elektriktrick_SerialDriver::setState(UInt32 state, UInt32 mask, void *refCon, bool haveLock) {
	IOLog("matthiasm: setState (0x%08x, 0x%08x)\n", state, mask);
	IOReturn ret = kIOReturnSuccess;

	if (!haveLock) {
		IOLockLock(pLock);
	}

	/* If the port has neither been acquired, nor is it being acquired, then inform the caller that
	 * the port is not open. */
	if ((pState & PD_S_ACQUIRED) == 0 && ((state & mask) & PD_S_ACQUIRED) == 0) {
		IOLog("matthiasm: setState: not open\n");
		ret = kIOReturnNotOpen;
	} else {
		UInt32 newState = (pState & ~mask) | (state & mask);
		UInt32 deltaState = newState ^ pState;
		pState = newState;
		if (deltaState & pWatchState) {
			pWatchState = 0;
			IOLockWakeup(pLock, &pStateEvent, false);
		}
	}

	if (!haveLock) {
		IOLockUnlock(pLock);
	}

	IOLog("matthiasm: setState: done (0x%08x)\n", pState);
	return ret;
}


/*
 *Get the state for the port device.
 */
UInt32 com_matthiasm_elektriktrick_SerialDriver::getState(void *refCon) {
	IOLog("matthiasm: getState (0x%08lx)\n", pState & STATE_EXTERNAL);
	return pState & STATE_EXTERNAL;
}

/*
 *The variables below make up the bits returned
 *by the State methods:- getState, setState, watchState

 #define PD_S_MASK               0xffff0000UL
 #define PD_S_RX_OFFSET          7

 #define PD_S_ACQUIRED           0x80000000UL
 #define PD_S_ACTIVE             0x40000000UL

 #define PD_S_TX_ENABLE          0x20000000UL
 #define PD_S_TX_BUSY            0x10000000UL
 #define PD_S_TX_EVENT           0x08000000UL
 #define PD_S_TXQ_EMPTY          0x04000000UL
 #define PD_S_TXQ_LOW_WATER      0x02000000UL
 #define PD_S_TXQ_HIGH_WATER     0x01000000UL
 #define PD_S_TXQ_FULL           0x00800000UL
 #define PD_S_TXQ_MASK           0x07800000UL

 #define PD_S_RX_ENABLE          0x00400000UL
 #define PD_S_RX_BUSY            0x00200000UL
 #define PD_S_RX_EVENT           0x00100000UL
 #define PD_S_RXQ_EMPTY          0x00080000UL
 #define PD_S_RXQ_LOW_WATER      0x00040000UL
 #define PD_S_RXQ_HIGH_WATER     0x00020000UL
 #define PD_S_RXQ_FULL           0x00010000UL
 #define PD_S_RXQ_MASK           0x000F0000UL
 */

/*
 *Wait for the at least one of the state bits defined in mask to be equal
 *to the value defined in state.
 *Check on entry then sleep until necessary.
 */
IOReturn com_matthiasm_elektriktrick_SerialDriver::watchState(UInt32 *state, UInt32 mask, void *refCon) {
	//  matthiasm: watchState (0x0e000040, 0x18000040)
	// Mask: PD_S_TX_BUSY  turns 0
	//       PD_S_TX_EVENT turns 1
	// watchState(0xe000040, 0x18000040
	return watchState(state, mask, refCon, false);
}

/**
 * Block until the backing state matches @a state, masked by @a mask has changed.
 *
 * @param state The required state values to wait for.
 * @param mask The mask to use when evaluating @a state
 * @param refCon Reference constant.
 * @param haveLock If true, the method will assume that _lock is held. If false, the lock will be acquired
 * automatically.
 */
IOReturn com_matthiasm_elektriktrick_SerialDriver::watchState (UInt32 *state, UInt32 mask, void *refCon, bool haveLock) {
	IOLog("matthiasm: watchState (state: 0x%08x, mask: 0x%08x, is: 0x%08x)\n", state ? *state : 0, mask, pState);

	/* Ensure that state is non-NULL */
	if (state == NULL) {
		IOLog("matthiasm: Watch request with NULL state");
		return kIOReturnBadArgument;
	}

	if (mask == 0) {
		IOLog("matthiasm: Watch request with 0 mask");
		return kIOReturnSuccess;
	}

	/* Acquire our lock */
	if (!haveLock) {
		IOLockLock(pLock);

		//		/* Verify that the driver has not been stopped while the lock was not held */
		//		if (_stopping) {
		//			LOG_DEBUG("watchState() - offline (stopping)");
		//			IOLockUnlock(_lock);
		//			return kIOReturnOffline;
		//		}
	}

	/* Limit mask to EXTERNAL_MASK. There are no comments or documentation describing why this is
	 * necessary, but this matches Apple's USBCDCDMM driver implementation. */
	mask &= STATE_EXTERNAL;

	/* There's nothing left to watch if the port has not been opened. */
	if ((pState & PD_S_ACQUIRED) == 0) {
		if (!haveLock)
			IOLockUnlock(pLock);
		return kIOReturnNotOpen;
	}

	UInt32 watchState = *state;

	/* To properly handle closure of the serial port, we must always track the PD_S_ACTIVE or PD_S_ACQUIRED state.
	 * If the caller is not already doing so, we register our interest here. */
	bool autoActive = false;
	if ((mask & (PD_S_ACQUIRED | PD_S_ACTIVE)) == 0) {
		/* Watch for low on PD_S_ACTIVE bit */
		mask |= PD_S_ACTIVE;
		watchState &= ~PD_S_ACTIVE;

		/* Record that we enabled PD_S_ACTIVE monitoring */
		autoActive = true;
	}

	/* Loop (and block) until one of our watched state values is achieved */
	while (true) {
		/* Determine the currently matching states. We invert the current state mask with ~, and then use ^ to implement
		 * XNOR. Truth table:
		 *
		 * X Y   O
		 * 1 0 = 0
		 * 0 0 = 1
		 * 0 1 = 0
		 * 1 1 = 1
		 */
		UInt32 matched = (watchState ^ pState) & mask;
		if (matched != 0) {
			*state = pState & STATE_EXTERNAL;

			/* Ensure that we drop our lock before returning. No further access to internal
			 * mutable state is required after this. */
			if (!haveLock)
				IOLockUnlock(pLock);

			/* If the port has been closed (and the caller was not tracking PD_S_ACTIVE),
			 * return an error. Otherwise we're just informing the caller that PD_S_ACTIVE was set low,
			 * closing the port. This must necessarily differ from success, as the caller's state changes
			 * of interest have not been detected.
			 */
			if (autoActive && (matched & PD_S_ACTIVE)) {
				return kIOReturnIOError;
			} else {
				return kIOReturnSuccess;
			}
		}

		/* Update the watched bits. This will reset the watchState on every loop -- it is reset to 0 when threads
		 * are signaled. */
		pWatchState |= mask;

		/* Wait to be signaled on a state change */
		int rtn = IOLockSleep(pLock, &pStateEvent, THREAD_ABORTSAFE);
		if (rtn == THREAD_TIMED_OUT) {
			if (!haveLock)
				IOLockUnlock(pLock);
			return kIOReturnTimeout;
		} else if (rtn == THREAD_INTERRUPTED) {
			if (!haveLock)
				IOLockUnlock(pLock);
			return kIOReturnAborted;
		}

		/* Check if we've been stopped while the lock was relinquished. */
		//		if (_stopping) {
		//			LOG_DEBUG("watchState() - offline (stopping) after IOLockSleep");
		//			if (!haveLock)
		//				IOLockUnlock(_lock);
		//			return kIOReturnOffline;
		//		}
	}

	if (!haveLock)
		IOLockUnlock(pLock);

	/* Should not be reachable */
	IOLog("matthiasm: Reached unreachable end of watchState()");
	return kIOReturnOffline;
}



/* nextEvent returns the type of the next event on the RX queue.  If no
 *events are present on the RX queue, then EOQ is returned.
 */
UInt32 com_matthiasm_elektriktrick_SerialDriver::nextEvent(void *refCon) {
	IOLog("matthiasm: nextEvent\n");
	// FIXME:
	return PD_E_EOQ; // no event in queue
}


// /Applications/Xcode.app//Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/Kernel.framework/Versions/A/Headers/IOKit//serial/IOSerialStreamSync.h

/* executeEvent causes the specified event to be processed immediately.
 *This is primarily used for channel control commands like START & STOP
 */
IOReturn com_matthiasm_elektriktrick_SerialDriver::executeEvent(UInt32 event, UInt32 data, void *refCon) {
	IOLog("matthiasm: executeEvent %d %d\n", event, data);
	IOReturn ret = kIOReturnSuccess;
	UInt32 stateUpdate;
	UInt32 stateMask;

	IOLockLock(pLock);

	/* Verify that the driver has not been stopped */
	//	if (_stopping) {
	//		IOLog("matthiasm:executeEvent() - offline (stopping)");
	//		IOLockUnlock(_lock);
	//		return kIOReturnOffline;
	//	}

	switch (event) {
		case PD_E_ACTIVE: { // matt: called first
			IOLog("matthiasm:executeEvent(PD_E_ACTIVE, %u, %p)", data, refCon);

			/* Start or stop the UART */
			bool starting = data;

			/* Skip if already started, already stopped */
			if (starting && (pState & PD_S_ACTIVE) != 0) {
				break;
			} else if (!starting && (pState & PD_S_ACTIVE) == 0) {
				break;
			}

			//			/* Set up the UART request */
			//			IOUSBDevRequest req;
			//			req.bmRequestType = USLCOM_WRITE;
			//			req.bRequest = USLCOM_IFC_ENABLE;
			//			req.wIndex = USLCOM_PORT_NO;
			//			req.wLength = 0;
			//			req.pData = NULL;
			//
			stateMask = PD_S_ACTIVE;
			if (starting) {
				IOLog("matthiasm:Enabling UART");
				stateUpdate = PD_S_ACTIVE;
				//				req.wValue = USLCOM_IFC_ENABLE_EN;
			} else {
				IOLog("matthiasm:Disabling UART");
				stateUpdate = 0;
				//				req.wValue = USLCOM_IFC_ENABLE_DIS;
			}
			//
			//			/* Issue request */
			//			ret = this->sendUSBDeviceRequest(&req);
			//			if (ret != kIOReturnSuccess) {
			//				/* Only return an error on start. At stop time, the device
			//				 * may have simply disappeared. */
			//				if (starting) {
			//					LOG_ERR("Set PD_E_ACTIVE (data=%u) failed: %u", data, ret);
			//					break;
			//				} else {
			//					IOLog("matthiasm:Ignoring PD_E_ACTIVE error %u on stop. The device was likely unplugged.", ret);
			//					ret = kIOReturnSuccess;
			//					break;
			//				}
			//			}
			//
			/* Update state */
			setState(stateUpdate, stateMask, refCon, true);

			if (starting) {
				setState(PD_S_TX_ENABLE|PD_S_TXQ_EMPTY|PD_S_TXQ_LOW_WATER, PD_S_TX_ENABLE|PD_S_TX_BUSY|PD_S_TXQ_MASK, refCon, true);
				setState(PD_S_RX_ENABLE|PD_S_RXQ_EMPTY|PD_S_RXQ_LOW_WATER, PD_S_RX_ENABLE|PD_S_RX_BUSY|PD_S_RXQ_MASK, refCon, true);
			}

			//#define PD_S_TX_ENABLE          0x20000000UL
			//#define PD_S_TX_BUSY            0x10000000UL
			//#define PD_S_TX_EVENT           0x08000000UL
			//#define PD_S_TXQ_EMPTY          0x04000000UL
			//#define PD_S_TXQ_LOW_WATER      0x02000000UL
			//#define PD_S_TXQ_HIGH_WATER     0x01000000UL
			//#define PD_S_TXQ_FULL           0x00800000UL
			//#define PD_S_TXQ_MASK           0x07800000UL
			//
			//#define PD_S_RX_ENABLE          0x00400000UL
			//#define PD_S_RX_BUSY            0x00200000UL
			//#define PD_S_RX_EVENT           0x00100000UL
			//#define PD_S_RXQ_EMPTY          0x00080000UL
			//#define PD_S_RXQ_LOW_WATER      0x00040000UL
			//#define PD_S_RXQ_HIGH_WATER     0x00020000UL
			//#define PD_S_RXQ_FULL           0x00010000UL
			//#define PD_S_RXQ_MASK           0x000F0000UL

			//
			//			// TODO - Restore any line state?
			//
			break;
		}


		case PD_E_RXQ_SIZE:
			/* Adjust receive queue size. We're not required to support this. */
			IOLog("matthiasm:executeEvent(PD_E_RXQ_SIZE, %u, %p)", data, refCon);
			break;
		case PD_E_TXQ_SIZE:
			/* Adjust send queue size. We're not required to support this. */
			IOLog("matthiasm:executeEvent(PD_E_TXQ_SIZE, %u, %p)", data, refCon);
			break;

		case PD_E_RXQ_HIGH_WATER:
			/* Optional */
			IOLog("matthiasm:executeEvent(PD_E_RXQ_HIGH_WATER, %u, %p)", data, refCon);
			break;

		case PD_E_RXQ_LOW_WATER:
			/* Optional */
			IOLog("matthiasm:executeEvent(PD_E_RXQ_HIGH_WATER, %u, %p)", data, refCon);
			break;

		case PD_E_TXQ_HIGH_WATER:
			/* Optional */
			IOLog("matthiasm:executeEvent(PD_E_TXQ_HIGH_WATER, %u, %p)", data, refCon);
			break;

		case PD_E_TXQ_LOW_WATER:
			/* Optional */
			IOLog("matthiasm:executeEvent(PD_E_TXQ_LOW_WATER, %u, %p)", data, refCon);
			break;

		case PD_E_TXQ_FLUSH: // matt: called 2nd,
			/* No-op. */
			IOLog("matthiasm:executeEvent(PD_E_TXQ_FLUSH, %u, %p)", data, refCon);
			break;

		case PD_E_RXQ_FLUSH: // matt: called 3rd
			/* No-op. */
			IOLog("matthiasm:executeEvent(PD_E_RXQ_FLUSH, %u, %p)", data, refCon);
			break;

		case PD_E_DATA_RATE: { // matt:
			/* Set the baud rate. */
			IOLog("matthiasm:executeEvent(PD_E_DATA_RATE, %u>>1, %p)", data, refCon);

			/*
			 * IOSerialBSDClient shifts the speed << 1 before issuing a PD_E_DATA_RATE,
			 * claiming that the speed is stored in half-bits, but this does not appear
			 * to be the case. Comments in Apple's serial drivers' PD_E_DATA_RATE merely
			 * state 'For API compatiblilty with Intel' before reversing the shift.
			 *
			 * Summary: This is necessary to keep IOSerialBSDClient happy, and why
			 * IOSerialBSDClient requires this is lost to the history of whatever
			 * Apple engineer is responsible.
			 */
			//			UInt32 baud = data >> 1;
			//
			//			/* Set up the UART request */
			//			IOUSBDevRequest req;
			//			uint32_t reqBuad = OSSwapHostToLittleInt32(baud);
			//			req.bmRequestType = USLCOM_WRITE;
			//			req.bRequest = USLCOM_SET_BAUDRATE;
			//			req.wValue = 0;
			//			req.wIndex = USLCOM_PORT_NO;
			//			req.wLength = sizeof(reqBuad);
			//			req.pData = &reqBuad;
			//
			//			/* Issue request */
			//			ret = this->sendUSBDeviceRequest(&req);
			//			if (ret == kIOReturnSuccess) {
			//				_baudRate = baud;
			//			} else {
			//				LOG_ERR("Set USLCOM_BAUD_RATE failed: %u", ret);
			//				break;
			//			}

			break;
		}

		case PD_E_RX_DATA_RATE:
			/* We don't support setting an independent RX data rate to anything but 0. It's unclear
			 * why we need to support a value of zero, but this matches Apple's USBCDCDMM implementation. */
			IOLog("matthiasm:executeEvent(PD_E_RX_DATA_RATE, %u>>1, %p)", data, refCon);
			//			if (data != 0)
			//				ret = kIOReturnBadArgument;
			break;

		case PD_E_DATA_INTEGRITY: // matt:
			// Fall-through
		case PD_E_RX_DATA_INTEGRITY:
			if (event == PD_E_DATA_INTEGRITY) {
				IOLog("matthiasm:executeEvent(PD_E_DATA_INTEGRITY, %u, %p)", data, refCon);
			} else {
				IOLog("matthiasm:executeEvent(PD_E_RX_DATA_INTEGRITY, %u, %p)", data, refCon);
			}

			//			switch (data) {
			//				case PD_RS232_PARITY_NONE:
			//				case PD_RS232_PARITY_ODD:
			//				case PD_RS232_PARITY_EVEN:
			//					/* Set TX+RX vs. RX-only parity */
			//					if (event == PD_E_DATA_INTEGRITY) {
			//						/* Attempt to write the new configuration */
			//						ret = writeCP210xDataConfig(data, _twoStopBits, _characterLength);
			//						if (ret == kIOReturnSuccess) {
			//							/* Update internal state on success */
			//							_txParity = data;
			//							_rxParity = PD_RS232_PARITY_DEFAULT;
			//						}
			//
			//					} else {
			//						_rxParity = data;
			//					}
			//
			//					break;
			//
			//				default:
			//					/* Unsupported parity setting */
			//					ret = kIOReturnBadArgument;
			//					break;
			//			}
			//
			break;

		case PD_RS232_E_STOP_BITS: // matt:
			/* Set the stop bits */
			IOLog("matthiasm:executeEvent(PD_RS232_E_STOP_BITS, %u>>1, %p)", data, refCon);

			//			/* Provided as half bits */
			//			data >>= 1;
			//			bool newTwoStopBits;
			//
			//			if (data == 1) {
			//				newTwoStopBits = false;
			//			} else if (data == 2) {
			//				newTwoStopBits = true;
			//			} else {
			//				LOG_ERR("PD_RS232_E_STOP_BITS with invalid data=%u", data);
			//				ret = kIOReturnBadArgument;
			//				break;
			//			}
			//
			//			/* Attempt to write the new configuration */
			//			ret = writeCP210xDataConfig(_txParity, newTwoStopBits, _characterLength);
			//			if (ret == kIOReturnSuccess) {
			//				_twoStopBits = newTwoStopBits;
			//			}

			break;

		case PD_RS232_E_RX_STOP_BITS:
			/* We don't support setting an independent RX stop bit value to anything but 0. It's unclear
			 * why we need to support a value of zero, but this matches Apple's USBCDCDMM implementation. */
			IOLog("matthiasm:executeEvent(PD_RS232_E_RX_STOP_BITS, %u>>1, %p)", data, refCon);
			//			if (data != 0)
			//				ret = kIOReturnBadArgument;
			break;

		case PD_E_DATA_SIZE: { // matt:
			/* Set the character bit size */
			IOLog("matthiasm:executeEvent(PD_E_DATA_SIZE, %u>>1, %p)", data, refCon);

			//			/* Provided as half bits */
			//			data >>= 1;
			//
			//			if (data < 5 || data > 8) {
			//				ret = kIOReturnBadArgument;
			//				break;
			//			}
			//
			//			/* Attempt to write the new configuration */
			//			ret = writeCP210xDataConfig(_txParity, _twoStopBits, data);
			//			if (ret == kIOReturnSuccess) {
			//				_characterLength = data;
			//			}
			//
			break;
		}

		case PD_E_RX_DATA_SIZE:
			/* We don't support setting an independent RX data size to anything but 0. It's unclear
			 * why we need to support a value of zero, but this matches Apple's USBCDCDMM implementation. */
			IOLog("matthiasm:executeEvent(PD_E_RX_DATA_SIZE, %u>>1, %p)", data, refCon);
			//			if (data != 0)
			//				ret = kIOReturnBadArgument;
			break;

		case PD_E_FLOW_CONTROL: // matt: called 4th
			IOLog("matthiasm:executeEvent(PD_E_FLOW_CONTROL, %x, %p)", data, refCon);

			/* Validate the data */
			//			if (data & ~PL_A_FLOWCONTROL_MASK) {
			//				LOG_ERR("PD_E_FLOW_CONTROL called with invalid data");
			//				ret = kIOReturnBadArgument;
			//			}

			/* Shift to PD_RS232_S_ */
			data >>= PD_RS232_A_SHIFT;

			/* Update state */
			setState(data, PD_RS232_S_MASK, refCon, true);
			break;

		case PD_RS232_E_XON_BYTE: // matt:
			IOLog("matthiasm:executeEvent(PD_RS232_E_XON_BYTE, %u, %p)", data, refCon);
			//			_xonChar = data;
			break;

		case PD_RS232_E_XOFF_BYTE: // matt:
			IOLog("matthiasm:executeEvent(PD_RS232_E_XOFF_BYTE, %u, %p)", data, refCon);
			//			_xoffChar = data;
			break;

		case PD_E_SPECIAL_BYTE:
			IOLog("matthiasm:executeEvent(PD_E_SPECIAL_BYTE, %u, %p)", data, refCon);
			/**
			 * 'Special' bytes are an optional optimization, used to implement
			 * wake up of waiting threads if a 'special' character is received. This
			 * is only used by the PPP and SLIP line disciplines. We do not support
			 * this feature.
			 */
			break;

		case PD_E_VALID_DATA_BYTE:
			IOLog("matthiasm:executeEvent(PD_E_VALID_DATA_BYTE, %u, %p)", data, refCon);
			/**
			 * Reset a 'special' byte set in PD_E_SPECIAL_BYTE.
			 */
			break;

		default:
			IOLog("matthiasm:Unsupported executeEvent(%x, %u, %p)", event, data, refCon);
			ret = kIOReturnBadArgument;
			break;
	}

	IOLockUnlock(pLock);
	return ret;
}


/* requestEvent processes the specified event as an immediate request and
 *returns the results in data.  This is primarily used for getting link
 *status information and verifying baud rate and such.
 */
IOReturn com_matthiasm_elektriktrick_SerialDriver::requestEvent(UInt32 event, UInt32 *data, void *refCon) {
	IOLog("matthiasm: requestEvent\n");
	return kIOReturnIOError;
}


/* enqueueEvent will place the specified event into the TX queue.  The
 *sleep argument allows the caller to specify the enqueueEvent's
 *behaviour when the TX queue is full.  If sleep is true, then this
 *method will sleep until the event is enqueued.  If sleep is false,
 *then enqueueEvent will immediatly return kIOReturnNoResources.
 */
IOReturn com_matthiasm_elektriktrick_SerialDriver::enqueueEvent(UInt32 event, UInt32 data,
																bool sleep, void *refCon) {
	IOLog("matthiasm: enqueueEvent\n");
	return kIOReturnIOError;
}


/* dequeueEvent will remove the oldest event from the RX queue and return
 *it in event & data.  The sleep argument defines the behaviour if the RX
 *queue is empty.  If sleep is true, then this method will sleep until an
 *event is available.  If sleep is false, then an EOQ event will be
 *returned.  In either case kIOReturnSuccess is returned.
 */
IOReturn com_matthiasm_elektriktrick_SerialDriver::dequeueEvent(UInt32 *event, UInt32 *data,
																bool sleep, void *refCon) {
	IOLog("matthiasm: dequeueEvent\n");
	return kIOReturnIOError;
}


/* enqueueData will attempt to copy data from the specified buffer to the
 *TX queue as a sequence of VALID_DATA events.  The argument bufferSize
 *specifies the number of bytes to be sent.  The actual number of bytes
 *transferred is returned in transferCount.  If sleep is true, then this
 *method will sleep until all bytes can be transferred.  If sleep is
 *false, then as many bytes as possible will be copied to the TX queue.
 */
IOReturn com_matthiasm_elektriktrick_SerialDriver::enqueueData(UInt8 *buffer, UInt32 size, UInt32 *count,
															   bool sleep, void *refCon) {
	IOLog("matthiasm: enqueueData\n");
	return kIOReturnIOError;
}


/* dequeueData will attempt to copy data from the RX queue to the specified
 *buffer.  No more than bufferSize VALID_DATA events will be transferred.
 *In other words, copying will continue until either a non-data event is
 *encountered or the transfer buffer is full.  The actual number of bytes
 *transferred is returned in transferCount.
 *
 *The sleep semantics of this method are slightly more complicated than
 *other methods in this API:  Basically, this method will continue to
 *sleep until either minCount characters have been received or a non
 *data event is next in the RX queue.  If minCount is zero, then this
 *method never sleeps and will return immediatly if the queue is empty.
 *
 *The latency parameter specifies the maximum amount of time that should
 *pass after the first character is available before the routine returns.
 *This allows the caller to specify a 'packet' timeout.  The unit of the
 *latency parameter is microseconds, though the exact delay may vary
 *depending on the granularity of the timeout services available to the
 *driver.
 */
IOReturn com_matthiasm_elektriktrick_SerialDriver::dequeueData(UInt8 *buffer, UInt32 size, UInt32 *count,
															   UInt32 min, void *refCon) {
	IOLog("matthiasm: dequeueData (%d:%.*s)\n", size, size, buffer);
	*count = size;
	// FIXME:
	return kIOReturnSuccess;
	return kIOReturnIOError;
}


