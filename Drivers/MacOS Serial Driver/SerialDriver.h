
#include <IOKit/IOService.h>

// Driver implementation
#include <IOKit/serial/IOSerialDriverSync.h>

// representation in user space ("/dev/tty.Whatever")
#include <IOKit/serial/IORS232SerialStreamSync.h>

class com_matthiasm_elektriktrick_SerialDriver: public IOSerialDriverSync
{
	OSDeclareDefaultStructors(com_matthiasm_elektriktrick_SerialDriver)

	IORS232SerialStreamSync *pNub;
	IOWorkLoop *pWorkLoop;
	UInt32 pState;
	UInt32 pWatchState;
	IOLock *pLock;
	void *pStateEvent;

public:
	virtual bool init(OSDictionary *dictionary = 0) override;
	virtual void free(void) override;
	virtual IOService *probe(IOService *provider, SInt32 *score) override;
	virtual bool start(IOService *provider) override;
	virtual void stop(IOService *provider) override;

	virtual IOReturn acquirePort(bool sleep, void *refCon) override;
	virtual IOReturn releasePort(void *refCon) override;
	virtual IOReturn setState(UInt32 state, UInt32 mask, void *refCon) override;
	IOReturn setState(UInt32 state, UInt32 mask, void *refCon, bool haveLock);
	virtual UInt32 getState(void *refCon) override;
	virtual IOReturn watchState(UInt32 *state, UInt32 mask, void *refCon) override;
	IOReturn watchState(UInt32 *state, UInt32 mask, void *refCon, bool haveLock);
	virtual UInt32 nextEvent(void *refCon) override;
	virtual IOReturn executeEvent(UInt32 event, UInt32 data, void *refCon) override;
	virtual IOReturn requestEvent(UInt32 event, UInt32 *data, void *refCon) override;
	virtual IOReturn enqueueEvent(UInt32 event, UInt32 data,
								  bool sleep, void *refCon) override;
	virtual IOReturn dequeueEvent(UInt32 *event, UInt32 *data,
								  bool sleep, void *refCon) override;
	virtual IOReturn enqueueData(UInt8 *buffer, UInt32 size, UInt32 *count,
								 bool sleep, void *refCon) override;
	virtual IOReturn dequeueData(UInt8 *buffer, UInt32 size, UInt32 *count,
								 UInt32 min, void *refCon) override;
};




