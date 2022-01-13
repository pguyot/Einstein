//
//  EinsteinTests.m
//  Einstein
//
//  Created by Paul Guyot on 08/12/2014.
//
//

#import <Cocoa/Cocoa.h>
#import <XCTest/XCTest.h>

#include "UMemoryTests.h"
#include "UProcessorTests.h"
#include "Emulator/Log/TRAMLog.h"
#include <Emulator/TARMProcessor.h>

@interface EinsteinTests : XCTestCase
@end

@implementation EinsteinTests

- (void)setUp
{
	[super setUp];
}

- (void)tearDown
{
	[super tearDown];
}

// Step tests require a ROM image

#define TEST(TestSuiteName, TestName) -(void) test##TestSuiteName##_##TestName
#define EXPECT_EQ(val1, val2) XCTAssertEqual(val1, val2)

#include "_Tests_/ExecuteInstructionState1Tests.t"
#include "_Tests_/ExecuteInstructionState2Tests.t"
#include "_Tests_/ExecuteInstructionTests.t"
#include "_Tests_/ExecuteTwoInstructionsTests.t"
#include "_Tests_/MemoryTests.t"
#include "_Tests_/RunCodeTests.t"

@end
