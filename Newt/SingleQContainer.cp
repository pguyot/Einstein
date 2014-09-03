/**
 * Collection of transcoded functions
 */

#include "SingleQContainer.h"


/**
 * Transcoded function __ct__17TSingleQContainerFv
 * ROM: 0x001E2BBC - 0x001E2BCC
 */
void Func_0x001E2BBC(TARMProcessor* ioCPU)
{
L001E2BBC: // 0xE3300000  teq	r0, #0x00000000
	{
		KUInt32 Opnd2 = 0x00000000;
		KUInt32 Opnd1 = ioCPU->mCurrentRegisters[0];
		const KUInt32 theResult = Opnd1 ^ Opnd2;
		SetCPSRBitsForLogicalOpLeaveCarry( ioCPU, theResult );
	}
L001E2BC0: // 0x03A00008  moveq	r0, #0x00000008
	if (ioCPU->TestEQ()) {
		KUInt32 Opnd2 = 0x00000008;
		const KUInt32 theResult = Opnd2;
		ioCPU->mCurrentRegisters[0] = theResult;
	}
L001E2BC4: // 0x0A67AEDB  beq	__nw__FUi
	if (ioCPU->TestEQ()) {
		return Func_0x00318EE8(ioCPU);
	}
L001E2BC8: // 0xE1A0F00E  mov	pc, lr
	{
		KUInt32 Opnd2 = ioCPU->mCurrentRegisters[14];
		const KUInt32 theResult = Opnd2;
		SETPC(theResult + 4);
		return; //FURTHERCALLNEXT_AFTERSETPC;
	}
	ioCPU->mCurrentRegisters[15] = 0x001E2BCC + 4;
}
T_ROM_SIMULATION(0x001E2BBC, "__ct__17TSingleQContainerFv")


/**
 * Transcoded function Init__17TSingleQContainerFUl
 * ROM: 0x001E2BCC - 0x001E2BDC
 */
void Func_0x001E2BCC(TARMProcessor* ioCPU)
{
L001E2BCC: // 0xE3A02000  mov	r2, #0x00000000
	{
		KUInt32 Opnd2 = 0x00000000;
		const KUInt32 theResult = Opnd2;
		ioCPU->mCurrentRegisters[2] = theResult;
	}
L001E2BD0: // 0xE5801004  str	r1, [r0, #0x004]
	{
		KUInt32 offset = 0x00000004;
		KUInt32 theAddress = ioCPU->mCurrentRegisters[0] + offset;
		KUInt32 theValue = ioCPU->mCurrentRegisters[1];
		ioCPU->ManagedMemoryWrite(theAddress, theValue, 0x001E2BD0);
	}
L001E2BD4: // 0xE5802000  str	r2, [r0]
	{
		KUInt32 offset = 0x00000000;
		KUInt32 theAddress = ioCPU->mCurrentRegisters[0] + offset;
		KUInt32 theValue = ioCPU->mCurrentRegisters[2];
		ioCPU->ManagedMemoryWrite(theAddress, theValue, 0x001E2BD4);
	}
L001E2BD8: // 0xE1A0F00E  mov	pc, lr
	{
		KUInt32 Opnd2 = ioCPU->mCurrentRegisters[14];
		const KUInt32 theResult = Opnd2;
		SETPC(theResult + 4);
		return; //FURTHERCALLNEXT_AFTERSETPC;
	}
	ioCPU->mCurrentRegisters[15] = 0x001E2BDC + 4;
}
T_ROM_SIMULATION(0x001E2BCC, "Init__17TSingleQContainerFUl")


/**
 * Transcoded function Add__17TSingleQContainerFPv
 * ROM: 0x001E2BDC - 0x001E2BF4
 */
void Func_0x001E2BDC(TARMProcessor* ioCPU)
{
L001E2BDC: // 0xE5902004  ldr	r2, [r0, #0x004]
	{
		KUInt32 offset = 0x00000004;
		KUInt32 theAddress = ioCPU->mCurrentRegisters[0] + offset;
		KUInt32 theData = ioCPU->ManagedMemoryRead(theAddress, 0x001E2BDC);
		ioCPU->mCurrentRegisters[2] = theData;
	}
L001E2BE0: // 0xE0821001  add	r1, r2, r1
	{
		KUInt32 Opnd2 = ioCPU->mCurrentRegisters[1];
		KUInt32 Opnd1 = ioCPU->mCurrentRegisters[2];
		const KUInt32 theResult = Opnd1 + Opnd2;
		ioCPU->mCurrentRegisters[1] = theResult;
	}
L001E2BE4: // 0xE5902000  ldr	r2, [r0]
	{
		KUInt32 offset = 0x00000000;
		KUInt32 theAddress = ioCPU->mCurrentRegisters[0] + offset;
		KUInt32 theData = ioCPU->ManagedMemoryRead(theAddress, 0x001E2BE4);
		ioCPU->mCurrentRegisters[2] = theData;
	}
L001E2BE8: // 0xE5812000  str	r2, [r1]
	{
		KUInt32 offset = 0x00000000;
		KUInt32 theAddress = ioCPU->mCurrentRegisters[1] + offset;
		KUInt32 theValue = ioCPU->mCurrentRegisters[2];
		ioCPU->ManagedMemoryWrite(theAddress, theValue, 0x001E2BE8);
	}
L001E2BEC: // 0xE5801000  str	r1, [r0]
	{
		KUInt32 offset = 0x00000000;
		KUInt32 theAddress = ioCPU->mCurrentRegisters[0] + offset;
		KUInt32 theValue = ioCPU->mCurrentRegisters[1];
		ioCPU->ManagedMemoryWrite(theAddress, theValue, 0x001E2BEC);
	}
L001E2BF0: // 0xE1A0F00E  mov	pc, lr
	{
		KUInt32 Opnd2 = ioCPU->mCurrentRegisters[14];
		const KUInt32 theResult = Opnd2;
		SETPC(theResult + 4);
		return; //FURTHERCALLNEXT_AFTERSETPC;
	}
	ioCPU->mCurrentRegisters[15] = 0x001E2BF4 + 4;
}
T_ROM_SIMULATION(0x001E2BDC, "Add__17TSingleQContainerFPv")


/**
 * Transcoded function Remove__17TSingleQContainerFv
 * ROM: 0x001E2BF4 - 0x001E2C18
 */
void Func_0x001E2BF4(TARMProcessor* ioCPU)
{
L001E2BF4: // 0xE5901000  ldr	r1, [r0]
	{
		KUInt32 offset = 0x00000000;
		KUInt32 theAddress = ioCPU->mCurrentRegisters[0] + offset;
		KUInt32 theData = ioCPU->ManagedMemoryRead(theAddress, 0x001E2BF4);
		ioCPU->mCurrentRegisters[1] = theData;
	}
L001E2BF8: // 0xE1B02001  movs	r2, r1
	{
		KUInt32 Opnd2 = ioCPU->mCurrentRegisters[1];
		const KUInt32 theResult = Opnd2;
		ioCPU->mCurrentRegisters[2] = theResult;
		SetCPSRBitsForLogicalOpLeaveCarry( ioCPU, theResult );
	}
L001E2BFC: // 0x15922000  ldrne	r2, [r2]
	if (ioCPU->TestNE()) {
		KUInt32 offset = 0x00000000;
		KUInt32 theAddress = ioCPU->mCurrentRegisters[2] + offset;
		KUInt32 theData = ioCPU->ManagedMemoryRead(theAddress, 0x001E2BFC);
		ioCPU->mCurrentRegisters[2] = theData;
	}
L001E2C00: // 0x15802000  strne	r2, [r0]
	if (ioCPU->TestNE()) {
		KUInt32 offset = 0x00000000;
		KUInt32 theAddress = ioCPU->mCurrentRegisters[0] + offset;
		KUInt32 theValue = ioCPU->mCurrentRegisters[2];
		ioCPU->ManagedMemoryWrite(theAddress, theValue, 0x001E2C00);
	}
L001E2C04: // 0xE3310000  teq	r1, #0x00000000
	{
		KUInt32 Opnd2 = 0x00000000;
		KUInt32 Opnd1 = ioCPU->mCurrentRegisters[1];
		const KUInt32 theResult = Opnd1 ^ Opnd2;
		SetCPSRBitsForLogicalOpLeaveCarry( ioCPU, theResult );
	}
L001E2C08: // 0x03A00000  moveq	r0, #0x00000000
	if (ioCPU->TestEQ()) {
		KUInt32 Opnd2 = 0x00000000;
		const KUInt32 theResult = Opnd2;
		ioCPU->mCurrentRegisters[0] = theResult;
	}
L001E2C0C: // 0x15900004  ldrne	r0, [r0, #0x004]
	if (ioCPU->TestNE()) {
		KUInt32 offset = 0x00000004;
		KUInt32 theAddress = ioCPU->mCurrentRegisters[0] + offset;
		KUInt32 theData = ioCPU->ManagedMemoryRead(theAddress, 0x001E2C0C);
		ioCPU->mCurrentRegisters[0] = theData;
	}
L001E2C10: // 0x10410000  subne	r0, r1, r0
	if (ioCPU->TestNE()) {
		KUInt32 Opnd2 = ioCPU->mCurrentRegisters[0];
		KUInt32 Opnd1 = ioCPU->mCurrentRegisters[1];
		const KUInt32 theResult = Opnd1 - Opnd2;
		ioCPU->mCurrentRegisters[0] = theResult;
	}
L001E2C14: // 0xE1A0F00E  mov	pc, lr
	{
		KUInt32 Opnd2 = ioCPU->mCurrentRegisters[14];
		const KUInt32 theResult = Opnd2;
		SETPC(theResult + 4);
		return; //FURTHERCALLNEXT_AFTERSETPC;
	}
	ioCPU->mCurrentRegisters[15] = 0x001E2C18 + 4;
}
T_ROM_SIMULATION(0x001E2BF4, "Remove__17TSingleQContainerFv")


/**
 * Transcoded function Peek__17TSingleQContainerFv
 * ROM: 0x001E2C18 - 0x001E2C30
 */
void Func_0x001E2C18(TARMProcessor* ioCPU)
{
L001E2C18: // 0xE5901000  ldr	r1, [r0]
	{
		KUInt32 offset = 0x00000000;
		KUInt32 theAddress = ioCPU->mCurrentRegisters[0] + offset;
		KUInt32 theData = ioCPU->ManagedMemoryRead(theAddress, 0x001E2C18);
		ioCPU->mCurrentRegisters[1] = theData;
	}
L001E2C1C: // 0xE3310000  teq	r1, #0x00000000
	{
		KUInt32 Opnd2 = 0x00000000;
		KUInt32 Opnd1 = ioCPU->mCurrentRegisters[1];
		const KUInt32 theResult = Opnd1 ^ Opnd2;
		SetCPSRBitsForLogicalOpLeaveCarry( ioCPU, theResult );
	}
L001E2C20: // 0x03A00000  moveq	r0, #0x00000000
	if (ioCPU->TestEQ()) {
		KUInt32 Opnd2 = 0x00000000;
		const KUInt32 theResult = Opnd2;
		ioCPU->mCurrentRegisters[0] = theResult;
	}
L001E2C24: // 0x15900004  ldrne	r0, [r0, #0x004]
	if (ioCPU->TestNE()) {
		KUInt32 offset = 0x00000004;
		KUInt32 theAddress = ioCPU->mCurrentRegisters[0] + offset;
		KUInt32 theData = ioCPU->ManagedMemoryRead(theAddress, 0x001E2C24);
		ioCPU->mCurrentRegisters[0] = theData;
	}
L001E2C28: // 0x10410000  subne	r0, r1, r0
	if (ioCPU->TestNE()) {
		KUInt32 Opnd2 = ioCPU->mCurrentRegisters[0];
		KUInt32 Opnd1 = ioCPU->mCurrentRegisters[1];
		const KUInt32 theResult = Opnd1 - Opnd2;
		ioCPU->mCurrentRegisters[0] = theResult;
	}
L001E2C2C: // 0xE1A0F00E  mov	pc, lr
	{
		KUInt32 Opnd2 = ioCPU->mCurrentRegisters[14];
		const KUInt32 theResult = Opnd2;
		SETPC(theResult + 4);
		return; //FURTHERCALLNEXT_AFTERSETPC;
	}
	ioCPU->mCurrentRegisters[15] = 0x001E2C30 + 4;
}
T_ROM_SIMULATION(0x001E2C18, "Peek__17TSingleQContainerFv")


/**
 * Transcoded function GetNext__17TSingleQContainerFPv
 * ROM: 0x001E2C30 - 0x001E2C50
 */
void Func_0x001E2C30(TARMProcessor* ioCPU)
{
L001E2C30: // 0xE3310000  teq	r1, #0x00000000
	{
		KUInt32 Opnd2 = 0x00000000;
		KUInt32 Opnd1 = ioCPU->mCurrentRegisters[1];
		const KUInt32 theResult = Opnd1 ^ Opnd2;
		SetCPSRBitsForLogicalOpLeaveCarry( ioCPU, theResult );
	}
L001E2C34: // 0x15900004  ldrne	r0, [r0, #0x004]
	if (ioCPU->TestNE()) {
		KUInt32 offset = 0x00000004;
		KUInt32 theAddress = ioCPU->mCurrentRegisters[0] + offset;
		KUInt32 theData = ioCPU->ManagedMemoryRead(theAddress, 0x001E2C34);
		ioCPU->mCurrentRegisters[0] = theData;
	}
L001E2C38: // 0x10801001  addne	r1, r0, r1
	if (ioCPU->TestNE()) {
		KUInt32 Opnd2 = ioCPU->mCurrentRegisters[1];
		KUInt32 Opnd1 = ioCPU->mCurrentRegisters[0];
		const KUInt32 theResult = Opnd1 + Opnd2;
		ioCPU->mCurrentRegisters[1] = theResult;
	}
L001E2C3C: // 0x15911000  ldrne	r1, [r1]
	if (ioCPU->TestNE()) {
		KUInt32 offset = 0x00000000;
		KUInt32 theAddress = ioCPU->mCurrentRegisters[1] + offset;
		KUInt32 theData = ioCPU->ManagedMemoryRead(theAddress, 0x001E2C3C);
		ioCPU->mCurrentRegisters[1] = theData;
	}
L001E2C40: // 0x13310000  teqne	r1, #0x00000000
	if (ioCPU->TestNE()) {
		KUInt32 Opnd2 = 0x00000000;
		KUInt32 Opnd1 = ioCPU->mCurrentRegisters[1];
		const KUInt32 theResult = Opnd1 ^ Opnd2;
		SetCPSRBitsForLogicalOpLeaveCarry( ioCPU, theResult );
	}
L001E2C44: // 0x03A00000  moveq	r0, #0x00000000
	if (ioCPU->TestEQ()) {
		KUInt32 Opnd2 = 0x00000000;
		const KUInt32 theResult = Opnd2;
		ioCPU->mCurrentRegisters[0] = theResult;
	}
L001E2C48: // 0x10410000  subne	r0, r1, r0
	if (ioCPU->TestNE()) {
		KUInt32 Opnd2 = ioCPU->mCurrentRegisters[0];
		KUInt32 Opnd1 = ioCPU->mCurrentRegisters[1];
		const KUInt32 theResult = Opnd1 - Opnd2;
		ioCPU->mCurrentRegisters[0] = theResult;
	}
L001E2C4C: // 0xE1A0F00E  mov	pc, lr
	{
		KUInt32 Opnd2 = ioCPU->mCurrentRegisters[14];
		const KUInt32 theResult = Opnd2;
		SETPC(theResult + 4);
		return; //FURTHERCALLNEXT_AFTERSETPC;
	}
	ioCPU->mCurrentRegisters[15] = 0x001E2C50 + 4;
}
T_ROM_SIMULATION(0x001E2C30, "GetNext__17TSingleQContainerFPv")



/**
 * End of transcoded functions
 */
