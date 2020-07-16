//
//  EinsteinTests.m
//  Einstein
//
//  Created by Paul Guyot on 08/12/2014.
//
//

#import <Cocoa/Cocoa.h>
#import <XCTest/XCTest.h>

#include "UProcessorTests.h"
#include "UMemoryTests.h"
#include "Emulator/Log/TRAMLog.h"

@interface EinsteinTests : XCTestCase
@end

typedef void (^LogBlock)(TLog* log);

@implementation EinsteinTests

- (void)doTest: (LogBlock)block withOutputFile:(NSString*)path {
	BOOL fileExistsAtPath = [[NSFileManager defaultManager] fileExistsAtPath:path isDirectory:nil];

	XCTAssertTrue(fileExistsAtPath, @"Expected test result file is missing: %@", path);
	
	if ( fileExistsAtPath == NO )
		return;
	
	NSString* master = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:nil];
	TRAMLog log;
	block(&log);
	NSString* output = [NSString stringWithUTF8String:log.GetContent().c_str()];
	XCTAssertEqualObjects(output, master, @"master = %@", path);
}

- (void)setUp {
    [super setUp];
}

- (void)tearDown {
    [super tearDown];
}

- (void)doTestProcessorExecuteInstruction:(NSString*) instruction {
	NSString *outputFilePath = [[NSBundle bundleForClass:[self class]] pathForResource:[NSString stringWithFormat:@"master-test-execute-instruction_%@", instruction] ofType:@""];
	
	[self doTest: ^(TLog* log){
			UProcessorTests::ExecuteInstruction([instruction cStringUsingEncoding:NSUTF8StringEncoding], log);
	} withOutputFile:outputFilePath];
}

- (void)doTestProcessorExecuteInstructionState1:(NSString*) instruction {
	NSString *outputFilePath = [[NSBundle bundleForClass:[self class]] pathForResource:[NSString stringWithFormat:@"master-test-execute-instruction-state1_%@", instruction] ofType:@""];

	[self doTest: ^(TLog* log){
			UProcessorTests::ExecuteInstructionState1([instruction cStringUsingEncoding:NSUTF8StringEncoding], log);
	} withOutputFile:outputFilePath];
}
- (void)doTestProcessorExecuteInstructionState2:(NSString*) instruction {
	NSString *outputFilePath = [[NSBundle bundleForClass:[self class]] pathForResource:[NSString stringWithFormat:@"master-test-execute-instruction-state2_%@", instruction] ofType:@""];
	
	[self doTest: ^(TLog* log){
			UProcessorTests::ExecuteInstructionState2([instruction cStringUsingEncoding:NSUTF8StringEncoding], log);
	} withOutputFile:outputFilePath];
}
- (void)doTestProcessorExecuteTwoInstructions:(NSString*) instructions {
	NSString *outputFilePath = [[NSBundle bundleForClass:[self class]] pathForResource:[NSString stringWithFormat:@"master-test-execute-two-instructions_%@", instructions] ofType:@""];

	[self doTest: ^(TLog* log){
			UProcessorTests::ExecuteTwoInstructions([instructions cStringUsingEncoding:NSUTF8StringEncoding], log);
	} withOutputFile:outputFilePath];
}
- (void)doTestProcessorRunCode:(NSString*) code master: (NSString*) suffix {
	NSString *outputFilePath = [[NSBundle bundleForClass:[self class]] pathForResource:[NSString stringWithFormat:@"master-test-run-code_%@", suffix] ofType:@""];

	[self doTest: ^(TLog* log){
		UProcessorTests::RunCode([code cStringUsingEncoding:NSUTF8StringEncoding], log);
	} withOutputFile:outputFilePath];
}

- (void)testProcessorExecuteInstruction_0A000007 {
    [self doTestProcessorExecuteInstruction:@"0A000007"];
}
- (void)testProcessorExecuteInstruction_1A000007 {
    [self doTestProcessorExecuteInstruction:@"1A000007"];
}
- (void)testProcessorExecuteInstruction_E10F1000 {
    [self doTestProcessorExecuteInstruction:@"E10F1000"];
}
- (void)testProcessorExecuteInstruction_E14F1000 {
    [self doTestProcessorExecuteInstruction:@"E14F1000"];
}
- (void)testProcessorExecuteInstruction_E282F014 {
	[self doTestProcessorExecuteInstruction:@"E282F014"];
}
- (void)testProcessorExecuteInstruction_E2922000 {
    [self doTestProcessorExecuteInstruction:@"E2922000"];
}
- (void)testProcessorExecuteInstruction_E3A0409C {
    [self doTestProcessorExecuteInstruction:@"E3A0409C"];
}
- (void)testProcessorExecuteInstruction_E5900000 {
    [self doTestProcessorExecuteInstruction:@"E5900000"];
}
- (void)testProcessorExecuteInstruction_E5900001 {
    [self doTestProcessorExecuteInstruction:@"E5900001"];
}
- (void)testProcessorExecuteInstruction_E5900002 {
    [self doTestProcessorExecuteInstruction:@"E5900002"];
}
- (void)testProcessorExecuteInstruction_E5900003 {
    [self doTestProcessorExecuteInstruction:@"E5900003"];
}
- (void)testProcessorExecuteInstruction_E5D00000 {
    [self doTestProcessorExecuteInstruction:@"E5D00000"];
}
- (void)testProcessorExecuteInstruction_E5D00001 {
    [self doTestProcessorExecuteInstruction:@"E5D00001"];
}
- (void)testProcessorExecuteInstruction_E5D00002 {
    [self doTestProcessorExecuteInstruction:@"E5D00002"];
}
- (void)testProcessorExecuteInstruction_E5D00003 {
    [self doTestProcessorExecuteInstruction:@"E5D00003"];
}
- (void)testProcessorExecuteInstruction_EA0061A0 {
    [self doTestProcessorExecuteInstruction:@"EA0061A0"];
}
- (void)testProcessorExecuteInstruction_EAFFFFFE {
    [self doTestProcessorExecuteInstruction:@"EAFFFFFE"];
}
- (void)testProcessorExecuteInstruction_EE010F11 {
    [self doTestProcessorExecuteInstruction:@"EE010F11"];
}
// 01F94573		mvneqs   r4, r3, ror r5
- (void)testProcessorExecuteInstruction_01F94573 {
	[self doTestProcessorExecuteInstruction:@"01F94573"];
}
// E3D004FF		bics     r0, r0, #-16777216
- (void)testProcessorExecuteInstruction_E3D004FF {
	[self doTestProcessorExecuteInstruction:@"E3D004FF"];
}
// E8DD2000		ldmia    sp, {sp}^
- (void)testProcessorExecuteInstruction_E8DD2000 {
	[self doTestProcessorExecuteInstruction:@"E8DD2000"];
}

// E321F010		msr	CPSR_c, #0x10
- (void)testProcessorExecuteInstruction_E321F010 {
	[self doTestProcessorExecuteInstruction:@"E321F010"];
}
// E321F011		msr	CPSR_c, #0x11
- (void)testProcessorExecuteInstruction_E321F011 {
	[self doTestProcessorExecuteInstruction:@"E321F011"];
}
// E321F012		msr	CPSR_c, #0x12
- (void)testProcessorExecuteInstruction_E321F012 {
	[self doTestProcessorExecuteInstruction:@"E321F012"];
}
// E321F013		msr	CPSR_c, #0x13
- (void)testProcessorExecuteInstruction_E321F013 {
	[self doTestProcessorExecuteInstruction:@"E321F013"];
}
// E321F017		msr	CPSR_c, #0x17
- (void)testProcessorExecuteInstruction_E321F017 {
	[self doTestProcessorExecuteInstruction:@"E321F017"];
}
// E321F01B		msr	CPSR_c, #0x1B
- (void)testProcessorExecuteInstruction_E321F01B {
	[self doTestProcessorExecuteInstruction:@"E321F01B"];
}
// E8D00800		ldm	r0, {r11}^
- (void)testProcessorExecuteInstruction_E8D00800 {
	[self doTestProcessorExecuteInstruction:@"E8D00800"];
}

- (void)testProcessorExecuteInstructionState1_E0049A22 {
    [self doTestProcessorExecuteInstructionState1:@"E0049A22"];
}
- (void)testProcessorExecuteInstructionState1_E0049C20 {
    [self doTestProcessorExecuteInstructionState1:@"E0049C20"];
}
- (void)testProcessorExecuteInstructionState1_E0049C22 {
    [self doTestProcessorExecuteInstructionState1:@"E0049C22"];
}
- (void)testProcessorExecuteInstructionState1_E0049C23 {
    [self doTestProcessorExecuteInstructionState1:@"E0049C23"];
}
- (void)testProcessorExecuteInstructionState1_E00B0A99 {
    [self doTestProcessorExecuteInstructionState1:@"E00B0A99"];
}
- (void)testProcessorExecuteInstructionState1_E01B0A99 {
    [self doTestProcessorExecuteInstructionState1:@"E01B0A99"];
}
- (void)testProcessorExecuteInstructionState1_E0205094 {
    [self doTestProcessorExecuteInstructionState1:@"E0205094"];
}
- (void)testProcessorExecuteInstructionState1_E02B0A99 {
    [self doTestProcessorExecuteInstructionState1:@"E02B0A99"];
}
- (void)testProcessorExecuteInstructionState1_E02BBA99 {
    [self doTestProcessorExecuteInstructionState1:@"E02BBA99"];
}
- (void)testProcessorExecuteInstructionState1_E03BAA99 {
    [self doTestProcessorExecuteInstructionState1:@"E03BAA99"];
}
- (void)testProcessorExecuteInstructionState1_E03BBA99 {
    [self doTestProcessorExecuteInstructionState1:@"E03BBA99"];
}
- (void)testProcessorExecuteInstructionState1_E0620202 {
    [self doTestProcessorExecuteInstructionState1:@"E0620202"];
}
- (void)testProcessorExecuteInstructionState1_E0800005 {
    [self doTestProcessorExecuteInstructionState1:@"E0800005"];
}
- (void)testProcessorExecuteInstructionState1_E0841005 {
    [self doTestProcessorExecuteInstructionState1:@"E0841005"];
}
- (void)testProcessorExecuteInstructionState1_E2200311 {
    [self doTestProcessorExecuteInstructionState1:@"E2200311"];
}
- (void)testProcessorExecuteInstructionState1_E2200801 {
    [self doTestProcessorExecuteInstructionState1:@"E2200801"];
}
- (void)testProcessorExecuteInstructionState1_E220C311 {
    [self doTestProcessorExecuteInstructionState1:@"E220C311"];
}
- (void)testProcessorExecuteInstructionState1_E22C0311 {
    [self doTestProcessorExecuteInstructionState1:@"E22C0311"];
}
- (void)testProcessorExecuteInstructionState1_E22D0311 {
    [self doTestProcessorExecuteInstructionState1:@"E22D0311"];
}
- (void)testProcessorExecuteInstructionState1_E2300CA1 {
    [self doTestProcessorExecuteInstructionState1:@"E2300CA1"];
}
- (void)testProcessorExecuteInstructionState1_E24CB004 {
    [self doTestProcessorExecuteInstructionState1:@"E24CB004"];
}
- (void)testProcessorExecuteInstructionState1_E24DD004 {
    [self doTestProcessorExecuteInstructionState1:@"E24DD004"];
}
- (void)testProcessorExecuteInstructionState1_E2855C0E {
    [self doTestProcessorExecuteInstructionState1:@"E2855C0E"];
}
- (void)testProcessorExecuteInstructionState1_E38000E0 {
    [self doTestProcessorExecuteInstructionState1:@"E38000E0"];
}
- (void)testProcessorExecuteInstructionState1_E38000EF {
    [self doTestProcessorExecuteInstructionState1:@"E38000EF"];
}
- (void)testProcessorExecuteInstructionState1_E3A060FE {
    [self doTestProcessorExecuteInstructionState1:@"E3A060FE"];
}
- (void)testProcessorExecuteInstructionState1_E5B40010 {
    [self doTestProcessorExecuteInstructionState1:@"E5B40010"];
}
- (void)testProcessorExecuteInstructionState1_E5BF0010 {
    [self doTestProcessorExecuteInstructionState1:@"E5BF0010"];
}

- (void)testProcessorExecuteInstructionState2_E1A0331C {
    [self doTestProcessorExecuteInstructionState2:@"E1A0331C"];
}

- (void)testProcessorExecuteTwoInstructions_E282F014_E282F014 {
    [self doTestProcessorExecuteTwoInstructions:@"E282F014-E282F014"];
}
- (void)testProcessorExecuteTwoInstructions_E3A000D3_E129F000 {
    [self doTestProcessorExecuteTwoInstructions:@"E3A000D3-E129F000"];
}
- (void)testProcessorExecuteTwoInstructions_E3A0100C_E1A01081 {
    [self doTestProcessorExecuteTwoInstructions:@"E3A0100C-E1A01081"];
}
// Test branches when they are not at address 0
// mov      r1, #12 (C)
// b        0x0001868C
- (void)testProcessorExecuteTwoInstructions_E3A0100C_EA0061A0 {
	[self doTestProcessorExecuteTwoInstructions:@"E3A0100C-EA0061A0"];
}
- (void)testProcessorExecuteTwoInstructions_E3A0C008_E91C0300 {
    [self doTestProcessorExecuteTwoInstructions:@"E3A0C008-E91C0300"];
}
- (void)testProcessorExecuteTwoInstructions_E3E02000_E2922001 {
    [self doTestProcessorExecuteTwoInstructions:@"E3E02000-E2922001"];
}
// e5901004		ldr	r1, [r0, #4]
// e7902003		ldr	r2, [r0, r3]
- (void)testProcessorExecuteTwoInstructions_E5901004_E7902003 {
	[self doTestProcessorExecuteTwoInstructions:@"E5901004-E7902003"];
}

//		mov      sp, #0x4000000
//		stmia    sp, {r8}^
- (void)testProcessorExecuteTwoInstructions_E3A0D301_E8CD0100 {
	[self doTestProcessorExecuteTwoInstructions:@"E3A0D301-E8CD0100"];
}
// E8D00800		ldm	r0, {r11}^
// E321F010		msr	CPSR_c, #0x10
- (void)testProcessorExecuteTwoInstructions_E8D00800_E321F010 {
	[self doTestProcessorExecuteTwoInstructions:@"E8D00800-E321F010"];
}

// e1200070		bkpt	#0x0
- (void)testProcessorRunCode_1 {
	[self doTestProcessorRunCode:@"E1200070" master: @"1"];
}

// e3a01001		mov		r1, #0x1
// e3310000		teq		r1, #0x0
// 03a02002		moveq	r2, #0x2
// e1200070		bkpt	#0x0
- (void)testProcessorRunCode_2 {
	[self doTestProcessorRunCode:@"E3A01001 E3310000 03A02002 E1200070" master: @"2"];
}

// e3a01001		mov		r1, #0x1
// e3310000		teq		r1, #0x0
// 03a02002		movne	r2, #0x2
// e1200070		bkpt	#0x0
- (void)testProcessorRunCode_3 {
	[self doTestProcessorRunCode:@"E3A01001 E3310000 13A02002 E1200070" master: @"3"];
}

// e3a01001		movs	r1, #0x1
// 03a02002		moveq	r2, #0x2
// e1200070		bkpt	#0x0
- (void)testProcessorRunCode_4 {
	[self doTestProcessorRunCode:@"E3B01001 03A02002 E1200070" master: @"4"];
}

// e3b01001		movs	r1, #0x1
// 03a02002		movne	r2, #0x2
// e1200070		bkpt	#0x0
- (void)testProcessorRunCode_5 {
	[self doTestProcessorRunCode:@"E3B01001 13A02002 E1200070" master: @"5"];
}

// E3A00301	mov		r0, #0x4000000
// E3A01001	mov		r1, #0x1
// E3A02002	mov		r2, #0x2
// E3A03003	mov		r3, #0x3
// E3A04004	mov		r4, #0x4
// E3A05005	mov		r5, #0x5
// E3A06006	mov		r6, #0x6
// E8A0007F	stm		r0!, {r0, r1, r2, r3, r4, r5, r6}
// E9101F80	ldmdb	r0, {r7, r8, r9, r10, r11, r12}
// E0400106	sub		r0, r0, r6, lsl #2
// E590D004	ldr		sp, [r0, #4]
// E790E004	ldr		lr, [r0, r4]
// E1200070	bkpt	#0x0
- (void)testProcessorRunCode_6 {
	[self doTestProcessorRunCode:@"E3A00301 E3A01001 E3A02002 E3A03003 E3A04004 E3A05005 E3A06006 E8A0007F E9101F80 E0400106 E590D004 E790E004 E1200070" master: @"6"];
}

// e361f017	msr		SPSR_c, #0x17
// e1a0e00f	mov		lr, pc
// e1b0f00e	movs	pc, lr
// e1200070	bkpt	#0x0
- (void)testProcessorRunCode_7 {
	[self doTestProcessorRunCode:@"e361f017 e1a0e00f e1b0f00e e1200070" master: @"7"];
}

// e361f010	msr		SPSR_c, #0x10
// e321f017	msr		CPSR_c, #0x17
// e361f011	msr		SPSR_c, #0x11
// e1a0e00f	mov		lr, pc
// e1b0f00e	movs	pc, lr
// e1200070	bkpt	#0x0
- (void)testProcessorRunCode_8 {
	[self doTestProcessorRunCode:@"e361f010 e321f017 e361f011 e1a0e00f e1b0f00e e1200070" master: @"8"];
}

// e361f010	msr		SPSR_c, #0x10
// e321f017	msr		CPSR_c, #0x17
// e3a00011	mov		r0, #0x11
// e2800206	add		r0, r0, #0x60000000
// e169f000	msr		SPSR_fc, r0
// e1a0e00f	mov		lr, pc
// e1b0f00e	movs	pc, lr
// e1200070	bkpt	#0x0
- (void)testProcessorRunCode_9 {
	[self doTestProcessorRunCode:@"e361f010 e321f017 e3a00011 e2800206 e169f000 e1a0e00f e1b0f00e e1200070" master: @"9"];
}

// e3a00017	mov		r0, #0x17
// e2800206	add		r0, r0, #0x60000000
// e129f000	msr		CPSR_fc, r0
// e1200070	bkpt	#0x0
- (void)testProcessorRunCode_10 {
	[self doTestProcessorRunCode:@"e3a00017 e2800206 e129f000 e1200070" master: @"10"];
}

// e3a00017	mov		r0, #0x17
// e2800206	add		r0, r0, #0x60000000
// e121f000	msr		CPSR_c, r0
// e1200070	bkpt	#0x0
- (void)testProcessorRunCode_11 {
	[self doTestProcessorRunCode:@"e3a00017 e2800206 e121f000 e1200070" master: @"11"];
}

// e3a00017	mov		r0, #0x17
// e2800206	add		r0, r0, #0x60000000
// e128f000	msr		CPSR_f, r0
// e1200070	bkpt	#0x0
- (void)testProcessorRunCode_12 {
	[self doTestProcessorRunCode:@"e3a00017 e2800206 e128f000 e1200070" master: @"12"];
}

// e3a00017	mov		r0, #0x17
// e2800206	add		r0, r0, #0x60000000
// e169f000	msr		SPSR_fc, r0
// e14f1000	mrs		r1, SPSR
// e10f2000	mrs		r2, CPSR
// e1a0e00f	mov		lr, pc
// e1b0f00e	movs	pc, lr
// e10f3000	mrs		r3, CPSR
// e1200070	bkpt	#0x0
- (void)testProcessorRunCode_13 {
	[self doTestProcessorRunCode:@"e3a00017 e2800206 e169f000 e14f1000 e10f2000 e1a0e00f e1b0f00e e10f3000 e1200070" master: @"13"];
}

// e3a00017	mov		r0, #0x17
// e2800206	add		r0, r0, #0x60000000
// e161f000	msr		SPSR_c, r0
// e14f1000	mrs		r1, SPSR
// e10f2000	mrs		r2, CPSR
// e1a0e00f	mov		lr, pc
// e1b0f00e	movs	pc, lr
// e10f3000	mrs		r3, CPSR
// e1200070	bkpt	#0x0
- (void)testProcessorRunCode_14 {
	[self doTestProcessorRunCode:@"e3a00017 e2800206 e161f000 e14f1000 e10f2000 e1a0e00f e1b0f00e e10f3000 e1200070" master: @"14"];
}

// e10f0000	mrs		r0, CPSR
// e169f000	msr		SPSR_fc, r0
// e3a00017	mov		r0, #0x17
// e2800206	add		r0, r0, #0x60000000
// e168f000	msr		SPSR_f, r0
// e14f1000	mrs		r1, SPSR
// e10f2000	mrs		r2, CPSR
// e1a0e00f	mov		lr, pc
// e1b0f00e	movs	pc, lr
// e10f3000	mrs		r3, CPSR
// e1200070	bkpt	#0x0
- (void)testProcessorRunCode_15 {
	[self doTestProcessorRunCode:@"e10f0000 e169f000 e3a00017 e2800206 e168f000 e14f1000 e10f2000 e1a0e00f e1b0f00e e10f3000 e1200070" master: @"15"];
}

// -- large block with a subroutine doing 64bits additions.
/*
 ldr     r0, =w1
 ldm     r0, {r0-r3}
 bl      sub
 mov     r2, r4
 mov     r3, r5
 mov     r10, r4
 bl      sub
 mov     r2, r4
 mov     r3, r5
 mov     r11, r4
 bl      sub
 mov     r2, r4
 mov     r3, r5
 mov     r12, r4
 bl      sub
 mov     r2, r4
 mov     r3, r5
 mov     r13, r4
 bl      sub
 sub     r2, r4
 mov     r3, r5
 bl      sub
 bkpt    0
 w1:
 .long   0x46474849
 w2:
 .long   0x41424344
 w3:
 .long   0x16171819
 w4:
 .long   0x11121314
 sub:
 adds    r4,r0,r2
 adcs    r5,r1,r3
 mov     r6, r6, LSL #4
 addeq   r6, #0x1
 addcs   r6, #0x2
 addmi   r6, #0x4
 addvs   r6, #0x8
 mov     r7, r7, ROR #4
 subne   r7, #0x1
 subcc   r7, #0x2
 subpl   r7, #0x4
 subvc   r7, #0x8
 orrhi   r8, #0x80000000
 orrge   r8, #0x40000000
 orrgt   r8, #0x20000000
 mov     r8, r8, LSR #3
 eorhi   r9, #0x80000000
 eorge   r9, #0x40000000
 eorgt   r9, #0x20000000
 mov     r9, r9, ASR #3
 mov     pc, lr
*/
- (void)testProcessorRunCode_16 {
	[self doTestProcessorRunCode:@"e59f00b8 e890000f eb000017 e1a02004 e1a03005 e1a0a004 eb000013 e1a02004 e1a03005 e1a0b004 eb00000f e1a02004 e1a03005 e1a0c004 eb00000b e1a02004 e1a03005 e1a0d004 eb000007 e0422004 e1a03005 eb000004 e1200070 46474849 41424344 16171819 11121314 e0904002 e0b15003 e1a06206 02866001 22866002 42866004 62866008 e1a07267 12477001 32477002 52477004 72477008 83888102 a3888101 c3888202 e1a081a8 82299102 a2299101 c2299202 e1a091c9 e1a0f00e 0000005c" master: @"16"];
}

/*
mov r0, #0x04000000
sub r1, pc, #0x0304
sub r1, r1, #0x00020000
sub r1, r1, #0x01000000
str r1, [r0]
mov r2, #0x05
swp r3, r1, [r0]
swpb r4, r2, [r0]
ldr r5, [r0]
bkpt 0
*/
- (void)testProcessorRunCode_17 {
	[self doTestProcessorRunCode:@"e3a00301 e24f1fc1 e2411802 e2411401 e5801000 e3a02005 e1003091 e1404092 e5905000 e1200070" master: @"17"];
}

/*
 Test with page jumps using RAM (we copy code there).
 
 00000000	e3a00301	mov	r0, #0x4000000
 00000004	e28f1008	add	r1, pc, #0x8
 00000008	e8b103f0	ldm	r1!, {r4, r5, r6, r7, r8, r9}
 0000000c	e88003f0	stm	r0, {r4, r5, r6, r7, r8, r9}
 00000010	e280f004	add	pc, r0, #0x4
 04000000	e2833001	add	r3, r3, #0x1		# not executed
 04000004	e2833002	add	r3, r3, #0x2		# executed
 04000008	e2800601	add	r0, r0, #0x100000
 0400000c	e8b13c00	ldm	r1!, {r10, r11, r12, sp}
 04000010	e8803c00	stm	r0, {r10, r11, r12, sp}
 04000014	ea03fffa	b	0x4100004
 04100000	e2833004	add	r3, r3, #0x4		# not executed
 04100004	e2833008	add	r3, r3, #0x8		# executed
 04100008	e1200070	bkpt	#0x0
 */
- (void)testProcessorRunCode_18 {
	[self doTestProcessorRunCode:@"e3a00301 e28f1008 e8b103f0 e88003f0 e280f004 e2833001 e2833002 e2800601 e8b13c00 e8803c00 ea03fffa e2833004 e2833008 e1200070" master: @"18"];
}

/*
 LDMDB with PC.
 
 add     r11, pc, #40        r11 = 48
 ldmdb   r11, {r4-r11, r13, pc}
 bkpt 0
 .long   0x00000010
 .long   0x0000000F
 .long   0x0000000E
 .long   0x0000000D
 .long   0x0000000C
 .long   0x0000000B
 .long   0x0000000A
 .long   0x00000009
 .long   0x00000008
 */
- (void)testProcessorRunCode_19 {
	[self doTestProcessorRunCode:@"e28fb028 e91baff0 e1200070 00000010 0000000F 0000000E 0000000D 0000000C 0000000B 0000000A 00000009 00000008" master: @"19"];
}

/*
 mov    r1, #3
 mov    r0, #0x200
 add    r0, #0x57
 mov	r2, r0, lsr r1
 mov	r3, r0, lsr #3
 mov	r4, r0, asr r1
 mov	r5, r0, asr #3
 bkpt 0
*/
- (void)testProcessorRunCode_20 {
	[self doTestProcessorRunCode:@"e3a01003 e3a00c02 e2800057 e1a02130 e1a031a0 e1a04150 e1a051c0 e1200070" master: @"20"];
}

/*
 * half-word store
   0:	e3a00301 	mov	r0, #67108864	; 0x4000000
   4:	e28f1b01 	add	r1, pc, #1024	; 0x400
   8:	e1c010b0 	strh	r1, [r0]
   c:	e5902000 	ldr	r2, [r0]
  10:   e1200070    bkpt 0
**/
- (void)testProcessorRunCode_21 {
	[self doTestProcessorRunCode:@"e3a00301 e28f1b01 e1c010b0 e1c010b0 e5902000 e1200070" master: @"21"];
}

// Step tests require a ROM image

- (void)testMemoryReadROM {
	NSString *outputFilePath = [[NSBundle bundleForClass:[self class]] pathForResource:@"master-test-memory-read-rom" ofType:@""];
	[self doTest: ^(TLog* log){
		UMemoryTests::ReadROMTest(log);
	} withOutputFile:outputFilePath];
}

- (void)testMemoryReadWriteRAM {
	NSString *outputFilePath = [[NSBundle bundleForClass:[self class]] pathForResource:@"master-test-memory-read-write-ram" ofType:@""];
	[self doTest: ^(TLog* log){
		UMemoryTests::ReadWriteRAMTest(log);
	} withOutputFile:outputFilePath];
}


@end
