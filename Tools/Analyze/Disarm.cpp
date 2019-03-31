/* Instruction printing code for the ARM
 Copyright (C) 1994, 95, 96, 97, 1998 Free Software Foundation, Inc. 
 Contributed by Richard Earnshaw (rwe@pegasus.esprit.ec.org)
 Modification by James G. Smith (jsmith@cygnus.co.uk)
 
 This file is part of libopcodes. 
 
 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free
 Software Foundation; either version 2 of the License, or (at your option)
 any later version. 
 
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 more details. 
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "Disarm.h"

#include "Database.h"
#include "Memory.h"

#include <stdio.h>
#include <string.h>


//const char *get_symbol_at(unsigned int addr);
//const char *get_plain_symbol_at(unsigned int addr);
//unsigned int rom_w(unsigned int addr);


static const char* arm_conditional[] = {
  "eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc", "hi", "ls", "ge", "lt", "gt", "le", "", "nv"};

static const char* arm_regnames[] = {
  "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "sp", "lr", "pc"};

static const char* arm_fp_const[] = {
  "0.0", "1.0", "2.0", "3.0", "4.0", "5.0", "0.5", "10.0"};

static const char* arm_shift[] = {
  "lsl", "lsr", "asr", "ror"};

static int print_insn_arm(unsigned int,
                          int,
                          char*,
                          char*);

// ============================================================	//
// void arm_decode_shift(int, char*)
// ============================================================	//

static void arm_decode_shift(int given,
                             char* str)
{
  char tmpStr[512];
  sprintf(tmpStr, "%s", arm_regnames[given & 0xf]);
  str = strcat(str, tmpStr);
  if ((given & 0xff0) != 0)
  {
    if ((given & 0x10) == 0)
    {
      int amount = (given & 0xf80) >> 7;
      int shift = (given & 0x60) >> 5;
      if (amount == 0)
      {
        if (shift == 3)
        {
          sprintf(tmpStr, ", rrx");
          str = strcat(str, tmpStr);
          return;
        }
        amount = 32;
      }
      sprintf(tmpStr, ", %s #%d", arm_shift[shift], amount);
      str = strcat(str, tmpStr);
    }
    else
    {
      sprintf(tmpStr, ", %s %s", arm_shift[(given & 0x60) >> 5], arm_regnames[(given & 0xf00) >> 8]);
      str = strcat(str, tmpStr);
    }
  }
}

// ============================================================	//
// void print_insn_arm (unsigned int, int, char*)
// ============================================================	//

static int print_insn_arm(unsigned int pc,
                          int given,
                          char* str,
                          char* cmt)
{
  struct arm_opcode*     insn;
  char tmpStr[512];
  
  for (insn = arm_opcodes; insn->assembler; insn++)
  {
    if ((given & insn->mask) == insn->value)
    {
      const char* c;
      for (c = insn->assembler; *c; c++)
      {
        if (*c == '%')
        {
          switch (*++c)
          {
            case '%':
              str = strcat(str, "%%");
              break;
              
            case 'a':
              if (((given & 0x000f0000) == 0x000f0000) && ((given & 0x02000000) == 0))
              {
                int offset = given & 0xfff;
                if ((given & 0x00800000) == 0)
                  offset = -offset;
                unsigned int addr = offset + pc + 8;
                const char *sym = DB.getLabelAt(addr);
                if (sym) 
                  sprintf(tmpStr, "%s", sym);
                else
                  sprintf(tmpStr, "L%.8X", addr);
                str = strcat(str, tmpStr);
                if (addr<0x00800000) {
                  unsigned int val = MEM.getWord(addr);
                  const char *vsym = DB.getLabelAt(val);
                  if (vsym) {
                    sprintf(cmt, " [ %s (0x%08X) ]", vsym, val);
                  } else {
                    sprintf(cmt, " [ 0x%08X ]", val);
                  }
                }
              }
              else
              {
                sprintf(tmpStr, "[%s", arm_regnames[(given >> 16) & 0xf]);
                str = strcat(str, tmpStr);
                if ((given & 0x01000000) != 0)
                {
                  if ((given & 0x02000000) == 0)
                  {
                    int offset = given & 0xfff;
                    if (offset)
                    {
                      sprintf(tmpStr, ", #%s%d", (((given & 0x00800000) == 0) ? "-" : ""), offset);
                      str = strcat(str, tmpStr);
                    }
                  }
                  else
                  {
                    sprintf(tmpStr, ", %s", (((given & 0x00800000) == 0) ? "-" : ""));
                    str = strcat(str, tmpStr);
                    arm_decode_shift(given, str);
                  }
                  
                  sprintf(tmpStr, "]%s", ((given & 0x00200000) != 0) ? "!" : "");
                  str = strcat(str, tmpStr);
                }
                else
                {
                  if ((given & 0x02000000) == 0)
                  {
                    int offset = given & 0xfff;
                    if (offset)
                    {
                      sprintf(tmpStr, "], #%s%d", (((given & 0x00800000) == 0) ? "-" : ""), offset);
                      str = strcat(str, tmpStr);
                    }
                    else
                      str = strcat(str, "]");
                  }
                  else
                  {
                    sprintf(tmpStr, "], %s", (((given & 0x00800000) == 0) ? "-" : ""));
                    str = strcat(str, tmpStr);
                    arm_decode_shift(given, str);
                  }
                }
              }
              break;
              
            case 's':
              if ((given & 0x004f0000) == 0x004f0000)
              {
                /* PC relative with immediate offset */
                int offset = ((given & 0xf00) >> 4) | (given & 0xf);
                if ((given & 0x00800000) == 0)
                  offset = -offset;
                sprintf(tmpStr, "0x%.8X", offset + pc + 8);
                str = strcat(str, tmpStr);
              }
              else
              {
                sprintf(tmpStr, "[%s", arm_regnames[(given >> 16) & 0xf]);
                str = strcat(str, tmpStr);
                if ((given & 0x01000000) != 0)
                {
                  /* pre-indexed */
                  if ((given & 0x00400000) == 0x00400000)
                  {
                    /* immediate */
                    int offset = ((given & 0xf00) >> 4) | (given & 0xf);
                    if (offset)
                    {
                      sprintf(tmpStr, ", #%s%d", (((given & 0x00800000) == 0) ? "-" : ""), offset);
                      str = strcat(str, tmpStr);
                    }
                  }
                  else
                  {
                    /* register */
                    sprintf(tmpStr, ", %s%s", (((given & 0x00800000) == 0) ? "-" : ""), arm_regnames[given & 0xf]);
                    str = strcat(str, tmpStr);
                  }
                  
                  sprintf(tmpStr, "]%s", ((given & 0x00200000) != 0) ? "!" : "");
                  str = strcat(str, tmpStr);
                }
                else
                {
                  /* post-indexed */
                  if ((given & 0x00400000) == 0x00400000)
                  {
                    /* immediate */
                    int offset = ((given & 0xf00) >> 4) | (given & 0xf);
                    if (offset)
                    {
                      sprintf(tmpStr, "], #%s%d", (((given & 0x00800000) == 0) ? "-" : ""), offset);
                      str = strcat(str, tmpStr);
                    }
                    else
                      strcat(str, "]");
                    
                  }
                  else
                  {
                    /* register */
                    sprintf(tmpStr, "], %s%s", (((given & 0x00800000) == 0) ? "-" : ""), arm_regnames[given & 0xf]);
                    str = strcat(str, tmpStr);
                  }
                }
              }
              break;
              
            case 'b': 
            {
              unsigned int dst = BDISP(given) * 4 + pc + 8;
              const char *sym = DB.getLabelAt(dst);
              if (sym) {
                if (dst<0x00800000)
                  sprintf(tmpStr, "%s", sym);
                else
                  sprintf(tmpStr, "VEC_%s", sym);
              } else {
                if (dst<0x00800000)
                  sprintf(tmpStr, "L%08X", dst);
                else
                  sprintf(tmpStr, "0x%08X", dst); // FIXME: wrong offset!
              }
              str = strcat(str, tmpStr);
            }
              break;
              
            case 'c':
              sprintf(tmpStr, "%s", arm_conditional[(given >> 28) & 0xf]);
              str = strcat(str, tmpStr);
              break;
              
            case 'm':
            {
              int started = 0;
              int RangeEnd = -2;
              int RangeFirst = -2;
              int reg;
              
              // Register List.
              
              str = strcat(str, "{");
              for (reg = 0; reg < 16; reg++)
                if ((given & (1 << reg)) != 0)
                {
                  if (RangeEnd + 1 == reg)
                  {
                    // I am in a block.
                    if (reg == 15)
                    {
                      // I should finish it anyway.
                      sprintf(tmpStr, "-%s", arm_regnames[reg]);
                      str = strcat(str, tmpStr);
                    }
                    else
                    {
                      RangeEnd++;
                    }
                  }
                  else
                  {
                    // I am not in a block.
                    // The block has been finished when processing the first reg out of it.
                    if (started)
                      str = strcat(str, ", ");
                    started = 1;
                    // Let's print this register & set both RangeEnd and RangeFirst.
                    RangeEnd = RangeFirst = reg;
                    sprintf(tmpStr, "%s", arm_regnames[reg]);
                    str = strcat(str, tmpStr);
                  }
                }
                else
                {
                  // This register is not here. Hence, I finish the old block.
                  if (started)
                  {
                    if (RangeEnd > RangeFirst)
                    {
                      if (RangeEnd == RangeFirst + 1)
                      {
                        // Two registers: I do comma.
                        sprintf(tmpStr, ", %s", arm_regnames[RangeEnd]);
                      }
                      else
                      {
                        // More: I do dash.
                        sprintf(tmpStr, "-%s", arm_regnames[RangeEnd]);
                      }
                      str = strcat(str, tmpStr);
                    }
                  }
                  RangeEnd = RangeFirst = -2;
                }
              str = strcat(str, "}");
            }
              break;
              
            case 'o':
              if ((given & 0x02000000) != 0)
              {
                int rotate = (given & 0xf00) >> 7;
                int immed = (given & 0xff);
                unsigned int val = ((immed << (32 - rotate)) | (immed >> rotate)) & 0xffffffff;
                sprintf(tmpStr, "#%d", val);
                str = strcat(str, tmpStr);
                sprintf(tmpStr, " [ 0x%08X ]", val);
                strcat(cmt, tmpStr);
              }
              else
                arm_decode_shift(given, str);
              break;
              
            case 'p':
              if ((given & 0x0000f000) == 0x0000f000)
                str = strcat(str, "p");
              break;
              
            case 't':
              if ((given & 0x01200000) == 0x00200000)
                str = strcat(str, "t");
              break;
              
            case 'h':
              if ((given & 0x00000020) == 0x00000020)
                str = strcat(str, "h");
              else
                str = strcat(str, "b");
              break;
              
            case 'A':
              sprintf(tmpStr, "[%s", arm_regnames[(given >> 16) & 0xf]);
              str = strcat(str, tmpStr);
              if ((given & 0x01000000) != 0)
              {
                int offset = given & 0xff;
                if (offset)
                {
                  sprintf(tmpStr, ", #%s%d]%s", ((given & 0x00800000) == 0 ? "-" : ""), offset * 4, ((given & 0x00200000) != 0 ? "!" : ""));
                  str = strcat(str, tmpStr);
                }
                else
                  str = strcat(str, "]");
              }
              else
              {
                int offset = given & 0xff;
                if (offset)
                {
                  sprintf(tmpStr, "], #%s%d", ((given & 0x00800000) == 0 ? "-" : ""), offset * 4);
                  str = strcat(str, tmpStr);
                }
                else
                  str = strcat(str, "]");
              }
              break;
              
            case 'C':
              switch (given & 0x00090000)
            {
              case 0:
                str = strcat(str, "_???");
                break;
              case 0x10000:
                str = strcat(str, "_ctl");
                break;
              case 0x80000:
                str = strcat(str, "_flg");
                break;
            }
              break;
              
            case 'F':
              switch (given & 0x00408000)
            {
              case 0:
                str = strcat(str, "4");
                break;
              case 0x8000:
                str = strcat(str, "1");
                break;
              case 0x00400000:
                str = strcat(str, "2");
                break;
              default:
                str = strcat(str, "3");
            }
              break;
              
            case 'P':
              switch (given & 0x00080080)
            {
              case 0:
                str = strcat(str, "s");
                break;
              case 0x80:
                str = strcat(str, "d");
                break;
              case 0x00080000:
                str = strcat(str, "e");
                break;
              default:
                str = strcat(str, "<illegal precision>");
                break;
            }
              break;
            case 'Q':
              switch (given & 0x00408000)
            {
              case 0:
                str = strcat(str, "s");
                break;
              case 0x8000:
                str = strcat(str, "d");
                break;
              case 0x00400000:
                str = strcat(str, "e");
                break;
              default:
                str = strcat(str, "p");
                break;
            }
              break;
            case 'R':
              switch (given & 0x60)
            {
              case 0:
                break;
              case 0x20:
                str = strcat(str, "p");
                break;
              case 0x40:
                str = strcat(str, "m");
                break;
              default:
                str = strcat(str, "z");
                break;
            }
              break;
              
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
              int bitstart = *c++ - '0';
              int bitend = 0;
              while (*c >= '0' && *c <= '9')
                bitstart = (bitstart * 10) + *c++ - '0';
              
              switch (*c)
              {
                case '-':
                  c++;
                  while (*c >= '0' && *c <= '9')
                    bitend = (bitend * 10) + *c++ - '0';
                  if (!bitend)
                    throw "disarm";
                  switch (*c)
                {
                  case 'r':
                  {
                    int reg;
                    reg = given >> bitstart;
                    reg &= (2 << (bitend - bitstart)) - 1;
                    sprintf(tmpStr, "%s", arm_regnames[reg]);
                    str = strcat(str, tmpStr);
                  }
                    break;
                  case 'd':
                  {
                    int reg;
                    reg = given >> bitstart;
                    reg &= (2 << (bitend - bitstart)) - 1;
                    sprintf(tmpStr, "%d", reg);
                    str = strcat(str, tmpStr);
                  }
                    break;
                  case 'x':
                  {
                    int reg;
                    reg = given >> bitstart;
                    reg &= (2 << (bitend - bitstart)) - 1;
                    sprintf(tmpStr, "0x%08X", reg);
                    str = strcat(str, tmpStr);
                  }
                    break;
                  case 'f':
                  {
                    int reg;
                    reg = given >> bitstart;
                    reg &= (2 << (bitend - bitstart)) - 1;
                    if (reg > 7)
                      sprintf(tmpStr, "#%s", arm_fp_const[reg & 7]);
                    else
                      sprintf(tmpStr, "f%d", reg);
                    str = strcat(str, tmpStr);
                  }
                    break;
                  default:
                    throw "disarm";
                }
                  break;
                case '`':
                  c++;
                  if ((given & (1 << bitstart)) == 0)
                  {
                    sprintf(tmpStr, "%c", *c);
                    str = strcat(str, tmpStr);
                  }
                  break;
                case '\'':
                  c++;
                  if ((given & (1 << bitstart)) != 0)
                  {
                    sprintf(tmpStr, "%c", *c);
                    str = strcat(str, tmpStr);
                  }
                  break;
                case '?':
                  ++c;
                  if ((given & (1 << bitstart)) != 0)
                    sprintf(tmpStr, "%c", *c++);
                  else
                    sprintf(tmpStr, "%c", *++c);
                  str = strcat(str, tmpStr);
                  break;
                default:
                  throw "disarm";
              }
              break;
              
            //default:
              //throw "disarm";
            }
          }
        }
        else
        {
          sprintf(tmpStr, "%c", *c);
          str = strcat(str, tmpStr);
        }
      }
      return 4;
    }
  }
  return 0;
}


int disarm(char *dst, unsigned int addr, unsigned int cmd)
{
  unsigned int given = cmd;
  char theString[5];
  char theDisasmLine[2048];
  char theCommentLine[2048];
  char theChar;
  int i;
  int pc = addr;
  int incr = 0;
  
  theString[4] = '\0';
  {
    for (i = 0; i < 4; i++)
    {
      theChar = ((char*) & given)[3-i];
      if (theChar<' ' || theChar>126 || theChar=='\\')
        theChar = '.';
      theString[i] = theChar;
    }
    
    theDisasmLine[0] = '\0';
    theCommentLine[0] = '\0';
    incr = print_insn_arm(pc, given, theDisasmLine, theCommentLine);
    
    char *t = strchr(theDisasmLine, '\t');
    if (t) {
      int dt = (int)(t-theDisasmLine);
      if (dt<8) {
        memmove(theDisasmLine+8, t+1, strlen(t));
        dt = 7;
      }
      memset(t, 32, dt-(t-theDisasmLine)+1);
    }
    sprintf(dst, "%-32s  ;%s 0x%.8X - %-5s", theDisasmLine, theCommentLine, given, theString);
  }
  return incr;
}


int disarm_word(char *dst, unsigned int addr, unsigned int data)
{
	unsigned int given = data;
	char theString[5];
	char theLabel[256];
	char theChar;
	int i;

	theString[4] = '\0';
	for (i = 0; i < 4; i++)
	{
		theChar = ((char*) & given)[3-i];
		if (theChar<' ' || theChar>126 || theChar=='\\')
			theChar = '.';
		theString[i] = theChar;
	}

	theLabel[0] = 0;
	const char *label = DB.findLabelAt(data);
	if (label) ::snprintf(theLabel, 256, ", %s", label);

	sprintf(dst, ".word 0x%08X                  ; \"%s\", %d%s", data, theString, data, theLabel);
	return 4;
}
