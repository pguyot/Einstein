

#ifndef _ARM_CODE_H
#define _ARM_CODE_H

#include <stdint.h>

extern void check_code_coverage(unsigned int addr, unsigned int flags=0);
extern int analyzeAllARMCodeCoverage();


#endif /* _ARM_CODE_H */

