

#ifndef _ARM_CODE_H
#define _ARM_CODE_H

#include <stdint.h>

extern void check_code_coverage(unsigned int addr, bool isJumpTarget=false);
extern int analyzeAllARMCodeCoverage();


#endif /* _ARM_CODE_H */

