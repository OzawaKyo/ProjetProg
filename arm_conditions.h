#ifndef __ARM_COND_H__
#define __ARM_COND_H__

#include <stdint.h>
#include "arm_core.h"

#define EQ 0x0 //Z set
#define NE 0x1 //Z clear
#define CS 0x2 //C set
#define CC 0x3 //C clear
#define MI 0x4 //N set
#define PL 0x5 //N clear
#define VS 0x6 //V set
#define VC 0x7 //V clear
#define HI 0x8 //C set && Z clear
#define LS 0x9 //C clear || Z set
#define GE 0xA //(N set && V set) || (N clear && V clear)
#define LT 0xB //(N set && V clear) || (N clear && V set)
#define GT 0xC //Z clear && ((N set && V set) || (N clear && V clear))
#define LE 0xD //Z set || ((N set && V clear) || (N clear && V set))
#define AL 0xE //Always
#define UN 0xF //depends on ARM version

uint8_t arm_check_cond(arm_core p, uint32_t instruction);

#endif