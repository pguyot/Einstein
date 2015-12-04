//
//  TObjCBridgeTests.m
//  Einstein
//
//  Created by Jake on 12/3/15.
//
//

#import <XCTest/XCTest.h>
#include "Emulator/TMemory.h"
#include <K/Defines/KDefinitions.h>
#include "Emulator/NativeCalls/TObjCBridgeCalls.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //
#if TARGET_OS_WIN32
#define kTempFlashPath "c:/EinsteinTests2.flash"
#else
#define kTempFlashPath "/tmp/EinsteinTests2.flash"
#endif


@interface TObjCBridgeTests : XCTestCase {
	KUInt8* romBuffer;
	TMemory* theMem;
	TObjCBridgeCalls* calls;
}
@end

@implementation TObjCBridgeTests

- (void)setUp {
    [super setUp];
	
    // Put setup code here. This method is called before the invocation of each test method in the class.
	romBuffer = (KUInt8*) malloc(TMemoryConsts::kLowROMEnd);
	theMem = new TMemory( nil, romBuffer, kTempFlashPath );
	calls = new TObjCBridgeCalls( theMem );
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
	delete theMem;
	theMem = NULL;
	free(romBuffer);

	[super tearDown];
}

- (void)testGetHostArchitecture {
    // This is an example of a functional test case.
    // Use XCTAssert and related functions to verify your tests produce the correct results.
	KUInt32 sizeOfVoidStar = calls->HostGetCPUArchitecture();
	XCTAssertTrue(sizeOfVoidStar == 8 || sizeOfVoidStar == 4, @"TObjCBridgeCalls::HostGetCPUArchitecture Expected 32 or 64 bit architecture");

}

- (void)testHostMakeNSInvocation_ClassMethod {
	// Some places to store things in our emulated "Memory"
	const KUInt32 classNameAddr = 0x04000000;
	const KUInt32 selectorNameAddr = 0x04000040;
	const KUInt32 resultAddr = 0x04000080;
	
	// Setup the memory with some strings
	NSString* className = @"NSDate\0";
	NSData* classNameData = [className dataUsingEncoding:NSUTF16BigEndianStringEncoding];
	theMem->FastWriteBuffer(classNameAddr, (KUInt32)[classNameData length], (KUInt8*)[classNameData bytes]);

	NSString* selectorName = @"+date\0";
	NSData* selectorNameData = [selectorName dataUsingEncoding:NSUTF16BigEndianStringEncoding];
	theMem->FastWriteBuffer(selectorNameAddr, (KUInt32)[selectorNameData length], (KUInt8*)[selectorNameData bytes]);
	
	// Make the call to the function under test
	calls->HostMakeNSInvocation(resultAddr, classNameAddr, selectorNameAddr);
	
	// Get the result out of memory
	void* invokePtr = nil;
	theMem->FastReadBuffer(resultAddr, sizeof(id), (KUInt8*)&invokePtr);
	NSInvocation* invocation = (__bridge_transfer NSInvocation*)invokePtr;
	
	XCTAssertNotNil(invocation, "TObjCBridgeCalls::HostMakeNSInvocation (class method) returned NIL invocation");
	
	// Test the invocation object that was returned;
	[invocation invoke];
	__unsafe_unretained NSDate* date = nil;
	[invocation getReturnValue:&date];
	
	// Final test
	XCTAssertTrue([date isKindOfClass:[NSDate class]], "TObjCBridgeCalls::HostMakeNSInvocation (class method) Did not properly create NSInvocation");
}

- (void)testHostMakeNSInvocation_InstanceMethod {
	// Some places to store things in our emulated "Memory"
	const KUInt32 classNameAddr = 0x04000000;
	const KUInt32 selectorNameAddr = 0x04000040;
	const KUInt32 invocationAddr = 0x04000080;
	
	// Setup the memory with some strings
	NSString* className = @"NSString\0";
	NSData* classNameData = [className dataUsingEncoding:NSUTF16BigEndianStringEncoding];
	theMem->FastWriteBuffer(classNameAddr, (KUInt32)[classNameData length], (KUInt8*)[classNameData bytes]);
	
	NSString* selectorName = @"-uppercaseString\0";
	NSData* selectorNameData = [selectorName dataUsingEncoding:NSUTF16BigEndianStringEncoding];
	theMem->FastWriteBuffer(selectorNameAddr, (KUInt32)[selectorNameData length], (KUInt8*)[selectorNameData bytes]);

	// Make the call to the functions under test
	calls->HostMakeNSInvocation(invocationAddr, classNameAddr, selectorNameAddr);
	
	// Get the result out of memory
	void* invokePtr = nil;
	theMem->FastReadBuffer(invocationAddr, sizeof(id), (KUInt8*)&invokePtr);
	NSInvocation* invocation = (__bridge_transfer NSInvocation*)invokePtr;
	
	XCTAssertNotNil(invocation, "TObjCBridgeCalls::HostMakeNSInvocation (instance method) returned NIL invocation");
	
	// Test the invocation object that was returned;
	NSString* testString =     @"This is a MiXeD CaSe StRiNg!";
	NSString* expectedResult = @"THIS IS A MIXED CASE STRING!";
	[invocation setTarget:testString];
	[invocation invoke];
	__unsafe_unretained NSString* resultString = nil;
	[invocation getReturnValue:&resultString];
	
	// Final test
	XCTAssertEqualObjects(resultString, expectedResult, "TObjCBridgeCalls::HostMakeNSInvocation (instance method) Did not properly create NSInvocation");
}

- (void)testHostSetInvocationTarget {
	const KUInt32 invocationAddr = 0x04000000;
	const KUInt32 arrayAddr = 0x04000040;

	NSMethodSignature* aSignature = [NSMutableArray instanceMethodSignatureForSelector:@selector(removeAllObjects)];
	NSInvocation* invocation = [NSInvocation invocationWithMethodSignature:aSignature];
	
	void* objectPtr = ((__bridge void*)invocation);
	theMem->FastWriteBuffer(invocationAddr, sizeof(id), (KUInt8*)&objectPtr);
	
	NSMutableArray* array = [NSMutableArray arrayWithObjects:@"1", @"2", @"3", @"4", nil];
	objectPtr = ((__bridge void*)array);
	theMem->FastWriteBuffer(arrayAddr, sizeof(id), (KUInt8*)&objectPtr);
	
	calls->HostSetInvocationTarget(invocationAddr, arrayAddr);

	invocation.selector = @selector(removeAllObjects);
	[invocation invoke];
	
	XCTAssertTrue(array.count == 0, "TObjCBridgeCalls::HostSetInvocationTarget failed");
}

- (void)testHostSetInvocationArgument_Object {
	const KUInt32 invocationAddr = 0x04000000;
	const KUInt32 itemAddr = 0x04000040;

	NSMutableArray* array = [NSMutableArray arrayWithObjects:@"1", @"2", @"3", @"4", nil];
	
	NSMethodSignature* aSignature = [NSMutableArray instanceMethodSignatureForSelector:@selector(addObject:)];
	NSInvocation* invocation = [NSInvocation invocationWithMethodSignature:aSignature];
	invocation.selector = @selector(addObject:);
	invocation.target = array;
	
	void* objectPtr = ((__bridge void*)invocation);
	theMem->FastWriteBuffer(invocationAddr, sizeof(id), (KUInt8*)&objectPtr);
	
	NSString* itemNumber5 = @"5";
	objectPtr = ((__bridge void*)itemNumber5);
	theMem->FastWriteBuffer(itemAddr, sizeof(id), (KUInt8*)&objectPtr);
	
	calls->HostSetInvocationArgument_Object(invocationAddr, itemAddr, 2);

	[invocation invoke];
	
	XCTAssertEqualObjects(array[4], @"5", "TObjCBridgeCalls::HostSetInvocationArgument_Object failed");
}

- (void)testHostGetInvocationReturn_Object {
	const KUInt32 returnObjAddr = 0x04000040;
	const KUInt32 invocationAddr = 0x04000080;

	// Test parameters
	NSString* testString =     @"This is a MiXeD CaSe StRiNg!";
	NSString* expectedResult = @"THIS IS A MIXED CASE STRING!";

	NSMethodSignature* aSignature = [NSString instanceMethodSignatureForSelector:@selector(uppercaseString)];
	NSInvocation* invocation = [NSInvocation invocationWithMethodSignature:aSignature];
	invocation.selector = @selector(uppercaseString);
	invocation.target = testString;
	
	// Move Invocation to memory
	void* objectPtr = ((__bridge void*)invocation);
	theMem->FastWriteBuffer(invocationAddr, sizeof(id), (KUInt8*)&objectPtr);

	[invocation invoke];
	
	// Get the return object
	calls->HostGetInvocationReturn_Object(returnObjAddr, invocationAddr);
	
	// Get the result out of memory
	void* returnPtr = nil;
	theMem->FastReadBuffer(returnObjAddr, sizeof(id), (KUInt8*)&returnPtr);
	NSString* resultString = (__bridge_transfer NSString*)returnPtr;
	
	XCTAssertNotNil(invocation, "TObjCBridgeCalls::HostGetInvocationReturn_Object (instance method) returned NIL invocation");
	
	// Test the  object that was returned;
	XCTAssertEqualObjects(resultString, expectedResult, "TObjCBridgeCalls::HostGetInvocationReturn_Object (instance method) Did not properly create NSInvocation");

}

- (void)testHostInvoke {
	const KUInt32 invocationAddr = 0x04000080;
	
	// Test parameters
	NSString* testString =     @"This is a MiXeD CaSe StRiNg!";
	NSString* expectedResult = @"THIS IS A MIXED CASE STRING!";
	
	NSMethodSignature* aSignature = [NSString instanceMethodSignatureForSelector:@selector(uppercaseString)];
	NSInvocation* invocation = [NSInvocation invocationWithMethodSignature:aSignature];
	invocation.selector = @selector(uppercaseString);
	invocation.target = testString;
	
	// Move Invocation to memory
	void* objectPtr = ((__bridge void*)invocation);
	theMem->FastWriteBuffer(invocationAddr, sizeof(id), (KUInt8*)&objectPtr);
	
	calls->HostInvoke(invocationAddr);
	
	// Get the return object
	__unsafe_unretained NSString* resultString = nil;
	[invocation getReturnValue:&resultString];

	XCTAssertNotNil(invocation, "TObjCBridgeCalls::HostMakeNSInvocation (instance method) returned NIL invocation");
	
	// Test the  object that was returned;
	XCTAssertEqualObjects(resultString, expectedResult, "TObjCBridgeCalls::HostInvoke (instance method) Did not properly create NSInvocation");
}

- (void)testHostReleaseObject {
	const KUInt32 objAddr = 0x04000080;
	NSObject* obj = [[NSObject alloc] init];
	
	// Increase the retain count by casting to __bridge_retained
	void* objPtr = (__bridge_retained void*)obj;
	theMem->FastWriteBuffer(objAddr, sizeof(id), (KUInt8*)&objPtr);
	
	NSInteger retainCountBefore = CFGetRetainCount((__bridge CFTypeRef)obj);

	calls->HostReleaseObject(objAddr);
	
	NSInteger retainCountAfter = CFGetRetainCount((__bridge CFTypeRef)obj);
	
	XCTAssertEqual(retainCountAfter, retainCountBefore - 1, "TObjCBridgeCalls::HostReleaseObject did not decrement retain count");
}

- (void)testHostMakeNSString {
	const KUInt32 nsStringAddr = 0x04000000;
	const KUInt32 cStringAddr = 0x04000010;
	
	// Load the test string into memory, that will be converted into an NSString
	NSString* testString = @"This is a test string.";
	NSData* testStringData = [testString dataUsingEncoding:NSUTF16BigEndianStringEncoding];
	theMem->FastWriteBuffer(cStringAddr, (KUInt32)[testStringData length], (KUInt8*)[testStringData bytes]);

	// Make the NSString
	calls->HostMakeNSString(nsStringAddr, cStringAddr);
	
	// Get the NSString object pointer out of memory
	void* returnPtr = nil;
	theMem->FastReadBuffer(nsStringAddr, sizeof(id), (KUInt8*)&returnPtr);
	NSString* resultString = (__bridge NSString*)returnPtr;

	// Lets test the retain count just because we can
	NSInteger retainCountBefore = CFGetRetainCount((__bridge CFTypeRef)resultString);
	
	XCTAssertEqualObjects(resultString, testString, "TObjCBridgeCalls::HostMakeNSString did not create the proper NSString object");
	
	// Decrement the retain count by using __bridge_transfer cast
	resultString = (__bridge_transfer NSString*)returnPtr;
	NSInteger retainCountAfter = CFGetRetainCount((__bridge CFTypeRef)resultString);
	resultString = nil;

	XCTAssertEqual(retainCountAfter, retainCountBefore - 1, "TObjCBridgeCalls::HostMakeNSString retain count was not properly handled");
}

@end
