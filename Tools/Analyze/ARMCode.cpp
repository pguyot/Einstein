

#include "ARMCode.h"

#include "Memory.h"
#include "Database.h"

#include <stdio.h>
#include <string.h>


#define BDISP(x) ((((x) & 0xffffff) ^ 0x800000) - 0x800000) /* 26 bit */

#if 0
#define ABORT_SCAN return
#define ABORT_SCAN_0 return 0
#else
#define ABORT_SCAN fflush(stdout); fflush(stderr); throw "ERROR: aborting scan"
#define ABORT_SCAN_0 fflush(stdout); fflush(stderr); throw "ERROR: aborting scan"
#endif


unsigned int branch_address(unsigned int addr, unsigned int cmd=0xFFFFFFFF)
{
	if (cmd==0xffffffff)
		cmd = MEM.getWord(addr);
	if (cmd&0x00800000) { // jump backwards
		return (((cmd&0x00ffffff)|0xff000000)<<2)+addr+8;
	} else { // jump forward
		return ((cmd&0x007fffff)<<2)+addr+8;
	}
}

void tagOffsetTarget(unsigned int addr, unsigned int cmd)
{
	if (((cmd & 0x000f0000) == 0x000f0000) && ((cmd & 0x02000000) == 0))
	{
		int offset = cmd & 0xfff;
		if ((cmd & 0x00800000) == 0)
			offset = -offset;
		DB[offset+addr+8].setAsJumpTarget();
	}
}



void check_switch_case(unsigned int addr, int n_case)
{
	int i;
	printf("Switch/Case statement with %d cases starting at %08X\n", n_case, addr);
	for (i=0; i<n_case; i++) { // nuber of cases plus default case
		printf("case %d at %08X\n", i, addr+4*i);
		check_code_coverage(addr+4*i, 0);
	}
}


// recursively dive into the ARM code from this point on and follow all possible
// execution paths
void check_code_coverage(unsigned int addr, unsigned int flags)
{
	// mark this as jump target
	// FIXME: if (addr<0x00800000 && flags) rom_flags_set(addr, flags); // mark this as a jump target
	DB[addr].setAsJumpTarget();

	for(;;) {

		if (addr>=0x00800000) {
			// see: http://40hz.org/Pages/Newton%20Patches
			if (addr>=0x01A00000 && addr<0x1D00000) {
				unsigned int prev = addr;
				unsigned int next = ( ((addr>>5)&0xffffff80) | (addr&0x0000007f) ) - 0xCE000;
				addr = branch_address(addr, MEM.getWord(next));
				const char *prev_sym = DB.getLabelAt(prev);
				const char *addr_sym = DB.getLabelAt(addr);
				printf("Redirecting call from %08X to %08X (%s->%s)\n", prev, addr, prev_sym, addr_sym);
				if ( prev!=0x01bdef54 && prev!=0x01bb294c && prev!=0x01bb4a50 && prev!=0x01bdef64 && prev!=0x01b4c658
					&& (prev_sym==0 || addr_sym==0 || strcmp(prev_sym, addr_sym)!=0) ) {
					//VERB2 printf("ERROR: Symbols don't match. Verify lookup table offsets! At: 0x%08X (0x%08X)\n", prev, addr);
					//VERB2 printf("  (%s!=%s)\n", prev_sym, addr_sym);
					return;
				}
			} else {
				printf("Can't follow addresses outside of ROM: %08X (%s)\n", addr, DB.getLabelAt(addr));
				return;
			}
		}

		// hmm, this is a dead end!
		if (addr==0x0001862C) return; // FIXME: dead end!

		// we verified this address already - leave
		if (DB[addr].getType()!=0) return;

		// mark this as executable
		DB[addr].setType(TYPE_ARM_CODE);

		// crude interpretation of commands
		unsigned int cmd = MEM.getWord(addr);

//		{
//			char buf[1024]; memset(buf, 0, 1024);
//			sprintf(buf, ":  ");
//			disarm(buf+3, addr, cmd);
//			puts(buf);
//		}

		if ( (cmd&0xf0000000) == 0xf0000000) {
			// special treatment for 0xf... commands
			printf("Aborting: Hit unknown command at %08X: %08X\n", addr, cmd);
			return;
		} else {

			// ldr immediate test
			if ( (cmd&0x0e5f0000) == 0x041f0000) { // test for word access inside the ROM
				int offset = cmd & 0xfff;
				if ((cmd & 0x00800000) == 0) offset = -offset;
				printf("  Read word at 0x%08X\n", addr+8+offset);
				DB[addr+8+offset].setType(TYPE_ARM_INLINE_DATA);
			}

			// follow execution threads
			if ( (cmd&0x0fefffff)==0x01a0f00e) { // quick return command (mov pc,lr)
				if ( (cmd&0xf0000000)==0xe0000000) return; // unconditional - we are done, otherwise continue
			} else if ( (cmd&0x0f000000) == 0x0a000000) { // jump instruction
				if ( (cmd&0xf0000000) == 0xe0000000) { // unconditional
					printf("%08X: unconditional jump to %08X\n", addr, branch_address(addr));
					addr = branch_address(addr);
					DB[addr].setAsJumpTarget();
					continue;
				} else { // conditional, follow both branches (finsih-thread-first recursion)
					unsigned int next = branch_address(addr);
					printf("%08X: conditional jump to %08X, follow later\n", addr, next);
					check_code_coverage(addr+4, 0);
					printf("%08X: following up on conditional jump to %08X\n", addr, next);
					addr = next;
					DB[addr].setAsJumpTarget();
					continue;
				}
			} else if ( (cmd&0x0f000000) == 0x0b000000) { // branch instruction, follow both branches
				unsigned int next = branch_address(addr);
				printf("%08X: subroutine call to %08X, follow later\n", addr, next);
				check_code_coverage(addr+4, 0);
				printf("%08X: following up on subroutine call to %08X\n", addr, next);
				addr = next;
				DB[addr].setAsJumpTarget();
				continue;
			} else if ( (cmd&0x0db6f000) == 0x0120f000) { // msr command does not modifiy pc
			} else if ( (cmd&0x0c000000) == 0x00000000) { // data processing, only important if pc is changed
				if ( (cmd&0x0000f000) == 0x0000f000) { // is the destination register the pc?
					if ( (cmd&0xfffffff0) == 0x908FF100 && (MEM.getWord(addr-4)&0xfff0f000) == 0xE3500000) {
						// cmp rx, #n; addls pc, pc, rx lsl 2
						// This is the pattern for a switch/case statement with default clause. A jump table of size n+1 follows.
						int n_case = (MEM.getWord(addr-4)&0x00000fff)+1, i; // FIXME: is this right?
						printf("Switch/Case statement with %d cases at %08X: %08X\n", n_case, addr, cmd);
						DB[addr].addComment("switch/case statement (0..%d)", n_case);
						addr+=4;
						for (i=0; i<=n_case; i++) { // nuber of cases plus default case
							if (i>0) {
								DB[addr+4*i].addComment("case %d (0x%02X):", i-1, i-1);
							} else {
								DB[addr+4*i].addComment("default:");
							}
							unsigned int cmd = MEM.getWord(addr+4*i);
							if ((cmd&0x0f000000)==0x0a000000) {
								unsigned int dst = BDISP(cmd)*4+(addr+4*i)+8;
								if (i>0) {
									DB[dst].addComment("switch at 0x%08X: case %d (0x%02X)", addr-4, i-1, i-1);
								} else {
									DB[dst].addComment("switch at 0x%08X: default", addr-4);
								}
							}
							printf("case %d at %08X\n", i-1, addr+4*i);
							check_code_coverage(addr+4*i, 0);
						}
						return;
					}
					unsigned int cmd1 = MEM.getWord(addr-4);
					if ( (cmd1&0x0fffffff)==0x01A0E00F && (cmd&0xf0000000)==(cmd1&0xf0000000)) { // mov lr,pc; ...
						// The return address is written into the link register, so in all probability this is a function call
						printf("Later: Register based call at %08X: %08X\n", addr, cmd);
						addr += 4; continue;
					}
					if (addr==0x0038d9a4) { check_switch_case(0x0038d9ac, 33); return; }
					if (addr==0x0038ec98) { check_switch_case(0x0038eca0,  9); return; }
					if (addr==0x00393384) { check_switch_case(0x0039338C,  4); return; }
					if ((cmd&0xf0000000)==0xe0000000) { // always
						printf("Aborting: Data processing command modifying R15 at %08X: %08X\n", addr, cmd);
						// FIXME: This is commonly used in stubs for Newtons style drivers, for example at 0x00387E4C
						// This can be used to find driver implementations in the REx.
						// Search for the ClassInof for TPlatformDriver/TVoyagerPlatform and others.
						// TODO: ABORT_SCAN;
						return;
					} else {
						addr += 4; continue;
					}
				}
			} else if ( (cmd&0x0f000000) == 0x0e000000) { // mcr, mrc (FIXME: probably not changing pc)
			} else if ( (cmd&0x0e000010) == 0x06000010) { // unknown (used to trigger interrupt, FIXME: and then?)
				int i;
				switch ((cmd&0x00ffff00)>>8) {
					case 0: // SystemBoot
					case 1: // ExitToShell
					case 2: // Debugger
					case 3: // DebugStr
					case 4: // PublicFiller
					case 7: // SendTestResults
					case 8: // TapFileCntl
						return;
					case 5: // SystemPanic
						for (i=addr+4; i<0x00800000; i++) {
							DB[i].setType(TYPE_ARM_INLINE_TEXT);
							if (MEM.getByte(i)==0) break;
						}
						addr = (i+1+3)&0xfffffffc; // align i+1 to 4
						if (MEM.getWord(addr)==0) return;
						continue;
				}
				printf("Aborting: opcode 'undefined' found at %08X: %08X\n", addr, cmd);
				ABORT_SCAN;
			} else if ( (cmd&0x0c100000) == 0x04000000) { // str (store to memory)
			} else if ( (cmd&0x0c100000) == 0x04100000) { // ldr (load from memory)
				if ( (cmd&0x0000f000) == 0x0000f000) { // is the destination register the pc?
					unsigned int cmd1 = MEM.getWord(addr-4);
					if ( (cmd1&0x0fffffff)==0x01A0E00F && (cmd&0xf0000000)==(cmd1&0xf0000000)) { // mov lr,pc; ...
						// The return address is writte into the link register, so in all probability this is a function call
						printf("Later: Register based call at %08X: %08X\n", addr, cmd);
						addr += 4; continue;
					}
					if ((cmd&0xf0000000)==0xe0000000) { // always
						if (addr==0x003AD74C) {
							// FIXME: another big switch statement
							return;
						}
						printf("Aborting: LDR command modifying R15 at %08X: %08X\n", addr, cmd);
						// FIXME: more lookup tables (may only be available at runtime)
						return; //ABORT_SCAN;
					} else {
						addr += 4; continue;
					}
				}
				tagOffsetTarget(addr, cmd); // FIXME: what does this do? And why don;t we recurse into the target address?
			} else if ( (cmd&0x0f000000) == 0x0f000000) { // swi (software interrupt)
			} else if ( (cmd&0x0e000000) == 0x0c000000) { // (coprocessor dat transfer) FIXME: may actuall tfer to pc?!
			} else if ( (cmd&0x0e100000) == 0x08000000) { // stm (store multiple to memory)
			} else if ( (cmd&0x0e100000) == 0x08100000) { // ldm (load from memory)
				if ( (cmd&0x00008000) == 0x00008000) { // is the pc among the destination registers?
					// we'll assume it's a return command
					if ( (cmd&0xf0000000)==0xe0000000) return; // unconditional - we are done
					// conditional - continue to check
				}
			} else {
				printf("Aborting: Hit unknown command at %08X: %08X\n", addr, cmd);
				ABORT_SCAN;
			}
		}
		addr += 4;
	}
}

int analyzeAllARMCodeCoverage()
{
	// Analyze ARM jump vectors
	for (uint32_t i=0; i<32; i+=4) {
		check_code_coverage(i);
	}
	// Analyze other known jump tables
	// (...)
	return 0;
}

