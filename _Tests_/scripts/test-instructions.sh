#!/bin/sh
. common.sh $*
# 00000000 b        0x00000000
perl tests.pl "$TESTSPATH" execute-instruction EAFFFFFE

# 00000000 b        0x00018688
#perl tests.pl "$TESTSPATH" execute-instruction EA0061A0

# 00000000 add      pc, r2, #20 (14)
perl tests.pl "$TESTSPATH" execute-instruction E282F014

# 00000000 mov      r4, #156 (9C)
perl tests.pl "$TESTSPATH" execute-instruction E3A0409C

# 00000000 ldr		r0, [r0]
perl tests.pl "$TESTSPATH" execute-instruction E5900000

# 00000000 ldr		r0, [r0, #1]
perl tests.pl "$TESTSPATH" execute-instruction E5900001

# 00000000 ldr		r0, [r0, #2]
perl tests.pl "$TESTSPATH" execute-instruction E5900002

# 00000000 ldr		r0, [r0, #3]
perl tests.pl "$TESTSPATH" execute-instruction E5900003

# 00000000 ldrb		r0, [r0]
perl tests.pl "$TESTSPATH" execute-instruction E5D00000

# 00000000 ldrb		r0, [r0, #1]
perl tests.pl "$TESTSPATH" execute-instruction E5D00001

# 00000000 ldrb		r0, [r0, #2]
perl tests.pl "$TESTSPATH" execute-instruction E5D00002

# 00000000 ldrb		r0, [r0, #3]
perl tests.pl "$TESTSPATH" execute-instruction E5D00003

# 00000000 beq      0x00000024 (will not branch)
perl tests.pl "$TESTSPATH" execute-instruction 0A000007

# 00000000 bne      0x00000024 (will branch)
perl tests.pl "$TESTSPATH" execute-instruction 1A000007

# ldrh     r7, [r1]
perl tests.pl "$TESTSPATH" execute-instruction E1D170B0

# ldrh     r7, [r1, #2]
perl tests.pl "$TESTSPATH" execute-instruction E1D170B2

# ldrh     r7, 0x00000000
# ldrh     r7, [r15, -#8]
perl tests.pl "$TESTSPATH" execute-instruction E15F70B8

# ldrh     r7, 0x00000002
# ldrh     r7, [r15, -#6]
perl tests.pl "$TESTSPATH" execute-instruction E15F70B6

# 00000000 mov      r0, #128
# 00000004 ldrh     r7, [r0, -#128]
perl tests.pl "$TESTSPATH" execute-two-instructions E3A00080-E15078B0

# 00000000 mov      r0, #128
# 00000004 ldrh     r7, [r0, -#124]
perl tests.pl "$TESTSPATH" execute-two-instructions E3A00080-E15077BC

# 00000000 mov      r0, #128
# 00000004 ldrh     r7, [r0, -#126]
perl tests.pl "$TESTSPATH" execute-two-instructions E3A00080-E15077BE

# 00000000 mov      r1, #12 (C)
# 00000004 mov      r1, r1, lsl #1 (1)
perl tests.pl "$TESTSPATH" execute-two-instructions E3A0100C-E1A01081

# 00000000 and      r9, r4, r2, lsr #20 (14)
perl tests.pl "$TESTSPATH" execute-instruction-state1 E0049A22

# 00000000 and      r9, r4, r0, lsr #24 (18)
perl tests.pl "$TESTSPATH" execute-instruction-state1 E0049C20

# 00000000 and      r9, r4, r2, lsr #24 (18)
perl tests.pl "$TESTSPATH" execute-instruction-state1 E0049C22

# 00000000 and      r9, r4, r3, lsr #24 (18)
perl tests.pl "$TESTSPATH" execute-instruction-state1 E0049C23

# 00000000 mul      r11, r9, r10
perl tests.pl "$TESTSPATH" execute-instruction-state1 E00B0A99

# 00000000 muls     r11, r9, r10
perl tests.pl "$TESTSPATH" execute-instruction-state1 E01B0A99

# 00000000 mla      r11, r9, r10, r0
perl tests.pl "$TESTSPATH" execute-instruction-state1 E02B0A99

# 00000000 mla      r11, r9, r10, r11
perl tests.pl "$TESTSPATH" execute-instruction-state1 E02BBA99

# 00000000 mlas     r11, r9, r10, r10
perl tests.pl "$TESTSPATH" execute-instruction-state1 E03BAA99

# 00000000 mlas     r11, r9, r10, r11
perl tests.pl "$TESTSPATH" execute-instruction-state1 E03BBA99

# 00000000 add      r0, r0, r5
perl tests.pl "$TESTSPATH" execute-instruction-state1 E0800005

# 00000000 add      r1, r4, r5
perl tests.pl "$TESTSPATH" execute-instruction-state1 E0841005

# eor	r0, r0, 0x44000000
perl tests.pl "$TESTSPATH" execute-instruction-state1 E2200311

# eor	r12, r0, 0x44000000
perl tests.pl "$TESTSPATH" execute-instruction-state1 E220C311

# eor	r0, r12, 0x44000000
perl tests.pl "$TESTSPATH" execute-instruction-state1 E22C0311

# eor	r0, r13, 0x44000000
perl tests.pl "$TESTSPATH" execute-instruction-state1 E22D0311

# eor	r0, r0, 0x10000
perl tests.pl "$TESTSPATH" execute-instruction-state1 E2200801

# eor	r0, r0, 0xA100
perl tests.pl "$TESTSPATH" execute-instruction-state1 E2300CA1

# rsb      r0, r2, r2, lsl #4
perl tests.pl "$TESTSPATH" execute-instruction-state1 E0620202

# orr      r0, r0, 0xE0
perl tests.pl "$TESTSPATH" execute-instruction-state1 E38000E0

# orr      r0, r0, 0xEF
perl tests.pl "$TESTSPATH" execute-instruction-state1 E38000EF

# 00000000 sub      r11, r12, #4 (4)
perl tests.pl "$TESTSPATH" execute-instruction-state1 E24CB004

# 00000000 sub      r13, r13, #4 (4)
perl tests.pl "$TESTSPATH" execute-instruction-state1 E24DD004

# 00000000 add      r5, r5, #3584 (E00)
perl tests.pl "$TESTSPATH" execute-instruction-state1 E2855C0E

# 00000000 mov      r6, #254 (FE)
perl tests.pl "$TESTSPATH" execute-instruction-state1 E3A060FE

# ldr   r0, [r4, #0x010]!
perl tests.pl "$TESTSPATH" execute-instruction-state1 E5B40010

# ldr   r0, [pc, #0x010]!
perl tests.pl "$TESTSPATH" execute-instruction-state1 E5BF0010

# mov   r3, r12, lsl r3
perl tests.pl "$TESTSPATH" execute-instruction-state2 E1A0331C

# Make sure we don't crash.
# 00000000 add      pc, r2, #20 (14)
# 00000004 add      pc, r2, #20 (14)
perl tests.pl "$TESTSPATH" execute-two-instructions E282F014-E282F014

# Make sure we don't crash.
# 00000000 mov      r0, #211 (D3)
# 00000004 msr      cpsr, r0
perl tests.pl "$TESTSPATH" execute-two-instructions E3A000D3-E129F000

# Test adds
# 00000000 adds     r2, r2, #0
perl tests.pl "$TESTSPATH" execute-instruction E2922000

# Test adds
# 00000000 mvn      r2, #0 (0)
# 00000000 adds     r2, r2, #1
perl tests.pl "$TESTSPATH" execute-two-instructions E3E02000-E2922001

# TODO: test that ldr pc, [rx] puts the value + 4.

# Test mrs
# 00000000 mrs      r1, spsr
# Note: SPSR is 0 now.
perl tests.pl "$TESTSPATH" execute-instruction E14F1000

# Test mrs
# 00000000 mrs      r1, cpsr
# Note: CPSR is 00000013 (user mode) now.
perl tests.pl "$TESTSPATH" execute-instruction E10F1000

# TODO: test that swp instructions can be followed by another instruction.

# 00000000 mov      r12, #8
# 00000004 ldmdb    r12, {r8, r9}
perl tests.pl "$TESTSPATH" execute-two-instructions E3A0C008-E91C0300

# 00000000 mcr      15, 0, r0, cr1, cr1, {0}
perl tests.pl "$TESTSPATH" execute-instruction EE010F11

# 00000000 mla      r0, r4, r0, r5
perl tests.pl "$TESTSPATH" execute-instruction-state1 E0205094
